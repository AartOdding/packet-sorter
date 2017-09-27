#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>


class Message
{
public:

    const size_t id;
    const size_t size;

    Message(const size_t id_, const size_t size_) :
        id{ id_ },
        size{ size_ },
        packets{ new std::string[size] }
    {
    }

    ~Message()
    {
        delete[] packets;
    }

    void add(const size_t index, std::string& packet)
    {
        if (index >= 0 && index < size && packets[index].empty())
        {
            packets[index] = std::move(packet);
            ++n_packets_arrived;
        }
    }

    bool finished() const
    {
        return n_packets_arrived >= size;
    }

    friend std::ostream& operator<<(std::ostream& out, const Message& m)
    {
        for (size_t i{ 0 }; i < m.size; ++i)
        {
            std::cout << "\t" << m.packets[i] << std::endl;
        }
        return out;
    }

private:

    std::string* packets;
    unsigned int n_packets_arrived{ 0 };

};

using HeapMessage = std::pair<size_t, std::unique_ptr<Message>>;


int main()
{
    std::stringstream interpreter;
    std::vector<HeapMessage> messages;

    while (true)
    {
        size_t message_id;
        size_t message_size;
        size_t packet_index;
        std::string packet_data;

        std::getline(std::cin, packet_data);
        interpreter.str(packet_data);

        if (interpreter >> message_id && interpreter >> packet_index && interpreter >> message_size)
        {
            auto message = std::find_if(messages.begin(), messages.end(), 
                [message_id](const HeapMessage& msg) { return message_id == msg.first; });

            if (message == messages.end())
            {
                messages.push_back(std::make_pair(message_id, std::make_unique<Message>(message_id, message_size)));
                message = --messages.end();
            }

            if (packet_index >= 0 && packet_index < message->second->size)
            {
                message->second->add(packet_index, packet_data);
            }
            
            if (message->second->finished())
            {
                std::cout << *message->second;
                std::swap(message, --messages.end());
                messages.pop_back();
            }
        }
    }
}
