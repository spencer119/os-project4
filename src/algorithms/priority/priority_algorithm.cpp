#include "algorithms/priority/priority_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the priority algorithm.
*/

PRIORITYScheduler::PRIORITYScheduler(int slice) {
    if (slice != -1) {
        throw("PRIORITY must have a timeslice of -1");
    }
    // if (slice == -1) {
    //     time_slice = 3;
    //     // throw("RR requires a slice greater than 0");
    // } else {
    //     this->time_slice = slice;
    // }
}

std::shared_ptr<SchedulingDecision> PRIORITYScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();
    if (ready_queue.empty()) {
        decision->time_slice = -1;
        decision->thread = nullptr;
        decision->explanation = "No threads available for scheduling.";
    } else {
        std::shared_ptr<Thread> next = ready_queue.top();
        auto before_counts = ready_queue.count();
        ready_queue.pop();
        auto after_counts = ready_queue.count();
        decision->thread = next;
        // [S: u I: u N: u B: u] -> [S: v I: v N: v B: v]. Will run to completion of burst.
        decision->explanation = fmt::format("[S: {} I: {} N: {} B: {}] -> [S: {} I: {} N: {} B: {}]. Will run to completion of burst.", before_counts[ProcessPriority::SYSTEM], before_counts[ProcessPriority::INTERACTIVE], before_counts[ProcessPriority::NORMAL], before_counts[ProcessPriority::BATCH], after_counts[ProcessPriority::SYSTEM], after_counts[ProcessPriority::INTERACTIVE], after_counts[ProcessPriority::NORMAL], after_counts[ProcessPriority::BATCH]);
    }
    return decision;
}

void PRIORITYScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    ready_queue.push(thread->priority, thread);
}

size_t PRIORITYScheduler::size() const {
    return ready_queue.size();
}
