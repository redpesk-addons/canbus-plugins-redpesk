/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <cerrno>
#include <vector>
#include <string>
#include <net/if.h>
#include <sys/socket.h>
#include <json-c/json.h>
#include <linux/can/raw.h>

#include "can-bus.hpp"

#include "can-signals.hpp"
#include "can-decoder.hpp"
#include "../configuration.hpp"
#include "../utils/signals.hpp"
#include "../utils/openxc-utils.hpp"

extern "C"
{
	#include <afb/afb-binding.h>
}

/// @brief Class constructor
///
/// @param[in] conf_file - handle to the json configuration file.
can_bus_t::can_bus_t(utils::config_parser_t conf_file)
	: conf_file_{conf_file}
{}

std::map<std::string, std::shared_ptr<can_bus_dev_t>> can_bus_t::can_devices_;

/// @brief Will make the decoding operation on a classic CAN message. It will not
/// handle CAN commands nor diagnostic messages that have their own method to get
/// this happens.
///
/// It will add to the vehicle_message queue the decoded message and tell the event push
/// thread to process it.
///
/// @param[in] can_message - a single CAN message from the CAN socket read, to be decode.
///
/// @return How many signals has been decoded.
int can_bus_t::process_can_signals(can_message_t& can_message)
{
	int processed_signals = 0;
	struct utils::signals_found signals;
	openxc_DynamicField search_key, decoded_message;
	openxc_VehicleMessage vehicle_message;
	configuration_t& conf = configuration_t::instance();
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	// First we have to found which can_signal_t it is
	search_key = build_DynamicField((double)can_message.get_id());
	signals = sm.find_signals(search_key);

	// Decoding the message ! Don't kill the messenger !
	for(auto& sig : signals.can_signals)
	{
		std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
		std::map<std::string, struct afb_event>& s = sm.get_subscribed_signals();

		// DEBUG message to make easier debugger STL containers...
		//DEBUG(binder_interface, "Operator[] key char: %s, event valid? %d", sig.generic_name, afb_event_is_valid(s[sig.generic_name]));
		//DEBUG(binder_interface, "Operator[] key string: %s, event valid? %d", sig.generic_name, afb_event_is_valid(s[std::string(sig.generic_name)]));
		//DEBUG(binder_interface, "Nb elt matched char: %d", (int)s.count(sig.generic_name));
		//DEBUG(binder_interface, "Nb elt matched string: %d", (int)s.count(std::string(sig.generic_name));
		if( s.find(sig->get_name()) != s.end() && afb_event_is_valid(s[sig->get_name()]))
		{
			decoded_message = decoder_t::translateSignal(*sig, can_message, conf.get_can_signals());

			openxc_SimpleMessage s_message = build_SimpleMessage(sig->get_name(), decoded_message);
			vehicle_message = build_VehicleMessage(s_message);

			std::lock_guard<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
			push_new_vehicle_message(vehicle_message);
			processed_signals++;
		}
	}

	DEBUG(binder_interface, "%s: %d/%d CAN signals processed.", __FUNCTION__, processed_signals, (int)signals.can_signals.size());
	return processed_signals;
}

/// @brief Will make the decoding operation on a diagnostic CAN message.Then it find the subscribed signal
/// corresponding and will add the vehicle_message to the queue of event to pushed before notifying
/// the event push thread to process it.
///
/// @param[in] manager - the diagnostic manager object that handle diagnostic communication
/// @param[in] can_message - a single CAN message from the CAN socket read, to be decode.
///
/// @return How many signals has been decoded.
int can_bus_t::process_diagnostic_signals(diagnostic_manager_t& manager, const can_message_t& can_message)
{
	int processed_signals = 0;

	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<std::string, struct afb_event>& s = sm.get_subscribed_signals();

	openxc_VehicleMessage vehicle_message = manager.find_and_decode_adr(can_message);
	if( (vehicle_message.has_simple_message && vehicle_message.simple_message.has_name) &&
		(s.find(vehicle_message.simple_message.name) != s.end() && afb_event_is_valid(s[vehicle_message.simple_message.name])))
	{
		std::lock_guard<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
		push_new_vehicle_message(vehicle_message);
		processed_signals++;
	}

	return processed_signals;
}

