#pragma once

#include "zmq.h"
#include "message.h"
#include <iostream>
#include <chrono>
#include <list>
#include <map>


class Node
{
public:
    Node() = default;
    ~Node() {
        if (socket) zmq_close(socket);
        if (context) zmq_ctx_destroy(context);
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&& other) noexcept;
    Node& operator=(Node&& other) noexcept;

//    bool operator==(const Node& other) const
//    {
//        return id == other.id && pid == other.pid;
//    }

    int id{-1};
    pid_t pid{-1};
    void* context{nullptr};
    void* socket{nullptr};
    std::string address;
};

Node create_node(int id, bool is_child);
Node create_process(int id);

class ComputingNode {
private:
    Node node_;
    std::list<Node> children_;

    void handle_create(const Message& message);
    void handle_ping(const Message& message);
    void handle_exec(Message& message);
    void broadcast_to_children(const Message& message);

public:
    explicit ComputingNode(int id)
        : node_(create_node(id, true)) {}

    void run();
};


void send_message(Node& node, const Message& msg);
Message receive_message(Node& node);

