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

#pragma once

/// @brief return epoch in milliseconds
///
/// @return long long int epoch in milliseconds
typedef long long int (*time_function_t)();

long long int system_time_us();
long long int system_time_ms();
long long int system_time_s();


/// @brief A frequency counting clock.
/// Utility class allowing some time function.
class frequency_clock_t
{
private:
	float unit_; ///< unit_ - multiplicator to make operation to be in the right unit (milli, micro, nano, etc)
	float frequency_; ///< the clock frequency in Hz.
	unsigned long last_tick_; ///< the last time (in milliseconds since startup) that the clock ticked.
	time_function_t time_function_; ///<  a function returning current time

public:
	frequency_clock_t();
	frequency_clock_t(float frequency);
	frequency_clock_t(float frequency, unsigned long last_tick, time_function_t time_function);

	float get_frequency() const;

	float frequency_to_period();
	bool started();
	time_function_t get_time_function();
	bool elapsed(bool stagger);

	void tick();
};