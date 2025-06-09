#pragma once

#include <vector>
#include <queue>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#include "processor.hpp"
#include "utils.hpp"


class MyMPI
{
public:
    static MyMPI &getInstance()
    {
        static MyMPI instance;
        return instance;
    }

    // Initialize the simulator with number of processes
    void init(int num_processes);

    // Simulated MPI_Send
    Event send(int source, int dest, const std::vector<int> &data, int tag, double current_time);

    // Simulated MPI_Recv with retry logic
    Event receive(int rank, int source,const std::vector<int> &data, int tag, double current_time);

    int getNumProcesses() const { return num_processes_; }

private:
    struct Message
    {
        int source;
        std::vector<int> data;
        int tag;
    };


    MyMPI() = default;
    ~MyMPI() = default;
    MyMPI(const MyMPI &) = delete;
    MyMPI &operator=(const MyMPI &) = delete;

    double calculateTransferTime(size_t data_size)
    {
        // Simulate network delay: 1ms base + 0.1ms per element
        return 0.001 + (data_size * 0.0001);
    }

    int num_processes_;

};