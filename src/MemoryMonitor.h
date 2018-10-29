/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */

#ifndef MARATHONMATCHSCORING_MEMORYMONITOR_H
#define MARATHONMATCHSCORING_MEMORYMONITOR_H

#include <mutex>
#include <thread>

// This class is used to monitor the memory usage of a given process ID.
class MemoryMonitor {
public:
    MemoryMonitor(int checkInterval, int processId);
    ~MemoryMonitor();

    // Retrieve the latest recorded memory usage information.
    unsigned long getCurrentMemory();

    // Retrieve the maximum memory usage since last reset operation.
    // A reset operation means a invocation of checkMemory(true).
    unsigned long getMaxMemory();

    // Refresh the memory usage information.
    void checkMemory(bool forceRefreshMax);

    // Start the monitoring thread.
    void start();

    // Terminate the monitoring thread.
    void terminate();

    // Join the monitoring thread.
    void join();

private:
    // The worker used to refresh memory usage periodically.
    static void worker(MemoryMonitor* mm);

private:
    // The thread.
    std::thread _thread;

    // The process to track.
    int _processId;

    // The latest memory usage.
    unsigned long currentMemory;

    // The lastest maximum memory usage.
    unsigned long maxMemory;

    // The interval in ms to update memory usage.
    int checkInterval;

    // Indicate whether we should break the monitoring loop.
    bool terminated;

    // Indicate whether we have launched the monitoring thread.
    bool started;

    // The thread locker.
    std::mutex _mutex;
};


#endif //MARATHONMATCHSCORING_MEMORYMONITOR_H
