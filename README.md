# ExecutionLogParser
This is a simple parser for the execution logs of the C11Tester tool.
# Testing execution Traces Test
To test execution traces for their sequential consistency add the trace as .txt file to test_traces directory and then simply run ./test.sh and it will test each execution trace for sequential consistency. If you want to run a specific trace file then provide the filename as an argument. 

```bash 
./test.sh [filename]
 ```