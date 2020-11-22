## CPE 442 : Performance of sobel program

## Author(s)
* Victor Delaplaine
* Tristan Chutka


# functions
`struct libperf_data * libperf_initialize(int pid, int cpu)`
    * `Description`: This function initalizes the library
    * `pid`: Can pass in `gettid()`/`getpid()` value, -1 for current process 
    * `cpu`: 
























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

## Usage
* To list all currently known events for your architecture run:
    `perf list`
![ perf list ](https://i.imgur.com/l6ODbDq.png)

**The above case shows me running the `perf list` command on a raspberry pi 4**

The idea of perf events are to use a counter to see how many of those events occur. You can also see the architecure specific counters (armv_* events).

## Events
* perf_events are ways to get statistics of the events, by using internal counters for each event.
* The `perf stat -e <event1, event2, .., eventn> <command>` gives you a print out of the hardware counters for each event.
    ![performance run](https://i.imgur.com/7LB4JPH.png)

**The above shows you some basic events it give you by default**

### Cache Events
* For our case we may want to find the number of:
    * L1 data cache misses so:
    ![ L1 cache misses ](https://i.imgur.com/D3vYDQw.png)
    * L2 data cache misses, but is the L2 Cache the Last Level Cache (LLC) (Look below)
    ![ L2 cache misses ](https://i.imgur.com/fBOFnPA.png)

    ![ Cortex-A53 ](https://images.anandtech.com/doci/7591/Cortex-A53-large_678x452.png)
    * This pictures of the `Cortex-A53` architecture helps indeicate that L2 is the LLC

### Architecture specific events
* Say we want to count the number of cpu cycles it takes to run your program:
    ![cpu cycles](https://i.imgur.com/PizsN16.png)

* I found the architecture specific event in the `perf list` table
    * Specific to the armv7_cortex_a7

## Reference(s) for `perf`
[ Linux Perf Examples ](http://www.brendangregg.com/perf.html)