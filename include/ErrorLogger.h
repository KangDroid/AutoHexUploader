#ifndef __ERRORLOGGER_H__
#define __ERRORLOGGER_H__

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

class ErrorLogger {
public:
    static string log_full;
    // Log_E for Error logging
    static void log_e(string output);

    // Log_V for verbose logging
    static void log_v(string output);

    // Timer function
    static string get_cur_time();
};
#endif // __ERRORLOGGER_H__