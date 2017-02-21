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

#include "can-utils.hpp"

/********************************************************************************
*
*		CanMessage method implementation
*
*********************************************************************************/

can_message_t::can_message_t(const struct afb_binding_interface* interface)
	: interface_{interface}
{}

uint32_t can_message_t::get_id() const
{
	if (id_ != 0)
		return id_;
	return 0;
}

int can_message_t::get_format() const
{
	if (format_ != CanMessageFormat::STANDARD || format_ != CanMessageFormat::EXTENDED)
		return -1;
	return format_;
}

const uint8_t* can_message_t::get_data() const
{
	return &data_;
}
uint8_t can_message_t::get_length() const
{
	return length_;
}

void can_message_t::set_id(const uint32_t new_id)
{
	switch(format_)
	{
		case CanMessageFormat::STANDARD:
			id_ = new_id & CAN_SFF_MASK;
			break;
		case CanMessageFormat::EXTENDED:
			id_ = new_id & CAN_EFF_MASK;
			break;
		default:
			ERROR(interface_, "ERROR: Can set id, not a compatible format or format not set prior to set id.");
			break;
	}
}

void can_message_t::set_format(const CanMessageFormat new_format)
{
	if(new_format == CanMessageFormat::STANDARD || new_format == CanMessageFormat::EXTENDED)
		format_ = new_format;
	else
		ERROR(interface_, "ERROR: Can set format, wrong format chosen");
}

void can_message_t::set_data(const uint8_t new_data)
{
	::memcpy(&data_, &new_data, sizeof(new_data));
	length_ = sizeof(new_data);
}

/*
 * @brief This is the preferred way to initialize a CanMessage object 
 * from a read canfd_frame message.
 * 
 * @param: canfd_frame pointer
 */
void can_message_t::convert_from_canfd_frame(const canfd_frame& frame)
{
	length_ = (frame.len > CAN_MAX_DLEN) ? (uint8_t)CAN_MAX_DLEN : frame.len;
	length_ = (frame.len > CANFD_MAX_DLEN) ? (uint8_t)CANFD_MAX_DLEN : frame.len;

	if (frame.can_id & CAN_ERR_FLAG)
		id_ = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
	else if (frame.can_id & CAN_EFF_FLAG)
	{
		id_ = frame.can_id & CAN_EFF_MASK;
		format_ = CanMessageFormat::EXTENDED;
	}
	else
	{
		id_ = frame.can_id & CAN_SFF_MASK;
		format_ = CanMessageFormat::STANDARD;
	}

	if (sizeof(frame.data) <= sizeof(data_))
		::memcpy(&data_, frame.data, length_);
	else if (sizeof(frame.data) >= CAN_MAX_DLEN)
		ERROR(interface_, "can_message_t: canfd_frame data too long to be stored into CanMessage object");
}

canfd_frame can_message_t::convert_to_canfd_frame()
{
	canfd_frame frame;

	frame.can_id = get_id();
	frame.len = get_length();
	::memcpy(frame.data, get_data(), length_);

	return frame;
}
/********************************************************************************
*
*		can_bus_dev_t method implementation
*
*********************************************************************************/

can_bus_dev_t::can_bus_dev_t(const std::string &dev_name)
	: device_name_{dev_name}
{
}

