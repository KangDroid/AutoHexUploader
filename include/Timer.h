#include <iostream>
#include <ctime>
#include <queue>

#include "BasicVariableInfo.h"

#define MAX_SCHEDULE 7

class Timer {
private:
    BasicVariableInfo* shared_var;
    queue<time_t> next_execution;
    string duration_string;
    int year_ref;
    int month_ref;
    int day_ref;
    int hour_ref;
public:
    Timer(BasicVariableInfo* v);
    // returns current time in epoch timestamp
    time_t get_current_time_ts();

    // returns current time in variable type[integer]
    void get_current_time_var(int& year, int& month, int& day, int& hour);

    // returns base time in varialbe type
    void timestamp_to_var(time_t timestamp, int& year, int& month, int& day, int& hour);

    // Create timestamp from human readable date
    time_t dateToStamp(int year, int month, int day, int hour);

    // Set Schedule
    void set_schedule();

    // Show pushed schedule
    void show_schedule();
};