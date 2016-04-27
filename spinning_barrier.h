#ifndef SPINNING_BARRIER_H
#define SPINNING_BARRIER_H

#include <atomic>

class SpinningBarrier {
public:
    SpinningBarrier(unsigned int);
    bool wait();
protected:
    //Number of synchronized threads.
    const unsigned int n_threads;
    
    //Number of threads currently spinning.
    std::atomic<unsigned int> nwait;
    
    //Number of barrier syncronizations completed so far.
    std::atomic<unsigned int> step;
};
#endif
