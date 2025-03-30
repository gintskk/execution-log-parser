#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;

struct TraceEntry
{
    int id;
    int threadId;
    string actionType;
    string memoryOrder;
    string location;
    string value;
    string rf;
    string cv;

    string toString() const
    {
        stringstream ss;
        ss << "TraceEntry { "
           << "id: " << id
           << ", threadId: " << threadId
           << ", actionType: " << actionType
           << ", memoryOrder: " << memoryOrder
           << ", location: " << location
           << ", value: " << value
           << ", rf: " << rf
           << ", cv: " << cv
           << " }";
        return ss.str();
    }
};

struct Execution
{
    int executionNumber;
    vector<string> programOutput;
    vector<TraceEntry> executionTrace;
    unsigned int hash;

    string toString() const
    {
        stringstream ss;
        ss << "Execution { "
           << "executionNumber: " << executionNumber
           << ", hash: " << hash
           << ", programOutput: [";
        for (const auto &line : programOutput)
        {
            ss << "\"" << line << "\", ";
        }
        if (!programOutput.empty())
            ss.seekp(-2, ss.cur); // Remove trailing comma and space
        ss << "], executionTrace: [";
        for (const auto &entry : executionTrace)
        {
            ss << "\n  " << entry.toString();
        }
        ss << "\n] }";
        return ss.str();
    }
};

vector<string> SplitLines(const string &input)
{
    vector<string> lines;
    stringstream ss(input);
    string line;
    while (getline(ss, line))
    {
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }
    return lines;
}

string Trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

vector<Execution> ParseLog(const string &input)
{
    vector<Execution> executions;
    vector<string> lines = SplitLines(input);
    Execution *currentExecution = NULL;
    bool readingProgramOutput = false;
    bool readingTrace = false;

    // regex traceRegex(R"(^\s*(\d+)\s+(\d+)\s+([\w\s]+)\s+([\w_]+)\s+([0-9A-Fa-fx]+)\s+([0-9A-Fa-fx]+)\s+([\d\s]*)\s+\(([\d,\s]*)\)$)");

    regex traceRegex("^\\s*(\\d+)\\s+(\\d+)\\s+([a-zA-Z\\s]+?)\\s+([\\w_]+)\\s+([0-9A-Fa-fx]+)\\s+([0-9A-Fa-fx]+)\\s+([\\d\\s]*)\\s+\\(([\\d,\\s]*)\\)$");

    for (const auto &line : lines)
    {
        if (line.find("Program output from execution") == 0)
        {
            executions.emplace_back();
            currentExecution = &executions.back();
            currentExecution->executionNumber = executions.size();
            readingProgramOutput = true;
            readingTrace = false;
        }
        else if (line.find("Execution trace") == 0)
        {
            readingProgramOutput = false;
            readingTrace = true;
        }
        else if (line.find("---- END PROGRAM OUTPUT") == 0)
        {
            readingProgramOutput = false;
        }
        else if (line.find("HASH") == 0)
        {
            if (currentExecution != NULL)
            {
                istringstream iss(line);
                string dummy;
                iss >> dummy >> currentExecution->hash;
            }
        }
        else if (readingProgramOutput && currentExecution != NULL)
        {
            currentExecution->programOutput.push_back(line);
        }
        else if (readingTrace && currentExecution != NULL)
        {
            smatch match;
  
            if (regex_match(line, match, traceRegex))
            {
                TraceEntry entry;
                entry.id = stoi(match[1].str());
                entry.threadId = stoi(match[2].str());
                entry.actionType = Trim(match[3].str());
                entry.memoryOrder = match[4].str();
                entry.location = match[5].str();
                entry.value = match[6].str();
                entry.rf = Trim(match[7].str());
                entry.cv = Trim(match[8].str());
                currentExecution->executionTrace.push_back(entry);
            }
        }
    }

    return executions;
}

string ReadMultiLineInput()
{
    vector<string> lines;
    string line;
    while (getline(cin, line))
    {
        if (line.find("Total executions:") == 0)
        {
            lines.push_back(line);
            break;
        }
        lines.push_back(line);
    }
    string result;
    for (const auto &l : lines)
    {
        result += l + "\n";
    }
    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }
    return result;
}

