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
    float progress_ = 0;
    const float increment_;
    std::ostream& os;
    bool changed = false, end = false;

    std::thread updater;

    void updaterRoutine();
    
public:
    TextProgressBar(float _increment);

    float readProgress();

    float awaitChanges();

    void incrementProgress();

    void stop();

    void launch(bool detach = false);

    void join();
};
