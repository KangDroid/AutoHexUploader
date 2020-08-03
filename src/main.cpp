#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"
#include "PrinterInfo.h"
#include "GithubRequestManager.h"
#include "UpdateManager.h"

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
    PrinterInfo* printer_info = nullptr;
    ArgumentParser parser(&bvi, &printer_info);
    GithubRequestManager grm(&printer_info);
    UpdateManager um(&bvi, &schedule_timer, &printer_info, &parser);

    int is_parsed = parser.parser_args(argc, argv);
    if (is_parsed == 0) {
        // Updated. Need to restore bunch of information and continue.
        um.restore_total();
        schedule_timer.sleep_des();
    }
    if (is_parsed >= 0) {
        // Set Next Schedule
        schedule_timer.set_schedule();
        um.backup_total();

        while(true) {
            int printer_ctr = parser.getPrinterCount();
            for (int i = 0 ; i < printer_ctr; i++) {
                LOG_V("Working printer " + to_string(i));
                // Caall Printing Status
                bool succeed = printer_info[i].checkPrintingStatus();
                if (!succeed) {
                    // Network might be failed or whatever.
                    // 5 hour delay
                    cout << "Cannot check printing status" << endl;
                } else {
                    if (printer_info[i].getisPrinting()) {
                        // Printer is using. 5 hour delay needed.
                    } else {
                        // Since printer is not using, so back up current connection settings
                        bool succeed = printer_info[i].backupConnectionInfo();

                        if (!succeed) {
                            // Network might be failed or whatever.
                            // 5 hour delay!
                            cout << "Cannot get printing information from local octoprint server" << endl;
                        } else {
                            // Get hex file from github
                            bool succ = grm.download_hex(i);
                            if (!succ) {
                                // Download or build failed
                                return -1;
                            }

                            // upload it
                            printer_info[i].upload_printer();

                            // clean up
                            grm.cleanup();
                            printer_info[i].cleanup();

                            // reconnect it
                            bool success = printer_info[i].reconnect_server();
                            if (!success) {
                                cout << "Error" << endl;
                                Logger::close_stream();
                                return -1;
                            }
                        }
                    }
                }
            }
            if (um.update_checker()) {
                // Needs update
                string output;
                WebRequestManager wrm;
                wrm.callRequest(__LINE__, __func__, output, "/Users/kangdroid/autohexuploader/assets/build_ahx.sh");
                if (!filesystem::exists("/Users/kangdroid/autohexuploader/bin/app.out")) {
                    LOG_E("Error building/Updating files");
                } else {
                    // Back up latest info
                    um.backup_total();
                    char *const argv[] = {"/Users/kangdroid/autohexuploader/bin/app.out", "--update", NULL};
                    execv(argv[0], argv);
                    LOG_E("Error executing new program");
                }
            }
            // sleep for desired seconds
            schedule_timer.sleep_des();
        }
    }
    if (printer_info != nullptr) {
        delete[] printer_info;
    }
    Logger::close_stream();
    return 0;
}