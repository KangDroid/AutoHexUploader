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

void Timer::timestamp_to_var(time_t timestamp, int& year, int& month, int& day, int& hour, int& min) {   
    struct tm tm_container;
    tm_container = *localtime(&timestamp);

    // All normalized format
    year = tm_container.tm_year + 1900;
    month = tm_container.tm_mon + 1;
    day = tm_container.tm_mday;
    hour = tm_container.tm_hour;
    min = tm_container.tm_min;

    //cout << "Year: " << year << " Month: " << month << " Day: " << day << " Hour: " << hour << endl;
}

void Timer::set_schedule() {
    int year, month, day, hour, min;
    // Step 1. Get human-readable date
    if (next_execution.size() > 0) {
        timestamp_to_var(next_execution.back(), year, month, day, hour, min);
    } else {
        get_current_time_var(year, month, day, hour, min);
    }

    // Step 2. Determine duration
    // Step 3. Set new date
    int size_queue = next_execution.size();
    int to_iterate = MAX_SCHEDULE - size_queue;
    LOG_V("Timer Queue Size: " + to_string(size_queue));
    LOG_V("Timer Queue Iteration Count: " + to_string(to_iterate));
    LOG_V("Timer Duration: " + shared_var->duration);
    if (shared_var->duration == "hour") {
        for (int i = 0; i < to_iterate; i++) {
            hour += shared_var->duration_number;
            time_t stamp_using = dateToStamp(year, month, day, hour, min);
            LOG_V("Next Execution[Epoch]: " + to_string(stamp_using));
            next_execution.push(stamp_using);
        }
    } else if (shared_var->duration == "month") {
        for (int i = 0; i < to_iterate; i++) {
            month += shared_var->duration_number;
            time_t stamp_using = dateToStamp(year, month, day, hour, min);
            LOG_V("Next Execution[Epoch]: " + to_string(stamp_using));
            next_execution.push(stamp_using);
        }
    } else if (shared_var->duration == "week") {
        for (int i = 0; i < to_iterate; i++) {
            day += 7 * shared_var->duration_number;
            time_t stamp_using = dateToStamp(year, month, day, hour, min);
            LOG_V("Next Execution[Epoch]: " + to_string(stamp_using));
            next_execution.push(stamp_using);
        }
        cout << "Week specified" << endl;
    } else if (shared_var->duration == "day") {
        for (int i = 0; i < to_iterate; i++) {
            day += shared_var->duration_number;
            time_t stamp_using = dateToStamp(year, month, day, hour, min);
            LOG_V("Next Execution[Epoch]: " + to_string(stamp_using));
            next_execution.push(stamp_using);
        }
    } else if (shared_var->duration == "minute") {
        for (int i = 0; i < to_iterate; i++) {
            min += shared_var->duration_number;
            time_t stamp_using = dateToStamp(year, month, day, hour, min);
            LOG_V("Next Execution[Epoch]: " + to_string(stamp_using));
            next_execution.push(stamp_using);
        }
    }
}

void Timer::show_schedule() {
    while (next_execution.size() != 0) {
        cout << next_execution.front() << endl;
        next_execution.pop();
    }
}

