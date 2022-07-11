# Modified-xv6

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6) which loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

## Syscall Tracing

```bash
strace mask [command] [args]
```
* strace runs the specified command until it exits. It intercepts and records the system calls which are called by a process during its execution.  
* It takes one argument, an integer `mask`, whose bits specify which system calls to trace.  

### Implementation

* Add `sys_trace()` in `kernel/sysproc.c` which loads the argument from the trapframe and stores the mask in the `proc` struct.
* `fork()` was modified to copy the trace mask from the parent to the child process.
* The `syscall()` function in `kernel/syscall` was modified to print the output of trace.  
* Implemented New user program `strace.c` in the `user` directory. It parses the user arguments and calls the system call.
* `$U/_strace` was added to UPROGS in the Makefile.
* A stub was added in `user/usys.pl` and a prototype for the system call in `user/user.h`.

## Scheduling 

* To invoke the bash ,  
```bash
make clean
make qemu SCHEDULER=XXXX [DEFAULT|FCFS|PBS|MLFQ]
```
The default scheduler of xv6 is round-robin-based. Three other schedulers were implemented :

### 1. First Come First Serve (FCFS)  

* It selects the process with the lowest creation time and keeps runnning until the process ends. The process will run until it no longer needs CPU time.  
* While process creation in ```allocproc()```, process creation time is stored in ```struct proc::ctime```.  
* In ```kernel/proc.c```, we search for the process with the lowest creation time and run that process.  
* Preemption is disabled by disabling the call to ```yield()``` in ```usertrap()``` and ```kerneltrap()``` in ```kernel/trap.c```, if FCFS is the choosen scheduler.

### 2. Priority Based Scheduling (PBS)  

* This is a non-preemptive priority-based scheduling policy that selects the process
with the highest priority for execution. In case two or more processes have the same priority, we use the number of times the process has been scheduled to break the tie. If the tie remains, use the start-time of the process to break the tie (processes with lower start times are scheduled earlier).  
* There are two types of priority :
    * Static Priority : Default value  
    * Dynamic Priority : Depends on running and sleeping time and Static priority . 
In practice , dynamic priority is used for scheduling the processes.

* Run a for loop to search for the process with the highest priority (lowest dynamic priority).
* To measure the sleeping time, when the process is sent to sleep via `sleep()` in `kernel/proc.c`, the number of ticks is stored in `struct proc::s_start_time`. Then, when `wakeup()` in `kernel/proc.c` is called, the difference between the current number of ticks and the previously stored time is stored in `struct proc::stime` as the sleeping time.
* Only the static priority (60 by default) is stored in `struct proc`. The niceness and the dynamic priority are calculated in the loop, when the process to be scheduled is being selected.
* As in FCFS, the call to `yield()` has been conditionally disabled for PBS.
* The `set_priority()` system call can be used to change the static priority of a process. It has been implemented in the same manner as in specification 1. A user program has also been implemented.

```bash
setpriority [priority] [pid]
```

### 3. Multi Level Feedback Queue (MLFQ)  

* This is a simplified preemptive scheduling policy that allows processes to move between different priority queues based on their behavior and CPU bursts.  
* If a process uses too much CPU time, it is pushed to a lower priority queue, leaving I/O bound and interactive processes in the higher priority queues.
* To prevent starvation, aging has been implemented.  
* The five priority queues have not been implemented physically; rather they have been stored as a member variable `current_queue` of `struct proc`. This facilitates adding and removing processes and "shifting between queues" by just changing the number, and eliminates the overhead in popping from one queue and pushing to another.
* Each queue has a time-slice as follows after which they are demoted to a lower priority queue (`current_queue` is decremented).

1. For priority 0: 1 timer tick
2. For priority 1: 2 timer ticks
3. For priority 2: 4 timer ticks
4. For priority 3: 8 timer ticks
5. For priority 4: 16 timer ticks
* Aging has been implemented, just before scheduling, via a simple for loop that iterates through the runnable processes, and promotes them to a higher-priority queue if `(ticks - <entry time in current queue>) > 16 ticks`.
* Demotion of processes after their time slice has been completed is done in `kernel/trap.c`, whenever a timer interrupt occurs. If the time spent in the current queue is greater than 2<sup>(current_queue_number)</sup>, then it is demoted (`current_queue` is incremented).
* The position of the process in the queue is determined by its `struct proc::entry_time`, which stores the entry time in the current queue. It is reset to the current time whenever it is scheduled, making the wait time in the queue 0.
* If it is relinquished by the CPU, its entry time is again reset to the current number of ticks.

## Specification 3: procdump

It prints the information as mentioned in the assignment doc. Most of this information is already present as part of ```struct proc```.
```wtime``` is calculated using ```current_time/end_time - creation_time - run_time```

### RESULTS 
Ran ```schedulertest.c``` for all the scheduling functions for their comparision :
| Scheduling Algorithm | Avg Running Time  | Avg Waiting Time  |
| :---:   | :-: | :-: |
| Round Robin | 110 | 18 |  
| FCFS | 40 | 35 |
| PBS | 105 | 17 |
| MLFQ | 108 | 16 |

* Round Robin performed the best followed by MLFQ .  
* FCFS as expected performed worst because of occurence of CPU bound process which in turn increase the waiting time for all the other processes .  
