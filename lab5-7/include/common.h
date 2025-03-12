#pragma once

#include <chrono>
#include <unistd.h>
#include <iostream>

enum class CommandType  : uint8_t {None, Create, Ping, Exec};

bool input_available();
std::chrono::system_clock::time_point now();