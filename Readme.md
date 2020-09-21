It is a simulation for testing the performance of the wa-bplustree on PCM(Phase-change memory).

The wa-bplustree is a variant bplustree using a wear leveling concept to reduce the overhead of total memory writes. 

Trace is a directory of benchmarks.

compile:
    make

execute:
    make run
    
STEPS:

    1. Reset MEMORY_SIZE and DATA_PATH in Makefile
    2. compile and execute
