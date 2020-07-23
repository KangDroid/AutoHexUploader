#include <iostream>
#include <ctime>

#include "BasicVariableInfo.h"

class Timer {
private:
    BasicVariableInfo shared_var;
public:
    Timer(BasicVariableInfo& v);
    // returns current time in epoch timestamp
    time_t get_current_time_ts();

    // returns current time in variable type[integer]
    void get_current_time_var(int& year, int& month, int& day, int& wday, int& hour);
};