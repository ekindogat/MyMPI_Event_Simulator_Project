#pragma once

#include <queue>
#include <deque>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <unordered_set>
#include <random>

#include "event_types.hpp"
#include "my_mpi.hpp"
#include "utils.hpp"


// Forward declaration
class Processor;

class EventSimulator
{
public:
    static EventSimulator &getInstance()
    {
        static EventSimulator instance;
        return instance;
    }

    // Initialize the simulator with number of processes and elements per processor
    void init(int num_processes, int elements_per_processor);

    // Initialize processors with random data
    void initializeData();

    void initializeData1();


    // run events in the simulator in order
    void run();

    void scheduleEvent(Event event) { event_queue_.push(std::move(event)); }
    double getCurrentTime() const { return current_time_; }
    void setCurrentTime(double time) { current_time_ = time; }
    std::deque<Message> &getProcessorQueue(int rank);
    int getNumProcesses() const { return num_processes_; }
    const std::vector<std::unique_ptr<Processor>> &getProcessors() const { return processors_; }

    std::string toStringEvent(const Event& event, double current_time) const;
  

    
    Processor* findProcessor(int rank); // Find processor by rank

private:
    EventSimulator() : current_time_(0.0), num_processes_(0), elements_per_processor_(0) {}
    ~EventSimulator() = default;
    EventSimulator(const EventSimulator &) = delete;
    EventSimulator &operator=(const EventSimulator &) = delete;

    void processSendEvent(const Event &event);
    void processRecvEvent(const Event &event);
    void processStartSortEvent(const Event &event);
    void processCompareSplitEvent(const Event &event);



    MyMPI* mpi;

    std::priority_queue<Event, std::vector<Event>, EventComparator> event_queue_;
    std::vector<std::unique_ptr<Processor>> processors_; // Own processors

     std::string event_log_;

    double current_time_;
    int num_processes_;
    int elements_per_processor_;
};