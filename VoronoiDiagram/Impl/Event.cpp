#include <VoronoiDiagram/Impl/Event.hpp>

namespace VoronoiDiagram::Impl {

// TODO: micro-optimization is to remove the bounds checking
//       from this method since it will always be guaranteed
//       to have space for the site events
Event* EventPool::new_event(const RealCoordinate& coord) {
    /*if (event_index == pool_size) {
        if (pool_index == pools.size() - 1) {
            pools.push_back(new Event[pool_size]);
        }
        ++pool_index;
        event_index = 0;
    }*/
    Event* event = &pools[pool_index][event_index++];
    event->coord = coord;
    event->associated_arc = nullptr;
    return event;
}

Event* EventPool::new_event(
    const RealCoordinate& coord, const RealCoordinate& intersect,
    Arc* associated_arc
) {
    if (event_index == pool_size) {
        if (pool_index == pools.size() - 1) {
            pools.push_back(new Event[pool_size]);
        }
        ++pool_index;
        event_index = 0;
    }
    Event* event = &pools[pool_index][event_index++];
    event->coord = coord;
    event->intersect_point = intersect;
    event->associated_arc = associated_arc;
    return event;
}

void EventPool::free(Event* event) {
    if (available_index < pool_size) {
        available_stack[++available_index] = event;
    }
}

void EventPool::reset() {
    pool_index = 0;
    event_index = 0;
    available_index = 0;
}

void EventQueue::insert(Event* event) {
    int id = heap.size();
    heap.push_back(event);
    event->queue_pos = id;
    up_heapify(id);
}

void EventQueue::remove(Event* event) {
    Event* replacement = heap.back(); heap.pop_back();
    heap[event->queue_pos] = replacement;
    replacement->queue_pos = event->queue_pos;
    if (event->coord.x > replacement->coord.x) {
        up_heapify(event->queue_pos);
    }
    else {
        down_heapify(event->queue_pos);
    }
}

void EventQueue::reserve_space(int amount) {
    heap.reserve(amount);
}

Event* EventQueue::consume_next() {
    Event* event = heap[0];
    swap(0, heap.size() - 1);
    heap.pop_back();
    down_heapify(0);
    return event;
}

void EventQueue::up_heapify(int id) {
    int parent_id = parent(id);
    while (id > 0 && compare(parent_id, id)) {
        swap(parent_id, id);
        id = parent_id;
        parent_id = parent(id);
    }
}

void EventQueue::down_heapify(int id) {
    while (id < heap.size()) {
        int id_of_lowest = id;
        int lc_id = lchild(id);
        int rc_id = rchild(id);
        int size = heap.size();
        if (lc_id < size && compare(id_of_lowest, lc_id)) { id_of_lowest = lc_id; }
        if (rc_id < size && compare(id_of_lowest, rc_id)) { id_of_lowest = rc_id; }
        if (id_of_lowest != id) {
            swap(id_of_lowest, id);
            id = id_of_lowest;
        }
        else {
            break;
        }
    }
}

void EventQueue::print_ordered_x() {
    std::queue<int> index_queue;
    index_queue.push(0);
    while (!index_queue.empty()) {
        int id = index_queue.front(); index_queue.pop();
        std::cout << heap[id]->coord.x << "\n";
        int lc_id = lchild(id);
        int rc_id = rchild(id);
        if (lc_id < heap.size()) { index_queue.push(lc_id); }
        if (rc_id < heap.size()) { index_queue.push(rc_id); }
    }
}

bool EventQueue::validate() {
    if (heap.size() == 0) { return true; }
    int size = heap.size();
    std::vector<int> node_stack;
    node_stack.push_back(0);
    while (!node_stack.empty()) {
        int cur_id = node_stack.back(); node_stack.pop_back();
        int lc_id = lchild(cur_id);
        int rc_id = rchild(cur_id);
        if (lc_id < size) {
            if (compare(cur_id, lc_id)) { return false; }
            node_stack.push_back(lc_id);
        }
        if (rc_id < size) {
            if (compare(cur_id, rc_id)) { return false; }
            node_stack.push_back(rc_id);
        }
    }
    return true;
}

} // namespace VoronoiDiagram::Impl