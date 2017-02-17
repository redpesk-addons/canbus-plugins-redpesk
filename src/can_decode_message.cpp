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

#include <linux/can.h>
#include <linux/can/raw.h>
#include <queue>
#include <sys/timeb.h>

#include <afb/afb-binding.h>

#include "can-utils.hpp"
#include "can-decoder.hpp"
#include "openxc.pb.h"

union DynamicField
{
	char string[100];
	double numeric_value;
	bool boolean_value;
};

void can_decode_message(can_bus_t &can_bus)
{
	can_message_t can_message;
	std:vector <CanSignal> signals;
	std:vector <CanSignal>::iterator signals_i;
	openxc_VehicleMessage vehicle_message;
	openxc_DynamicField search_key, ret;
	bool send = true;
	
	decoder_t decoder();

	while(true)
	{
		if(can_message = can_bus.next_can_message())
		{
			/* First we have to found which CanSignal is */
			search_key = build_DynamicField(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, (double)can_message.get_id())

			signals = find_can_signals(search_key);
			
			/* Decoding the message ! Don't kill the messenger ! */
			for(const auto& sig : signals)
			{	
			  subscribed_signals_i = subscribed_signals.find(sig);
				
				if(subscribed_signals_i != subscribed_signals.end() &&
					afb_event_is_valid(subscribed_signals_i->second))
				{
					ret = decoder.decodeSignal(sig, can_message, getSignals(), &send);

					s_message = build_SimpleMessage(sig.genericName, ret);
						
					vehicle_message = build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, s_message);
					vehicle_message_q.push(vehicle_message);
				}
			}
		}
	}
}

/*
 * Build a specific VehicleMessage containing a SimpleMessage.
 */
openxc_VehicleMessage build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type type,
	const openxc_SimpleMessage& message)
{
	struct timeb t_msec;
	long long int timestamp_msec;
	
	openxc_VehicleMessage v = {0};
	
	if(!ftime(&t_msec))
	{
		timestamp_msec = ((long long int) t_msec.time) * 1000ll + 
                        (long long int) t_msec.millitm;

	  v.has_type = true:
		v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
		v.has_simple_message = true;
		v.simple_message =  message;
		v.has_timestamp = true;
		v.timestamp = timestamp_msec;
		
	  return v;
	}

	v.has_type = true,
	v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
	v.has_simple_message = true;
	v.simple_message =  message;
					  
  return v;
}

/*
 * Build an openxc_SimpleMessage associating a name to an openxc_DynamicField
 */
openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value)
{
  
  openxc_SimpleMessage s = {0};
  
  s.has_name = true;
  ::strncpy(s.name, name.c_str(), 100);
	s.has_value = true;
  s.value = value;
  
	return s;
}

/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(const std::string& value)
{
  openxc_DynamicField d = {0}
	d.has_type = true;
	d.type = openxc_DynamicField_Type_STRING;
	
	d.has_string_value = true;
	::strncpy(d.string_value, value.c_tr(), 100);
	
	return d;
}

/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(double value)
{
  openxc_DynamicField d = {0}
	d.has_type = true;
	d.type = openxc_DynamicField_Type_NUM;
	
	d.has_numeric_value = true;
	d.numeric_value = field;
	
	return d;
}

/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(bool value)
{
  openxc_DynamicField d = {0}
	d.has_type = true;
	d.type = openxc_DynamicField_Type_BOOL;
	
	d.has_boolean_value = true;
	d.boolean_value = field;
	
	return d;
}