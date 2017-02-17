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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <systemd/sd-event.h>
#include <errno.h>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <fstream>

#include <json-c/json.h>
#include <openxc.pb.h>

#include <afb/afb-binding.h>
#include <afb/afb-service-itf.h>

#include "obd2.hpp"
#include "can-utils.hpp"
#include "can-signals.hpp"

/*
 *	Interface between the daemon and the binding
 */
static const struct afb_binding_interface *interface;
static obd2_handler_t obd2_handler();

/********************************************************************************
*
*		Event management
*
*********************************************************************************/

/********************************************************************************
*
*		Subscription and unsubscription
*
*********************************************************************************/

static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, std::vector<CanSignal>::const_iterator& sig_i)
{
	int ret;

	const auto& ss_i = subscribed_signals.find(sig_i);
	if (ss_i != subscribed_signals.end())
	{
		if(!afb_event_is_valid(ss_i->second))
		{
			if(!subscribe)
			{
				NOTICE(interface, "Event isn't valid, it can't be unsubscribed.");
				ret = 1;
			}
			else
			{
				ss_i->second = afb_daemon_make_event(interface->daemon, ss_i->first.genericName);
				if (!afb_event_is_valid(ss_i->second)) 
				{
					ERROR(interface, "Can't create an event, something goes wrong.");
					ret = 0;
				}
			}
		}
	}
	else
	{
		subscribed_signals[sig_i] = afb_daemon_make_event(interface->daemon, sig_i->genericName);
		if (!afb_event_is_valid(ss_i->second)) 
		{
			ERROR(interface, "Can't create an event, something goes wrong.");
			ret = 0;
		}
	}

	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, subscribed_signals[sig_i])) < 0)
	{
		ERROR(interface, "Operation goes wrong for signal: %s", sig_i->genericName);
		ret = 0;
	}
	else
		ret = 1;
	
	return ret;
}

static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const std::vector<CanSignal>& signals)
{
	int ret;

	// TODO: lock the subscribed_signals when insert/remove
	for(const auto& signal_i : signals)
	{
		ret = subscribe_unsubscribe_signal(request, subscribe, signal_i);
		if(ret == 0)
			return ret;
	}
}

static int subscribe_unsubscribe_all(struct afb_req request, bool subscribe)
{
	int n, e;

	n = obd2_handler.OBD2_PIDS.size();
	e = 0;
	for (const auto& pid : obd2_handler.OBD2_PIDS)
		e += !subscribe_unsubscribe_signals(request, subscribe, pid);
	
	return e == 0;
}

static int subscribe_unsubscribe_name(struct afb_req request, bool subscribe, const char *name)
{
	std::vector<CanSignal> sig;
	int ret = 0;

	if (!::strcmp(name, "*"))
		ret = subscribe_unsubscribe_all(request, subscribe);
	else
	{
		//if(obd2_handler_c.is_obd2_signal(name))
		if(false)
		{
		// TODO
		}
		else
		{
			sig = find_can_signals(name);
			if (sig.empty())
				ret = 0;
		}
		ret = subscribe_unsubscribe_signals(request, subscribe, sig);
	}
	return ret;
}

static void subscribe_unsubscribe(struct afb_req request, bool subscribe)
{
	int ok, i, n;
	struct json_object *args, *a, *x;

	/* makes the subscription/unsubscription */
	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &a)) {
		ok = subscribe_unsubscribe_all(request, subscribe);
	} else if (json_object_get_type(a) != json_type_array) {
		ok = subscribe_unsubscribe_name(request, subscribe, json_object_get_string(a));
	} else {
		n = json_object_array_length(a);
		ok = 0;
		for (i = 0 ; i < n ; i++) {
			x = json_object_array_get_idx(a, i);
			if (subscribe_unsubscribe_name(request, subscribe, json_object_get_string(x)))
				ok++;
		}
		ok = (ok == n);
	}

	/* send the report */
	if (ok)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

static void subscribe(struct afb_req request)
{
	subscribe_unsubscribe(request, true);
}

static void unsubscribe(struct afb_req request)
{
	subscribe_unsubscribe(request, false);
}

static const struct afb_verb_desc_v1 verbs[]=
{
	{ .name= "subscribe",    .session= AFB_SESSION_NONE, .callback= subscribe,	.info= "subscribe to notification of CAN bus messages." },
	{ .name= "unsubscribe",  .session= AFB_SESSION_NONE, .callback= unsubscribe,	.info= "unsubscribe a previous subscription." }
};

static const struct afb_binding binding_desc = {
	.type = AFB_BINDING_VERSION_1,
	.v1 = {
		.info = "CAN bus service",
		.prefix = "can",
		.verbs = verbs
	}
};

const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
{
	interface = itf;

	return &binding_desc;
}

/**
 * @brief Initialize the binding.
 * 
 * @param[in] service Structure which represent the Application Framework Binder.
 * 
 * @return Exit code, zero if success.
 */
int afbBindingV1ServiceInit(struct afb_service service)
{
	std::ifstream fd_conf;
	fd_conf = afb_daemon_rootdir_open_locale(interface->daemon, "can_bus.json", O_RDONLY, NULL);

	/* Open CAN socket */
	can_bus_t can_bus_handler(interface, fd_conf);
	if(can_bus_handler.init_can_dev() == 0)
	{
		can_bus_handler.start_threads();
		return 0;
	}

	return 1;
}