void Timer::update_file(Timer* tmr, BasicVariableInfo* sh_var, PrinterInfo** printer_info, ArgumentParser* ap, atomic< bool >& run) {
    //ap->set_cur_modfile(decltype(file_time)::clock::to_time_t(file_time));
    while (run) {
        string path = ap->getfile_path();
        filesystem::path path_in_fs = path;
        auto file_time = filesystem::last_write_time(path_in_fs);
        time_t tmp_time = decltype(file_time)::clock::to_time_t(file_time);
        LOG_V("Current[saved]: " + to_string(ap->get_mod_file()));
        LOG_V("File Time[cur]: " + to_string(tmp_time));
        if (ap->get_mod_file() < tmp_time) {
            ap->set_cur_modfile(tmp_time);
            ifstream input_stream(path);
            Json::Value main_json;
            Json::Reader tmp_reader;
            if (!tmp_reader.parse(input_stream, main_json)) {
                // error
                LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
                return;
            }
            // Parse duration information
            Json::Value duration_info = main_json[0];
            if (duration_info["duration_int"].isNull() || duration_info["duration_string"].isNull()) {
                LOG_E("Cannot find information about timer");
                return;
            }
            int duration_num = duration_info["duration_int"].asInt();
            string duration_specifier = duration_info["duration_string"].asString();

            if (duration_num < 1 || duration_num > 999) {
                LOG_E("Duration: Number should be between 0 ~ 999, input was: " + to_string(duration_num));
                return;
            }

            // Check duration
            if (duration_specifier != "hour" && duration_specifier != "month" && duration_specifier != "week" && duration_specifier != "day" && duration_specifier != "minute")  {
                LOG_E("Duration: Unsupported duration: " + duration_specifier);
                return;
            }

            sh_var->duration = duration_specifier;
            sh_var->duration_number = duration_num;
            tmr->clear_schedule(); // clear schedule before rescheduling it
            tmr->set_schedule();

            // For now, support single one.
            int prt_count = main_json.size()-1;
            LOG_V("Total " + to_string(prt_count) + " printers are detected");
            if (*printer_info != nullptr) {
                delete[] *printer_info;
            }
            *printer_info = new PrinterInfo[prt_count];
            for (int i = 1; i < prt_count; i++) {
                Json::Value cur_printer = main_json[i];

                // Printer Type
                string printer_type_tmp = cur_printer["printer_type"].asString();
                if (printer_type_tmp != "CoreM" && printer_type_tmp != "SlideFast" && printer_type_tmp != "CoreM_Multi" && printer_type_tmp != "Lugo") {
                    // Error - not supported
                    LOG_E("Unsupported printer type detected, specified printer was: " + printer_type_tmp);
                    return;
                }

                // ApiKey
                string apikey = cur_printer["api_key"].asString();
                if (apikey.length() != 32) {
                    // Something happened with apkey
                    LOG_E("!!!APIKEY MUST BE 32-BIT LENGTH!!!");
                    LOG_E("SOMEONE MIGHT MANIPULATED PROGRAM!");
                    LOG_E("ABORTING PROGRAM TO PROTECT OVERALL SYSTEM!!!");
                    LOG_E("Specified Apikey: " + apikey);
                    LOG_E("Apikey Length: " + to_string(apikey.length()));
                    return;
                }
                
                // URL
                string url = cur_printer["main_url"].asString();
                if (url.find("http") == string::npos) {
                    // HTTP Protocol not found.
                    LOG_E("Something wrong about main_url, It usally happens when main_url its syntax is wrong.");
                    LOG_E("Specified URL: " + url);
                    return;
                }

                // PORT
                string port = to_string(cur_printer["port"].asInt());

                // force - skip for now.

                // Set printer information
                (*printer_info)[i].set_command_info(url, apikey, port);
                (*printer_info)[i].set_printer_type(printer_type_tmp);
            }
            LOG_V("File modified! - Reconfiguration");
        }
        sleep(3);
    }
}

void Timer::sleep_des() {
    const int offset = -1;
    time_t popped_val = next_execution.front();
    time_t cur = get_current_time_ts();
    while (popped_val < cur) {
        if (next_execution.size() == 0) {
            set_schedule();
        }
        popped_val = next_execution.front(); next_execution.pop();
    }
    set_schedule(); // Check for more schedule

    time_t to_sleep = popped_val - cur;
    LOG_V("Time to sleep: " + to_string(to_sleep - offset));
    LOG_V("Cur: " + to_string(cur));
    LOG_V("Popped Val: " + to_string(popped_val));
    sleep(to_sleep - offset);
}

void Timer::clear_schedule() {
    queue<time_t> empty;
    swap(next_execution, empty);
}

void Timer::get_backup_info(int& year, int& month, int& day, int& hour, int& min, string& duration, queue<time_t>& tmp_queue) {
    year = year_ref;
    month = month_ref;
    day = day_ref;
    hour = hour_ref;
    min = min_ref;
    duration = duration_string;
    tmp_queue = this->next_execution;
}

void Timer::restore_info(int year, int month, int day, int hour, int min, string duration, queue<time_t>& queue_restore) {
    this->year_ref = year;
    this->month_ref = month;
    this->day_ref = day;
    this->hour_ref = hour;
    this->min_ref = min;
    this->duration_string = duration;
    this->next_execution = queue_restore;
}

Timer::Timer(BasicVariableInfo* v) {
    this->shared_var = v;
    this->duration_string = shared_var->duration;
    get_current_time_var(this->year_ref, this->month_ref, this->day_ref, this->hour_ref, this->min_ref);
    LOG_V("Reference Time[Timer First Call]: " + to_string(year_ref)+ "-" + to_string(month_ref) + "-" + to_string(day_ref) + ", at: " + to_string(hour_ref) + ":" + to_string(min_ref));
}
