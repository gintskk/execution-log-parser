# ExecutionLogParser
This is a simple parser for the execution logs of the C11Tester tool.
# Checking if execution trace is seqentially consistent.
To test execution traces for their sequential consistency add the trace as .txt file to test_traces/sc_violating_traces or test_traces/sc_traces and run the command ``` ./sc_check < path_to_file...```. In the case of finding if the trace store_store_executionTrace.txt is consistent you can do this by ``` ./sc_checker < test_traces\sc_violating_traces\store_store_executionTrace.txt```

# Running the unit tests for Sequential Consistency checker and the parser.(Google Tests for C++ needed)
Once you have downloaded Google Tests for C++ all you have to do is
``` 
cmake .
ctest
``` 
if you want to run an indiviual test just run the indiviual test executables created.
There are two unit tests. 

The first one ***test_execution_trace_parsing*** check whether the ***parser.cpp*** and the method ***ParseLog*** parses the log correctly for the ***fences.c_without_fences.txt*** correctly. Only one trace file is checked as the whole TraceEntry needs to be initilaised correctly for that trace to compare against what the parser returns.

The second one ***test_sequential_consistency*** checks whether each of the traces in the ***sc_traces*** folder of ***test_traces*** directory are sequentially consistent. Therefore only programs which adhere to this order should be put there. The other thing it does should be to check if the traces in ***sc_violating_traces*** are not seqentially consistent and the program is correctly identifying this. This is an overall test. 

Note: If you put an execution trace that violates sequential consistency for one of its exectuions inside ***sc_traces*** or one that doesn't violate sequential consistency for one of its executions 
inside ***sc_violating_traces*** the second test ***run_sc_consistency*** will fail, so be sure to check this before adding the trace in one of the directory. 