#include "algorithms/mlfq/mlfq_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "mlfq_algorithm.hpp"
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

std::shared_ptr<SchedulingDecision> MLFQScheduler::get_next_thread() {  // TODO
    if (this->size() == 0) {
        auto decision = std::make_shared<SchedulingDecision>();
        decision->explanation = "No threads available for scheduling.";
        decision->time_slice = -1;
        decision->thread = nullptr;
        return decision;
    }
    for (int i = 0; i < n;) {
        time_slice = pow(2, i);
        if (queues[i].empty()) {
            i++;
            continue;
        }
        int current_slice = pow(2, i);
        auto thread = queues[i].top();
        if (thread->queue_map[i] >= current_slice) {  // move down a queue
            if (i < n - 1) {
                queues[i].pop();
                queues[i + 1].push(thread->priority, thread);
                continue;
            } else {
                // thread->queue_map[i] += current_slice;
                queues[i].pop();
                thread->schedule_time = thread->service_time;
                thread->last_queue = i;
                return create_decision(thread, i);
            }
        } else {
            queues[i].pop();
            thread->schedule_time = thread->service_time;
            thread->last_queue = i;
            // thread->queue_map[i] += current_slice;
            return create_decision(thread, i);
        }
    }
    return nullptr;
}
void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {  // TODO
    thread->time_added = thread->service_time;
    if (thread->schedule_time == -1) {
        thread->schedule_time = 0;
    } else {
        // std::cout << "Times " << thread->schedule_time << " " << thread->service_time << std::endl;
        thread->queue_map[thread->last_queue] += thread->service_time - thread->schedule_time;
    }
    queues[0].push(thread->priority, thread);
}

size_t MLFQScheduler::size() const {
    size_t total = 0;
    for (const auto& queue : queues) {
        total += queue.size();
    }
    return total;
}

int MLFQScheduler::update_thread_queue_time(std::shared_ptr<Thread> thread, int queue_index) {
    int delta = thread->service_time - thread->time_added;
    thread->queue_map[queue_index] += delta;
    thread->time_added = thread->service_time;
    return thread->queue_map[queue_index];
}

std::shared_ptr<SchedulingDecision> MLFQScheduler::create_decision(std::shared_ptr<Thread> thread = nullptr, int queue_index = -1) {
    auto decision = std::make_shared<SchedulingDecision>();
    if (thread == nullptr) {
        decision->explanation = "No threads available for scheduling.";
        decision->time_slice = -1;
        decision->thread = nullptr;
        return decision;
    }
    decision->thread = thread;
    decision->time_slice = static_cast<int>(pow(2, queue_index));
    decision->explanation = fmt::format("Selected from queue {} (priority = {}, runtime = {}). Will run for at most {} ticks.", queue_index, PROCESS_PRIORITY_MAP[thread->priority], thread->queue_map[queue_index], static_cast<int>(pow(2, queue_index)));
    // thread->queue_map[queue_index] += static_cast<int>(pow(2, queue_index));
    return decision;
}
