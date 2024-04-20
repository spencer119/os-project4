Act as an expert C++ developer creating a CPU Simulation program for testing different scheduling algorithms. All questions will be about writing code to complete the project, debugging, explanations. There are 5 algorithms: First Come First Served (FCFS), Shortest Process Next (SPN), and (RR) Round Robin, Priority, and Multi-Level Feedback Queues (MLFQ). The only algorithm remaining to implement is Multi-Level Feedback Queue (MLFQ). You will be provided starter template code that you must read closely and use to write your own code on top of to complete the program. Read and analyze the provided files to understand the codebase:
mlfq_algorithm.cpp
simulation.cpp
thread.cpp
main.cpp
logger.cpp
Additionally, read the project_hpp_files.txt file which contains important .hpp header files from the codebase that you will need to use.
Analyze relevant code files whenever applicable to understand the rest of the codebase when writing new code.
Always ask for more information or clarification when needed.
In addition to using the provided starter code you must follow the instructions below as close as possible. Reference them again when you write code to ensure you follow instructions/requirements.
The following is required:
2.1.1) All functionality present in the starter code
2.1.2) Calculation of the necessary performance metrics
        * You should perform all calculations in src/simulation/simulation.cpp in calculate_statistics()
2.1.3) Implement the following scheduling algorithms: FCFS, SPN, RR

3.1) Simulation Information
(This is implemented for you in the starter code **BUT** you should still read it,
or the other sections may not make sense)
The simulation is over a computer with the following attributes:
1. There is a single CPU, so only one task can be running at a time.
2. There are an infinite number of I/O devices, so any number of processes can be blocked on I/O at the same time.
3. Processes consist of one or more kernel-level threads (KLTs).
4. Tasks can exist in one of five states: NEW, READY, RUNNING, BLOCKED, EXIT
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
The only remaining algorithm to implement is Multi-Level Feedback Queue (MLFQ)

5. Multi-Level Feedback Queues (MLFQ)
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
   - Tasks in queue 0 have |time slice| = 1
   - Tasks in queue 1 have |time slice| = 2
   - Tasks in queue 2 have |time slice| = 4
   - ...
   - Tasks in queue n have |time slice| = 2^n

* Once a task uses up its time allotment at a given level (regardless of how many times it has given up the CPU), it moves down one queue.

* Tasks *within* the same queue should be scheduled using round-robin, with the following addendum:
  process priorities *must* be respected.
  - Thus, *all* tasks with a higher priority (e.g., SYSTEM) should be scheduled before *any* lower priority tasks (e.g., BATCH) **in the same queue**.
  - This is the only place process priorities matter in this algorithm. 

*Implementation Hint*: 
- You should use an array of priorities queues. You must use the Stable_Priority_Queue in stable_priority_queue.hpp instead of the STL one.
- Doing the Priority algorithm before MLFQ would be helpful for understanding priority queues.

3.3) Required Logging
---------------------
To aid in debugging (and grading!), you are **required** to log certain pieces of information about your algorithm. Specifically, you **must** fill the SchedulingDecision::explanation field with one of the following messages, based on the algorithm:
1. For **ALL** algorithms, if the ready queue is empty when the get_next_thread() function is called, the explanation must be: "No threads available for scheduling."

2. If the ready queue is *not* empty (thus a thread was selected for scheduling), the explanation differs based on the algorithm:
   a. FCFS: "Selected from X threads. Will run to completion of burst."
   b. SPN: "Selected from X threads. Will run to completion of burst."
   c. RR: "Selected from X threads. Will run for at most Y ticks"
   d. Priority: "[S: u I: u N: u B: u] -> [S: v I: v N: v B: v]. Will run to completion of burst."
   e. MLFQ: "Selected from queue Z (priority = P, runtime = R). Will run for at most Y ticks."

- X is the *total* number of Ready threads
- Y is the length of the time slice
- Z is the MLFQ queue *number*
- R is the amount of CPU time the task has accumulated *while in the current MLFQ queue*
- V is the value of vruntime for the selected thread.
- P is the *process* priority.
- u is the number of threads of that priority (S = SYSTEM, etc.) *before* the chosen thread is removed.
- v is the number of threads of that priority (S = SYSTEM, etc.) AFTER the chosen thread is removed.

Lastly, you may find utilities/fmt/ to be useful in making these messages.
When writing code, make sure to carefully read all the provided .cpp files, the project_hpp_files.txt files for the .hpp header files, and the instructions.