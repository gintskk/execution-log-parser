#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stdexcept>
#include <iostream>

template <typename T>
class Graph
{
private:
    bool directed;
    std::unordered_map<T, std::unordered_set<T> > adjacencyList;

    enum Color
    {
        WHITE,
        GRAY,
        BLACK
    };

    bool hasCycleDFS(const T &vertex, std::unordered_map<T, Color> &colors)
    {
        colors[vertex] = GRAY;

        for (const T &neighbor : adjacencyList[vertex])
        {
            if (colors[neighbor] == GRAY)
            {
                return true;
            }
            if (colors[neighbor] == WHITE && hasCycleDFS(neighbor, colors))
            {
                return true;
            }
        }

        colors[vertex] = BLACK;
        return false;
    }

public:
    Graph(bool isDirected = false) : directed(isDirected) {}

    void addVertex(const T &vertex)
    {
        if (adjacencyList.find(vertex) == adjacencyList.end())
        {
            adjacencyList[vertex] = std::unordered_set<T>();
        }
    }

    void addEdge(const T &from, const T &to)
    {
        if (adjacencyList.find(from) == adjacencyList.end() ||
            adjacencyList.find(to) == adjacencyList.end())
        {
            throw std::invalid_argument("Vertex not found");
        }

        adjacencyList[from].insert(to);
        if (!directed)
        {
            adjacencyList[to].insert(from);
        }
    }

    void removeVertex(const T &vertex)
    {
        if (adjacencyList.find(vertex) == adjacencyList.end())
        {
            return;
        }

        // Remove all edges pointing to this vertex
        for (auto &pair : adjacencyList)
        {
            pair.second.erase(vertex);
        }

        // Remove the vertex and its edges
        adjacencyList.erase(vertex);
    }

    void removeEdge(const T &from, const T &to)
    {
        if (adjacencyList.find(from) == adjacencyList.end() ||
            adjacencyList.find(to) == adjacencyList.end())
        {
            return;
        }

        adjacencyList[from].erase(to);
        if (!directed)
        {
            adjacencyList[to].erase(from);
        }
    }

    std::vector<T> bfs(const T &start)
    {
        std::vector<T> result;
        std::unordered_set<T> visited;
        std::queue<T> queue;

        queue.push(start);
        visited.insert(start);

        while (!queue.empty())
        {
            T current = queue.front();
            queue.pop();
            result.push_back(current);

            for (const T &neighbor : adjacencyList[current])
            {
                if (visited.find(neighbor) == visited.end())
                {
                    visited.insert(neighbor);
                    queue.push(neighbor);
                }
            }
        }

        return result;
    }

    std::vector<T> dfs(const T &start)
    {
        std::vector<T> result;
        std::unordered_set<T> visited;
        dfsUtil(start, visited, result);
        return result;
    }

    bool hasCycle()
    {
        std::unordered_map<T, Color> colors;
        for (const auto &pair : adjacencyList)
        {
            colors[pair.first] = WHITE;
        }

        for (const auto &pair : adjacencyList)
        {
            if (colors[pair.first] == WHITE)
            {
                if (hasCycleDFS(pair.first, colors))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void print()
    {
        for (const auto &pair : adjacencyList)
        {
            std::cout << pair.first << " -> ";
            for (const T &neighbor : pair.second)
            {
                std::cout << neighbor << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    void dfsUtil(const T &vertex, std::unordered_set<T> &visited, std::vector<T> &result)
    {
        visited.insert(vertex);
        result.push_back(vertex);

        for (const T &neighbor : adjacencyList[vertex])
        {
            if (visited.find(neighbor) == visited.end())
            {
                dfsUtil(neighbor, visited, result);
            }
        }
    }
};

#endif
