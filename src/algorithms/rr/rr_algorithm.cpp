#include "algorithms/rr/rr_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the round robin algorithm.
*/

RRScheduler::RRScheduler(int slice) {
    if (slice == -1) {
        time_slice = 3;
        // throw("RR requires a slice greater than 0");
    } else {
        this->time_slice = slice;
    }
    this->ready_queue = std::deque<std::shared_ptr<Thread>>();
}

std::shared_ptr<SchedulingDecision> RRScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();
    if (ready_queue.empty()) {
        decision->time_slice = -1;
        decision->thread = nullptr;
        decision->explanation = "No threads available for scheduling.";
    } else {
        std::shared_ptr<Thread> next = ready_queue.front();
        ready_queue.pop_front();
        decision->explanation = fmt::format("Selected from {} threads. Will run for at most {} ticks.", ready_queue.size() + 1, time_slice);
        decision->thread = next;
        decision->time_slice = time_slice;
    }
    return decision;
}

void RRScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    ready_queue.push_back(thread);
}

size_t RRScheduler::size() const {
    return ready_queue.size();
}
