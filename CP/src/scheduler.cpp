#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>

#include <zmq.h>

#include "../include/graph.h"
#include "../include/message.h"


struct NodeInfo
{
    bool started = false;
    pid_t pid = -1;
    void* sock = nullptr;
};

static std::vector<pid_t> g_allPids;


static void failAndExit(const std::string &msg, void* context)
{
    std::cerr << "ERROR: " << msg << "\n";

    for (pid_t pid : g_allPids)
    {
        kill(pid, SIGTERM);
    }
    g_allPids.clear();

    if (context)
    {
        zmq_ctx_destroy(context);
    }
    exit(1);
}


int main(int argc, char* argv[])
{
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dag_json_file>\n";
        return 1;
    }
    std::string dagFile = argv[1];

    Graph graph;

    try
    {
        graph.loadFromJson(dagFile);
        graph.validateDAG();
    }
    catch(const std::exception &ex)
    {
        std::cerr << "DAG Error: " << ex.what() << "\n";
        return 1;
    }

    auto startNodes = graph.findStartNodes();
    auto endNodes   = graph.findEndNodes();
    auto allNodes   = graph.getAllNodes();

    std::unordered_map<int,int> parentCount;
    std::unordered_map<int,int> results;
    for (int n : allNodes)
    {
        parentCount[n] = 0;
        results[n] = 0;
    }

    for (auto &kv : graph.adj)
    {
        for (int c : kv.second)
        {
            parentCount[c]++;
        }
    }

    for (int s : startNodes)
    {
        results[s] = 1;
    }

    void* context = zmq_ctx_new();
    if (!context)
    {
        std::cerr << "zmq_ctx_new failed\n";
        return 1;
    }


    std::unordered_map<int, NodeInfo> info;
    for (int n : allNodes)
    {
        info[n] = NodeInfo{};
    }

    auto launchReadyNodes = [&]()
    {
        for (int n : allNodes)
        {
            if (!info[n].started && parentCount[n] == 0)
            {
                std::string addr = "tcp://127.0.0.1:" + std::to_string(5555 + n);

                void* sock = zmq_socket(context, ZMQ_DEALER);
                if (!sock)
                {
                    failAndExit("zmq_socket failed for node " + std::to_string(n), context);
                }

                if (zmq_bind(sock, addr.c_str()) != 0)
                {
                    zmq_close(sock);
                    failAndExit("zmq_bind(" + addr + ") failed", context);
                }

                pid_t pid = fork();
                if (pid < 0)
                {
                    zmq_close(sock);
                    failAndExit("fork() failed for node " + std::to_string(n), context);
                }
                if (pid == 0)
                {
                    char* args[3];
                    args[0] = (char*)"./job";
                    args[1] = (char*)addr.c_str();
                    args[2] = nullptr;

                    execvp(args[0], args);
                    exit(1);
                }

                g_allPids.push_back(pid);
                info[n].started = true;
                info[n].pid = pid;
                info[n].sock = sock;

                int val = results[n];
                std::cout << "Launch job " << n << " (pid=" << pid << "), input=" << val << "\n";

                if (!send_string(sock, std::to_string(val)))
                {
                    failAndExit("fail to send input to node " + std::to_string(n), context);
                }
            }
        }
    };

    launchReadyNodes();

    int finishedEndCount = 0;
    int totalEnds = (int)endNodes.size();
    bool done = false;

    while (!done)
    {
        int status = 0;
        pid_t w = wait(&status);
        if (w == -1)
        {
            failAndExit("wait() failed", context);
        }

        int nodeFinished = -1;
        for (int n : allNodes)
        {
            if (info[n].pid == w)
            {
                nodeFinished = n;
                break;
            }
        }

        if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
        {
            failAndExit("Node " + std::to_string(nodeFinished) +
                        " (pid=" + std::to_string(w) + ") exited with error", context);
        }

        std::string rstr;
        if (!recv_string(info[nodeFinished].sock, rstr)) {
            failAndExit("Cannot read result from node " + std::to_string(nodeFinished), context);
        }
        int val = std::stoi(rstr);

        zmq_close(info[nodeFinished].sock);
        info[nodeFinished].sock = nullptr;

        std::cout << "Node " << nodeFinished
                  << " ended, result=" << val << "\n";

        if (std::find(endNodes.begin(), endNodes.end(), nodeFinished) != endNodes.end())
        {
            std::cout << "End node " << nodeFinished
                      << " => final result = " << val << "\n";
            finishedEndCount++;
            if (finishedEndCount == totalEnds)
            {
                done = true;
            }
        }

        for (int child : graph.adj[nodeFinished])
        {
            parentCount[child]--;
            results[child] += val;
        }

        if (!done)
        {
            launchReadyNodes();
        }
    }
    std::cout << "All end nodes finished.\n";

    zmq_ctx_destroy(context);
    return 0;
}