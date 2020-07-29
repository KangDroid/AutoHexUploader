#include "Logger.h"
string Logger::log_full = "";

void Logger::log_e(string function_name, string output) {
    string final_output = get_cur_time() + "::" + "[E]/" + function_name + ": " + output + "\n";
    log_full.append(final_output);
}

void Logger::log_v(string function_name, string output) {
    string final_output = get_cur_time() + "::" + "[V]/" + function_name + ": " + output + "\n";
    log_full.append(final_output);
}

string Logger::get_cur_time() {
    time_t cur = time(NULL);
    struct tm tmp_timerst;
    char buffer[40];

    // init tm
    tmp_timerst = *localtime(&cur);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &tmp_timerst);

    return string(buffer);
}
Logger::Logger(string path) {
    this->path_to_save = path;
    save.open(path_to_save);
}

void Logger::write_file() {
    if (!save.is_open()) {
        return;
    }

    save << log_full;
}