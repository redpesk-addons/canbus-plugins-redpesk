#include "application.hpp"
#include "../can/can-decoder.hpp"
#include "../can/can-encoder.hpp"

application_t::application_t()
	: can_bus_manager_{utils::config_parser_t{"/etc/dev-mapping.conf"}}
	, message_set_{
		{std::make_shared<message_set_t>(message_set_t{0,"example",
			{ // beginning message_definition_ vector
				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3D9,0,0,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"engine.speed",// generic_name
							16,// bit_position
							16,// bit_size
							0.250000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"fuel.level.low",// generic_name
							55,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"fuel.level",// generic_name
							8,// bit_position
							8,// bit_size
							0.392157f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x3E9,0,0,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"vehicle.average.speed",// generic_name
							0,// bit_position
							15,// bit_size
							0.0156250f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x4D1,0,0,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"engine.oil.temp",// generic_name
							16,// bit_position
							8,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							nullptr,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"engine.oil.temp.high",// generic_name
							7,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x620,0,0,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"doors.boot.open",// generic_name
							47,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"doors.front_left.open",// generic_name
							43,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"doors.front_right.open",// generic_name
							44,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"doors.rear_left.open",// generic_name
							46,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"doors.rear_right.open",// generic_name
							45,// bit_position
							4,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry
,				{std::make_shared<message_definition_t>(message_definition_t{"hs",0x799,0,0,message_format_t::STANDARD,frequency_clock_t(5.00000f),true,
					{ // beginning signals vector
						{std::make_shared<signal_t> (signal_t{
							"windows.front_left.open",// generic_name
							43,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"windows.front_right.open",// generic_name
							44,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"windows.rear_left.open",// generic_name
							46,// bit_position
							1,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})},
						{std::make_shared<signal_t> (signal_t{
							"windows.rear_right.open",// generic_name
							45,// bit_position
							4,// bit_size
							1.00000f,// factor
							0.00000,// offset
							0,// min_value
							0,// max_value
							frequency_clock_t(0.00000f),// frequency
							true,// send_same
							false,// force_send_changed
							{
							},// states
							false,// writable
							decoder_t::decode_boolean,// decoder
							nullptr,// encoder
							false,// received
							std::make_pair<bool, int>(false, 0),// multiplex
							0,// is_big_endian
							0,// is_signed
							""// unit
						})}
					} // end signals vector
				})} // end message_definition entry

		}, // end message_definition vector
			{ // beginning diagnostic_messages_ vector
				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					4,
					"engine.load",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					5,
					"engine.coolant.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					10,
					"fuel.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					11,
					"intake.manifold.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					12,
					"engine.speed",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					13,
					"vehicle.speed",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					15,
					"intake.air.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					16,
					"mass.airflow",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					17,
					"throttle.position",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					31,
					"running.time",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					45,
					"EGR.error",
					0,
					0,
					UNIT::INVALID,
					0.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					47,
					"fuel.level",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					51,
					"barometric.pressure",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					70,
					"ambient.air.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					76,
					"commanded.throttle.position",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					82,
					"ethanol.fuel.percentage",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					90,
					"accelerator.pedal.position",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					91,
					"hybrid.battery-pack.remaining.life",
					0,
					0,
					UNIT::INVALID,
					5.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					92,
					"engine.oil.temperature",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					94,
					"engine.fuel.rate",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}
,				{std::make_shared<diagnostic_message_t>(diagnostic_message_t{
					99,
					"engine.torque",
					0,
					0,
					UNIT::INVALID,
					1.00000f,
					decoder_t::decode_obd2_response,
					nullptr,
					true,
					false
				})}

			} // end diagnostic_messages_ vector
		})} // end message_set entry
	} // end message_set vector
{
	for(std::shared_ptr<message_set_t> cms: message_set_)
	{
		std::vector<std::shared_ptr<message_definition_t>> messages_definition = cms->get_messages_definition();
		for(std::shared_ptr<message_definition_t> cmd : messages_definition)
		{
			cmd->set_parent(cms);
			std::vector<std::shared_ptr<signal_t>> signals = cmd->get_signals();
			for(std::shared_ptr<signal_t> sig: signals)
			{
				sig->set_parent(cmd);
			}
		}

		std::vector<std::shared_ptr<diagnostic_message_t>> diagnostic_messages = cms->get_diagnostic_messages();
		for(std::shared_ptr<diagnostic_message_t> dm : diagnostic_messages)
		{
			dm->set_parent(cms);
		}
	}
		}

const std::string application_t::get_diagnostic_bus() const
{
	return "hs";
}


