#include <VoronoiDiagram/Impl/Event.hpp>

namespace VoronoiDiagram::Impl {

Event& EventManager::get(int id) {
    return events[id];
}

int EventManager::create(const RealCoordinate& site) {
    int id;
    if (available_stack.size()) {
        id = available_stack.back(); available_stack.pop_back();
        events[id].sweepline = site.x;
        events[id].point = site;
        events[id].associated_arc = nullptr;
        events[id].next = -1;
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
        events[id].next = -1;
    }
    else {
        id = events.size();
        events.emplace_back(sweepline, intersect, associated_arc);
    }
    return id;
}

void EventManager::remove(int id) {
    available_stack.push_back(id);
}

void EventQueue::initialize(EventManager* manager, int psize, double start, double end) {
    this->manager = manager;
    int n_buckets = 4 * std::sqrt(psize);
    bucket_start = start;
    inv_bucket_step = n_buckets / (end - start);
    buckets = std::vector<Event>(n_buckets);
    last_id = n_buckets - 1;
}

void EventQueue::insert(int id) {
    Event& event = manager->get(id);
    int bucket_id = compute_bucket(event.sweepline);
    current_bucket = std::min(current_bucket, bucket_id);
    ++_size;
    Event* last = &buckets[bucket_id];
    
    if (last->next == -1) {
        //std::cout << "Setting first id to: " << id << std::endl;
        last->next = id;
        return;
    }
    Event* curr = &manager->get(last->next);
    while (curr->sweepline < event.sweepline) {
        last = curr;
        if (last->next == -1) {
            break;
        }
        curr = &manager->get(last->next);
    }
    event.next = last->next; 
    last->next = id;
}

void EventQueue::remove(int id) {
    Event& event = manager->get(id);
    int bucket_id = compute_bucket(event.sweepline);
    Event* last = &buckets[bucket_id];
    while(last->next != id) {
        last = &manager->get(last->next);
    }
    last->next = event.next;
    --_size;
}

int EventQueue::consume_next() {
    while (buckets[current_bucket].next == -1) { ++current_bucket; }
    int id = buckets[current_bucket].next;
    buckets[current_bucket].next = manager->get(id).next;
    //std::cout << "New first id is: " << buckets[current_bucket].next << std::endl;
    --_size;
    return id;
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

} // namespace VoronoiDiagram::Impl