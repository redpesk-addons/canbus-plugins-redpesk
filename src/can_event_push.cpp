/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
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

#include "can_event_push.hpp"

void can_event_push(can_bus_t& can_bus)
{
	openxc_VehicleMessage v_message;
	openxc_SimpleMessage s_message;
	json_object* jo;
	
	while(can_bus.has_vehicle_message())
	{
		v_message = can_bus.next_vehicle_message();
		s_message = get_simple_message(v_message);
		std::map<std::string, struct afb_event> subscribed_signals = get_subscribed_signals();
		const auto& it_event = subscribed_signals.find(s_message.name);
		if(it_event != subscribed_signals.end() && afb_event_is_valid(it_event->second))
		{
			jo = json_object_new_object();
			jsonify_simple(s_message, jo);
			afb_event_push(it_event->second, jo);
		}
	}
}