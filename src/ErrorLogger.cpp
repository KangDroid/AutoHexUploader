#include "ErrorLogger.h"
string ErrorLogger::log_full = "";

void ErrorLogger::log_e(string output) {
    string final_output = "[E]" + get_cur_time() + ":: " + output + "\n";
    log_full.append(final_output);
}

void ErrorLogger::log_v(string output) {
    string final_output = "[V]" + get_cur_time() + ":: " + output + "\n";
    log_full.append(final_output);
}

string ErrorLogger::get_cur_time() {
    time_t cur = time(NULL);
    struct tm tmp_timerst;
    char buffer[40];

    // init tm
    tmp_timerst = *localtime(&cur);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &tmp_timerst);

    return string(buffer);
}