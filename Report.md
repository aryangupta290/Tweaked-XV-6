# Assignment 4

Dennis Ritchie and Ken Thompson's Unix Version 6 is re-implemented in xv6 (v6). xv6 is based on v6 in structure and style, but it is written in ANSI C for current RISC-V multiprocessors.

# how to implement 

1) strace :

a) We firstly modify the proc struct so as to store the mask value for every process.
b) Then we  modify fork to copy the trace mask from the parent to its child process and  the syscall function was also modified to print the trace output.
c) The command was implemented via a user program `strace.c` which parses the user arguments and calls the system call.
d) Now we make changes in Makefile and other appropriate areas to declare our strace function.

2) Scheduler 

FCFS:

In FCFS , we always run the process with the lowest cretion time  and we had to implement in a non-preempt form .
Here we just stored the creation time (ctime) in the proc struct and iterted through all the process in the infinite loop to find the one with the minimum creation time.
After we find the required process , we just run it the way we did in Round Robin.
Also since we implement it in a non-preemt fashion , we comment out the yield code in trap.c


PBS: 

Here we have to run the process with the highest priority . To calculate the priority (dynamic priority) , we store run_time and sleep_time in the proc struct since we would need them to find the niceness values . Now just like before , we iterate through all the process and find the one with the highest priority (that is lowest dynamic priority value ) and run it . 
We change the clock_itr function to change the run_time and change sleep and wakeup function to accomote for sleep_time.

MLFQ : 

Here we have not physically created 5 separate queues but rather we just stored in which queue each process is present and the time it has spend in that particular queue.
Now to pick a process we run , we iterate through all the process and pick the one with the minimum queue number( max priority) and break the tie using the variable (time spent in each queue).
We also increment the time slice for each queue so that after a certain period , we decrease the process of the priority . 
To implement aging , we just iterate and check that if (ticks - entry time ) > 15 ticks , we increase its priority .
If the process is relinquished by the CPU, its entry time is again reset to the current number of ticks.
Whenever we schedule a process to a new queue, we make the wait time of the process to be 0 by changing the entry_time in queue to be current ticks. 

By combining all above ways , we are able to implement MLFQ

3) Procdump :

Here , we have already stored everything that we need to print in the proc stat apart from the wait time which can be easily calculted by using the sleep time and run_time.
After we have all the required values , all we need to do is to print the required stuff using the indef blocks .

# Explain in the README how could this be exploited by a process

Say a process relinquished the control of the CPU(may be due to I/O) , so now it will leave the queue network and now when the process bceomes ready again , it will be inserted at the end of the same queue from where it was evicted .
This can easily be exploited by a process since just when the time slice is about to expire , the process can voluntarily release the control of the CPU and get inserted in the same queue again . 
Now suppose a process ran as normal , then after the time slice , it would habe been preempted to the lower queue and now it's chances of getting CPU time will be low atleast for following few ticks ( until it is say upgraded to higher queue by aging may be) . 
So by using this method a process could bluff CPU by remaining in the higher priority queue so that it will get more CPU time ( as soon as possible).  

# peformance comparision 

To get the values , we used the schedulertest.c benchmark program

                   Avg Running Time   Avg Waiting Time  
For Round robin :     117                   17
For FCFS :            55                    47    
For PBS :             109                   24  
For MLFQ :            113                    20

We can observe that the waiting time for FCFS is max and the ratio of run_time/wait_time is worst for FCFS which is also expected .
Next we see that Roubt Robin performs best , following by MLFQ and PBS with very minute changes in the ratio of (run_time/wait_time).


