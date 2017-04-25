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
#include <fnmatch.h>

#include "active-diagnostic-request.hpp"

#include "../configuration.hpp"

#define ERROR_PID 0xFF

std::string active_diagnostic_request_t::prefix_ = "diagnostic_messages";

bool active_diagnostic_request_t::operator==(const active_diagnostic_request_t& b)
{
	return (bus_ == b.bus_ && id_ == b.id_ && handle_ == b.handle_);
}

active_diagnostic_request_t& active_diagnostic_request_t::operator=(const active_diagnostic_request_t& adr)
{
	if (this != &adr)
	{
		bus_ = adr.bus_;
		id_ = adr.id_;
		handle_ = adr.handle_;
		name_ = adr.name_;
		decoder_ = adr.decoder_;
		callback_ = adr.callback_;
		recurring_ = adr.recurring_; 
		wait_for_multiple_responses_ = adr.wait_for_multiple_responses_;
		in_flight_ = adr.in_flight_;
		frequency_clock_ = adr.frequency_clock_;
		timeout_clock_ = adr.timeout_clock_;
	}
	
	return *this;
}

active_diagnostic_request_t::active_diagnostic_request_t()
	: bus_{nullptr}, id_{0}, handle_{nullptr}, name_{""},
	  decoder_{nullptr}, callback_{nullptr}, recurring_{false}, wait_for_multiple_responses_{false},
	  in_flight_{false}, frequency_clock_{frequency_clock_t()}, timeout_clock_{frequency_clock_t()}
{}

active_diagnostic_request_t::active_diagnostic_request_t(const std::string& bus, DiagnosticRequest* request,
		const std::string& name, bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz)
	: bus_{bus}, id_{request->arbitration_id}, handle_{nullptr}, name_{name},
	  decoder_{decoder}, callback_{callback}, recurring_{frequencyHz ? true : false}, wait_for_multiple_responses_{wait_for_multiple_responses},
	  in_flight_{false}, frequency_clock_{frequency_clock_t(frequencyHz)}, timeout_clock_{frequency_clock_t(10)}
{}

uint32_t active_diagnostic_request_t::get_id() const
{
	return id_;
}

const std::shared_ptr<can_bus_dev_t> active_diagnostic_request_t::get_can_bus_dev() const
{
	return can_bus_t::get_can_device(bus_);
}

uint16_t active_diagnostic_request_t::get_pid() const
{
	if (handle_->request.has_pid)
		return handle_->request.pid;
	return ERROR_PID;
}

DiagnosticRequestHandle* active_diagnostic_request_t::get_handle()
{
	return handle_;
}

const std::string active_diagnostic_request_t::get_name() const
{
	return name_;
}

std::string& active_diagnostic_request_t::get_prefix()
{
	return active_diagnostic_request_t::prefix_;
}

DiagnosticResponseDecoder& active_diagnostic_request_t::get_decoder()
{
	return decoder_;
}

DiagnosticResponseCallback& active_diagnostic_request_t::get_callback()
{
	return callback_;
}

bool active_diagnostic_request_t::get_recurring() const
{
	return recurring_;
}

bool active_diagnostic_request_t::get_in_flight() const
{
	return in_flight_;
}

frequency_clock_t& active_diagnostic_request_t::get_frequency_clock()
{
	return frequency_clock_;
}

frequency_clock_t& active_diagnostic_request_t::get_timeout_clock()
{
	return timeout_clock_;
}

void active_diagnostic_request_t::set_handle(DiagnosticShims& shims, DiagnosticRequest* request)
{
	handle_ = new DiagnosticRequestHandle(generate_diagnostic_request(&shims, request, nullptr));
}

void active_diagnostic_request_t::set_in_flight(bool val)
{
	in_flight_ = val;
}

///
/// @brief Check if requested signal name is a diagnostic message. If the name
///  begin with the diagnostic message prefix then true else false.
///
/// @param[in] name - A signal name.
///
/// @return true if name began with the diagnostic message prefix else false.
///
bool active_diagnostic_request_t::is_diagnostic_signal(const std::string& name)
{
	const std::string p = active_diagnostic_request_t::prefix_ + "*";
	if(::fnmatch(p.c_str(), name.c_str(), FNM_CASEFOLD) == 0)
		return true;
	return false;
}

/// @brief Check is the request should be sent or not
///
/// @return true if the request is not running or recurring nor completed,
/// or it's recurring, its clock elapsed
/// so it's time to send another one.
bool active_diagnostic_request_t::should_send()
{
	return !in_flight_ && ( (!recurring_ && !request_completed()) ||
			(recurring_ && frequency_clock_.elapsed(true)) );
}

/// @brief check if the timeout clock has elapsed
///
/// @return true if elapsed, so it is a timeout, else false.
bool active_diagnostic_request_t::timed_out()
{
	// don't use staggered start with the timeout clock
	return timeout_clock_.elapsed(false);
}

/// @brief Returns true if a sufficient response has been received for a
/// diagnostic request.
///
/// This is true when at least one response has been received and the request is
/// configured to not wait for multiple responses. Functional broadcast requests
/// may often wish to wait the full 100ms for modules to respond.
bool active_diagnostic_request_t::response_received() const
{
	return !wait_for_multiple_responses_ &&
				handle_->completed;
}

/// @brief Returns true if the request has timed out waiting for a response,
/// or a sufficient number of responses has been received.
bool active_diagnostic_request_t::request_completed()
{
	return response_received() || 
		(timed_out() && diagnostic_request_sent(handle_));
}