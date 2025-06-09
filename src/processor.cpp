#include "processor.hpp"

class EventSimulator;

void Processor::setNeighbors()
{
    if (rank_ % 2 == 0)
    {
        odd_neighbor = rank_ + 1;
        even_neighbor = rank_ - 1;
    }
    else
    {
        odd_neighbor = rank_ - 1;
        even_neighbor = rank_ + 1;
    }
}

// Get new array into its cache
void Processor::receiveMessage()
{
    std::cout << "\n[Processor " << rank_ << "] Performing \"Receive\""
              << "\n  Local data: ";
    for (int val : local_data_)
    {
        std::cout << val << " ";
    }
    std::cout << "\n  Received data: ";
    for (int val : received_data_)
    {
        std::cout << val << " ";
    }
}
// Perform a local sort of the processor's data
void Processor::localSort()
{
    std::cout << "\n[Processor " << rank_ << "] Performing local sort on local caches";
    //           << "\n  Before: ";
    // for (int val : local_data_)
    // {
    //     std::cout << val << " ";
    // }
    std::sort(local_data_.begin(), local_data_.end());

    // std::cout << "\n  After:  ";
    // for (int val : local_data_)
    // {
    //     std::cout << val << " ";
    // }
    std::cout << std::endl;

    std::cout << "\n[Processor " << rank_ << "] Performing local sort on received cache";
    //           << "\n  Before: ";
    // for (int val : received_data_)
    // {
    //     std::cout << val << " ";
    // }
    std::sort(received_data_.begin(), received_data_.end());
    // std::cout << "\n  After:  ";
    // for (int val : received_data_)
    // {
    //     std::cout << val << " ";
    // }
    std::cout << std::endl;
}

// Handle merge event from event simulator
void Processor::handleMerge(bool isOddPhase)
{
    std::vector<int> res_arr;

    // double index pointer
    int i = 0, j = 0;
    int cache_size = local_data_.size();

    while (i < cache_size && j < cache_size)
    {
        if (local_data_[i] <= received_data_[j])
        {
            res_arr.push_back(local_data_[i++]);
        }
        else
        {
            res_arr.push_back(received_data_[j++]);
        }
    }

    // for remaining elements
        // Copy remaining elements from local_data_
    while (i < cache_size)
    {
        res_arr.push_back(local_data_[i++]);
    }

    // Copy remaining elements from received_data_
    while (j < cache_size)
    {
        res_arr.push_back(received_data_[j++]);
    }

    // DEBUG
    // printVector(res_arr, "Workspace");

    // DECISION OF KEEPING WHICH HALF
    /* in EVEN phase, EVEN ranks' neighbor is rank-1 ----> EVEN ranks keeps HIGHER half  */
    /* in EVEN phase, ODD ranks' neighbor is rank+1 ----> ODD ranks keeps LOWER half  */

    /* in ODD phase, EVEN ranks' neighbor is rank+1 ----> EVEN ranks keeps LOWER half  */
    /* in ODD phase, ODD ranks' neighbor is rank-1 ----> ODD ranks keeps HIGHER half  */
    bool isEvenRank = this->getRank() % 2 == 0;

    // odd phase, even rank AND even phase odd rank keeps LOWER part of the sorted array
    if (isOddPhase == isEvenRank)
    {
        for (int i = 0; i < cache_size; i++)
        {
            local_data_[i] = res_arr[i];
        }
    }
    // Even phase, even rank AND odd phase odd rank keeps HIGHER part of the sorted array
    else
    {
        for (int i = 0; i < cache_size; i++)
        {
            local_data_[i] = res_arr[i+cache_size];
        }
    }

    // std::cout << "\n[Processor " << rank_ << "] After performing compare-split:"
    //           << std::endl;
    // for (int val : local_data_)
    // {
    //     std::cout << val << " ";
    // }
}

