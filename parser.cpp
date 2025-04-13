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
#include <algorithm>
#include <cctype>
using namespace std;
bool isNumeric(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

vector<string> SplitLines(const string &input)
{
    vector<string> lines;
    std::istringstream ss(input);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }
    return lines;
}



vector<Execution> ParseLog(const string &input)
{
    vector<Execution> executions;
    vector<string> lines = SplitLines(input);
    Execution *currentExecution = NULL;
    bool readingProgramOutput = false;
    bool readingTrace = false;
    std::istringstream ss(input);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        if (!line.empty())
        {
            if (line.rfind("Program output from execution", 0) == 0) {
                executions.emplace_back();
                currentExecution = &executions.back();
                currentExecution->executionNumber = executions.size();
                readingProgramOutput = true;
                readingTrace = false;
            } else if (line.rfind("Execution trace", 0) == 0) {
                readingProgramOutput = false;
                readingTrace = true;
            } else if (line.rfind("---- END PROGRAM OUTPUT", 0) == 0) {
                readingProgramOutput = false;
            } else if (line.rfind("HASH", 0) == 0) {
                if (currentExecution != NULL) {
                    istringstream iss(line);
                    string dummy;
                    iss >> dummy >> currentExecution->hash;
                }
            } else if (readingProgramOutput && currentExecution != NULL) {
                currentExecution->programOutput.push_back(line);
            } else if (readingTrace && currentExecution != NULL) {
                std::istringstream iss(line);
                TraceEntry entry;
                //skip empty lines 
                if (line.empty())
                {
                    continue;
                }
                // Parse columns
                iss >> entry.id >> entry.threadId;
    
                // Read the action type (two words: "atomic read")
                std::string actionPart1, actionPart2;
                iss >> actionPart1 >> actionPart2;
                entry.actionType = actionPart1 + " " + actionPart2;
                if (entry.actionType.empty() || std::all_of(entry.actionType.begin(), entry.actionType.end(), 
                [](unsigned char c) { return std::isspace(c); }))
                {
                    continue;
                }
                if (entry.actionType != "atomic read" && entry.actionType != "atomic write" && entry.actionType != "fence" &&   entry.actionType != "atomic rmw")
                {
                    continue;
                }
        
                // Continue parsing the remaining columns
                iss >> entry.memoryOrder >> entry.location >> entry.value;
                 if (entry.actionType == "atomic rmw") {
                    std::string extraField;
                    iss >> extraField; // Skip the extra field (e.g., "()")
                }
                iss >> entry.rf;
                
                // Validate the RF field
                if (!isNumeric(entry.rf)) {
                    entry.rf = ""; // Set RF to an empty string if it's not numeric
                }
                currentExecution->executionTrace.push_back(entry);
    
        }
        }
    }
 

    return executions;
}



