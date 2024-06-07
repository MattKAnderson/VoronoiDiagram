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
    int next = -1;
    Event(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    Event(const RealCoordinate& site);
    Event() {}
};

class EventManager {
public:
    EventManager() {};
    Event& get(int id);
    int create(const RealCoordinate& site);
    int create(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    void remove(int id);

private:
    std::vector<Event> events;
    std::vector<int> available_stack;
};

class EventQueue {
public:
    EventQueue() {}
    void initialize(EventManager* manager, int psize, double start, double end);
    void insert(int id);
    void remove(int id);
    int consume_next();
    int size();
    bool empty();
private:
    EventManager* manager;
    double bucket_start;
    double inv_bucket_step;
    int current_bucket = 0;
    int _size = 0;
    int last_id;
    std::vector<Event> buckets;
    int compute_bucket(double sweepline);
};

inline Event::Event(const RealCoordinate& site): 
    sweepline(site.x), point(site) {}

inline Event::Event(
    double sweepline, const RealCoordinate& intersect,
    Arc* associated_arc
): sweepline(sweepline), point(intersect), associated_arc(associated_arc) {}

} // namespace VoronoiDiagram::Impl