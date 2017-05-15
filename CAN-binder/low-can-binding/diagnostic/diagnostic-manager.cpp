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

#include <systemd/sd-event.h>
#include <algorithm>
#include <string.h>

#include "diagnostic-manager.hpp"

#include "../utils/openxc-utils.hpp"
#include "../utils/signals.hpp"
#include "../binding/configuration.hpp"

#define MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ 10
#define MAX_SIMULTANEOUS_DIAG_REQUESTS 50
// There are only 8 slots of in flight diagnostic requests
#define MAX_SIMULTANEOUS_IN_FLIGHT_REQUESTS 8
#define TIMERFD_ACCURACY 0
#define MICRO 1000000

diagnostic_manager_t::diagnostic_manager_t()
	: initialized_{false}
{}

/// @brief Diagnostic manager isn't initialized at launch but after
///  CAN bus devices initialization. For the moment, it is only possible
///  to have 1 diagnostic bus which are the first bus declared in the JSON
///  description file. Configuration instance will return it.
///
/// this will initialize DiagnosticShims and cancel all active requests 
///  if there are any.
bool diagnostic_manager_t::initialize()
{
	// Mandatory to set the bus before intialize shims.
	bus_ = configuration_t::instance().get_diagnostic_bus();

	init_diagnostic_shims();
	reset();

	initialized_ = true;
	DEBUG(binder_interface, "%s: Diagnostic Manager initialized", __FUNCTION__);
	return initialized_;
}

void diagnostic_manager_t::read_socket()
{
	can_message_t msg;
	can_bus_t& cbm = configuration_t::instance().get_can_bus_manager();
	socket_ >> msg;
	std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	{ cbm.push_new_can_message(msg); }
	cbm.get_new_can_message_cv().notify_one();
}

utils::socketcan_bcm_t& diagnostic_manager_t::get_socket()
{
	return socket_;
}

/// @brief initialize shims used by UDS lib and set initialized_ to true.
///  It is needed before used the diagnostic manager fully because shims are
///  required by most member functions.
void diagnostic_manager_t::init_diagnostic_shims()
{
	shims_ = diagnostic_init_shims(shims_logger, shims_send, NULL);
	DEBUG(binder_interface, "%s: Shims initialized", __FUNCTION__);
}

/// @brief Force cleanup all active requests.
void diagnostic_manager_t::reset()
{
	DEBUG(binder_interface, "%s: Clearing existing diagnostic requests", __FUNCTION__);
	cleanup_active_requests(true);
}

/// @brief Adds 8 RX_SETUP jobs to the BCM rx_socket_ then diagnotic manager
///  listens on CAN ID range 7E8 - 7EF affected to the OBD2 communications.
///
/// @return -1 or negative value on error, 0 if ok.
int diagnostic_manager_t::add_rx_filter(uint32_t can_id)
{
	// Make sure that socket has been opened.
	if(! socket_)
		socket_.open(bus_);

	struct utils::simple_bcm_msg bcm_msg;
	memset(&bcm_msg.msg_head, 0, sizeof(bcm_msg.msg_head));

	const struct timeval freq =  recurring_requests_.back()->get_timeout_clock().get_timeval_from_period();

	bcm_msg.msg_head.opcode  = RX_SETUP;
	bcm_msg.msg_head.flags = SETTIMER|RX_FILTER_ID;
	bcm_msg.msg_head.ival2.tv_sec = freq.tv_sec;
	bcm_msg.msg_head.ival2.tv_usec = freq.tv_usec;

	// If it isn't an OBD2 CAN ID then just add a simple RX_SETUP job
	if(can_id != OBD2_FUNCTIONAL_RESPONSE_START) 
	{
		bcm_msg.msg_head.can_id  = can_id;

		socket_ << bcm_msg;
			if(! socket_)
				return -1;
	}
	else
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			can_id  =  OBD2_FUNCTIONAL_RESPONSE_START + i;
			bcm_msg.msg_head.can_id  = can_id;

			socket_ << bcm_msg;
			if(! socket_)
				return -1;
		}
	}

	return 0;
}

