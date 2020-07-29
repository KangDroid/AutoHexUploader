#include "Timer.h"
time_t Timer::get_current_time_ts() {
    time_t time_cur;
    time(&time_cur);
    return time_cur;
}

time_t Timer::dateToStamp(int year, int month, int day, int hour, int min) {
    struct tm  tm;
    time_t rawtime;
    time(&rawtime);
    tm = *localtime(&rawtime);
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = 0;
    
    time_t final_tmp = mktime(&tm);
    return final_tmp;
}

void Timer::get_current_time_var(int& year, int& month, int& day, int& hour, int& min) {
    struct tm tm_container;
    time_t time_cur;
    time(&time_cur);
    tm_container = *localtime(&time_cur);

    // All normalized format
    year = tm_container.tm_year + 1900;
    month = tm_container.tm_mon + 1;
    day = tm_container.tm_mday;
    hour = tm_container.tm_hour;
    min = tm_container.tm_min;
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
    int year, month, day, hour, min;
    // Step 1. Get human-readable date
    get_current_time_var(year, month, day, hour, min);

    // Step 2. Determine duration
    // Step 3. Set new date
    int size_queue = next_execution.size();
    int to_iterate = MAX_SCHEDULE - size_queue;
    cout << shared_var->duration << endl;
    if (shared_var->duration == "hour") {
        for (int i = 0; i < to_iterate; i++) {
            hour += shared_var->duration_number;
            next_execution.push(dateToStamp(year, month, day, hour, min));
        }
    } else if (shared_var->duration == "month") {
        for (int i = 0; i < to_iterate; i++) {
            month += shared_var->duration_number;
            next_execution.push(dateToStamp(year, month, day, hour, min));
        }
    } else if (shared_var->duration == "week") {
        for (int i = 0; i < to_iterate; i++) {
            day += 7 * shared_var->duration_number;
            next_execution.push(dateToStamp(year, month, day, hour, min));
        }
        cout << "Week specified" << endl;
    } else if (shared_var->duration == "day") {
        for (int i = 0; i < to_iterate; i++) {
            day += shared_var->duration_number;
            next_execution.push(dateToStamp(year, month, day, hour, min));
        }
    } else if (shared_var->duration == "minute") {
        for (int i = 0; i < to_iterate; i++) {
            min += shared_var->duration_number;
            next_execution.push(dateToStamp(year, month, day, hour, min));
        }
    }
}

void Timer::show_schedule() {
    while (next_execution.size() != 0) {
        cout << next_execution.front() << endl;
        next_execution.pop();
    }
}

Timer::Timer(BasicVariableInfo* v) {
    this->shared_var = v;
    get_current_time_var(this->year_ref, this->month_ref, this->day_ref, this->hour_ref, this->min_ref);
    // this->duration_integer = shared_var->duration_number;
    // this->duration_string = shared_var->duration;
}
