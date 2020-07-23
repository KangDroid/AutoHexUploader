#include "Timer.h"
time_t Timer::get_current_time_ts() {
    struct tm tm_container;
    time_t time_cur;
    time(&time_cur);
    return time_cur;
}

void Timer::get_current_time_var(int& year, int& month, int& day, int& wday, int& hour) {
    struct tm tm_container;
    time_t time_cur;
    time(&time_cur);
    tm_container = *localtime(&time_cur);

    // All normalized format
    year = tm_container.tm_year + 1900;
    month = tm_container.tm_mon + 1;
    day = tm_container.tm_mday;
    wday = tm_container.tm_wday;
    hour = tm_container.tm_hour;
}

Timer::Timer(BasicVariableInfo& v) {
    this->shared_var = v;
}
