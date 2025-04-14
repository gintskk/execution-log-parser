# ExecutionLogParser
This is a simple parser for the execution logs of the C11Tester tool.
# Checking if execution trace is seqentially consistent.
Before running you need to compile. The command to compile is 

```bash 
g++ sc-checker.cpp parser.cpp graph_checker.cpp -o sc_check
```

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

# Instructions to run the benchmarks in the report.
The way to run the benchmarks in the report is to first clone this repo inside the c11tester docker container.
Before running any of the bench run these two command to configure C11tester to produce test output 

```bash 
export C11TESTER='--verbose=2'
export LD_LIBRARY_PATH="/home/c11tester/c11tester:../src"
```

1. Iris
The Iris benchmark can be done using the following command
```bash
 time ./c11tester-benchmarks/iris/test_lfringbuffer |pv | /home/c11tester/execution-log-parser/sc_check
 ```
 Note that the execution trace producded shoudnt be greater than 615-620 MB. If it then you have to chnage the max max_iterations definition inide c11tester-benchmarks/iris/tests/test_lfringbuffer.cpp

 ```cpp
 #define ITERATIONS (int) 100000
 ```
then 
```bash
cd ..
make test_lfringbuffer
```
and then run the test again from the root dir.
2. Silo DB
To run the silo DB test run the following from root directory.

```bash
./c11tester-benchmarks/silo/out-perf.debug.check.masstree/benchmarks/dbtest -n 1800 -t 4 | pv| /home/c
11tester/execution-log-parser/sc_check
```
The trace should be between 580-680 MB.

3. Mabain
To run the Mabain test run the following command
```bash
cd c11tester-benchmarks/mabain/example
rm ./multi_test/* 2> /dev/null
/mb_multi_thread_insert_test | pv | /home/c11tester/execution-log-parser/sc_check
```
The execution trace should be roughly 390 MB. If it is large then change the max key field of the mb_multi_thread_insert_test.cpp file.
```cpp
static int max_key = 10000;
```
then 
```bash 
make
```
run the test again.


For each of the tests additionally 
 /usr/bin/time can be added in front on the /sc_check executable to test get the memeory taken data.

