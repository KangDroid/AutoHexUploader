#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"
#include "PrinterInfo.h"

#define ARGS_COUNT 3

using namespace std;

int main(int argc, char** argv) {
    BasicVariableInfo bvi;
    Timer schedule_timer(&bvi);
    ArgumentParser parser(&bvi);
    PrinterInfo printer_info;

    bool is_parsed = parser.parser_args(argc, argv);
    if (is_parsed) {
        // Set Next Schedule
        schedule_timer.set_schedule();

        // Caall Printing Status
        printer_info.checkPrintingStatus();
        if (printer_info.getisPrinting()) {
            // Printer is using. 5 hour delay needed.
        } else {
            // Since printer is not using, so back up current connection settings
            printer_info.backupConnectionInfo();

            // Get hex file from github
            // upload it
        }
    }
    return 0;
}