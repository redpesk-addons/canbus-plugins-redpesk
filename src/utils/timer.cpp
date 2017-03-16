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

#include "utils/timer.hpp"

long long int systemTimeMs()
{
	struct timeb t_msec;
	long long int timestamp_msec;
	
	if(!::ftime(&t_msec))
	{
		timestamp_msec = (t_msec.time) * 1000ll + 
			t_msec.millitm;
	}
	return timestamp_msec;
}