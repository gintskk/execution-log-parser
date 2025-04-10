#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <fstream>
#include "parser.h"


// New utility function to generate DOT file for graph visualization
std::string generateDotFile(const std::unordered_map<int, std::vector<int>> &graph,
                            const std::vector<int> &cycle = {})
{
    std::stringstream dot;
    dot << "digraph execution {\n";
    dot << "  node [shape=circle];\n";

    // Set for quick lookup if an edge is part of a cycle
    std::set<std::pair<int, int>> cycleEdges;
    if (!cycle.empty())
    {
        for (size_t i = 0; i < cycle.size() - 1; ++i)
        {
            cycleEdges.insert({cycle[i], cycle[i + 1]});
        }
    }

    // Add all edges
    for (const auto &[from, toList] : graph)
    {
        for (int to : toList)
        {
            bool isCycleEdge = cycleEdges.find({from, to}) != cycleEdges.end();
            if (isCycleEdge)
            {
                dot << "  " << from << " -> " << to
                    << " [color=red, penwidth=2.0];\n";
            }
            else
            {
                dot << "  " << from << " -> " << to << ";\n";
            }
        }
    }

    // Highlight nodes that are part of a cycle
    if (!cycle.empty())
    {
        dot << "  {node [style=filled, fillcolor=lightpink] ";
        for (int node : cycle)
        {
            dot << node << " ";
        }
        dot << "}\n";
    }

    dot << "}\n";
    return dot.str();
}

// Function to save DOT file to disk
bool saveDotFile(const std::string &dotContent, int executionNum)
{
    std::string filename = "execution_" + std::to_string(executionNum) + ".dot";
    std::ofstream outFile(filename);

    if (!outFile.is_open())
    {
        std::cerr << "Failed to create DOT file: " << filename << std::endl;
        return false;
    }

    outFile << dotContent;
    outFile.close();
    std::cout << "Graph visualization saved to " << filename << std::endl;
    return true;
}

// Modified cycle detection to return the cycle path
std::vector<int> findCycle(const std::unordered_map<int, std::vector<int>> &graph)
{
    std::set<int> visited;
    std::set<int> recStack;
    std::unordered_map<int, int> parent;

    std::function<bool(int)> dfs = [&](int node) -> bool
    {
        visited.insert(node);
        recStack.insert(node);

        auto it = graph.find(node);
        if (it != graph.end())
        {
            for (int neighbor : it->second)
            {
                if (visited.find(neighbor) == visited.end())
                {
                    parent[neighbor] = node;
                    if (dfs(neighbor))
                        return true;
                }
                else if (recStack.find(neighbor) != recStack.end())
                {
                    // Cycle found
                    parent[neighbor] = node;
                    return true;
                }
            }
        }

        recStack.erase(node);
        return false;
    };

    for (const auto &[node, _] : graph)
    {
        if (visited.find(node) == visited.end())
        {
            parent.clear();
            if (dfs(node))
            {
                // Reconstruct cycle path
                std::vector<int> cycle;
                // Find a node that is in the recursion stack
                int cycleNode = -1;
                for (auto &[n, p] : parent)
                {
                    if (recStack.find(n) != recStack.end())
                    {
                        cycleNode = n;
                        break;
                    }
                }

                if (cycleNode != -1)
                {
                    // Build the cycle
                    int current = cycleNode;
                    do
                    {
                        cycle.push_back(current);
                        current = parent[current];
                    } while (current != cycleNode);

                    cycle.push_back(cycleNode); // Close the cycle
                    std::reverse(cycle.begin(), cycle.end());
                    return cycle;
                }
            }
        }
    }

    return {}; // No cycle found
}

bool isSequentiallyConsistent(const Execution &exec, std::string &dotOutput)
{
    // Filter relevant actions
    std::vector<TraceEntry> filtered;
    for (const auto &entry : exec.executionTrace)
    {
        if (entry.actionType == "atomic read" || entry.actionType == "atomic write" || entry.actionType == "fence")
            filtered.push_back(entry);
    }

    // Build edges
    std::unordered_map<int, std::vector<int>> graph;

    // 1. Program Order (po)
    std::unordered_map<int, std::vector<TraceEntry>> threadActions;
    for (const auto &entry : filtered)
        threadActions[entry.threadId].push_back(entry);

    for (auto &[tid, actions] : threadActions)
    {
        sort(actions.begin(), actions.end(), [](const TraceEntry &a, const TraceEntry &b)
             { return a.id < b.id; });
        for (size_t i = 1; i < actions.size(); ++i)
            graph[actions[i - 1].id].push_back(actions[i].id);
    }

    // 2. Reads-From (rf)
    std::unordered_map<int, TraceEntry> writes; // id -> write action
    for (const auto &entry : filtered)
    {
        if (entry.actionType == "atomic write")
            writes[entry.id] = entry;
    }

    for (const auto &entry : filtered)
    {
        if (entry.actionType == "atomic read" && !entry.rf.empty())
        {
            int rfId = std::stoi(entry.rf);
            if (writes.count(rfId))
                graph[rfId].push_back(entry.id);
        }
    }

    // 3. Modification Order (mo)
    std::unordered_map<std::string, std::vector<TraceEntry>> locationWrites;
    for (const auto &entry : filtered)
    {
        if (entry.actionType == "atomic write")
            locationWrites[entry.location].push_back(entry);
    }

    for (auto &[loc, writes] : locationWrites)
    {
        sort(writes.begin(), writes.end(), [](const TraceEntry &a, const TraceEntry &b)
             { return a.id < b.id; });
        for (size_t i = 1; i < writes.size(); ++i)
            graph[writes[i - 1].id].push_back(writes[i].id);
    }

    // 4. From-Reads (fr)
    for (const auto &readEntry : filtered)
    {
        if (readEntry.actionType != "atomic read" || readEntry.rf.empty())
            continue;
        int rfId = std::stoi(readEntry.rf);
        if (!writes.count(rfId))
            continue;

        TraceEntry &writeEntry = writes[rfId];
        std::string loc = writeEntry.location;
        auto &moWrites = locationWrites[loc];

        // Find all writes after 'writeEntry' in mo order
        bool found = false;
        for (size_t i = 0; i < moWrites.size(); ++i)
        {
            if (moWrites[i].id == rfId)
            {
                found = true;
                for (size_t j = i + 1; j < moWrites.size(); ++j)
                    graph[readEntry.id].push_back(moWrites[j].id);
                break;
            }
        }
    }

    // Find cycle if exists
    std::vector<int> cycle = findCycle(graph);
    bool hasCycle = !cycle.empty();

    // Generate DOT file content
    dotOutput = generateDotFile(graph, cycle);

    return !hasCycle;
}