/// @brief thread to decoding raw CAN messages.
///
///  Depending on the nature of message, if arbitration ID matches ID for a diagnostic response
///  then decoding a diagnostic message else use classic CAN signals decoding functions.
///
/// It will take from the can_message_q_ queue the next can message to process then it search
///  about signal subscribed if there is a valid afb_event for it. We only decode signal for which a
///  subscription has been made. Can message will be decoded using translateSignal that will pass it to the
///  corresponding decoding function if there is one assigned for that signal. If not, it will be the default
///  noopDecoder function that will operate on it.
///
///  TODO: make diagnostic messages parsing optionnal.
void can_bus_t::can_decode_message()
{
	can_message_t can_message;

	while(is_decoding_)
	{
		{
			std::unique_lock<std::mutex> can_message_lock(can_message_mutex_);
			new_can_message_cv_.wait(can_message_lock);
			while(!can_message_q_.empty())
			{
				can_message = next_can_message();

				if(configuration_t::instance().get_diagnostic_manager().is_diagnostic_response(can_message))
					process_diagnostic_signals(configuration_t::instance().get_diagnostic_manager(), can_message);
				else
					process_can_signals(can_message);
			}
		}
		new_decoded_can_message_.notify_one();
	}
}

/// @brief thread to push events to suscribers. It will read subscribed_signals map to look
/// which are events that has to be pushed.
void can_bus_t::can_event_push()
{
	openxc_VehicleMessage v_message;
	openxc_SimpleMessage s_message;
	json_object* jo;
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	while(is_pushing_)
	{
		std::unique_lock<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
		new_decoded_can_message_.wait(decoded_can_message_lock);
		while(!vehicle_message_q_.empty())
		{
			v_message = next_vehicle_message();

			s_message = get_simple_message(v_message);
			{
				std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
				std::map<std::string, struct afb_event>& s = sm.get_subscribed_signals();
				if(s.find(std::string(s_message.name)) != s.end() && afb_event_is_valid(s[std::string(s_message.name)]))
				{
					jo = json_object_new_object();
					jsonify_simple(s_message, jo);
					if(afb_event_push(s[std::string(s_message.name)], jo) == 0)
						on_no_clients(std::string(s_message.name));
				}
			}
		}
	}
}

/// @brief Will initialize threads that will decode
///  and push subscribed events.
void can_bus_t::start_threads()
{
	is_decoding_ = true;
	th_decoding_ = std::thread(&can_bus_t::can_decode_message, this);
	if(!th_decoding_.joinable())
		is_decoding_ = false;

	is_pushing_ = true;
	th_pushing_ = std::thread(&can_bus_t::can_event_push, this);
	if(!th_pushing_.joinable())
		is_pushing_ = false;
}

/// @brief Will stop all threads holded by can_bus_t object
///  which are decoding and pushing then will wait that's
/// they'll finish their job.
void can_bus_t::stop_threads()
{
	is_decoding_ = false;
	is_pushing_ = false;
}

/// @brief Will initialize can_bus_dev_t objects after reading
/// the configuration file passed in the constructor. All CAN buses
/// Initialized here will be added to a vector holding them for
/// inventory and later access.
///
/// That will initialize CAN socket reading too using a new thread.
///
/// @return 0 if ok, other if not.
int can_bus_t::init_can_dev()
{
	std::vector<std::string> devices_name;
	int i = 0;
	size_t t;

	if(conf_file_.check_conf())
	{
		devices_name = conf_file_.get_devices_name();
		if (! devices_name.empty())
		{
			t = devices_name.size();

			for(const auto& device : devices_name)
			{
				can_bus_t::can_devices_[device] = std::make_shared<can_bus_dev_t>(device, i);
				if (can_bus_t::can_devices_[device]->open() >= 0)
				{
					can_bus_t::can_devices_[device]->configure();
					DEBUG(binder_interface, "%s: Start reading thread", __FUNCTION__);
					NOTICE(binder_interface, "%s: %s device opened and reading", __FUNCTION__, device.c_str());
					can_bus_t::can_devices_[device]->start_reading(*this);
					i++;
				}
				else
				{
					ERROR(binder_interface, "%s: Can't open device %s", __FUNCTION__, device.c_str());
					return 1;
				}
			}
			NOTICE(binder_interface, "%s: Initialized %d/%d can bus device(s)", __FUNCTION__, i, (int)t);
			return 0;
		}
		ERROR(binder_interface, "%s: Error at CAN device initialization. No devices read from configuration file", __FUNCTION__);
		return 1;
	}
	ERROR(binder_interface, "%s: Can't read INI configuration file", __FUNCTION__);
	return 2;
}

