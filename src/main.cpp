#include <iostream>
#include <chrono>
#include <iomanip>

#include "utils.hpp"
#include "event_simulator.hpp"
#include "my_mpi.hpp"

// util signatures
void printVector(const std::vector<int> &vec, const std::string &label);
bool isSorted(const std::vector<int> &vec);
void printProcessorState(const EventSimulator &simulator);
std::vector<int> getSortedData(const EventSimulator &simulator);

int main(int argc, char *argv[])
{

    int num_processes = 10;          // Default value
    int elements_per_processor = 100; // Default value

    // Parse command line arguments
    if (argc >= 3)
    {
        num_processes = std::atoi(argv[1]);
        elements_per_processor = std::atoi(argv[2]);

        // Validate arguments
        if (num_processes <= 0 || elements_per_processor <= 0)
        {
            std::cerr << "Error: Number of processors and elements per processor must be positive!" << std::endl;
            std::cerr << "Usage: " << argv[0] << " <num_processors> <elements_per_processor>" << std::endl;
            return 1;
        }
    }
    else if (argc == 2)
    {
        std::cerr << "Error: Please provide both arguments!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <num_processors> <elements_per_processor>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 4 10" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Using default values:" <<num_processes<<" "<< elements_per_processor <<" (no arguments provided)" << std::endl;
        std::cout << "Usage: " << argv[0] << " <num_processors> <elements_per_processor>" << std::endl;
        std::cout << "Example: " << argv[0] << " 4 10" << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Starting Odd-Even Sort Simulation" << std::endl;
    std::cout << "Number of processors: " << num_processes << std::endl;
    std::cout << "Elements per processor: " << elements_per_processor << std::endl;
    std::cout << "Total elements: " << (num_processes * elements_per_processor) << std::endl;
    std::cout << std::endl;

    // Initialize the event simulator
    auto &simulator = EventSimulator::getInstance();
    simulator.init(num_processes, elements_per_processor);

    // Initialize random number array
    // and partition it to the processors
    simulator.initializeData();

    std::cout << "Initial state:" << std::endl;
    printProcessorState(simulator);
    std::cout << std::endl;

    // Measure Simulation in real-time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Run the sort simulation
    simulator.run();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Final state:" << std::endl;
    printProcessorState(simulator);
    std::cout << std::endl;

    // Get and verify the sorted data
    std::vector<int> sorted_data = getSortedData(simulator);

    std::cout << "Verification:" << std::endl;
    printVector(sorted_data, "Sorted data");
    std::cout << "Is correctly sorted: " << (isSorted(sorted_data) ? "Yes" : "No") << std::endl;
    std::cout << "Sorting time: " << duration.count() << " microseconds" << "\t"<<duration.count() / 1e+6 << " seconds" << std::endl;
    std::cout << "Simulation time: " << simulator.getCurrentTime() << " units" << std::endl;

    return 0;
}

// UTIL FUNCTIONS
void printVector(const std::vector<int> &vec, const std::string &label)
{
    std::cout << label << ": ";
    for (int val : vec)
    {
        std::cout << std::setw(4) << val << " ";
    }
    std::cout << std::endl;
}

bool isSorted(const std::vector<int> &vec)
{
    for (size_t i = 1; i < vec.size(); ++i)
    {
        if (vec[i] < vec[i - 1])
            return false;
    }
    return true;
}

void printProcessorState(const EventSimulator &simulator)
{
    const auto &processors = simulator.getProcessors();
    for (const auto &processor : processors)
    {
        std::cout << "Processor " << processor->getRank() << ": ";
        const auto &data = processor->getData();
        for (int val : data)
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<int> getSortedData(const EventSimulator &simulator)
{
    std::vector<int> result;
    const auto &processors = simulator.getProcessors();
    for (const auto &processor : processors)
    {
        const auto &data = processor->getData();
        result.insert(result.end(), data.begin(), data.end());
    }
    return result;
}