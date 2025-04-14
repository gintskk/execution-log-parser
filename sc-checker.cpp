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
#include "graph_checker.h"
#include "grf.hpp"
#include <chrono>
using namespace std;
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

int main(){
    auto start_time = std::chrono::high_resolution_clock::now();
    string input = ReadMultiLineInput();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Time taken to read input: " << elapsed.count() << " seconds" << std::endl;
    auto start_time2 = std::chrono::high_resolution_clock::now();
    vector<Execution> executions = ParseLog(input);
    auto end_time2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end_time2 - start_time2;
    std::cout << "Time taken to parse log: " << elapsed2.count() << " seconds" << std::endl;

    int i = 1;

    for (const auto &execution : executions)
    {
        std::string dotOutput;
        start_time = std::chrono::high_resolution_clock::now();
        bool isConsistent = isSequentiallyConsistent(execution, dotOutput);
        end_time = std::chrono::high_resolution_clock::now();
        elapsed = end_time - start_time;
        std::cout << "Time taken to check consistency: " << elapsed.count() << " seconds" << std::endl;
        cout << "Execution trace " << i << " is "
             << (isConsistent ? "Sequentially Consistent" : "Not Sequentially Consistent") << endl;

       saveDotFile(dotOutput, i);
        i++;
    }
}
