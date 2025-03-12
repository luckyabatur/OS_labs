#include <string>
#include <thread>

#include "../include/message.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        return 1;
    }
    std::string address = argv[1];

    void* ctx = zmq_ctx_new();
    if(!ctx)
    {
        return 1;
    }
    void* sock = zmq_socket(ctx, ZMQ_DEALER);
    if(!sock)
    {
        zmq_ctx_destroy(ctx);
        return 1;
    }
    if(zmq_connect(sock, address.c_str()) != 0)
    {
        zmq_close(sock);
        zmq_ctx_destroy(ctx);
        return 1;
    }

    std::string inputStr;
    if(!recv_string(sock, inputStr))
    {
        zmq_close(sock);
        zmq_ctx_destroy(ctx);
        return 1;
    }
    int val = std::stoi(inputStr);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    int result = val * 2;

    std::string out = std::to_string(result);
    send_string(sock, out);

    zmq_close(sock);
    zmq_ctx_destroy(ctx);
    return 0;
}
