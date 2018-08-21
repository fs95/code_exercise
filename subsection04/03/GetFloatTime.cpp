//
// Created by fs on 8/21/18.
//

#include "GetFloatTime.h"
#include <time.h>

float GetFloatTime()
{
    // Get current time
    timespec tsp1{};
    clock_gettime(CLOCK_REALTIME, & tsp1); // Get high precision UTC time
    return tsp1.tv_sec + tsp1.tv_nsec/1e9f;
}

long GetIntTime()
{
    time_t utcTime;
    time(&utcTime); // Get system UTC time
    return utcTime;
}

float GetFloatTimeDiff(float lastTime)
{
    float curTime;
    curTime = GetFloatTime();
    return curTime-lastTime;
}
