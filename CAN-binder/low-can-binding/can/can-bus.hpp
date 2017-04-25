/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <linux/can.h>
#include <condition_variable>

#include "openxc.pb.h"
#include "can-message.hpp"
#include "can-bus-dev.hpp"
#include "can-signals.hpp"
#include "../utils/config-parser.hpp"
#include "../diagnostic/diagnostic-manager.hpp"
#include "../low-can-binding.hpp"

// TODO actual max is 32 but dropped to 24 for memory considerations
#define MAX_ACCEPTANCE_FILTERS 24
// TODO this takes up a ton of memory
#define MAX_DYNAMIC_MESSAGE_COUNT 12

#define CAN_ACTIVE_TIMEOUT_S 30

/// @brief Object used to handle decoding and manage event queue to be pushed.
///
/// This object is also used to initialize can_bus_dev_t object after reading
/// json conf file describing the CAN devices to use. Thus, those object will read
/// on the device the CAN frame and push them into the can_bus_t can_message_q_ queue.
///
/// That queue will be later used to be decoded and pushed to subscribers.
class can_bus_t
{
private:
	utils::config_parser_t conf_file_; ///< configuration file handle used to initialize can_bus_dev_t objects.

	int process_can_signals(can_message_t& can_message);
	int process_diagnostic_signals(diagnostic_manager_t& manager, const can_message_t& can_message);

	void can_decode_message();
	std::thread th_decoding_; ///< thread that'll handle decoding a can frame
	bool is_decoding_ = false; ///< boolean member controling thread while loop

	void can_event_push();
	std::thread th_pushing_; ///< thread that'll handle pushing decoded can frame to subscribers
	bool is_pushing_ = false; ///< boolean member controling thread while loop

	std::condition_variable new_can_message_cv_; ///< condition_variable use to wait until there is a new CAN message to read
	std::mutex can_message_mutex_; ///< mutex protecting the can_message_q_ queue.
	std::queue <can_message_t> can_message_q_; ///< queue that'll store can_message_t to decoded

	std::condition_variable new_decoded_can_message_; ///< condition_variable use to wait until there is a new vehicle message to read from the queue vehicle_message_q_
	std::mutex decoded_can_message_mutex_;  ///< mutex protecting the vehicle_message_q_ queue.
	std::queue <openxc_VehicleMessage> vehicle_message_q_; ///< queue that'll store openxc_VehicleMessage to pushed

	static std::map<std::string, std::shared_ptr<can_bus_dev_t>> can_devices_; ///< Can device map containing all can_bus_dev_t objects initialized during init_can_dev function

public:
	can_bus_t(utils::config_parser_t conf_file);
	can_bus_t(can_bus_t&&);

	int init_can_dev();
	std::vector<std::string> read_conf();

	void start_threads();
	void stop_threads();

	can_message_t next_can_message();
	void push_new_can_message(const can_message_t& can_msg);
	std::mutex& get_can_message_mutex();
	std::condition_variable& get_new_can_message_cv();

	openxc_VehicleMessage next_vehicle_message();
	void push_new_vehicle_message(const openxc_VehicleMessage& v_msg);

	int create_rx_filter(const can_signal_t& signal);

	const std::map<std::string, std::shared_ptr<can_bus_dev_t>>& get_can_devices() const;
	static std::shared_ptr<can_bus_dev_t> get_can_device(std::string bus);
};