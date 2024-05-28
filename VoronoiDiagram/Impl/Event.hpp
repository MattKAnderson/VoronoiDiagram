#pragma once
#include <queue>
#include <iostream>
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/BeachLine.hpp>

namespace VoronoiDiagram::Impl {
struct Event {
    RealCoordinate coord;
    RealCoordinate intersect_point;
    Arc* associated_arc = nullptr;
    int queue_pos;
    Event() {}
    Event(
        const RealCoordinate& coord, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    Event(const RealCoordinate& coord);
};

class EventPool {
public:
    EventPool(int pool_size);
    ~EventPool();
    Event* new_event(const RealCoordinate& coord);
    Event* new_event(
        const RealCoordinate& coord, const RealCoordinate& intersect,
        Arc* associated_arc
    );
    void free(Event* event);
    void reset();
private:
    std::vector<Event*> pools;
    Event** available_stack;
    int pool_size;
    int pool_index = 0;
    int event_index = 0;
    int available_index = 0;
    
};

class EventQueue {
public:
    void insert(Event* event);
    void remove(Event* event);
    void reserve_space(int amount);
    void print_ordered_x();
    Event* consume_next();
    int size();
    bool empty();
    bool validate();
private:
    std::vector<Event*> heap;
    int lchild(int id);
    int rchild(int id);
    int parent(int id);
    void up_heapify(int id);
    void down_heapify(int id);
    void swap(int ida, int idb);
    bool compare(int ida, int idb);
};

inline Event::Event(const RealCoordinate& coord): coord(coord) {}

inline Event::Event(
    const RealCoordinate& coord, const RealCoordinate& intersect,
    Arc* associated_arc
): coord(coord), intersect_point(intersect), 
   associated_arc(associated_arc) {}

inline EventPool::EventPool(int pool_size): pool_size(pool_size) {
    pools.push_back(new Event[pool_size]);
    available_stack = new Event*[pool_size + 1];
}

inline EventPool::~EventPool() {
    delete[] available_stack;
    for (Event* pool : pools) {
        delete[] pool;
    }
}

inline int EventQueue::size() { return heap.size(); }

inline bool EventQueue::empty() { return heap.size() == 0; }

inline int EventQueue::lchild(int id) { return 2 * id + 1; }

inline int EventQueue::rchild(int id) { return 2 * id + 2; }

inline int EventQueue::parent(int id) { return (id - 1) >> 1; }

inline void EventQueue::swap(int ida, int idb) {
    std::swap(heap[ida], heap[idb]);
    std::swap(heap[ida]->queue_pos, heap[idb]->queue_pos);
}

inline bool EventQueue::compare(int ida, int idb) {
    return heap[ida]->coord.x > heap[idb]->coord.x;
}
} // namespace VoronoiDiagram::Impl