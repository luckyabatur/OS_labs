#include "../include/message.h"
#include <cstring>

bool send_string(void* socket, const std::string &s)
{
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, s.size());
    memcpy(zmq_msg_data(&msg), s.data(), s.size());
    int rc = zmq_msg_send(&msg, socket, 0);
    zmq_msg_close(&msg);
    return (rc != -1);
}

bool recv_string(void* socket, std::string &s)
{
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    int rc = zmq_msg_recv(&msg, socket, 0);
    if(rc == -1)
    {
        zmq_msg_close(&msg);
        return false;
    }
    s.assign((char*)zmq_msg_data(&msg), zmq_msg_size(&msg));
    zmq_msg_close(&msg);
    return true;
}