/// @brief send function use by diagnostic library. Only one bus used for now
///  so diagnostic request is sent using the default diagnostic bus not matter of
///  which is specified in the diagnostic message definition.
///
/// @param[in] arbitration_id - CAN arbitration ID to use when send message. OBD2 broadcast ID
///  is 0x7DF by example.
/// @param[in] data - The data payload for the message. NULL is valid if size is also 0.
/// @param[in] size - The size of the data payload, in bytes.
///
/// @return true if the CAN message was sent successfully. 
bool diagnostic_manager_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	diagnostic_manager_t& dm = configuration_t::instance().get_diagnostic_manager();
	active_diagnostic_request_t* current_adr = dm.get_last_recurring_requests();
	utils::socketcan_bcm_t& tx_socket = current_adr->get_socket();

	// Make sure that socket has been opened.
	if(! tx_socket)
		tx_socket.open(
			dm.get_can_bus());

	struct utils::simple_bcm_msg bcm_msg;
	struct can_frame cfd;

	memset(&cfd, 0, sizeof(cfd));
	memset(&bcm_msg.msg_head, 0, sizeof(bcm_msg.msg_head));

	struct timeval freq = current_adr->get_frequency_clock().get_timeval_from_period();

	bcm_msg.msg_head.opcode  = TX_SETUP;
	bcm_msg.msg_head.can_id  = arbitration_id;
	bcm_msg.msg_head.flags = SETTIMER|STARTTIMER|TX_CP_CAN_ID;
	bcm_msg.msg_head.ival2.tv_sec = freq.tv_sec;
	bcm_msg.msg_head.ival2.tv_usec = freq.tv_usec;
	bcm_msg.msg_head.nframes = 1;
	::memcpy(cfd.data, data, size);

	bcm_msg.frames = cfd;

	tx_socket << bcm_msg;
	if(tx_socket)
		return true;
	return false;
}

/// @brief The type signature for an optional logging function, if the user
/// wishes to provide one. It should print, store or otherwise display the
/// message.
///
/// message - A format string to log using the given parameters.
/// ... (vargs) - the parameters for the format string.
///
void diagnostic_manager_t::shims_logger(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[256];
	vsnprintf(buffer, 256, format, args);

	DEBUG(binder_interface, "%s: %s", __FUNCTION__, buffer);
}

/// @brief The type signature for a... OpenXC TODO: not used yet.
void diagnostic_manager_t::shims_timer()
{}

std::shared_ptr<can_bus_dev_t> diagnostic_manager_t::get_can_bus_dev()
active_diagnostic_request_t* diagnostic_manager_t::get_last_recurring_requests() const
{
	return recurring_requests_.back();
}

/// @brief Return diagnostic manager shims member.
DiagnosticShims& diagnostic_manager_t::get_shims()
{
	return shims_;
}

/// @brief Search for a specific active diagnostic request in the provided requests list
/// and erase it from the vector. This is useful at unsubscription to clean up the list otherwize
/// all received CAN messages will be passed to DiagnosticRequestHandle of all active diagnostic request
/// contained in the vector but no event if connected to, so we will decode uneeded request.
///
/// @param[in] entry - a pointer of an active_diagnostic_request instance to clean up
/// @param[in] requests_list - a vector where to make the search and cleaning.
void diagnostic_manager_t::find_and_erase(active_diagnostic_request_t* entry, std::vector<active_diagnostic_request_t*>& requests_list)
{
	auto i = std::find(requests_list.begin(), requests_list.end(), entry);
	if ( i != requests_list.end())
		requests_list.erase(i);
}

// @brief TODO: implement cancel_request if needed... Don't know.
void diagnostic_manager_t::cancel_request(active_diagnostic_request_t* entry)
{

	/* TODO: implement acceptance filters.
	if(entry.arbitration_id_ == OBD2_FUNCTIONAL_BROADCAST_ID) {
		for(uint32_t filter = OBD2_FUNCTIONAL_RESPONSE_START;
				filter < OBD2_FUNCTIONAL_RESPONSE_START +
					OBD2_FUNCTIONAL_RESPONSE_COUNT;
				filter++) {
			removeAcceptanceFilter(entry.bus_, filter,
					CanMessageFormat::STANDARD, getCanBuses(),
					getCanBusCount());
		}
	} else {
		removeAcceptanceFilter(entry.bus_,
				entry.arbitration_id_ +
					DIAGNOSTIC_RESPONSE_ARBITRATION_ID_OFFSET,
				CanMessageFormat::STANDARD, getCanBuses(), getCanBusCount());
	}*/
}

