#include <sstream>
#include <string>
#include <fstream>

#include "event_simulator.hpp"
#include "processor.hpp"
#include "utils.hpp"

void EventSimulator::init(int num_processes, int elements_per_processor)
{

    // processor declarations
    num_processes_ = num_processes;
    elements_per_processor_ = elements_per_processor;

    // MyMPI init
    mpi = &MyMPI::getInstance();
    mpi->init(num_processes_);

    // Create processor instances
    processors_.clear();
    processors_.reserve(num_processes);
    for (int i = 0; i < num_processes; ++i)
    {
        processors_.push_back(std::make_unique<Processor>(i, num_processes));
    }

    // Reset simulation state
    current_time_ = 0.0;

    // MAX HEAP FOR EVENT TIME, (comparator reverse the sort so it is MIN HEAP now)
    event_queue_ = std::priority_queue<Event, std::vector<Event>, EventComparator>();
}

void EventSimulator::initializeData()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100000);

    for (auto &processor : processors_)
    {
        std::vector<int> data(elements_per_processor_);
        for (int &val : data)
        {
            val = dis(gen);
        }
        processor->setData(data);
    }
}


// For debugging purposes
void EventSimulator::initializeData1()
{

    for (auto &processor : processors_)
    {
        std::vector<int> data(elements_per_processor_);
        int i = 0;
        for (int &val : data)
        {
            val = (processor->getRank() * elements_per_processor_) + i++;
            val *= -1;
            std::cout << val << " ";
        }
        processor->setData(data);
    }
}

Processor *EventSimulator::findProcessor(int rank)
{
    if (rank < 0 || rank >= num_processes_)
    {
        return nullptr; // Invalid rank
    }

    // Search through processors to find matching rank
    for (const auto &processor : processors_)
    {
        if (processor->getRank() == rank)
        {
            return processor.get();
        }
    }

    return nullptr; // No processor found with this rank
}

void EventSimulator::run()
{

    // logging events to output file
    std::ofstream logFile("event_log.txt");
    if (logFile.is_open())
    {
        // HEADER (might turn it to csv for visualization)
        logFile << "=== DISCRETE EVENT SIMULATION LOG ===\n";
        logFile << "Number of Processors: " << num_processes_ << "\n";
        logFile << "Elements per Processor: " << elements_per_processor_ << "\n";
        logFile << "Total Elements: " << (num_processes_ * elements_per_processor_) << "\n";
        logFile << "========================================\n\n";
    }

    event_queue_.push(Event(current_time_ + SimTime::START_SORT_TIME, EventType::START_SORT, 0, 0, {}));

    while (!event_queue_.empty())
    {
        const Event &event = event_queue_.top();
        current_time_ = event.getTime();

        // Process each event based on its type
        switch (event.getType())
        {
        case EventType::SEND:
            processSendEvent(event);
            break;
        case EventType::RECV:
            processRecvEvent(event);
            break;
        case EventType::START_SORT:
            processStartSortEvent(event);
            break;
        case EventType::COMPARE_SPLIT:
            processCompareSplitEvent(event);
            break;
        }

        event_queue_.pop();

        // append for logging
        if (logFile.is_open())
        {
            logFile << toStringEvent(event, current_time_) << "\n";
        }
        // event_log_ += toStringEvent(event, current_time_) + "\n";
    }

    // print processed events log
    // std::cout << "\n\n\t --EVENT LOGGED IN PROCESS ORDER--\n";
    // std::cout << event_log_;
    if (logFile.is_open())
    {
        logFile.close();
        std::cout << "\nEvent log saved to: event_log.txt" << std::endl;
    }
}

void EventSimulator::processSendEvent(const Event &event)
{
    double event_process_time = event.getTime();
    if (event_process_time >= current_time_)
        setCurrentTime(event_process_time);

    auto curr_processor = findProcessor(event.getSourceRank());
    auto curr_message = curr_processor->getData();

    std::cout << "\n[Event Time: " << current_time_ << "] Processing SEND event:"
              << "\n  From: Processor " << event.getSourceRank()
              << "\n  To: Processor " << event.getDestRank();
    //           << "\n  Data: ";

    // for (int val : curr_message)
    // {
    //     std::cout << val << " ";
    // }
    std::cout << "\n  Tag: " << event.getTag() << std::endl;

    double expected_arrival_time = current_time_ + SimTime::RECV_TIME;

    // Schedule the RECV event after SEND_TIME
    scheduleEvent(mpi->receive(event.getDestRank(), event.getSourceRank(), curr_message, event.getTag(), expected_arrival_time));
}

// this function's aim is to get new array to local cache!!
void EventSimulator::processRecvEvent(const Event &event)
{
    // time calculation
    double event_process_time = event.getTime();
    if (event_process_time >= current_time_)
        setCurrentTime(event_process_time);

    // find processor
    Processor *curr_processor = findProcessor(event.getDestRank());

    std::cout << "\n[Event Time: " << current_time_ << "] Processing RECV event:"
              << "\n  Receiver: Processor " << event.getDestRank()
              << "\n  From: Processor " << event.getSourceRank();
            //   << "\n  Data: ";
    // for (int val : event.getData())
    // {
    //     std::cout << val << " ";
    // }
    std::cout << "\n  Tag: " << event.getTag() << std::endl;

    // std::cout << " Current local_cache: \n\t";
    // for (int val : curr_processor->getData())
    // {
    //     std::cout << val << " ";
    // }

    curr_processor->setReceived(event.getData());

    // std::cout << "\n Current received_cache: \n\t";
    // for (int val : curr_processor->getReceived())
    // {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;
}

