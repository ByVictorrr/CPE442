## CPE 442 : Performance of sobel program

## Author(s)
* Victor Delaplaine
* Tristan Chutka

## Profiling C/C++ Applications
* This tutorials is to show how to profile an application. For this task we have chosen the Linux tool `perf`. This tool is called Performance Counters for Linux (PCL), or perf_events. 

This event tool can help us answer some of the questions:
* Why is the kernel on-CPU so much? What code-paths?
* Which code-paths are causing CPU level 2 cache misses?
* Are the CPUs stalled on memory I/O?
* Which code-paths are allocating memory, and how much?
* What is triggering TCP retransmits?
* Is a certain kernel function being called, and how often?
* What reasons are threads leaving the CPU?


![ perf_event event sources ](http://www.brendangregg.com/perf_events/perf_events_map.png)

## Installing libperf
`sudo apt update`
`sudo apt install linux-tools-$(uname -r)`

## How to access a list of performance measures
* Using the `perf stat <program> [args]`
* When we ran `perf` on our sobel program:
    ![performance run](https://i.imgur.com/jre9NPQ.png)


## Reference(s) for `perf`
[ Linux Perf Examples ](http://www.brendangregg.com/perf.html)