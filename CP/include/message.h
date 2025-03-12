#pragma once

#include <string>
#include <zmq.h>

bool send_string(void* socket, const std::string &s);
bool recv_string(void* socket, std::string &s);

