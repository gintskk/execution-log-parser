#ifndef GRAPH_CHECKER_H
#define GRAPH_CHECKER_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <fstream>

bool isSequentiallyConsistent(const Execution &exec, std::string &dotOutput);
bool saveDotFile(const std::string &dotContent, int executionNum);

#endif