/// @brief return new_can_message_cv_ member
///
/// @return  return new_can_message_cv_ member
std::condition_variable& can_bus_t::get_new_can_message_cv()
{
	return new_can_message_cv_;
}

/// @brief return can_message_mutex_ member
///
/// @return  return can_message_mutex_ member
std::mutex& can_bus_t::get_can_message_mutex()
{
	return can_message_mutex_;
}

/// @brief Return first can_message_t on the queue
///
/// @return a can_message_t
can_message_t can_bus_t::next_can_message()
{
	can_message_t can_msg;

	if(!can_message_q_.empty())
	{
		can_msg = can_message_q_.front();
		can_message_q_.pop();
		DEBUG(binder_interface, "%s: Here is the next can message : id %X, length %X, data %02X%02X%02X%02X%02X%02X%02X%02X", __FUNCTION__, can_msg.get_id(), can_msg.get_length(),
			can_msg.get_data()[0], can_msg.get_data()[1], can_msg.get_data()[2], can_msg.get_data()[3], can_msg.get_data()[4], can_msg.get_data()[5], can_msg.get_data()[6], can_msg.get_data()[7]);
		return can_msg;
	}

	return can_msg;
}

/// @brief Push a can_message_t into the queue
///
/// @param[in] can_msg - the const reference can_message_t object to push into the queue
void can_bus_t::push_new_can_message(const can_message_t& can_msg)
{
	can_message_q_.push(can_msg);
}

/// @brief Return first openxc_VehicleMessage on the queue
///
/// @return a openxc_VehicleMessage containing a decoded can message
openxc_VehicleMessage can_bus_t::next_vehicle_message()
{
	openxc_VehicleMessage v_msg;

	if(! vehicle_message_q_.empty())
	{
		v_msg = vehicle_message_q_.front();
		vehicle_message_q_.pop();
		DEBUG(binder_interface, "%s: next vehicle message poped", __FUNCTION__);
		return v_msg;
	}

	return v_msg;
}

/// @brief Push a openxc_VehicleMessage into the queue
///
/// @param[in] v_msg - const reference openxc_VehicleMessage object to push into the queue
void can_bus_t::push_new_vehicle_message(const openxc_VehicleMessage& v_msg)
{
	vehicle_message_q_.push(v_msg);
}

/// @brief Create a RX_SETUP receive job for the BCM socket of a CAN signal.
///
/// @return 0 if ok.
	int can_bus_t::create_rx_filter(const can_signal_t& s)
	{
		const std::string& bus  = s.get_message().get_bus_name();
		return can_bus_t::can_devices_[bus]->create_rx_filter(s);
	}

/// @brief Return a map with the can_bus_dev_t initialized
///
/// @return map can_bus_dev_m_ map
const std::map<std::string, std::shared_ptr<can_bus_dev_t>>& can_bus_t::get_can_devices() const
{
	return can_bus_t::can_devices_;
}

/// @brief Return the shared pointer on the can_bus_dev_t initialized 
/// with device_name "bus"
///
/// @param[in] bus - CAN bus device name to retrieve.
///
/// @return A shared pointer on an object can_bus_dev_t
std::shared_ptr<can_bus_dev_t> can_bus_t::get_can_device(std::string bus)
{
	return can_bus_t::can_devices_[bus];
}