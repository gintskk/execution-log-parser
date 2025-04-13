#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm> // For std::all_of

using namespace std;

bool isNumeric(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

struct TraceEntry {
    int id;
    int threadId;
    string actionType;
    string memoryOrder;
    string location;
    string value;
    string rf; // RF can be numeric or empty
};

int main() {
    string trace = "717385 7    atomic rmw      seq_cst  0000000000611C00   0                 (1)  717142 ( 0, 717332, 716953, 8945, 717136, 717313, 717301, 717385)";
    std::istringstream iss(trace);
    TraceEntry entry;

    // Parse columns
    iss >> entry.id >> entry.threadId;

    // Read the action type (two words: "atomic read")
    std::string actionPart1, actionPart2;
    iss >> actionPart1 >> actionPart2;
    entry.actionType = actionPart1 + " " + actionPart2;

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

    // Output the parsed fields
    cout << "ID: " << entry.id << endl;
    cout << "Thread ID: " << entry.threadId << endl;
    cout << "Action Type: " << entry.actionType << endl;
    cout << "Memory Order: " << entry.memoryOrder << endl;
    cout << "Location: " << entry.location << endl;
    cout << "Value: " << entry.value << endl;
    cout << "RF: " << entry.rf << endl;

    return 0;
}