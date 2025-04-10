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
    string input = ReadMultiLineInput();
    vector<Execution> executions = ParseLog(input);
    int i = 1;
    for (const auto &execution : executions)
    {
        std::string dotOutput;
        bool isConsistent = isSequentiallyConsistent(execution, dotOutput);
        cout << "Execution trace " << i << " is "
             << (isConsistent ? "Sequentially Consistent" : "Not Sequentially Consistent") << endl;

        saveDotFile(dotOutput, i);
        i++;
    }
}