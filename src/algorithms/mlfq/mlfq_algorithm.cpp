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
    queues = std::vector<MLFQQueue>(n);
}

std::shared_ptr<SchedulingDecision> MLFQScheduler::get_next_thread() {
        // TODO: implement me!
        return nullptr;
}
void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
        // TODO: implement me!
    queues[0].push(thread->priority, thread);
}

size_t MLFQScheduler::size() const {
    size_t total = 0;
    for (const auto& queue : queues) {
        total += queue.size();
    }
    return total;
}
