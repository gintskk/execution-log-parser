#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <sstream>

// Represents a single memory action in the trace
struct TraceEntry
{
    int id;
    int threadId;
    std::string actionType;
    std::string memoryOrder;
    std::string location;
    std::string value;
    std::string rf;
    std::string cv;

    std::string toString() const
    {
        std::stringstream ss;
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
    std::vector<TraceEntry> executionTrace;
    unsigned int hash = 0;

    std::string toString() const
    {
        std::stringstream ss;
        ss << "Execution { "
           << "executionNumber: " << executionNumber
           << ", hash: " << hash  << "}\n";
           
        return ss.str();
    }
};

// Parses raw log input and returns a list of executions
std::vector<Execution> ParseLog(const std::string &input);

#endif // PARSER_H
