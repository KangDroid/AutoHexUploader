#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"
#include "PrinterInfo.h"
#include "GithubRequestManager.h"

#define ARGS_COUNT 3

using namespace std;

int main(int argc, char** argv) {
    BasicVariableInfo bvi;
    Timer schedule_timer(&bvi);
    ArgumentParser parser(&bvi);
    PrinterInfo printer_info;
    GithubRequestManager grm(&bvi);

    bool is_parsed = parser.parser_args(argc, argv);
    if (is_parsed) {
        // Set Next Schedule
        schedule_timer.set_schedule();

        // Caall Printing Status
        bool succeed = printer_info.checkPrintingStatus();
        if (!succeed) {
            // Network might be failed or whatever.
            // 5 hour delay
            cout << "Cannot check printing status" << endl;
        } else {
            if (printer_info.getisPrinting()) {
                // Printer is using. 5 hour delay needed.
            } else {
                // Since printer is not using, so back up current connection settings
                bool succeed = printer_info.backupConnectionInfo();

                if (!succeed) {
                    // Network might be failed or whatever.
                    // 5 hour delay!
                    cout << "Cannot ge t printing information from local octoprint server" << endl;
                } else {
                    // Get hex file from github
                    bool succ = grm.download_hex();
                    if (!succ) {
                        // Download or build failed
                        return -1;
                    }

                    // upload it
                    printer_info.upload_printer();

                    // clean up
                    grm.cleanup();
                    printer_info.cleanup();

                    // reconnect it
                    bool success = printer_info.reconnect_server();
                    if (!success) return -1;
                }
            }
        }
    }
    ErrorLogger::print_all();
    return 0;
}