<instructions>
1) Introduction
===============
The goal of this project is to implement many of the scheduling algorithms discussed in class in a discrete-event 
simulator. Additionally, at the end of execution, your program will calculate and display several
performance criteria from the simulation.

Since this is a large project, you are given a **LOT** of starter code, which implements the simulation for you and nicely sets up what you are required to implement.

2) Project Requirements
=======================
You will create a program called cpu-sim, which simulates a variety of possibly multi-threaded processes using a specified scheduling algorithm.

3.1) Simulation Information
---------------------------
(This is implemented for you in the starter code)
The simulation is over a computer with the following attributes:
1. There is a single CPU, so only one task can be running at a time.
2. There are an infinite number of I/O devices, so any number of processes can be blocked on I/O at the same time.
3. Processes consist of one or more kernel-level threads (KLTs).
4. Tasks can exist in one of five states: NEW, READY, RUNNING, BLOCKED, EXIT
    - NEW
    - READY
    - RUNNING
    - BLOCKED
    - EXIT
5. Scheduling tasks requires a non-zero amount of OS overhead:
    - If the previously executed thread belongs to a different process than the new thread, a full *process switch* occurs. This is also the case for the first thread being executed.
    - If the previously executed thread belongs to the same process as the new thread being dispatched, a cheaper *thread switch* is done.
    - A full process switch includes any work required by a thread switch.
    - Running the scheduler (dispatcher) also requires a certain amount of overhead.
6) Threads, processes, and dispatch overhead are specified via the input file
7) Each thread requires a sequence of CPU and I/O bursts of varying lengths as specified by the input file.
    - You can think of "bursts" as an observation of the task's behavior: a task wanting needing to use the CPU for 10 ms, then read a file (which takes 500 ms), then use the CPU for another 10 ms; would be composed of 3 bursts:
            a. A CPU burst of 10 ms
            b. An IO burst of 500 ms
            c. A CPU burst of 10 ms
    - Note that all tasks will end with a CPU burst.
8) Processes have an associated priority, specified as part of the file. Each thread in a process has the same priority as its parent process.
        - 0: SYSTEM (highest priority)
        - 1: INTERACTIVE
        - 2: NORMAL
        - 3: BATCH (lowest priority)

9) All processes have a distinct process ID, specified as part of the file. Thread IDs are unique only within the context of their owning process (so the first thread in every process has an ID of 0).

10) Overhead is incurred only when dispatching a thread (transitioning it from READY to RUNNING); all other OS actions require zero OS overhead. For example, adding a thread to a ready queue or initiating I/O are both ”free”.

11) Threads for a given process can arrive at any time, even if some other process is currently running (i.e., some external entity—not the CPU—is responsible for creating threads).

12) Tasks are executed on the CPU. For our purposes, a task is either:
a. A single-threaded process
b. A single thread of a multi-threaded process.
    - Note this means a "task" is synonymous with a "thread" in this project, since we do not care about kernel workers. Thus, the two are used interchangably throughout this writeup.

3.2) Scheduling Algorithms
--------------------------
1. Multi-Level Feedback Queues (MLFQ)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* There are n queues, numbered 0 ... n-1     
    - For this project, n = 10  

* The priority of a queue is given by: n - <queue number>
    - This means lower numbered queues have higher priority.
    - E.g., queue 0 has priority n, queue 3 has priority n - 3, and so forth

* Tasks in lower-numbered (i.e., higher-priority) queues should be scheduled before higher-numbered queues
    - E.g., *all* tasks in queue 0 should be scheduled before *any* in queue 1, etc.

* When a task enters the system, it should be placed in the topmost queue (queue 0)

* The time slice a task is given is based off of its queue number.
    - Tasks in queue 0 have time_slice = 1
    - Tasks in queue 1 have time_slice = 2
    - Tasks in queue 2 have time_slice = 4
    - ...
    - Tasks in queue n have time_slice = 2^n

* Once a task uses up its time allotment at a given level (regardless of how many times it has given
  up the CPU), it moves down one queue.
* Tasks *within* the same queue should be scheduled using round-robin, with the following addendum:
  process priorities *must* be respected.
    - Thus, *all* tasks with a higher priority (e.g., SYSTEM) should be scheduled before *any* lower priority tasks (e.g., BATCH) **in the same queue**.
    - This is the only place process priorities matter in this algorithm. 

*Implementation Hint*: 
- You should use an array of priorities queues
- Doing the Priority algorithm before MLFQ would be helpful for understanding priority queues.

2. Round Robin (RR)
~~~~~~~~~~~~~~~~~~~
* Tasks are scheduled in the order they are added to the ready queue
* Tasks may be preempted if their CPU burst length is greater than the *time slice*
* In the event of a preemption:
    a. The task is removed from the CPU
    b. Its CPU burst length is updated to reflect the fact that it got some CPU time (how much?)
    c. The task is added to the back of the ready queue.
...which implies:
1. There **IS** preemption in this algorithm.
2. All process priorities are treated as equal.

3. Priority
~~~~~~~~~~~
* Tasks priorities have the following order:
    a. SYSTEM (highest)
    b. INTERACTIVE
    c. NORMAL
    d. BATCH  (lowest)
* Tasks *of the same priority* are scheduled in the order they are added to the ready queue
* Tasks *of different* priorities should follow the order given above (i.e., *all* SYSTEM 
  tasks in the ready queue should be executed before *ANY* INTERACTIVE tasks, and so forth)
* Tasks run until their CPU burst is completed.
...which implies:
1. There is no preemption in this algorithm 
2. Process priorities are NOT to be ignored.
*Implementation Hint:*
- ...As mentioned before, the standard library priority queue is not deterministic when multiple entries share the same priority.
- Use the Stable_Priority_Queue class in stable_priority_queue.hpp (you can find the contents of stable_priority_queue.hpp in project_hpp_files.txt) to ensure that tasks of the same priority are scheduled in the order they were added to the ready queue.

3.3) Required Logging
---------------------

To aid in debugging (and grading!), you are **required** to log certain pieces of information
about your algorithm. Specifically, you **must** fill the SchedulingDecision::explanation field
with one of the following messages, based on the algorithm:

1. For **ALL** algorithms, if the ready queue is empty when the get_next_thread() function is called,
   the explanation must be: "No threads available for scheduling.""

2. If the ready queue is *not* empty (thus a thread was selected for scheduling), the explanation differsbased on the algorithm:
    - MLFQ: Selected from queue Z (priority = P, runtime = R). Will run for at most Y ticks. 
    - RR: Selected from X threads. Will run for at most Y ticks.          

* X is the *total* number of Ready threads
* Y is the length of the time slice
* Z is the MLFQ queue *number*
* R is the amount of CPU time the task has accumulated *while in the current MLFQ queue*
* P is the *process* priority.

Lastly, you may find utilities/fmt/ to be useful in making these messages.
3.4) Performance Metrics
------------------------
You need to calculate the following performance metrics:
  #. Number of Threads per Process Priority
  #. Average Turnaround Time per Process Priority 
  #. Average Response Time per Process Priority
  #. Total Service Time
  #. Total I/O Time
  #. Total Idle Time
  #. CPU Utilization = [(Total Time - Total Idle Time) / (Total Time)] * 100
  #. CPU Efficiency = [(Total Service Time) / (Total Time)] * 100

See the SystemStatistics class and Simulation::calculate_statistics() for more information.
</instructions>