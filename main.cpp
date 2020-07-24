#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"

#define ARGS_COUNT 3

using namespace std;

int main(int argc, char** argv) {
    BasicVariableInfo bvi;
    Timer schedule_timer(&bvi);
    ArgumentParser parser(&bvi);
    bool is_parsed = parser.parser_args(argc, argv);
    if (is_parsed) {
        schedule_timer.set_schedule();
    }
    return 0;
}