/// @brief Cleanup a specific request if it isn't running and get complete. As it is almost
/// impossible to get that state for a recurring request without waiting for that, you can 
/// force the cleaning operation.
///
/// @param[in] entry - the request to clean
/// @param[in] force - Force the cleaning or not ?
void diagnostic_manager_t::cleanup_request(active_diagnostic_request_t* entry, bool force)
{
	if((force || (entry != nullptr && entry->get_in_flight() && entry->request_completed())))
	{
		entry->set_in_flight(false);

		char request_string[128] = {0};
		diagnostic_request_to_string(&entry->get_handle()->request,
			request_string, sizeof(request_string));
		if(force && entry->get_recurring())
		{
			find_and_erase(entry, recurring_requests_);
			cancel_request(entry);
			DEBUG(binder_interface, "%s: Cancelling completed, recurring request: %s", __FUNCTION__, request_string);
		}
		else
		{
			DEBUG(binder_interface, "%s: Cancelling completed, non-recurring request: %s", __FUNCTION__, request_string);
			find_and_erase(entry, non_recurring_requests_);
			cancel_request(entry);
		}
	}
}

/// @brief Clean up all requests lists, recurring and not recurring.
///
/// @param[in] force - Force the cleaning or not ? If true, that will do
/// the same effect as a call to reset().
void diagnostic_manager_t::cleanup_active_requests(bool force)
{
	for(auto& entry : non_recurring_requests_)
		if (entry != nullptr)
			cleanup_request(entry, force);

	for(auto& entry : recurring_requests_)
		if (entry != nullptr)
			cleanup_request(entry, force);
}

/// @brief Will return the active_diagnostic_request_t pointer for theDiagnosticRequest or nullptr if
/// not found.
///
/// @param[in] request - Search key, method will go through recurring list to see if it find that request
///  holded by the DiagnosticHandle member.
active_diagnostic_request_t* diagnostic_manager_t::find_recurring_request(const DiagnosticRequest* request)
{
	for (auto& entry : recurring_requests_)
	{
		if(entry != nullptr)
		{
			if(diagnostic_request_equals(&entry->get_handle()->request, request))
			{
				return entry;
				break;
			}
		}
	}
	return nullptr;
}

