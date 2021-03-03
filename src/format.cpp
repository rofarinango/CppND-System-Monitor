#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    string time = "00:00:00";
    long t = 0;
    if(seconds > 0){
        // Hours
        t = seconds / 3600;
        time = TimeValueToString(t) + ":";
        // Minutes
        t = (seconds/60) % 60;
        time += TimeValueToString(t) + ":";
        // Seconds
        t = seconds % 60;
        time += TimeValueToString(t);
    }
    return time;
}

std::string Format::TimeValueToString(long time){
    string formatted;
    time < 10  ? formatted = "0" + std::to_string(time) : formatted = std::to_string(time);
    return formatted;
}