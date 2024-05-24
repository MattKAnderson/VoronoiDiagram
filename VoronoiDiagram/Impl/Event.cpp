#include <VoronoiDiagram/Impl/Event.hpp>

namespace VoronoiDiagram::Impl {

const Event& EventManager::get(int id) {
    return events[id];
}

int EventManager::create(const RealCoordinate& coord) {
    int id;
    if (available_stack.size()) {
        id = available_stack.back(); available_stack.pop_back();
        events[id].coord = coord;
        events[id].associated_arc = nullptr;
        // intersect_point will contain garbage, it should not be accessed
        // given that associated_arc is set to nullptr
    }   
    else {
        id = events.size();
        events.emplace_back(coord);
    }
    return id;
}

int EventManager::create(
    const RealCoordinate& coord, const RealCoordinate& intersect, 
    Arc* associated_arc
) {
    int id;
    if (available_stack.size()) {
        id = available_stack.back(); available_stack.pop_back();
        events[id].coord = coord;
        events[id].associated_arc = associated_arc;
        events[id].intersect_point = intersect;
    }
    else {
        id = events.size();
        events.emplace_back(coord, intersect, associated_arc);
    }
    return id;
}

void EventManager::remove(int id) {
    available_stack.push_back(id);
}

void EventQueue::insert(int event_id) {
    event_id_heap.push_back(event_id);
    if (event_id >= id_to_location.size()) {
        int new_size = id_to_location.size() * 2;
        while (new_size <= event_id) { new_size *= 2; }
        id_to_location.resize(new_size, -1);
    }
    id_to_location[event_id] = event_id_heap.size() - 1;
    up_heapify(event_id_heap.size() - 1);
}

void EventQueue::remove(int event_id) {
    int heap_id = id_to_location[event_id];
    if (heap_id == -1) { return; } 
    id_to_location[event_id] = -1;
    event_id_heap[heap_id] = event_id_heap.back();
    id_to_location[event_id_heap.back()] = heap_id;
    event_id_heap.pop_back();
    if (compare_event_id(event_id, event_id_heap[heap_id])) {
        up_heapify(heap_id);
    }
    else {
        down_heapify(heap_id);
    }
}

int EventQueue::consume_next() {
    int event_id = event_id_heap[0];
    swap(0, event_id_heap.size() - 1);
    event_id_heap.pop_back();
    id_to_location[event_id] = -1;
    down_heapify(0);
    return event_id;
}

bool EventQueue::empty() {
    return event_id_heap.size() == 0;
}

int EventQueue::lchild(int id) {
    return 2 * id + 1;
}

int EventQueue::rchild(int id) {
    return 2 * id + 2;
}

int EventQueue::parent(int id) {
    return (id - 1 - (id & 1 ^ 1)) >> 1;
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
    while (id < event_id_heap.size()) {
        int id_of_lowest = id;
        int lc_id = lchild(id);
        int rc_id = rchild(id);
        int size = event_id_heap.size();
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

void EventQueue::swap(int ida, int idb) {
    int event_id_a = event_id_heap[ida];
    int event_id_b = event_id_heap[idb];
    id_to_location[event_id_a] = idb;
    id_to_location[event_id_b] = ida;
    event_id_heap[ida] = event_id_b;
    event_id_heap[idb] = event_id_a;
 }

// this might be really inefficient
bool EventQueue::compare(int ida, int idb) {
    int event_ida = event_id_heap[ida];
    int event_idb = event_id_heap[idb];
    return compare_event_id(event_ida, event_idb);
}

bool EventQueue::compare_event_id(int event_ida, int event_idb) {
    const RealCoordinate& ca = em->get(event_ida).coord;
    const RealCoordinate& cb = em->get(event_idb).coord;
    return ca.x > cb.x || (ca.x == cb.x && ca.y > cb.y);
}

void EventQueue::print_ordered_x() {
    std::queue<int> index_queue;
    index_queue.push(0);
    while (!index_queue.empty()) {
        int id = index_queue.front(); index_queue.pop();
        std::cout << em->get(event_id_heap[id]).coord.x << "\n";
        int lc_id = lchild(id);
        int rc_id = rchild(id);
        if (lc_id < event_id_heap.size()) { index_queue.push(lc_id); }
        if (rc_id < event_id_heap.size()) { index_queue.push(rc_id); }
    }
}

bool EventQueue::validate() {
    if (event_id_heap.size() == 0) { return true; }
    int size = event_id_heap.size();
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