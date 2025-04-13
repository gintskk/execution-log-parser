#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include <filesystem>

#include "../parser.h" 
using namespace std;

// Execution trace 1
vector<TraceEntry> trace1 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2,  10)"},
    {12, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  12)"}
};

// Execution trace 2
vector<TraceEntry> trace2 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  10)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2,  12)"}
};

// Execution trace 3
vector<TraceEntry> trace3 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2,  10)"},
    {12, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  13)"},
    {17, 7, "atomic rmw", "seq_cst", "0000000000611C00", "0x0", "717142", "( 0, 717332, 716953, 8945, 717136, 717313, 717301, 717385)"}
};

// Execution trace 4
vector<TraceEntry> trace4 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2,  10)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  13)"}
};

// Execution trace 5
vector<TraceEntry> trace5 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0,  10)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2,  12)"}
};

// Execution trace 6
vector<TraceEntry> trace6 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2,  10)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0,  13)"}
};

// Execution trace 7
vector<TraceEntry> trace7 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0,  10)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2,  12)"}
};

// Execution trace 8
vector<TraceEntry> trace8 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2,  10)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  13)"}
};

// Execution trace 9
vector<TraceEntry> trace9 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  10)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2,  12)"}
};

// Execution trace 10
vector<TraceEntry> trace10 = {
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,   6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,   7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2,  10)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0,  13)"}
};

class ParseLogFunctionalTest : public ::testing::Test {
protected:
    vector<vector<TraceEntry>> allTraces;

    void SetUp() override {
        allTraces.push_back(trace1);
        allTraces.push_back(trace2);
        allTraces.push_back(trace3);
        allTraces.push_back(trace4);
        allTraces.push_back(trace5);
        allTraces.push_back(trace6);
        allTraces.push_back(trace7);
        allTraces.push_back(trace8);
        allTraces.push_back(trace9);
        allTraces.push_back(trace10);
    }


    std::string readFileContent(const std::string& filePath) {
        std::ifstream file(filePath);
        std::cout << "pwd:" << std::filesystem::current_path() << std::endl;
        EXPECT_TRUE(file.is_open()) << "Failed to open file: " << filePath;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

};
    
TEST_F(ParseLogFunctionalTest, ParseLogFromFile) {

    string filePath = "test_traces/sc_violating_traces/fences.c_without_fences.txt";
    
    string fileContent = readFileContent(filePath);
    ASSERT_FALSE(fileContent.empty()) << "File is empty or could not be read";
    
    vector<Execution> executions = ParseLog(fileContent);
    for (const auto& execution : executions) {
        for (const auto& entry : execution.executionTrace) {
            std::cout << "  Trace Entry ID: " << entry.id << ", Thread ID: " << entry.threadId
                      << ", Action Type: " << entry.actionType << ", Memory Order: " << entry.memoryOrder
                      << ", Location: " << entry.location << ", Value: " << entry.value
                      << ", RF: " << entry.rf << std::endl;
        }
    }
    ASSERT_EQ(executions.size(), allTraces.size()) << "Number of executions does not match expected";
    
    for (size_t i = 0; i < executions.size(); ++i) {
        SCOPED_TRACE("Checking execution " + std::to_string(i + 1));
        
        ASSERT_EQ(executions[i].executionTrace.size(), allTraces[i].size()) 
            << "Trace size mismatch for execution " << i + 1;
        
        for (size_t j = 0; j < executions[i].executionTrace.size(); ++j) {
            SCOPED_TRACE("Checking trace entry " + std::to_string(j + 1));
            
            EXPECT_EQ(executions[i].executionTrace[j].id, allTraces[i][j].id)
                << "ID mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].threadId, allTraces[i][j].threadId)
                << "Thread ID mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].actionType, allTraces[i][j].actionType)
                << "Action type mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].memoryOrder, allTraces[i][j].memoryOrder)
                << "Memory order mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].location, allTraces[i][j].location)
                << "Location mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].value, allTraces[i][j].value)
                << "Value mismatch at execution " << i + 1 << ", entry " << j + 1;
                
            EXPECT_EQ(executions[i].executionTrace[j].rf, allTraces[i][j].rf)
                << "RF mismatch at execution " << i + 1 << ", entry " << j + 1;
                
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}