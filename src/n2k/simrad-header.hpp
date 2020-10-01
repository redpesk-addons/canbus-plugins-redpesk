#ifndef SIMRAD_HEADER_HPP
#define SIMRAD_HEADER_HPP 1

#include <low-can/utils/signals.hpp>
#include <low-can/utils/openxc-utils.hpp>
#include <algorithm>
class pgn_60416_BAM_t
{
private:
    int code_;
    int size_;
    int packets_;
    pgn_t pgn_;
    uint8_t reserved_;
    int sub_id_;

public:
    pgn_60416_BAM_t(){};
    pgn_60416_BAM_t(int code, int size, int packets, pgn_t pgn, uint8_t reserved, int sub_id)
    {
        code_ = code;
        size_ = size;
        packets_ = packets;
        pgn_ = pgn;
        reserved_ = reserved;
        sub_id_ = sub_id;
    };
    int get_code()
    {
        return code_;
    }
    int get_size()
    {
        return size_;
    }
    int get_packets()
    {
        return packets_;
    }
    pgn_t get_pgn()
    {
        return pgn_;
    }
    int get_sub_id()
    {
        return sub_id_;
    }
};

class pgn_60160_t
{
private:
    int sid_;
    uint8_t data_[8];

public:
    pgn_60160_t(){};
    pgn_60160_t(int sid, uint8_t data[])
    {
        sid_ = sid;
        //data_ = [8];
        for (int i = 0; i < 8; i++)
            data_[i] = data[i];
    };
    int get_sid()
    {
        return sid_;
    };
    uint8_t *get_data()
    {
        return data_;
    };
};

class fast_packet_t
{
public:
	pgn_t pgn_;
	uint8_t data_[8];
    fast_packet_t(){};
    fast_packet_t(pgn_t pgn, const uint8_t data[])
    {
        pgn_ = pgn;
        //data_ = [8];
        for (int i = 0; i < 8; i++)
            data_[7-i] = data[i];
    };
	int get_pgn()
    {
        return pgn_;
    };
    uint8_t *get_data()
    {
        return data_;
    };
	uint8_t get_sid()
	{
		return data_[0];
	}
	uint8_t get_size()
	{
		// ONLY VALID IF FIRST FAST PACKET
		return data_[1];
	}
};


class stack_message_t
{
private:
    static std::vector<pgn_60416_BAM_t> stack_60416_;
    static std::vector<pgn_60160_t> stack_60160_;
	static std::list<fast_packet_t> stack_fast_packet_;
    application_t *app_;

public:
    stack_message_t(application_t *app)
    {
        app_ = app;
    };
    static stack_message_t &instance(application_t *app)
    {
        static stack_message_t stack(app);
        return stack;
    }
    ~stack_message_t(){};
    std::vector<pgn_60416_BAM_t> *get_stack_60416()
    {
        return &stack_60416_;
    };
    std::vector<pgn_60160_t> *get_stack_60160()
    {
        return &stack_60160_;
    };
	std::list<fast_packet_t> *get_stack_fast_packet()
    {
        return &stack_fast_packet_;
    };

    std::map<std::string, openxc_DynamicField> translate_message(std::shared_ptr<message_t> message, pgn_t pgn)
    {
        vect_ptr_msg_def_t messages_definition = app_->get_messages_definition(pgn);
        if (messages_definition.empty())
            return std::map<std::string, openxc_DynamicField>();

        bool send = true;

        openxc_DynamicField dynamicField_tmp;

        for (std::shared_ptr<message_definition_t> message_definition : messages_definition)
        {

            bool valid = true;
            std::map<std::string, openxc_DynamicField> tmp = std::map<std::string, openxc_DynamicField>();
            for (std::shared_ptr<signal_t> sig : message_definition->get_signals())
            {
                dynamicField_tmp = decoder_t::translate_signal(*sig, message, &send);
                if (!dynamicField_tmp.has_type)
                {
                    valid = false;
                }
                tmp.insert(std::pair<std::string, openxc_DynamicField>(sig->get_name(), dynamicField_tmp));
            }
            if (valid)
            {
                return tmp;
            }
        }
        return std::map<std::string, openxc_DynamicField>();
    };

    j1939_message_t get_multi_packet_message()
    {
        int need_nb_packet = 0;
        for (auto pgn_60416 : stack_60416_)
        {
            need_nb_packet += pgn_60416.get_packets();
        }

        if (need_nb_packet == stack_60160_.size())
        {
            pgn_60416_BAM_t last_60416 = stack_60416_.back();
            stack_60416_.pop_back();

            std::vector<pgn_60160_t> lasts_60160 = std::vector<pgn_60160_t>();
            int nb_packets = last_60416.get_packets();
            for (int i = 0; i < nb_packets; i++)
            {
                lasts_60160.push_back(stack_60160_.back());
                stack_60160_.pop_back();
            }

            uint8_t sorted[nb_packets][8];
            for (pgn_60160_t pgn : lasts_60160)
            {
                int sid = pgn.get_sid();
                if (sid > 0 && sid < nb_packets)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        sorted[sid][i] = pgn.get_data()[i];
                    }
                }
            }

            std::vector<uint8_t> data = std::vector<uint8_t>();

            for (int i = 0; i < nb_packets; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    data.push_back(sorted[i][j]);
                }
            }

            j1939_message_t jm = j1939_message_t(last_60416.get_size(),
							data,
							12,
							1,
							last_60416.get_pgn(),
							1);

            return jm;
        }

        return j1939_message_t();
    }


    j1939_message_t get_fast_packet_message()
    {

		fast_packet_t first = stack_fast_packet_.front();
		int size_bytes = first.get_size();

		int nb_packet = ((size_bytes - 1) >> 3) + 1;

		if(stack_fast_packet_.size() < nb_packet)
			return j1939_message_t();

		int sid = first.get_sid();
		std::vector<fast_packet_t*> all = std::vector<fast_packet_t*>();
		all.push_back(&first);
		for (fast_packet_t packet : stack_fast_packet_)
		{
			if(packet.get_sid() == sid + 1 && packet.get_pgn() == first.get_pgn())
			{
				all.push_back(&packet);
				sid++;
			}
		}

		if(all.size() != nb_packet)
			return j1939_message_t();


		std::vector<uint8_t> data = std::vector<uint8_t>();


		int cpt = size_bytes;

		for(int i = 2; i < 8; i++)
		{
			data.push_back(first.get_data()[i]);
			cpt--;
		}

		for(int i = 1; i < nb_packet; i++)
		{
			for(int j = 1; j < 8; j++)
			{

				data.push_back(all[i]->get_data()[j]);
				cpt--;
				if(cpt == 0)
				{
					break;
				}
			}
			if(cpt == 0)
			{
				break;
			}
		}

		std::reverse(data.begin(), data.end());

        j1939_message_t jm = j1939_message_t(size_bytes,
						data,
						12,
						1,
						first.get_pgn(),
						1);

	for(auto packet_remove : all)
	{
		for(std::list<fast_packet_t>::iterator i = stack_fast_packet_.begin(); i != stack_fast_packet_.end(); i++)
		{
			if(packet_remove->get_pgn() == (*i).get_pgn() && packet_remove->get_sid() == (*i).get_sid())
			{
				stack_fast_packet_.erase(i);
				break;
			}
		}
	}

	return jm;
    }
};


#endif
