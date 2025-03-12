#pragma once

#include <cstring>
#include "common.h"

#define MESSAGE_VALUE_LENGTH 50

class Message
{
public:
    Message() = default;

    Message(CommandType cmd, int id, int add_data)
        : command(cmd)
        , id(id)
        , add_data(add_data)
        , sent_time(std::chrono::system_clock::now())
    {}

    Message(CommandType cmd, int id, int add_data, const int* arr)
        : command(cmd)
        , id(id)
        , add_data(add_data)
        , sent_time(std::chrono::system_clock::now())
    {

        for (int i{0}; i < add_data; i++)
            numbers[i] = arr[i];
    }

    CommandType command{CommandType::None};
    int id{-1};
    int add_data{-1};
    std::chrono::system_clock::time_point sent_time;
    int numbers[MESSAGE_VALUE_LENGTH]{};
};

