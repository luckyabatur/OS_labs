#include "../include/graph.h"

#include <fstream>
#include <queue>
#include <set>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Graph::loadFromJson(const std::string &filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    json j;
    f >> j;

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        int node = std::stoi(it.key());
        auto children = it.value();

        std::vector<int> vec;
        vec.reserve(children.size());
        for (const auto &ch : children) {
            vec.push_back(ch.get<int>());
        }
        adj[node] = vec;
    }
}


void Graph::validateDAG() const
{
    std::vector<int> nodes = getAllNodes();

    if(nodes.empty())
    {
        throw std::runtime_error("Graph is empty");
    }


    //проверка на циклы
    std::map<int,int> inDeg;
    for (int n : nodes)
    {
        inDeg[n] = 0;
    }


    for (auto &kv : adj)
    {
        for (int c : kv.second)
        {
            inDeg[c]++;
        }
    }

    std::queue<int> q;
    for (auto &p : inDeg)
    {
        if (p.second == 0)
        {
            q.push(p.first);
        }
    }

    int visitedCount = 0;
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        visitedCount++;

        auto it = adj.find(u);
        if (it != adj.end())
        {
            for (int c : it->second)
            {
                inDeg[c]--;
                if (inDeg[c] == 0)
                {
                    q.push(c);
                }
            }
        }
    }

    if (visitedCount != (int)nodes.size())
    {
        throw std::runtime_error("Graph has a cycle => not a DAG.");
    }

    //проверка на компоненту связности
    std::map<int, std::vector<int>> undirected;
    for(int n : nodes)
    {
        undirected[n] = {};
    }

    for(const auto &kv : adj)
    {
        int u = kv.first;
        for(int v : kv.second)
        {
            undirected[u].push_back(v);
            undirected[v].push_back(u);
        }
    }

    int start = *nodes.begin();

    std::set<int> visited;

    std::queue<int> qq;
    visited.insert(start);
    qq.push(start);
    while(!qq.empty()) {
        int u = qq.front();
        qq.pop();
        for(int nei : undirected[u]) {
            if(!visited.count(nei)) {
                visited.insert(nei);
                qq.push(nei);
            }
        }
    }


    if (visited.size() != nodes.size())
    {
        throw std::runtime_error( "Graph has more than one connectivity component");
    }
}


std::vector<int> Graph::findStartNodes() const
{
    std::vector<int> nodes = getAllNodes();

    std::map<int,int> inDeg;
    for (int n : nodes) {
        inDeg[n] = 0;
    }
    for (auto &kv : adj)
    {
        for (int c : kv.second)
        {
            inDeg[c]++;
        }
    }

    std::vector<int> starts;
    for (auto &x : inDeg)
    {
        if (x.second == 0)
        {
            starts.push_back(x.first);
        }
    }
    return starts;
}


std::vector<int> Graph::findEndNodes() const
{
    std::vector<int> nodes = getAllNodes();
    std::vector<int> ends;

    for (int n : nodes)
    {
        auto it = adj.find(n);
        if (it->second.empty())
        {
            ends.push_back(n);
        }
    }
    return ends;
}


std::vector<int> Graph::getAllNodes() const
{
    std::vector<int> out;
    out.reserve(adj.size());
    for (auto &kv : adj)
    {
        out.push_back(kv.first);
    }
    return out;
}
