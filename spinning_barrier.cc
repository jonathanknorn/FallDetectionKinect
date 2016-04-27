#include "spinning_barrier.h"

SpinningBarrier::SpinningBarrier (unsigned int n) : n_threads(n), nwait(0), step(0) {}

bool SpinningBarrier::wait() {
    unsigned int step_n = step.load();

    if (nwait.fetch_add (1) == n_threads - 1) {
        // Last thread to come.
        nwait.store(0);
        step.fetch_add(1);
        return true;
    } else {
        // Busy wait
        while (step.load () == step_n){}
        
        return false;
    }
}