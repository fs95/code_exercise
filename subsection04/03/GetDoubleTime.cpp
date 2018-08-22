//
// Created by fs on 8/21/18.
//

#include "GetDoubleTime.h"
#include <time.h>

double GetDoubleTime()
{
    // Get current time
    timespec tsp1{};
    clock_gettime(CLOCK_REALTIME, &tsp1); // Get high precision UTC time
    return tsp1.tv_sec + tsp1.tv_nsec/1e9;
}

long GetIntTime()
{
    time_t utcTime;
    time(&utcTime); // Get system UTC time
    return utcTime;
}

double GetDoubleTimeDiff(double lastTime)
{
    return GetDoubleTime() - lastTime;
}
