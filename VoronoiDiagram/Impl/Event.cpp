#include <VoronoiDiagram/Impl/Event.hpp>
#include <iostream>

namespace VoronoiDiagram::Impl {
/*
EventManager::~EventManager() {
    for (Event* pool : pools) {
        free(pool);
    }
    delete[] available;
}

void EventManager::initialize(int psize) {
    for (Event* pool : pools) {
        free(pool);
    }
    if (available) {
        delete[] available;
    }
    pool = 0;
    next_index = 0;
    next_id = 0;
    pool_size = psize * 4;
    available_size = psize / 10;
    next_available = -1;
    available = new Event*[available_size];
    pools = {static_cast<Event*>(malloc(pool_size * sizeof(Event)))};
}

void EventManager::reset() {
    pool = 0;
    next_id = 0;
    next_index = 0;
}

Event* EventManager::create(const RealCoordinate& site) {
    Event* event = get_available_ptr();
    *event = Event(site);
    return event;
}

Event* EventManager::create(
    double sweepline, const RealCoordinate& intersect, Arc* associated_arc
) {
    Event* event = get_available_ptr();
    *event = Event(sweepline, intersect, associated_arc);
    return event;
}

void EventManager::remove(Event* event) {
    next_available += next_available < (available_size - 1);
    available[next_available] = event;
}

Event* EventManager::get_available_ptr() {
    if (next_available >= 0) { return available[next_available--]; }
    else if (next_index < pool_size) { return &pools[pool][next_index++]; }
    else if (++pool < pools.size()) {  
        next_index = 1;
        return &pools[pool][0];
    }
    else {
        pools.push_back(static_cast<Event*>(malloc(pool_size * sizeof(Event))));
        next_index = 1;
        return &pools[pool][0];
    } 
}
*/

const Event& EventIndexManager::get(int id) {
    return events[id];
}

int EventIndexManager::create(const RealCoordinate& site) {
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

int EventIndexManager::create(
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

void EventIndexManager::remove(int id) {
    if (available_stack.size() < max_stack_size) {
        available_stack.push_back(id);
    }
}

void EventHashVectorQueue::initialize(int psize, double start, double end) {
    int n_buckets = psize / 4.0;
    double delta = (end - start);
    bucket_start = start;// - 0.25 * delta;
    inv_bucket_step = n_buckets / (end - start);
    buckets = std::vector<std::vector<EventReference>>(n_buckets);
    for (auto& bucket : buckets) {
        bucket.reserve(16);
    }
    last_id = n_buckets - 1;
}

void EventHashVectorQueue::insert(double sweepline, int id) {
    int bucket_index = compute_bucket(sweepline);
    current_bucket = std::min(current_bucket, bucket_index);
    ++_size;
    auto r_it = buckets[bucket_index].rbegin();
    auto r_end = buckets[bucket_index].rend();
    for (; r_it != r_end && sweepline > r_it->sweepline; ++r_it) { }
    buckets[bucket_index].emplace(r_it.base(), sweepline, id);
}

void EventHashVectorQueue::remove(double sweepline, int id) {
    int bucket_index = compute_bucket(sweepline);
    auto it = buckets[bucket_index].begin();
    auto end = buckets[bucket_index].end();
    for (; it != end && id != it->event_id; it++) { }
    buckets[bucket_index].erase(it); // guaranteed to be called on existing item
    --_size; 
}

int EventHashVectorQueue::consume_next() {
    --_size;
    while (buckets[current_bucket].size() == 0) { ++current_bucket; }
    int event_id = buckets[current_bucket].back().event_id;
    buckets[current_bucket].pop_back();
    return event_id;
}

int EventHashVectorQueue::size() {
    return _size;
}

bool EventHashVectorQueue::empty() {
    return _size == 0;
}
/*
void EventQueue::initialize(int psize, double start, double end) {
    int n_buckets = psize / 4.0;
    bucket_start = start;
    inv_bucket_step = n_buckets / (end - start);
    buckets = std::vector<Event>(n_buckets);
    last_id = n_buckets - 1;
}

void EventQueue::insert(Event* event) {
    int bucket_id = compute_bucket(event->sweepline);
    current_bucket = std::min(current_bucket, bucket_id);
    ++_size;
    Event* last = &buckets[bucket_id];
    while (last->next != nullptr && last->next->sweepline < event->sweepline) {
        last = last->next;
    }
    if (last->next) {
        last->next->prev = event;
    }
    event->next = last->next;
    event->prev = last;
    last->next = event;
}

void EventQueue::remove(Event* event) {
    if (event->next) {
        event->next->prev = event->prev;
    }
    event->prev->next = event->next;
    --_size;
}

Event* EventQueue::consume_next() {
    --_size;
    while (buckets[current_bucket].next == nullptr) { ++current_bucket; }
    Event* event = buckets[current_bucket].next;
    if (event->next) {
        event->next->prev = event->prev;        
    }
    buckets[current_bucket].next = event->next;
    return event;
}

int EventQueue::size() {
    return _size;
}

bool EventQueue::empty() {
    return _size == 0;
}

int EventQueue::compute_bucket(double sweepline) {
    return std::max(0, std::min(
        static_cast<int>((sweepline - bucket_start) * inv_bucket_step), 
        last_id
    ));
}
*/
} // namespace VoronoiDiagram::Impl