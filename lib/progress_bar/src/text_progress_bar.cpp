#include "progress_bar/text_progress_bar.hpp"

TextProgressBar::TextProgressBar(float _increment)
    : barWidth{70}, increment_{_increment}, os{std::cout}
{}

float TextProgressBar::readProgress()
{
    std::unique_lock<std::mutex> lock(mtx);
    return progress_;
}

float TextProgressBar::awaitChanges()
{
    std::unique_lock<std::mutex> lock(mtx);
    while (!changed && !end)
        waitingChanges.wait(lock);

    changed = false;
    return end ? -1 : progress_;
}

void TextProgressBar::incrementProgress()
{
    std::unique_lock<std::mutex> lock(mtx);
    changed = true;
    progress_ += increment_;
    waitingChanges.notify_one();
}

void TextProgressBar::stop()
{
    std::unique_lock<std::mutex> lock(mtx);
    end = true;
    waitingChanges.notify_one();
}

void TextProgressBar::launch(bool detach)
{
    updater = std::thread{[&](){this->updaterRoutine();}};
    if (detach)
        updater.detach();
}

void TextProgressBar::join()
{
    updater.join();
}

void TextProgressBar::updaterRoutine()
{    
    using FormatedTime = std::chrono::hh_mm_ss<std::chrono::seconds>;
    auto printBar = [&](float progress, FormatedTime timeLeft)
    {
        char timestring[20];
        int hh = timeLeft.hours().count();
        int mm = timeLeft.minutes().count();
        int ss = timeLeft.seconds().count();
        sprintf(timestring, "%02i:%02i:%02i", hh, mm, ss);
        os << "[";
        unsigned int pos = barWidth * progress;
        for (unsigned int i = 0; i < barWidth; i++) {
            if (i < pos) os << "=";
            else if (i == pos) os << ">";
            else os << " ";
        }
        os << "] " << int(progress * 100.0) << " % (" << timestring << ")\r";
        os.flush();
    };

    auto start = std::chrono::system_clock::now();
    auto timepointPrev = start;
    float progress = 0;
    FormatedTime timeLeft {};
    do {
        printBar(progress, timeLeft);
        progress = std::min(awaitChanges(), 1.0f);
        if (progress == 1.0f)
        {
            timeLeft = FormatedTime{};
            break;
        }
        auto timepoint = std::chrono::system_clock::now();
        auto updatetime = std::chrono::duration_cast<std::chrono::seconds>(timepoint - timepointPrev);
        if (updatetime.count() >= 1)
        {   
            auto timeconsumed = std::chrono::duration_cast<std::chrono::seconds>(timepoint - start);
            auto totaltime =  std::chrono::seconds{(unsigned long long)(std::round(timeconsumed.count() / progress))};
            timeLeft = FormatedTime(totaltime - timeconsumed);

            timepointPrev = timepoint;
        }
    } while (progress > 0);
    printBar(1.0f, FormatedTime{});
    os << std::endl;
}