/// @brief Add and send a new one-time diagnostic request.
///
/// A one-time (aka non-recurring) request can existing in parallel with a
/// recurring request for the same PID or mode, that's not a problem.
///
/// For an example, see the docs for addRecurringRequest. This function is very
/// similar but leaves out the frequencyHz parameter.
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - Human readable name this response, to be used when
///      publishing received responses. TODO: If the name is NULL, the published output
///      will use the raw OBD-II response format.
/// @param[in] wait_for_multiple_responses - If false, When any response is received
///      for this request it will be removed from the active list. If true, the
///      request will remain active until the timeout clock expires, to allow it
///      to receive multiple response. Functional broadcast requests will always
///      waint for the timeout, regardless of this parameter.
/// @param[in] decoder - An optional DiagnosticResponseDecoder to parse the payload of
///      responses to this request. If the decoder is NULL, the output will
///      include the raw payload instead of a parsed value.
/// @param[in] callback - An optional DiagnosticResponseCallback to be notified whenever a
///      response is received for this request.
///
/// @return true if the request was added successfully. Returns false if there
/// wasn't a free active request entry, if the frequency was too high or if the
/// CAN acceptance filters could not be configured,
active_diagnostic_request_t* diagnostic_manager_t::add_request(DiagnosticRequest* request, const std::string name,
	bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
	const DiagnosticResponseCallback callback)
{
	cleanup_active_requests(false);

	active_diagnostic_request_t* entry = nullptr;

	if (non_recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
	{
		// TODO: implement Acceptance Filter
		//	if(updateRequiredAcceptanceFilters(bus, request)) {
			active_diagnostic_request_t* entry = new active_diagnostic_request_t(bus_, request, name,
					wait_for_multiple_responses, decoder, callback, 0);
			entry->set_handle(shims_, request);

			char request_string[128] = {0};
			diagnostic_request_to_string(&entry->get_handle()->request, request_string,
					sizeof(request_string));

			find_and_erase(entry, non_recurring_requests_);
			DEBUG(binder_interface, "%s: Added one-time diagnostic request on bus %s: %s", __FUNCTION__,
					bus_.c_str(), request_string);

			non_recurring_requests_.push_back(entry);
	}
	else
	{
		WARNING(binder_interface, "%s: There isn't enough request entry. Vector exhausted %d/%d", __FUNCTION__, (int)non_recurring_requests_.size(), MAX_SIMULTANEOUS_DIAG_REQUESTS);
		non_recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
	}
	return entry;
}

bool diagnostic_manager_t::validate_optional_request_attributes(float frequencyHz)
{
	if(frequencyHz > MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ) {
		DEBUG(binder_interface, "%s: Requested recurring diagnostic frequency %lf is higher than maximum of %d", __FUNCTION__,
			frequencyHz, MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ);
		return false;
	}
	return true;
}

/// @brief Add and send a new recurring diagnostic request.
///
/// At most one recurring request can be active for the same arbitration ID, mode
/// and (if set) PID on the same bus at one time. If you try and call
/// addRecurringRequest with the same key, it will return an error.
///
/// TODO: This also adds any neccessary CAN acceptance filters so we can receive the
/// response. If the request is to the functional broadcast ID (0x7df) filters
/// are added for all functional addresses (0x7e8 to 0x7f0).
///
/// Example:
///
///     // Creating a functional broadcast, mode 1 request for PID 2.
///     DiagnosticRequest request = {
///         arbitration_id: 0x7df,
///         mode: 1,
///         has_pid: true,
///         pid: 2
///     };
///
///     // Add a recurring request, to be sent at 1Hz, and published with the
///     // name "my_pid_request"
///     addRecurringRequest(&getConfiguration()->diagnosticsManager,
///          canBus,
///          &request,
///          "my_pid_request",
///          false,
///          NULL,
///          NULL,
///          1);
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - An optional human readable name this response, to be used when
///      publishing received responses. If the name is NULL, the published output
///      will use the raw OBD-II response format.
/// @param[in] wait_for_multiple_responses - If false, When any response is received
///      for this request it will be removed from the active list. If true, the
///      request will remain active until the timeout clock expires, to allow it
///      to receive multiple response. Functional broadcast requests will always
///      waint for the timeout, regardless of this parameter.
/// @param[in] decoder - An optional DiagnosticResponseDecoder to parse the payload of
///      responses to this request. If the decoder is NULL, the output will
///      include the raw payload instead of a parsed value.
/// @param[in] callback - An optional DiagnosticResponseCallback to be notified whenever a
///      response is received for this request.
/// @param[in] frequencyHz - The frequency (in Hz) to send the request. A frequency above
///      MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ is not allowed, and will make this
///      function return false.
///
/// @return true if the request was added successfully. Returns false if there
/// was too much already running requests, if the frequency was too high TODO:or if the
/// CAN acceptance filters could not be configured,
active_diagnostic_request_t* diagnostic_manager_t::add_recurring_request(DiagnosticRequest* request, const char* name,
		bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz)
{
	active_diagnostic_request_t* entry = nullptr;

	if(!validate_optional_request_attributes(frequencyHz))
		return entry;

	cleanup_active_requests(false);

	if(find_recurring_request(request) == nullptr)
	{
		if(recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
		{
			// TODO: implement Acceptance Filter
			//if(updateRequiredAcceptanceFilters(bus, request)) {
			active_diagnostic_request_t* entry = new active_diagnostic_request_t(bus_, request, name,
					wait_for_multiple_responses, decoder, callback, frequencyHz);
			recurring_requests_.push_back(entry);

			entry->set_handle(shims_, request);
			if(add_rx_filter(OBD2_FUNCTIONAL_BROADCAST_ID) < 0)
			{ recurring_requests_.pop_back(); }
			else
			{ start_diagnostic_request(&shims_, entry->get_handle()); }
		}
		else
		{
			WARNING(binder_interface, "%s: There isn't enough request entry. Vector exhausted %d/%d", __FUNCTION__, (int)recurring_requests_.size(), MAX_SIMULTANEOUS_DIAG_REQUESTS);
			recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
		}
	}
	else
	{ DEBUG(binder_interface, "%s: Can't add request, one already exists with same key", __FUNCTION__);}
	return entry;
}

/// @brief Returns true if there are two active requests running for the same arbitration ID.
bool diagnostic_manager_t::conflicting(active_diagnostic_request_t* request, active_diagnostic_request_t* candidate) const
{
	return (candidate->get_in_flight() && candidate != request &&
			candidate->get_can_bus_dev() == request->get_can_bus_dev() &&
			candidate->get_id() == request->get_id());
}


/// @brief Returns true if there are no other active requests to the same arbitration ID
/// and if there aren't more than 8 requests in flight at the same time.
bool diagnostic_manager_t::clear_to_send(active_diagnostic_request_t* request) const
{
	int total_in_flight = 0;
	for ( auto entry : non_recurring_requests_)
	{
		if(conflicting(request, entry))
			return false;
		if(entry->get_in_flight())
			total_in_flight++;
	}

	for ( auto entry : recurring_requests_)
	{
		if(conflicting(request, entry))
			return false;
		if(entry->get_in_flight())
			total_in_flight++;
	}

	if(total_in_flight > MAX_SIMULTANEOUS_IN_FLIGHT_REQUESTS)
		return false;
	return true;
}

/// @brief Will decode the diagnostic response and build the final openxc_VehicleMessage to return.
///
/// @param[in] adr - A pointer to an active diagnostic request holding a valid diagnostic handle
/// @param[in] response - The response to decode from which the Vehicle message will be built and returned
///
/// @return A filled openxc_VehicleMessage or a zeroed struct if there is an error.
openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_response(active_diagnostic_request_t* adr, const DiagnosticResponse& response)
{
	openxc_VehicleMessage message = build_VehicleMessage();
	float value = (float)diagnostic_payload_to_integer(&response);
	if(adr->get_decoder() != nullptr)
	{
		value = adr->get_decoder()(&response, value);
	}

	if((response.success && strnlen(adr->get_name().c_str(), adr->get_name().size())) > 0)
	{
		// If name, include 'value' instead of payload, and leave of response
		// details.
		message = build_VehicleMessage(build_SimpleMessage(adr->get_name(), build_DynamicField(value)));
	}
	else
	{
		// If no name, send full details of response but still include 'value'
		// instead of 'payload' if they provided a decoder. The one case you
		// can't get is the full detailed response with 'value'. We could add
		// another parameter for that but it's onerous to carry that around.
		message = build_VehicleMessage(adr, response, value);
	}

	// If not success but completed then the pid isn't supported
	if(!response.success)
	{
		struct utils::signals_found found_signals;
		found_signals = utils::signals_manager_t::instance().find_signals(build_DynamicField(adr->get_name()));
		found_signals.diagnostic_messages.front()->set_supported(false);
		cleanup_request(adr, true);
		NOTICE(binder_interface, "%s: PID not supported or ill formed. Please unsubscribe from it. Error code : %d", __FUNCTION__, response.negative_response_code);
		message = build_VehicleMessage(build_SimpleMessage(adr->get_name(), build_DynamicField("This PID isn't supported by your vehicle.")));
	}

	if(adr->get_callback() != nullptr)
	{
		adr->get_callback()(adr, &response, value);
	}

	// Reset the completed flag handle to make sure that it will be reprocessed the next time.
	adr->get_handle()->completed = false;
	return message;
}

/// @brief Will take the CAN message and pass it to the receive functions that will process
/// diagnostic handle for each active diagnostic request then depending on the result we will 
/// return pass the diagnostic response to decode it.
///
/// @param[in] entry - A pointer to an active diagnostic request holding a valid diagnostic handle
/// @param[in] cm - A raw CAN message.
///
/// @return A pointer to a filled openxc_VehicleMessage or a nullptr if nothing has been found.
openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_handle(active_diagnostic_request_t* entry, const can_message_t& cm)
{
	DiagnosticResponse response = diagnostic_receive_can_frame(&shims_, entry->get_handle(), cm.get_id(), cm.get_data(), cm.get_length());
	if(response.completed && entry->get_handle()->completed)
	{
		if(entry->get_handle()->success)
			return relay_diagnostic_response(entry, response);
	}
	else if(!response.completed && response.multi_frame)
	{
		// Reset the timeout clock while completing the multi-frame receive
		entry->get_timeout_clock().tick();
	}

	return build_VehicleMessage();
}

/// @brief Find the active diagnostic request with the correct DiagnosticRequestHandle
/// member that will understand the CAN message using diagnostic_receive_can_frame function
/// from UDS-C library. Then decode it with an ad-hoc method.
///
/// @param[in] cm - Raw CAN message received
///
/// @return VehicleMessage with decoded value.
openxc_VehicleMessage diagnostic_manager_t::find_and_decode_adr(const can_message_t& cm)
{
	openxc_VehicleMessage vehicle_message = build_VehicleMessage();

	for ( auto entry : non_recurring_requests_)
	{
		vehicle_message = relay_diagnostic_handle(entry, cm);
		if (is_valid(vehicle_message))
			return vehicle_message;
	}

	for ( auto entry : recurring_requests_)
	{
		vehicle_message = relay_diagnostic_handle(entry, cm);
		if (is_valid(vehicle_message))
			return vehicle_message;
	}

	return vehicle_message;
}

/// @brief Tell if the CAN message received is a diagnostic response.
/// Request broadcast ID use 0x7DF and assigned ID goes from 0x7E0 to Ox7E7. That allows up to 8 ECU to respond 
/// at the same time. The response is the assigned ID + 0x8, so response ID can goes from 0x7E8 to 0x7EF.
///
/// @param[in] cm - CAN message received from the socket.
///
/// @return True if the active diagnostic request match the response.
bool diagnostic_manager_t::is_diagnostic_response(const can_message_t& cm)
{
	if (cm.get_id() >= 0x7e8 && cm.get_id() <= 0x7ef)
			return true;
	return false;
}
