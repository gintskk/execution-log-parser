#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include <filesystem>

#include "../parser.h"
#include "../graph_checker.h"
#include "../grf.hpp"

using namespace std;
namespace fs = std::filesystem;

class SequentialConsistencyTest : public ::testing::Test {
protected:
    vector<string> sc_violating_files;
    vector<string> sc_traces;

    void SetUp() override {
        read_traces_path("test_traces/sc_violating_traces", sc_violating_files);
        read_traces_path("test_traces/sc_traces", sc_traces);
    }

    void read_traces_path(const string& directoryPath, vector<string>& traces) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                string filePath = entry.path().string();
                traces.push_back(filePath); 
            }
        }
    }

    string readFileContent(const string& filePath) {
        ifstream file(filePath);
        cout << "pwd:" << fs::current_path() << endl;
        EXPECT_TRUE(file.is_open()) << "Failed to open file: " << filePath;

        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

TEST_F(SequentialConsistencyTest, test_seq_consistent) {
    for (auto& trace : sc_violating_files) {
    
        string fileContent = readFileContent(trace);
        vector<Execution> executions = ParseLog(fileContent);
        bool Consistent = true;
        for (auto& execution : executions) {
            std::string dotOutput;
            bool isConsistent = isSequentiallyConsistent(execution, dotOutput);
            Consistent = Consistent && isConsistent;
        }
        EXPECT_FALSE(Consistent) << "Execution trace " << trace << " should not be sequentially inconsistent.";
    
    }
    for (auto& trace : sc_traces) {

        string fileContent = readFileContent(trace);
        vector<Execution> executions = ParseLog(fileContent);
        bool Consistent = true;
        for (auto& execution : executions) {
            std::string dotOutput;
            bool isConsistent = isSequentiallyConsistent(execution, dotOutput);
            Consistent = Consistent && isConsistent;
        }

        EXPECT_TRUE(Consistent) << "Execution trace " << trace << " should be sequentially consistent.";
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}