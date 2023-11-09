#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

// Monitor to signal a parallel thread to update a text-based progress bar
class ProgressBar
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
public:
    ProgressBar(float _increment)
        : barWidth{70}, increment_{_increment}, os{std::cout}
    {}

    float readProgress()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return progress_;
    }

    float awaitChanges()
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (!changed && !end)
            waitingChanges.wait(lock);

        changed = false;
        return end ? -1 : progress_;
    }

    void incrementProgress()
    {
        std::unique_lock<std::mutex> lock(mtx);
        changed = true;
        progress_ += increment_;
        waitingChanges.notify_one();
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        end = true;
        waitingChanges.notify_one();
    }

    void update()
    {    
        auto printBar = [&](float progress)
        {
            os << "[";
            unsigned int pos = barWidth * progress;
            for (unsigned int i = 0; i < barWidth; i++) {
                if (i < pos) os << "=";
                else if (i == pos) os << ">";
                else os << " ";
            }
            os << "] " << int(progress * 100.0) << " %\r";
            os.flush();
        };

        float progress = 0;
        do {
            printBar(progress);
            progress = awaitChanges();
        } while (progress > 0);
        printBar(1.0); // 100%
        os << std::endl;
    }

    void launch(bool detach = false)
    {
        updater = std::thread{[&](){this->update();}};
        if (detach)
            updater.detach();
    }

    void join()
    {
        updater.join();
    }
};
