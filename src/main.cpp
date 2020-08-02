#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"
#include "PrinterInfo.h"
#include "GithubRequestManager.h"

#define ARGS_COUNT 2

using namespace std;

int main(int argc, char** argv) {
    bool stream_opened = Logger::initiate_stream("test.txt");
    if (!stream_opened) {
        cout << "Error: Logging failed at some reason." << endl;
        cout << "Skipping error logging" << endl;
    }
    BasicVariableInfo bvi;
    Timer schedule_timer(&bvi);
    PrinterInfo printer_info;
    ArgumentParser parser(&bvi, &printer_info);
    GithubRequestManager grm(&bvi);

    bool is_parsed = parser.parser_args(argc, argv);
    if (is_parsed) {
        // Set Next Schedule
        schedule_timer.set_schedule();

        while(true) {
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
                        if (!success) {
                            cout << "Error" << endl;
                            Logger::close_stream();
                            return -1;
                        }
                    }
                }
            }

            // sleep for desired seconds
            schedule_timer.sleep_des();
        }
    }
    Logger::close_stream();
    return 0;
}