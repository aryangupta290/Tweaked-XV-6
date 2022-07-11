# Enhanced-xv6

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

## Specification 1: Syscall Tracing

```bash
strace mask [command] [args]
```
strace runs the specified command until it exits. It intercepts and records the system calls which are called by a process during its execution. It takes one argument, an integer `mask`, whose bits specify which system calls to trace.

### Implementation

* The `sys_trace()` in `kernel/sysproc.c` loads the argument from the trapframe and calls the `trace()` system call to store the mask in the `proc` struct.
* `fork()` was modified to copy the trace mask from the parent to the child process.
* The `syscall()` function in `kernel/syscall` was modified to print the trace output.
* The command was implemented via a user program `strace.c` in the `user` directory. It parses the user arguments and calls the system call.
* `$U/_strace` was added to UPROGS in the Makefile.
* A stub was added in `user/usys.pl` and a prototype for the system call in `user/user.h`.

## Specification 2: Scheduling Algorithms

```bash
make clean
make qemu SCHEDULER=XXXX [DEFAULT|FCFS|PBS|MLFQ]
```
The default scheduler of xv6 is round-robin-based. 3 additional scheduling policies have been implemented:

* First-come, first-serve
* Priority-based
* Multi-level feedback queue

### 1. First-come, first-serve

This is a non-preemptive policy that selects the process with the lowest creation time (creation time refers to the tick number when the process was created). The process will run until it no longer needs CPU time.

#### Implementation

* The only modification made was in `kernel/proc.c`. We run a for loop to search for the process with the lowest process creation time (`struct proc::ctime`, which stores the number of ticks when the process is allocated and initialized).
* To disable preemption, the call to `yield()` in `usertrap()` and `kerneltrap()` in `kernel/trap.c` was disabled conditionally, depending on the scheduler chosen. For FCFS, it has been disabled.

### 2. Priority-based

This is a non-preemptive priority-based scheduling policy that selects the process
with the highest priority for execution. In case two or more processes have the same priority, we use the number of times the process has been scheduled to break the tie. If the tie remains, use the start-time of the process to break the tie (processes with lower start times are scheduled earlier).

Here, we have static priority and dynamic priority. Dynamic priority varies with running time and sleeping time and decides scheduling. Static priority is used to calculate dynamic priority.

#### Implementation

* Again, we run a for loop to search for the process with the highest priority (lowest dynamic priority).
* To measure the sleeping time, when the process is sent to sleep via `sleep()` in `kernel/proc.c`, the number of ticks is stored in `struct proc::s_start_time`. Then, when `wakeup()` in `kernel/proc.c` is called, the difference between the current number of ticks and the previously stored time is stored in `struct proc::stime` as the sleeping time.
* Only the static priority (60 by default) is stored in `struct proc`. The niceness and the dynamic priority are calculated in the loop, when the process to be scheduled is being selected.
* As in FCFS, the call to `yield()` has been conditionally disabled for PBS.
* The `set_priority()` system call can be used to change the static priority of a process. It has been implemented in the same manner as in specification 1. A user program has also been implemented.

```bash
setpriority [priority] [pid]
```

### 3. Multi-level feedback queue

This is a simplified preemptive scheduling policy that allows processes to move between different priority queues based on their behavior and CPU bursts.
* If a process uses too much CPU time, it is pushed to a lower priority queue, leaving I/O bound and interactive processes in the higher priority queues.
* To prevent starvation, aging has been implemented.

#### Implementation

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

`procdump()` prints a list of processes to the console when a user enters <kbd>Ctrl</kbd>+<kbd>P</kbd> on the console.
Here, I have extended this function to print more information about all
the active processes.

* process ID
* priority (PBS and MLFQ only)
* state
* running time (`struct proc::rtime`)
* waiting time (current ticks or `struct proc::etime` - creation time - running time)
* number of times scheduled (stored in `struct proc::no_of_times_scheduled`)
* q<sub>i</sub> (MLFQ only) (number of ticks the process spent in each of the 5 queues)
