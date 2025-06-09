#pragma once

#include <vector>
#include "utils.hpp"

const int RANDOM_INIT_PROCESSOR_RANK = -7;
// Timing constants for discrete event simulation
namespace SimTime {
    constexpr double LOCAL_SORT_TIME = 2.0;    // time for local sorting (NOT USED)
    constexpr double SEND_TIME = 1.0;          // time for sending a message
    constexpr double RECV_TIME = 1.0;          // time for receiving a message
    constexpr double START_SORT_TIME = 2.5;         // time for start sorting message
    constexpr double PHASE_DELAY = 50.0;        // time for delay between phases (NOT USED)
    constexpr double COMPARE_SPLIT_TIME = 4.0;    // time for compare split event
    constexpr double SORT_TIME = 500.;      // time for sort operation so that always handled at the end of message passing

}

enum class EventType {
    SEND,
    RECV,
    START_SORT,      // start sorting
    COMPARE_SPLIT,  // start compare split
};

struct Message {
    int source;
    std::vector<int> data;
    int tag;
};

class Event {
public:
    Event(double time, EventType type, int source_rank, int dest_rank, 
          std::vector<int> data = std::vector<int>(), int tag = 0)
        : time_(time), type_(type), source_rank_(source_rank), 
          dest_rank_(dest_rank), data_(std::move(data)), tag_(tag) {}

    double getTime() const { return time_; }
    EventType getType() const { return type_; }
    int getSourceRank() const { return source_rank_; }
    int getDestRank() const { return dest_rank_; }
    const std::vector<int>& getData() const { return data_; }
    int getTag() const { return tag_; }


private:
    double time_;  // Discrete simulation time
    EventType type_;
    int source_rank_;
    int dest_rank_;
    std::vector<int> data_;
    int tag_;
    
};

class EventComparator {
public:
    bool operator()(const Event& a, const Event& b) const {
        return a.getTime() > b.getTime();
    }
};