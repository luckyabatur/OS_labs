#include <unordered_set>
#include "../include/nodes.h"


bool check_input()
{
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: incorrect input" << std::endl;
        return false;
    }
    return true;
}

class Controller
{
private:
    std::unordered_set<int> node_ids_;
    std::list<Node> children_;


    std::list<Message> create_exec_messages_;
    std::list<Message> ping_messages_;


    bool availability_check_started_ {false};
    std::chrono::system_clock::time_point check_start_time_ {};


    void handle_child_message(const Message& message)
    {
        if (message.command == CommandType::Create)
            node_ids_.insert(message.id);

        if (message.command != CommandType::Ping)
            std::cout << "Ok: " << message.add_data << std::endl;

        remove_message(message);
    }

    void check_pending_messages()
    {
        auto it = create_exec_messages_.begin();
        while (it != create_exec_messages_.end())
        {
            if (std::chrono::duration_cast<std::chrono::seconds>(now() - it->sent_time).count() > 5)
            {
                handle_timeout(*it);
                it = create_exec_messages_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void remove_message(const Message& message)
    {
        CommandType cmd = message.command;
        int id = message.id;

        if (cmd == CommandType::Create || cmd == CommandType::Exec)
        {
            auto it = std::find_if(create_exec_messages_.begin(), create_exec_messages_.end(),
                               [cmd, id](const Message& m) {
                                   return (m.command == cmd && m.id == id) ||
                                          (m.command == cmd && m.command == CommandType::Create && m.add_data == id);
                               });
            if (it != create_exec_messages_.end())
            {
                create_exec_messages_.erase(it);
            }
        }

        else if (cmd == CommandType::Ping)
        {
            auto it = std::find_if(ping_messages_.begin(), ping_messages_.end(),
                                   [id](const Message &m)
                                   {
                                       return (m.id == id);
                                   });
            if (it != ping_messages_.end())
            {
                ping_messages_.erase(it);
            }
        }
    }

    void handle_timeout(const Message& message)
    {
            if (message.command == CommandType::Create)
                std::cout << "Error: Parent " << message.id << " is unavailable" << std::endl;
            else
                std::cout << "Error: Node " << message.id << " is unavailable" << std::endl;
    }


    void broadcast_message(const Message& message)
    {
        if (message.command == CommandType::Ping)
            ping_messages_.push_back(message);
        else
            create_exec_messages_.push_back(message);

        for (auto& child : children_)
        {
            send_message(child, message);
        }
    }

    void handle_create_command()
    {
        int parent_id, child_id;
        std::cin >> child_id >> parent_id;
        if (!check_input())
            return;


        if (node_ids_.count(child_id))
        {
            std::cout << "Error: Node with id " << child_id << " already exists" << std::endl;
            return;
        }

        if (!node_ids_.count(parent_id))
        {
            std::cout << "Error: Parent with id " << parent_id << " not found" << std::endl;
            return;
        }

        if (parent_id == -1)
        {
            Node child = create_process(child_id);
            children_.push_back(std::move(child));
            node_ids_.insert(child_id);
            std::cout << "Ok: " << child.pid << std::endl;
        }
        else
        {
            broadcast_message(Message(CommandType::Create, parent_id, child_id));
        }
    }

    void handle_exec_command()
    {
        int id{}, count{};

        int numbers[MESSAGE_VALUE_LENGTH];

        std::cin >> id >> count;

        if (!check_input())
            return;

        if (!node_ids_.count(id))
        {
            std::cout << "Error: Node with id " << id << " doesn't exist" << std::endl;
            return;
        }

        for (int i{0}; i < count; i++)
            std::cin >> numbers[i];

        if (!check_input())
            return;

        broadcast_message(Message(CommandType::Exec, id, count, numbers));
    }

    void handle_pingall_command()
    {
        for (const int& id : node_ids_)
        {
            if (id != -1)
                broadcast_message(Message(CommandType::Ping, id, 0));
        }

        availability_check_started_ = true;
        check_start_time_ = now();
    }

    void check_nodes_availability()
    {
        if (!availability_check_started_)
            return;

        std::ostringstream os;
        os << "Ok: ";
        if (std::chrono::duration_cast<std::chrono::seconds>(now() - check_start_time_).count() > 6)
        {
            for (const int &id: node_ids_)
            {
                if (id != -1)
                {
                    auto it = std::find_if(ping_messages_.begin(), ping_messages_.end(),
                                           [id](const Message &m)
                                           {
                                               return (m.id == id);
                                           });
                    if (it != ping_messages_.end())
                    {
                                if (os.str().size() != 4)
                                    os << ';';
                                os << id;
                                ping_messages_.erase(it);
                    }
                }
            }
            if (os.str().size() == 4)
                os << -1;

            std::cout << os.str() << std::endl;
            availability_check_started_ = false;
        }

    }

public:
    Controller()
    {
        node_ids_.insert(-1);
    }

    void run()
    {
        std::string command;
        while (true)
        {
            for (auto& child : children_)
            {
                Message message = receive_message(child);
                if (message.command != CommandType::None)
                {
                    handle_child_message(message);
                }
            }

            check_pending_messages();

            check_nodes_availability();

            if (!input_available())
            {
                continue;
            }

            std::cin >> command;
            if (command == "create")
            {
                handle_create_command();
            }
            else if (command == "exec")
            {
                handle_exec_command();
            }
            else if (command == "pingall")
            {
                handle_pingall_command();
            }
            else
            {
                std::cout << "Error: Command doesn't exist!" << std::endl;
                std::cin.ignore(1000, '\n');
            }
        }
    }
};

int main()
{
    try
    {
        Controller controller;
        controller.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
