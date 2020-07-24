#include <iostream>
#include <ctime>

#include "BasicVariableInfo.h"

class Timer {
private:
    BasicVariableInfo* shared_var;
    time_t next_execution;
    int duration_integer;
    string duration_string;
public:
    Timer(BasicVariableInfo* v);
    // returns current time in epoch timestamp
    time_t get_current_time_ts();

    // returns current time in variable type[integer]
    void get_current_time_var(int& year, int& month, int& day, int& wday, int& hour);

    // returns base time in varialbe type
    void timestamp_to_var(time_t timestamp, int& year, int& month, int& day, int& hour);

    // Create timestamp from human readable date
    time_t dateToStamp(int year, int month, int day, int hour);

    // Set Schedule
    void set_schedule();
};