void EventSimulator::processStartSortEvent(const Event &event)
{
    /** Sort Event:
     * It iterates PROC_NUM times to create odd then even phase
     * For each iteration i = 1 ,
     *      for each processor
     *          - we send a message to neighbors according to phase,
     *                  ARRIVAL TIME of send events: curr_time + SEND_TIME + i * PHASE_DELAY
     *          - we create a receive message event to accept neighbor array.
     *                  ARRIVAL TIME :    curr_time + SEND_TIME + RECV_TIME + i * PHASE_DELAY
     *          - we create comparesplit event, managing local cache of processor to merge
     *                  ARRIVAL TIME :    curr_time + SEND_TIME + RECV_TIME + COMPARE_SPLIT_TIME + i * PHASE_DELAY
     *
     *  each iteration increase arrival time
     *
     *  AFTER PROCESSING CURR_TIME becomes curr_time + START_SORT_TIME !!!!! not arrival time
     */

    double event_process_time = event.getTime();
    if (event_process_time >= current_time_)
        setCurrentTime(event_process_time);

    std::cout << "\n[Event Time: " << current_time_ << "] Starting SORT event:"
              << std::endl;

    for (int i = 0; i < (int)processors_.size(); i++)
    {
        bool isOddPhase = i % 2 != 0;
        std::cout << "\n\t Entered " << i + 1 << ". Phase: " << (isOddPhase ? "ODD" : "EVEN") << std::endl;

        for (auto &&p : processors_)
        {
            // schedule send event
            int my_rank = p->getRank();                     // current processor id
            int neighbor_rank = p->getNeighbor(isOddPhase); // current processor's corresponding phase's neighbor id
            if (neighbor_rank < 0 || neighbor_rank >= (int)processors_.size())
                continue;
            double expected_arrival_time = current_time_ + SimTime::SEND_TIME + i * SimTime::PHASE_DELAY;

            std::cout << "\t [Processor " << my_rank << " ] Neighbor: [Processor " << neighbor_rank << "]" << std::endl;
            scheduleEvent(mpi->send(my_rank, neighbor_rank, p->getData(), 0, expected_arrival_time));
            std::cout << "\t SEND Event scheduled FROM [ " << my_rank
                      << " ] TO: " << neighbor_rank << " AT ARRIVAL TIME: " << expected_arrival_time
                      << std::endl;

            // // schedule recv event REDUNDANT
            // expected_arrival_time += SimTime::RECV_TIME;
            // scheduleEvent(mpi->receive(neighbor_rank, my_rank, p->getData(), 0, expected_arrival_time));
            // std::cout << "\t RECV Event scheduled FOR [ " << neighbor_rank
            //           << " ] FROM: " << my_rank << " AT ARRIVAL TIME: " << expected_arrival_time
            //           << std::endl;

            // schedule comparesplit event

            expected_arrival_time += SimTime::RECV_TIME + SimTime::COMPARE_SPLIT_TIME;

            // pass isOddPhase boolean to determine which half of the array will be discarded
            scheduleEvent(Event(expected_arrival_time, EventType::COMPARE_SPLIT,
                                my_rank, isOddPhase ? 1 : 0,
                                event.getData(), event.getTag()));

            std::cout << "\t COMPARE-SPLIT Event scheduled FOR [ " << my_rank
                      << " ] " << " AT ARRIVAL TIME: " << expected_arrival_time
                      << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
void EventSimulator::processCompareSplitEvent(const Event &event)
{
    double event_process_time = event.getTime();
    if (event_process_time >= current_time_)
        setCurrentTime(event_process_time);

    bool isOddPhase = (event.getDestRank() == 1);
    std::string phaseName = isOddPhase ? "(ODD PHASE)" : "(EVEN_PHASE)";
    std::cout << "\n[Event Time: " << current_time_ << "] Starting COMPARE - SPLIT event" << phaseName << ":"
              << std::endl;

    auto p = findProcessor(event.getSourceRank());

    // std::cout << "Processor: [" << p->getRank() << " ]" << std::endl;

    // printVector(p->getData(), "Local cache");
    // printVector(p->getReceived(), "Received cache");

    // LOCAL sort before compare-split
    p->localSort();

    // Handle compare-split logic in processor cache
    p->handleMerge(isOddPhase);

    std::cout << "\n[Event Time: " << current_time_ << "] Completed COMPARE - SPLIT event:"
              << std::endl;
}

// toString() function to log events easily
std::string EventSimulator::toStringEvent(const Event &event, double current_time) const
{
    std::ostringstream oss;

    // Convert EventType enum to string
    std::string type_str;
    switch (event.getType())
    {
    case EventType::SEND:
        type_str = "SEND";
        break;
    case EventType::RECV:
        type_str = "RECV";
        break;
    case EventType::START_SORT:
        type_str = "START_SORT";
        break;
    case EventType::COMPARE_SPLIT:
        type_str = "COMPARE_SPLIT";
        break;

    default:
        type_str = "UNKNOWN_TYPE";
        break;
    }

    oss << "Time: " << current_time << ", ";
    oss << "Type: " << type_str << ", ";
    oss << "Src: " << event.getSourceRank() << ", ";
    oss << "Dest: " << event.getDestRank() << ", ";
    oss << "Tag: " << event.getTag() << "\n";
    oss << "Data: [";


    for (size_t i = 0; i < event.getData().size(); ++i)
    {
        oss << event.getData()[i];
        if (i != event.getData().size() - 1)
            oss << ", ";
    }

    oss << "]\n";

    return oss.str();
}
