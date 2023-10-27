#include "task_divider.hpp"

bool PixelDivider::getNextTask(Task& task)
{
    if (i + 1 == height && j == width)
        return false;

    if (j == width)
    {
        j = 0;
        i++;
    }
    
    task.start = {i, j};
    task.end = {i + 1, j + 1};

    j++;

    return true;
}

bool RowDivider::getNextTask(Task& task)
{
    if (i == height)
        return false;

    task.start = {i, j};
    task.end = {i + 1, j + width};

    i++;

    return true;
}

bool ColumnDivider::getNextTask(Task& task)
{
    if (j == width)
        return false;

    task.start = {i, j};
    task.end = {i + height, j + 1};

    j++;

    return true;
}

bool RegionDivider::getNextTask(Task& task)
{
    if (j == width)
    {
        j = 0;
        i = numbers::min(i + regionHeight, height);
        if (i == height)
            return false;
    }
    
    task.start = {i, j};
    task.end = {numbers::min(i + regionHeight, height),
                numbers::min(j + regionWidth, width)};

    j = numbers::min(j + regionWidth, width);

    return true;
}