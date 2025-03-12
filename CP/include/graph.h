#pragma once

#include <map>
#include <vector>
#include <string>


class Graph
{
public:
    std::map<int, std::vector<int>> adj;

    void loadFromJson(const std::string &filename);

    void validateDAG() const;

    std::vector<int> findStartNodes() const;

    std::vector<int> findEndNodes() const;

    std::vector<int> getAllNodes() const;
};
