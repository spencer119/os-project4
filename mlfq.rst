I need to trouble shoot the functionality of my MLFQ scheduler in a C++ CPU Simulation program for testing different scheduling algorithms. It doesn't produce the expected output for my Multi-Level Feedback Queues (MLFQ) code. Read these instructions carefully to understand the requirements of the project better: 
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
Here is my current code for the MLFQ algorithm that isn't working correctly, mlfq_algorithm.cpp and mlfq_algorithm.hpp:
<mlfq_algorithm.hpp>
#ifndef MFLQ_ALGORITHM_HPP
#define MFLQ_ALGORITHM_HPP

#include <map>
#include <memory>

#include "algorithms/scheduling_algorithm.hpp"
#include "utilities/stable_priority_queue/stable_priority_queue.hpp"

/*
    MLFQScheduler:
        A representation of a multi-level feedback queue scheduling algorithm.

        You are free to add any member functions or member variables that you
        feel are helpful for implementing the algorithm.
*/

using MLFQQueue = Stable_Priority_Queue<std::shared_ptr<Thread>>;

class MLFQScheduler : public Scheduler {
   public:
    //==================================================
    //  Member variables
    //==================================================

    // TODO: Add any member variables you may need.
    int n = 10;
    std::vector<Stable_Priority_Queue<std::shared_ptr<Thread>>> queues;

    //==================================================
    //  Member functions
    //==================================================

    MLFQScheduler(int slice = -1);

    std::shared_ptr<SchedulingDecision> get_next_thread();

    void add_to_ready_queue(std::shared_ptr<Thread> thread);

    size_t size() const;
};

#endif
</mlfq_algorithm.hpp>
<mlfq_algorithm.cpp>
#include "algorithms/mlfq/mlfq_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the MLFQ algorithm.
*/

MLFQScheduler::MLFQScheduler(int slice) {
    if (slice != -1) {
        throw("MLFQ does NOT take a customizable time slice");
    }
    queues.resize(n, Stable_Priority_Queue<std::shared_ptr<Thread>>());
}

std::shared_ptr<SchedulingDecision> MLFQScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();

    if (queues.empty()) {
        decision->time_slice = -1;
        decision->thread = nullptr;
        decision->explanation = "No threads available for scheduling.";
        // return decision;
    } else {
        for (int i = 0; i < n; i++) {
            if (!queues[i].empty()) {
                auto thread = queues[i].top();
                queues[i].pop();
                if (thread->service_time - thread->time_added_to_queue >= pow(2, i)) {
                    // If so, move it down one queue if it's not already in the lowest priority queue
                    if (i < n - 1) {
                        thread->time_added_to_queue = thread->service_time;
                        queues[i + 1].push(thread->priority, thread);
                        continue;
                    }
                } else {
                    decision->thread = thread;
                    decision->time_slice = pow(2, i);
                    // Selected from queue Z (priority = P, runtime = R). Will run for at most Y ticks.
                    decision->explanation = fmt::format("Selected from queue {} (priority = {}, runtime = {}). Will run for at most {} ticks.", i, PROCESS_PRIORITY_MAP[thread->priority], thread->service_time - thread->time_added_to_queue, decision->time_slice);
                }

                return decision;
            }
        }
    }
    return decision;
}

void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    thread->time_added_to_queue = thread->service_time;
    queues[0].push(thread->priority, thread);
    // queues[thread->priority].push(thread->priority, thread);
}

size_t MLFQScheduler::size() const {
    size_t total = 0;
    for (const auto& queue : queues) {
        total += queue.size();
    }
    return total;
}

</mlfq_algorithm.cpp>
In addition to all the information provided already, before you write code you must analyze the file codebase.txt which contains the contents of project code files you have to understand and utilize in your code. Read it carefully and re-read it as needed to understand the requirements of the project.
What in the code is not meeting the requirements of the project causing the output to not match the expected output?