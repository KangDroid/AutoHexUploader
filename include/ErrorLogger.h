#ifndef __ERRORLOGGER_H__
#define __ERRORLOGGER_H__

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

class ErrorLogger {
private:
    string path_to_save;
    ofstream save;
public:
    ErrorLogger(string path);
    void write_file();
    static string log_full;
    // Log_E for Error logging
    static void log_e(string fcode, string output);

    // Log_V for verbose logging
    static void log_v(string fcode, string output);

    // Timer function
    static string get_cur_time();

    static void print_all() {
        cout << log_full << endl;
    }
};
#endif // __ERRORLOGGER_H__