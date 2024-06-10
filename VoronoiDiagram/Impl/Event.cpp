#include <VoronoiDiagram/Impl/Event.hpp>
#include <iostream>

namespace VoronoiDiagram::Impl {

const Event& EventManager::get(int id) {
    return events[id];
}

int EventManager::create(const RealCoordinate& site) {
    int id;
    if (available_stack.size()) {
        id = available_stack.back(); available_stack.pop_back();
        events[id].sweepline = site.x;
        events[id].point = site;
        events[id].associated_arc = nullptr;
    }   
    else {
        id = events.size();
        events.emplace_back(site);
    }
    return id;
}

int EventManager::create(
    double sweepline, const RealCoordinate& intersect, 
    Arc* associated_arc
) {
    int id;
    if (available_stack.size()) {
        id = available_stack.back(); available_stack.pop_back();
        events[id].sweepline = sweepline;
        events[id].point = intersect;
        events[id].associated_arc = associated_arc;
    }
    else {
        id = events.size();
        events.emplace_back(sweepline, intersect, associated_arc);
    }
    return id;
}

void EventManager::remove(int id) {
    if (available_stack.size() < max_stack_size) {
        available_stack.push_back(id);
    }
}

void EventQueue::initialize(int psize, double start, double end) {
    int n_buckets = psize / 2.0;
    bucket_start = start;
    inv_bucket_step = n_buckets / (end - start);
    buckets = std::vector<std::vector<EventReference>>(n_buckets);
    for (auto& bucket : buckets) {
        bucket.reserve(16);
    }
    last_id = n_buckets - 1;
}

void EventQueue::insert(double sweepline, int id) {
    int bucket_index = compute_bucket(sweepline);
    current_bucket = std::min(current_bucket, bucket_index);
    ++_size;
    auto r_it = buckets[bucket_index].rbegin();
    auto r_end = buckets[bucket_index].rend();
    for (; r_it != r_end && sweepline > r_it->sweepline; ++r_it) { }
    buckets[bucket_index].emplace(r_it.base(), sweepline, id);
}

void EventQueue::remove(double sweepline, int id) {
    int bucket_index = compute_bucket(sweepline);
    auto it = buckets[bucket_index].begin();
    auto end = buckets[bucket_index].end();
    for (; it != end && id != it->event_id; it++) { }
    buckets[bucket_index].erase(it); // guaranteed to be called on existing item
    --_size; 
}

int EventQueue::consume_next() {
    --_size;
    while (buckets[current_bucket].size() == 0) { ++current_bucket; }
    int event_id = buckets[current_bucket].back().event_id;
    buckets[current_bucket].pop_back();
    return event_id;
}

int EventQueue::size() {
    return _size;
}

bool EventQueue::empty() {
    return _size == 0;
}
} // namespace VoronoiDiagram::Impl