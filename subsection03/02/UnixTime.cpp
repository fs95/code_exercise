// Output UTC time and high precision local time
#include <iostream>
#include <time.h>
#include <sys/time.h>

using namespace std;

int main(int argc, char const *argv[])
{
    time_t utcTime;
    struct tm *tmp;
    char buff[32];
    
    time(&utcTime); // Get system UTC time
    tmp = gmtime(&utcTime); // Seconds converted to time structure

    // Time format
    if (strftime(buff, 32, "UTC: %F %T", tmp) != 0) {
        cout << buff << endl;
    } else {
        cout << "Buffer lenth is too small." << endl;
    }

    timespec tsp;
    clock_gettime(CLOCK_REALTIME, &tsp); // Get high precision UTC time
    tmp = localtime((time_t*)&tsp.tv_sec); //Get local time
    long tv_msec = tsp.tv_nsec/(1000*1000); // Nanoseconds to milliseconds
    
    // Time format
    if (strftime(buff, 32, "Local: %F %T", tmp) != 0) {
        cout << buff << "." << tsp.tv_nsec / (1000*1000) << endl;
    } else {
        cout << "Buffer lenth is too small." << endl;
    }

    return 0;
}
