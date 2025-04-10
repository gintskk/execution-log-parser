
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

// Execution trace 1a
vector<TraceEntry>  trace1 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 12)"},
    {13, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 11)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 13)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 13)"}
};

// Execution trace 2
vector<TraceEntry> trace2 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 10)"},
    {11, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 11)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2, 12)"},
    {13, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 13)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 13, 11)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 13, 11)"}
};

// Execution trace 3
vector<TraceEntry> trace3 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 12)"},
    {13, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 11)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 13)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 13)"}
};

// Execution trace 4
vector<TraceEntry> trace4 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 12, 11)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 13)"},
    {14, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 14)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 14)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 14)"}
};

// Execution trace 5
vector<TraceEntry> trace5 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0, 10)"},
    {11, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 11)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2, 12)"},
    {13, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 13)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 13, 11)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 13, 11)"}
};

// Execution trace 6
vector<TraceEntry> trace6 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 12, 11)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0, 13)"},
    {14, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 14)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 14)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 14)"}
};

// Execution trace 7
vector<TraceEntry> trace7 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "0,  1"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x2", "7", "( 0,  4,  0, 10)"},
    {11, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 11)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x2", "9", "( 0,  2, 12)"},
    {13, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 13)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 13, 11)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 13, 11)"}
};

// Execution trace 8
vector<TraceEntry> trace8 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 12, 11)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 13)"},
    {14, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 14)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 14)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 14)"}
};

// Execution trace 9
vector<TraceEntry> trace9 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 10)"},
    {11, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 11)"},
    {12, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2, 12)"},
    {13, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 13)"},
    {14, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 14, 13)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 13, 11)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 13, 11)"}
};

// Execution trace 10
vector<TraceEntry> trace10 = {
    {1, 1, "thread start", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0,  1)"},
    {2, 1, "thread create", "seq_cst", "00007FFEA5873F68", "0x7ffea5873ef0", "", "( 0,  2)"},
    {3, 2, "thread start", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2,  3)"},
    {4, 1, "thread create", "seq_cst", "00007FFEA5873F60", "0x7ffea5873ef0", "", "( 0,  4)"},
    {5, 3, "thread start", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0,  5)"},
    {6, 2, "atomic write", "relaxed", "000000000040406C", "0x1", "", "( 0,  2,  6)"},
    {7, 2, "atomic write", "relaxed", "000000000040406C", "0x2", "", "( 0,  2,  7)"},
    {8, 3, "atomic write", "relaxed", "0000000000404070", "0x1", "", "( 0,  4,  0,  8)"},
    {9, 3, "atomic write", "relaxed", "0000000000404070", "0x2", "", "( 0,  4,  0,  9)"},
    {10, 2, "atomic read", "relaxed", "0000000000404070", "0x1", "8", "( 0,  2, 10)"},
    {11, 2, "thread finish", "seq_cst", "00007F0904C71A98", "0xdeadbeef", "", "( 0,  2, 11)"},
    {12, 1, "thread join", "seq_cst", "00007F0904C71A98", "0x2", "", "( 0, 12, 11)"},
    {13, 3, "atomic read", "relaxed", "000000000040406C", "0x1", "6", "( 0,  4,  0, 13)"},
    {14, 3, "thread finish", "seq_cst", "00007F0904E73040", "0xdeadbeef", "", "( 0,  4,  0, 14)"},
    {15, 1, "thread join", "seq_cst", "00007F0904E73040", "0x3", "", "( 0, 15, 11, 14)"},
    {16, 1, "thread finish", "seq_cst", "00007F09034F0110", "0xdeadbeef", "", "( 0, 16, 11, 14)"}
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