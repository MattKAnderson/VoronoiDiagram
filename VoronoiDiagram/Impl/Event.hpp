#pragma once
#include <queue>
#include <iostream>
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/BeachLine.hpp>

namespace VoronoiDiagram::Impl {
struct Event {
    double sweepline;
    RealCoordinate point;
    Arc* associated_arc = nullptr;
    Event(
        double sweepline, const RealCoordinate& intersect, 
        Arc* associated_arc
    );
    Event(const RealCoordinate& site);
};

class EventManager {
public:
    EventManager() {};
    const Event& get(int id);
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
    void set_event_manager(EventManager* em) { this->em = em; }
    void insert(int event_id);
    void remove(int event_id);
    void reserve_space(int nevents);
    void print_ordered_x();
    int consume_next();
    int size();
    bool empty();
    bool validate();
private:
    EventManager* em = nullptr;
    std::vector<int> event_id_heap;
    std::vector<int> id_to_location{8}; // for fast delete
    int lchild(int id);
    int rchild(int id);
    int parent(int id);
    void up_heapify(int id);
    void down_heapify(int id);
    void swap(int ida, int idb);
    bool compare(int ida, int idb);
    bool compare_event_id(int event_ida, int event_idb);
};

inline Event::Event(const RealCoordinate& site): 
    sweepline(site.x), point(site) {}

inline Event::Event(
    double sweepline, const RealCoordinate& intersect,
    Arc* associated_arc
): sweepline(sweepline), point(intersect), associated_arc(associated_arc) {}

} // namespace VoronoiDiagram::Impl