bool hasCycle(const std::unordered_map<int, std::vector<int>>& graph, int node, std::set<int>& visited, std::set<int>& recStack) {
    if (visited.find(node) == visited.end()) {
        visited.insert(node);
        recStack.insert(node);

        // Check if the node exists in the graph
        auto it = graph.find(node);
        if (it != graph.end()) {
            for (int neighbor : it->second) {
                if (hasCycle(graph, neighbor, visited, recStack) || recStack.count(neighbor)) {
                    return true;
                }
            }
        }

        recStack.erase(node);
    }
    return false;
}

bool isSequentiallyConsistent(const Execution& exec) {

    // Filter relevant actions
    std::vector<TraceEntry> filtered;
    for (const auto& entry : exec.executionTrace) {
        if (entry.actionType == "atomic read" || entry.actionType == "atomic write" || entry.actionType == "fence")
            filtered.push_back(entry);
    }

    // Build edges
    std::unordered_map<int, std::vector<int>> graph;
    
    // 1. Program Order (po)
    std::unordered_map<int, std::vector<TraceEntry>> threadActions;
    for (const auto& entry : filtered)
        threadActions[entry.threadId].push_back(entry);

    for (auto& [tid, actions] : threadActions) {
        sort(actions.begin(), actions.end(), [](const TraceEntry& a, const TraceEntry& b) { return a.id < b.id; });
        for (size_t i = 1; i < actions.size(); ++i)
            graph[actions[i-1].id].push_back(actions[i].id);
    }

    // 2. Reads-From (rf)
    std::unordered_map<int, TraceEntry> writes; // id -> write action
    for (const auto& entry : filtered) {
        if (entry.actionType == "atomic write")
            writes[entry.id] = entry;
    }

    for (const auto& entry : filtered) {
        if (entry.actionType == "atomic read" && !entry.rf.empty()) {
            int rfId = std::stoi(entry.rf);
            if (writes.count(rfId))
                graph[rfId].push_back(entry.id);
        }
    }

    // 3. Modification Order (mo)
    std::unordered_map<std::string, std::vector<TraceEntry>> locationWrites;
    for (const auto& entry : filtered) {
        if (entry.actionType == "atomic write")
            locationWrites[entry.location].push_back(entry);
    }

    for (auto& [loc, writes] : locationWrites) {
        sort(writes.begin(), writes.end(), [](const TraceEntry& a, const TraceEntry& b) { return a.id < b.id; });
        for (size_t i = 1; i < writes.size(); ++i)
            graph[writes[i-1].id].push_back(writes[i].id);
    }

    // 4. From-Reads (fr)
    for (const auto& readEntry : filtered) {
        if (readEntry.actionType != "atomic read" || readEntry.rf.empty()) continue;
        int rfId = std::stoi(readEntry.rf);
        if (!writes.count(rfId)) continue;

        TraceEntry& writeEntry = writes[rfId];
        std::string loc = writeEntry.location;
        auto& moWrites = locationWrites[loc];

        // Find all writes after 'writeEntry' in mo order
        bool found = false;
        for (size_t i = 0; i < moWrites.size(); ++i) {
            if (moWrites[i].id == rfId) {
                found = true;
                for (size_t j = i+1; j < moWrites.size(); ++j)
                    graph[readEntry.id].push_back(moWrites[j].id);
                break;
            }
        }
    }

    // Print all edges in the graph
    // cout << "Edges in the graph:" << endl;
    // for (const auto& [from, toList] : graph) {
    //     for (int to : toList) {
    //         cout << "Edge: " << from << " -> " << to << endl;
    //     }
    // }

    // Check for cycles
    std::set<int> visited, recStack;
    for (const auto& [node, _] : graph) {
        if (hasCycle(graph, node, visited, recStack))
            return false;
    }
    return true;
}

int main()
{
    string input = ReadMultiLineInput();
    vector<Execution> executions = ParseLog(input);
    int i = 1;
    for (const auto &execution : executions)
    {
        bool isConsistent = isSequentiallyConsistent(execution);
        cout << "Execution trace " << i << " is "
             << (isConsistent ? "Sequentially Consistent" : "Not Sequentially Consistent") << endl;
        i++;
    }
    cout << "Parsed " << executions.size() << " executions." << endl;
    return 0;
}