#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <ctime>
#include <queue>
#include <unistd.h>
#include <atomic>
#include <thread>

#include "BasicVariableInfo.h"
#include "Logger.h"
#include "ArgumentParser.h"

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
    int min_ref;
public:
    Timer(BasicVariableInfo* v);
    // returns current time in epoch timestamp
    time_t get_current_time_ts();

    // returns current time in variable type[integer]
    void get_current_time_var(int& year, int& month, int& day, int& hour, int& min);

    // returns base time in varialbe type
    void timestamp_to_var(time_t timestamp, int& year, int& month, int& day, int& hour, int& min);

    // Create timestamp from human readable date
    time_t dateToStamp(int year, int month, int day, int hour, int min);

    // Set Schedule
    void set_schedule();

    // Show pushed schedule
    void show_schedule();

    // sleep for desired seconds
    void sleep_des();

    void get_backup_info(int& year, int& month, int& day, int& hour, int& min, string& duration, queue<time_t>& tmp_queue);
    void restore_info(int year, int month, int day, int hour, int min, string duration, queue<time_t>& queue_restore);
    static void update_file(PrinterInfo** printer_info, ArgumentParser* ap, atomic< bool >& run );
};
#endif // __TIMER_H__