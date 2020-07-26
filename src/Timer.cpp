#include "Timer.h"
time_t Timer::get_current_time_ts() {
    struct tm tm_container;
    time_t time_cur;
    time(&time_cur);
    return time_cur;
}

time_t Timer::dateToStamp(int year, int month, int day, int hour) {
    struct tm  tm;
    time_t rawtime;
    time(&rawtime);
    tm = *localtime(&rawtime);
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    
    time_t final_tmp = mktime(&tm);
    return final_tmp;
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

void Timer::timestamp_to_var(time_t timestamp, int& year, int& month, int& day, int& hour) {   
    struct tm tm_container;
    tm_container = *localtime(&timestamp);

    // All normalized format
    year = tm_container.tm_year + 1900;
    month = tm_container.tm_mon + 1;
    day = tm_container.tm_mday;
    hour = tm_container.tm_hour;

    cout << "Year: " << year << " Month: " << month << " Day: " << day << " Hour: " << hour << endl;
}

void Timer::set_schedule() {   
    // Step 1. Get human-readable date
    int year, month, day, wday, hour;
    get_current_time_var(year, month, day, wday, hour);

    // Step 2. Determine duration
    // Step 3. Set new date
    cout << shared_var->duration << endl;
    if (shared_var->duration == "hour") {
        hour += shared_var->duration_number;
    } else if (shared_var->duration == "month") {
        month += shared_var->duration_number;
    } else if (shared_var->duration == "week") {
        day += 7 * shared_var->duration_number;
        cout << "Week specified" << endl;
    } else if (shared_var->duration == "day") {
        day += shared_var->duration_number;
    }
    // Step 4. convert to timestamp and set it.
    this->next_execution = dateToStamp(year, month, day, hour);
}

Timer::Timer(BasicVariableInfo* v) {
    this->shared_var = v;
    // this->duration_integer = shared_var->duration_number;
    // this->duration_string = shared_var->duration;
}
