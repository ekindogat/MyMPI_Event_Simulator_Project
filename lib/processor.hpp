#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <string>

#include "utils.hpp"
#include "event_types.hpp"

// Forward declaration
class EventSimulator;

class Processor
{
public:
    // constr
    Processor(int rank, int num_processes)
        : rank_(rank), num_processes_(num_processes)
    {
        // Initialize workspace array
        workspace_.reserve(1000); // Reserve space for merging
        setNeighbors();

    }

    // Set the local data for this processor
    void setData(const std::vector<int> &data)
    {
        local_data_ = std::vector<int>(data); // Create a copy of the input data
        received_data_.clear();
        workspace_.clear();
    }
    void setNeighbors();
  

    // Get the local data
    const std::vector<int> &getData() const
    {
        return local_data_;
    }
    const std::vector<int> &getReceived() const {
        return received_data_;
    }

    void setReceived(const std::vector<int> &data)
    {
        received_data_ = std::vector<int>(data);
        // printVector(received_data_, "processor successfully received##");
    }
    void receiveMessage(); // get message data to its local cache

    void localSort(); // sort local cache

    void handleMerge(bool isOddPhase);

    int getRank() const { return rank_; }
    int getNeighbor(bool isOddPhase) {
        return (isOddPhase ? odd_neighbor : even_neighbor);
    }

private:
    int rank_;
    int odd_neighbor;
    int even_neighbor;
    int num_processes_;
    double processor_time = 0.;
    std::vector<int> local_data_;    // Local array holding processor's numbers
    std::vector<int> received_data_; // Array holding received data
    std::vector<int> workspace_;     // Workspace array for merging
};