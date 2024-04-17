#include "algorithms/spn/spn_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the SPN algorithm.
*/

SPNScheduler::SPNScheduler(int slice) {
    if (slice != -1) {
        throw("SPN must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> SPNScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();
    decision->time_slice = -1;
    if (ready_queue.empty()) {
        decision->thread = nullptr;
        decision->explanation = "No threads available for scheduling.";
    } else {
        decision->thread = ready_queue.top();
        decision->explanation = fmt::format("Selected from {} threads. Will run to completion of burst.", ready_queue.size());
        ready_queue.pop();
    }
    return decision;
}

void SPNScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    if (!thread->bursts.empty()) {
        int next_burst_length = thread->bursts.front()->length;
        ready_queue.push(next_burst_length, thread);  // Add thread to the queue with the bust length as the priority
    }
}

size_t SPNScheduler::size() const {
    return ready_queue.size();
}