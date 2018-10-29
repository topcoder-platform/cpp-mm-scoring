/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */

#include "MemoryMonitor.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "third-party/memory_usage.h"
using namespace std;

MemoryMonitor::MemoryMonitor(int checkInterval, int processId)
        : _thread(&MemoryMonitor::worker, this), _processId(processId) {
    this->checkInterval = checkInterval;
    this->terminated = false;
    this->started = false;
    this->maxMemory = 0;
    this->currentMemory = 0;
    checkMemory(true);
}

MemoryMonitor::~MemoryMonitor() {
    if (started) {
        terminate();
        //_thread.join();
    }
}

unsigned long MemoryMonitor::getCurrentMemory() {
    return currentMemory;
}

unsigned long MemoryMonitor::getMaxMemory() {
    return maxMemory;
}

void MemoryMonitor::checkMemory(bool forceRefreshMax) {
    if (_mutex.try_lock()) {
        unsigned long memory = getCurrentRSS();
        if (memory > maxMemory || forceRefreshMax) {
            maxMemory = memory;
        }
        currentMemory = memory;
        _mutex.unlock();
    }
}

void MemoryMonitor::start() {
    if (!started) {
        started = true;
        _thread.detach();
    } else {
        cout << "error: the memory monitor has already started!" << endl;
    }
}

void MemoryMonitor::terminate() {
    this->terminated = true;
}

void MemoryMonitor::join() {
    _thread.join();
}

void MemoryMonitor::worker(MemoryMonitor* mm) {
    std::chrono::milliseconds duration(1);
    while (!mm->terminated) {
        mm->checkMemory(false);
        for (int i=0; i<mm->checkInterval; ++i) {
            if (mm->terminated) {
                break;
            }
            std::this_thread::sleep_for(duration);
        }
    }
}
