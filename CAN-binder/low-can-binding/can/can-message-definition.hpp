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

/**
 * @class can_message_definition_t
 *
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 */

#pragma once

#include <vector>
#include <memory>

#include "can-message.hpp"
#include "../utils/timer.hpp"

class can_message_definition_t
{
private:
	std::uint8_t message_set_id_;
	std::string bus_; /*!< bus_ - Address of CAN bus device. */
	uint32_t id_; /*!< id_ - The ID of the message.*/
	can_message_format_t format_; /*!< format_ - the format of the message's ID.*/
	frequency_clock_t frequency_clock_; /*!<  clock_ - an optional frequency clock to control the output of this
							*      message, if sent raw, or simply to mark the max frequency for custom
							*      handlers to retrieve.*/
	bool force_send_changed_; /*!< force_send_changed_ - If true, regardless of the frequency, it will send CAN
 							 *	message if it has changed when using raw passthrough.*/
	std::vector<uint8_t> last_value_; /*!< last_value_ - The last received value of the message. Defaults to undefined.
 										  *	This is required for the forceSendChanged functionality, as the stack
 										  *	needs to compare an incoming CAN message with the previous frame.*/
	
public:
	can_message_definition_t(std::uint8_t message_set_id, const std::string bus);
	can_message_definition_t(std::uint8_t message_set_id, const std::string bus, uint32_t id, frequency_clock_t frequency_clock, bool force_send_changed);
	can_message_definition_t(std::uint8_t message_set_id, const std::string bus, uint32_t id, can_message_format_t format, frequency_clock_t frequency_clock, bool force_send_changed);

	const std::string& get_bus_name() const;
	uint32_t get_id() const;
};
