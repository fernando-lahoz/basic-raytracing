#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cmath>
#include <chrono>
#include <cstring>

// Monitor to signal a parallel thread to update a text-based progress bar
class TextProgressBar
{
private:
    std::condition_variable waitingChanges;
	std::mutex mtx;

    const unsigned int barWidth;
    float progress_;
    std::ostream& os;
    bool changed = false, end = false, clear;

    std::thread updater;

    void updaterRoutine();
    
public:
    TextProgressBar();

    float readProgress();

    float awaitChanges();

    void incrementProgress(float increment);

    void stop(bool clear = false);

    void launch(bool detach = false);

    void join();
};
