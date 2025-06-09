#include "my_mpi.hpp"


void MyMPI::init(int num_processes)
{
    num_processes_ = num_processes;
}

Event MyMPI::send(int source, int dest, const std::vector<int> &data, int tag, double current_time)
{

    if (
        dest < 0 || dest >= num_processes_)
    {
        throw std::runtime_error("Invali  d process rank");
    }
    // At beginning sending unsorted arrays to processors
    if (source == RANDOM_INIT_PROCESSOR_RANK ||
        !(source < 0 || source >= num_processes_))
    {

        // Calculate message transfer time (simulated network delay)
        double transfer_time = SimTime::SEND_TIME;
        double arrival_time = current_time + transfer_time;

        // Schedule the SEND event (using an Event object) instead of a lambda

        return Event(arrival_time, EventType::SEND, source, dest, data, tag);
    }

    else
    {
        throw std::runtime_error(" 2 Invalid process rank");
    }
}

Event MyMPI::receive(int rank, int source, const std::vector<int> &data, int tag, double current_time)

{
    if (
        rank < 0 || rank >= num_processes_)
    {
        throw std::runtime_error(" 3Invalid process rank");
    }

    if (source == RANDOM_INIT_PROCESSOR_RANK || !(source < 0 || source >= num_processes_))
    {
        // Calculate simulated network delay
        double arrival_time = current_time + SimTime::RECV_TIME;

        // Schedule the RECV event
            return Event(arrival_time, EventType::RECV, source, rank, data, tag);
    }
    // Validate source and destination ranks
    else
    {
        throw std::runtime_error("4 Invalid process rank");
    }
}
