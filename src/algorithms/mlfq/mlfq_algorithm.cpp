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
}

std::shared_ptr<SchedulingDecision> MLFQScheduler::get_next_thread() {
    auto decision = std::make_shared<SchedulingDecision>();

       return decision;
}
// for (int i = 0; i < n; i++) {
//     if (!queues[i].empty()) {
//         auto thread = queues[i].top();
//         queues[i].pop();
//         int current_slice = pow(2, i);
//         std::cout << "Time Slice: " << pow(2, i) << std::endl;
//         if (thread->service_time - thread->time_added_to_queue >= current_slice) {
//             if (i < n - 1) {
//                 thread->time_added_to_queue = thread->service_time;
//                 queues[i + 1].push(thread->priority, thread);
//             }
//         } else {
//             decision->thread = thread;
//             decision->time_slice = pow(2, i);
//             // Selected from queue Z (priority = P, runtime = R). Will run for at most Y ticks.
//             decision->explanation = fmt::format("Selected from queue {} (priority = {}, runtime = {}). Will run for at most {} ticks.", i, PROCESS_PRIORITY_MAP[thread->priority], thread->service_time - thread->time_added_to_queue, decision->time_slice);
//             return decision;
//         }
//     }
// }
void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    // TODO: implement me!
}

size_t MLFQScheduler::size() const {
    // TODO: implement me!
    return 0;
}
