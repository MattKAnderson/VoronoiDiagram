#pragma once
#include <algorithm>
#include <queue>
#include <iostream>
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/BeachLine.hpp>

namespace VoronoiDiagram::Impl {
struct Event {
    double sweepline;
    RealCoordinate point;
    Arc* associated_arc = nullptr;
    //Event* next = nullptr;
    //Event* prev = nullptr;
    Event(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    Event(const RealCoordinate& site);
    Event() {}
};
/*
class EventManager {
public:
    EventManager() {};
    ~EventManager();
    void initialize(int psize);
    void reset();
    Event* create(const RealCoordinate& site);
    Event* create(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    void remove(Event* event);

private:
    std::vector<Event*> pools;
    int pool;
    int next_index;
    int pool_size;
    Event** available = nullptr;
    int available_size;
    int next_available = -1;
    Event* get_available_ptr();
    int next_id = 0;
};
*/
class EventIndexManager {
public:
    EventIndexManager() {}
    EventIndexManager(int psize) { 
        events.reserve(2 * psize); 
        max_stack_size = std::min(1024, psize);
        available_stack.reserve(max_stack_size);
    };
    const Event& get(int id);
    int create(const RealCoordinate& site);
    int create(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    void remove(int id);

private:
    int max_stack_size;
    std::vector<Event> events;
    std::vector<int> available_stack;    
};

class EventHashVectorQueue {
public:
    struct EventReference {
        double sweepline;
        int event_id;
        EventReference(double sweepline, int event_id): 
            sweepline(sweepline), event_id(event_id) {}
    };
    EventHashVectorQueue() {}
    void initialize(int psize, double start, double end);
    void insert(double sweepline, int id);
    void remove(double sweepline, int id);
    int consume_next();
    int size();
    bool empty();
private:
    double bucket_start;
    double inv_bucket_step;
    int current_bucket = 0;
    int _size = 0;
    int last_id;
    std::vector<std::vector<EventReference>> buckets;
    int compute_bucket(double sweepline);
};

inline int EventHashVectorQueue::compute_bucket(double sweepline) {
    return std::max(0, static_cast<int>(std::min(
        (sweepline - bucket_start) * inv_bucket_step, 
        static_cast<double>(last_id)
    )));
}
/*
class EventQueue {
public:
    EventQueue() {}
    void initialize(int psize, double start, double end);
    void insert(Event* event);
    void remove(Event* event);
    Event* consume_next();
    int size();
    bool empty();
private:
    double bucket_start;
    double inv_bucket_step;
    int current_bucket = 0;
    int _size = 0;
    int last_id;
    std::vector<Event> buckets;
    int compute_bucket(double sweepline);
};
*/
inline Event::Event(const RealCoordinate& site): 
    sweepline(site.x), point(site) {}

inline Event::Event(
    double sweepline, const RealCoordinate& intersect,
    Arc* associated_arc
): sweepline(sweepline), point(intersect), associated_arc(associated_arc) {}

} // namespace VoronoiDiagram::Impl