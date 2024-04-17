#include "algorithms/fcfs/fcfs_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the FCFS algorithm.
*/

FCFSScheduler::FCFSScheduler(int slice) {
    if (slice != -1) {
        throw("FCFS must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> FCFSScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();

    if (ready_queue.empty()) {  // No threads in queue
        decision->thread = nullptr;
        decision->explanation = "No threads available for scheduling.";
    } else {
        decision->thread = ready_queue.front();
        ready_queue.pop_front();
        decision->explanation = fmt::format("Selected from {} threads. Will run to completion of burst.", ready_queue.size() + 1);
    }
    decision->time_slice = -1;
    return decision;
}

void FCFSScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    ready_queue.push_back(thread);
}

size_t FCFSScheduler::size() const {
    return ready_queue.size();
}
