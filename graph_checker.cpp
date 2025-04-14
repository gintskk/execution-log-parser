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
#include <chrono>


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

std::vector<int> findCycle(const std::unordered_map<int, std::vector<int>> &graph) {
    std::set<int> visited;
    std::unordered_map<int, int> parent;

    for (const auto &[start, _] : graph) {
        if (visited.count(start)) continue;

        std::unordered_map<int, bool> inStack;
        std::stack<int> stk;
        stk.push(start);
        parent[start] = -1;

        while (!stk.empty()) {
            int node = stk.top();

            if (!visited.count(node)) {
                visited.insert(node);
                inStack[node] = true;
            } else {
                stk.pop();
                inStack[node] = false;
                continue;
            }
            auto it = graph.find(node);
            if (it == graph.end()) {
                stk.pop();
                continue;
            }
            for (int neighbor : it ->second) {
                if (!visited.count(neighbor)) {
                    stk.push(neighbor);
                    parent[neighbor] = node;
                } else if (inStack[neighbor]) {
                    std::vector<int> cycle;
                    int current = node;
                    while (current != neighbor && current != -1) {
                        cycle.push_back(current);
                        current = parent[current];
                    }
                    cycle.push_back(neighbor);
                    cycle.push_back(node); 
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
    auto start_time = std::chrono::high_resolution_clock::now();

    // Separate vectors for atomic reads, writes, and rmws
    std::vector<TraceEntry> atomicReads;
    std::vector<TraceEntry> atomicWrites;
    std::vector<TraceEntry> atomicRMWs;
    std::unordered_map<int, std::vector<TraceEntry>> threadActions;

    auto start = std::chrono::high_resolution_clock::now();
        // Categorize entries
    for (const auto &entry : exec.executionTrace) {
        threadActions[entry.threadId].push_back(entry);
        if (entry.actionType == "atomic read") {
            atomicReads.push_back(entry);
        } else if (entry.actionType == "atomic write") {
            atomicWrites.push_back(entry);
        } else if (entry.actionType == "atomic rmw") {
            atomicRMWs.push_back(entry);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken to categorize entries: " << elapsed.count() << " seconds" << std::endl;

    std::unordered_map<int, std::vector<int>> graph;
    graph.reserve(exec.executionTrace.size());
    start = std::chrono::high_resolution_clock::now();

    // 1. Program Order (po)
    for (auto &[tid, actions] : threadActions)
    {
        for (size_t i = 1; i < actions.size(); ++i)
            graph[actions[i - 1].id].push_back(actions[i].id);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time taken to build PO edges: " << elapsed.count() << " seconds" << std::endl;
    
    // 2. Reads-From (rf)
    start = std::chrono::high_resolution_clock::now();
    std::unordered_map<int, TraceEntry> writes;
    for (const auto &entry : atomicWrites) {
        writes[entry.id] = entry;
    }
    for (const auto &entry : atomicRMWs) {
        writes[entry.id] = entry;
    }

    for (const auto &entry : atomicReads) {
        if (!entry.rf.empty() && writes.count(std::stoi(entry.rf))) {
            graph[std::stoi(entry.rf)].push_back(entry.id);
        }
    }
    for (const auto &entry : atomicRMWs) {
        if (!entry.rf.empty() && writes.count(std::stoi(entry.rf))) {
            graph[std::stoi(entry.rf)].push_back(entry.id);
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time taken to build RF edges: " << elapsed.count() << " seconds" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    
    std::unordered_map<int, size_t> lastWriteIndex; 
    std::unordered_map<std::string, int> writeSize;
    for (auto &entry : atomicWrites) {
        writeSize[entry.location] = 0;
    }
    std::unordered_map<std::string, std::vector<TraceEntry>> locationWrites;
    // 3. Modification Order (mo)

    for (const auto &entry : atomicWrites) {
        auto &writes = locationWrites[entry.location];
        writes.push_back(entry);
        writeSize[entry.location] += 1;
        lastWriteIndex[entry.id] = writeSize[entry.location] - 1; // Update the last write index
    }

    for (auto &entry : atomicRMWs) {
        auto &writes = locationWrites[entry.location];
        auto it = std::lower_bound(writes.begin(), writes.end(), entry, [](const TraceEntry &a, const TraceEntry &b) {
            return a.id < b.id;
        });
        size_t index = std::distance(writes.begin(), it);
        writes.insert(it, entry);
        lastWriteIndex[entry.id] = index; // Update the last write index
    }


    for (auto &[loc, writes] : locationWrites) {
        for (size_t i = 1; i < writes.size(); ++i) {
            graph[writes[i - 1].id].push_back(writes[i].id);
        }
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time taken to build MO edges: " << elapsed.count() << " seconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
  
    // 4. From-Reads (fr)
    for (const auto &readEntry : atomicReads) {
        if (readEntry.rf.empty())
            continue;

        int rfId = std::stoi(readEntry.rf);
        if (!writes.count(rfId))
            continue;

        auto &moWrites = locationWrites[readEntry.location];

        size_t startIndex = lastWriteIndex[rfId];
        for (size_t i = startIndex + 1; i < moWrites.size(); ++i) {
            if (moWrites[i].id > readEntry.id)
                break;

            graph[readEntry.id].push_back(moWrites[i].id);
        }
    }

    for (const auto &rmwEntry : atomicRMWs) {
        if (rmwEntry.rf.empty())
            continue;

        int rfId = std::stoi(rmwEntry.rf);
        if (!writes.count(rfId))
            continue;

        auto &moWrites = locationWrites[rmwEntry.location];

        size_t startIndex = lastWriteIndex[rfId];
        for (size_t i = startIndex + 1; i < moWrites.size(); ++i) {
            if (moWrites[i].id > rmwEntry.id)
                break;

            graph[rmwEntry.id].push_back(moWrites[i].id);
        }
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time taken to build FR edges: " << elapsed.count() << " seconds" << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    elapsed = end_time - start_time;
    std::cout << "Time taken to build graph: " << elapsed.count() << " seconds" << std::endl;

    // Find cycle if exists
    std::vector<int> cycle = findCycle(graph);
    bool hasCycle = !cycle.empty();

    // Generate DOT file content
    dotOutput = generateDotFile(graph, cycle);

    return !hasCycle;
}

