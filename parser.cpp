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

using namespace std;

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