int can_bus_dev_t::open(const struct afb_binding_interface* interface)
{
	const int canfd_on = 1;
	struct ifreq ifr;
	struct timeval timeout = {1, 0};

	DEBUG(interface, "open_can_dev: CAN Handler socket : %d", can_socket_);
	if (can_socket_ >= 0)
		return 0;

	can_socket_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (can_socket_ < 0)
	{
		ERROR(interface, "open_can_dev: socket could not be created");
	}
	else
	{
		/* Set timeout for read */
		::setsockopt(can_socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		/* try to switch the socket into CAN_FD mode */
		if (::setsockopt(can_socket_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(interface, "open_can_dev: Can not switch into CAN Extended frame format.");
			is_fdmode_on_ = false;
		} else {
			is_fdmode_on_ = true;
		}

		/* Attempts to open a socket to CAN bus */
		::strcpy(ifr.ifr_name, device_name_.c_str());
		if(::ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0)
			ERROR(interface, "open_can_dev: ioctl failed");
		else
		{
			txAddress_.can_family = AF_CAN;
			txAddress_.can_ifindex = ifr.ifr_ifindex;

			/* And bind it to txAddress */
			if (::bind(can_socket_, (struct sockaddr *)&txAddress_, sizeof(txAddress_)) < 0)
			{
				ERROR(interface, "open_can_dev: bind failed");
			}
			else
			{
				::fcntl(can_socket_, F_SETFL, O_NONBLOCK);
				return 0;
			}
		}
		close();
	}
	return -1;
}

int can_bus_dev_t::close()
{
	::close(can_socket_);
	can_socket_ = -1;
	return can_socket_;
}

canfd_frame can_bus_dev_t::read(const struct afb_binding_interface* interface)
{
	ssize_t nbytes;
	//int maxdlen;
	canfd_frame canfd_frame;

	/* Test that socket is really opened */
	if (can_socket_ < 0)
	{
		ERROR(interface, "read_can: Socket unavailable. Closing thread.");
		is_running_ = false;
	}

	nbytes = ::read(can_socket_, &canfd_frame, CANFD_MTU);

	switch(nbytes)
	{
		case CANFD_MTU:
			DEBUG(interface, "read_can: Got an CAN FD frame with length %d", canfd_frame.len);
			//maxdlen = CANFD_MAX_DLEN;
			break;
		case CAN_MTU:
			DEBUG(interface, "read_can: Got a legacy CAN frame with length %d", canfd_frame.len);
			//maxdlen = CAN_MAX_DLEN;
			break;
		default:
			if (errno == ENETDOWN)
					ERROR(interface, "read_can: %s interface down", device_name_);
			ERROR(interface, "read_can: Error reading CAN bus");
			::memset(&canfd_frame, 0, sizeof(canfd_frame));
			is_running_ = false;
			break;
	}
	
	return canfd_frame;
}

/**
 * @brief start reading threads and set flag is_running_
 * 
 */
void can_bus_dev_t::start_reading()
{
	th_reading_ = std::thread(can_reader, *this);
	is_running_ = true;
}

/*
 * Return is_running_ bool
 */
bool can_bus_dev_t::is_running()
{
	return is_running_;
}

/**
 * @brief: Get a can_message_t from can_message_q and return it
 * then point to the next can_message_t in queue.
 * 
 * @return the next queue element or NULL if queue is empty.
 */
can_message_t can_bus_dev_t::next_can_message(const struct afb_binding_interface* interface)
{
	can_message_t can_msg(interface);

	if(!can_message_q_.empty())
	{
		can_msg = can_message_q_.front();
		can_message_q_.pop();
		DEBUG(interface, "next_can_message: Here is the next can message : id %d, length %d", can_msg.get_id(), can_msg.get_length());
		return can_msg;
	}
	
	NOTICE(interface, "next_can_message: End of can message queue");
	has_can_message_ = false;
	return can_msg;
}

/**
 * @brief Append a new element to the can message queue and set
 * has_can_message_ boolean to true
 * 
 * @params[const can_message_t& can_msg] the can_message_t to append
 * 
 */
void can_bus_dev_t::push_new_can_message(const can_message_t& can_msg)
{
	can_message_q_.push(can_msg);
}

/**
 * @brief Flag that let you know when can message queue is exhausted
 * 
 * @return[bool] has_can_message_ bool
 */
bool can_bus_dev_t::has_can_message() const
{
	return has_can_message_;
}

/**
 * @brief Send a can message from a can_message_t object.
 * 
 * params[const can_message_t& can_msg] the can message object to send
 * 
 */
int can_bus_dev_t::send_can_message(can_message_t& can_msg, const struct afb_binding_interface* interface)
{
	ssize_t nbytes;
	canfd_frame f;

	f = can_msg.convert_to_canfd_frame();

	if(can_socket_ >= 0)
	{
		nbytes = ::sendto(can_socket_, &f, sizeof(struct canfd_frame), 0,
				(struct sockaddr*)&txAddress_, sizeof(txAddress_));
		if (nbytes == -1)
		{
			ERROR(interface, "send_can_message: Sending CAN frame failed.");
			return -1;
		}
		return (int)nbytes;
	}
	else
	{
		ERROR(interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open(interface);
	}
	return 0;
}
/********************************************************************************
*
*		can_bus_t method implementation
*
*********************************************************************************/

can_bus_t::can_bus_t(const afb_binding_interface *itf, int& conf_file)
	: interface_{itf}, conf_file_{conf_file}
{
}

/**
 * @brief start threads relative to the can bus: decoding and pushing
 * as the reading is handled by can_bus_dev_t object
 */
void can_bus_t::start_threads()
{
	th_decoding_ = std::thread(can_decode_message, *this);
	th_pushing_ = std::thread(can_event_push, *this);
}

/**
 * @brief Initialize as many as can_bus_dev_t objects with their respective reading thread
 * 
 * params[std::ifstream& conf_file] conf_file ifstream to the JSON configuration 
 * file located at the rootdir of the binding
 */
int can_bus_t::init_can_dev()
{
	std::vector<std::string> devices_name;
	int i;
	size_t t;

	devices_name = read_conf();

	if (! devices_name.empty())
	{
		t = devices_name.size();
		i=0;

		for(const auto& device : devices_name)
		{
			can_bus_dev_t can_bus_device_handler(device);
			can_bus_device_handler.open(interface_);
			can_bus_device_handler.start_reading();
			i++;
		}

		NOTICE(interface_, "Initialized %d/%d can bus device(s)", i, t);
		return 0;
	}
	ERROR(interface_, "init_can_dev: Error at CAN device initialization.");
	return 1;
}

/** 
 * @brief Read the conf file and extract device name
 * 
 * @return[std:vector<std::string>] return a vector of device name
 */
std::vector<std::string> can_bus_t::read_conf()
{
	std::vector<std::string> ret;
	json_object *jo, *canbus;
	int n, i;

	FILE *fd = fdopen(conf_file_, "r");
	if (fd)
	{
		std::string fd_conf_content;
		std::fseek(fd, 0, SEEK_END);
		fd_conf_content.resize(std::ftell(fd));
		std::rewind(fd);
		std::fread(&fd_conf_content[0], 1, fd_conf_content.size(), fd);
		std::fclose(fd);

		jo = json_tokener_parse(fd_conf_content.c_str());

		if (jo == NULL || !json_object_object_get_ex(jo, "canbus", &canbus))
		{
			ERROR(interface_, "Can't find canbus node in the configuration file. Please review it.");
			ret.clear();
		}
		else if (json_object_get_type(canbus) != json_type_array)
			ret.push_back(json_object_get_string(canbus));
		else
		{
			n = json_object_array_length(canbus);
			for (i = 0 ; i < n ; i++)
				ret.push_back(json_object_get_string(json_object_array_get_idx(canbus, i)));
		}
		return ret;
	}
	ERROR(interface_, "Problem at reading the conf file");
	ret.clear();
	return ret;
}

/**
 * @brief: Get a VehicleMessage from vehicle_message_q and return it
 * then point to the next VehicleMessage in queue.
 * 
 * @return the next queue element or NULL if queue is empty.
 */
openxc_VehicleMessage can_bus_t::next_vehicle_message()
{
	openxc_VehicleMessage v_msg;

	if(! vehicle_message_q_.empty())
	{
		v_msg = vehicle_message_q_.front();
		vehicle_message_q_.pop();
		DEBUG(interface_, "next_vehicle_message: next vehicle message poped");
		return v_msg;
	}
	
	NOTICE(interface_, "next_vehicle_message: End of vehicle message queue");
	has_vehicle_message_ = false;
	return v_msg;
}

/**
 * @brief Append a new element to the vehicle message queue and set
 * has_vehicle_message_ boolean to true
 * 
 * @params[const openxc_VehicleMessage& v_msg] the openxc_VehicleMessage to append
 * 
 */
void can_bus_t::push_new_vehicle_message(const openxc_VehicleMessage& v_msg)
{
	vehicle_message_q_.push(v_msg);
	has_vehicle_message_ = true;
}

/**
 * @brief Flag that let you know when vehicle message queue is exhausted
 * 
 * @return[bool] has_vehicle_message_ bool
 */
bool can_bus_t::has_vehicle_message() const
{
	return has_vehicle_message_;
}