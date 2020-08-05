#include <iostream>
#include <iomanip>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"
#include "Timer.h"
#include "PrinterInfo.h"
#include "GithubRequestManager.h"
#include "UpdateManager.h"

#define ARGS_COUNT 1

using namespace std;

int main(int argc, char** argv) {
    time_t current_time;
    time(&current_time);
    char buffer_tmp[128];
    strftime(buffer_tmp, sizeof(buffer_tmp), "%Y%m%d-%H%M%S", localtime(&current_time));
    string file_name(buffer_tmp);

    bool stream_opened = Logger::initiate_stream("LOG_" + file_name + ".log");
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
                        LOG_V("Skipping update due to printer is using..");
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
                                LOG_V("Actaully cannot download hex somehow. Skipping this");
                                continue;
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
                #if defined(__APPLE__)
                string script_file = "/Users/kangdroid/autohexuploader/assets/build_ahx.sh";
                string output_file = "/Users/kangdroid/autohexuploader/bin/app.out";
                char *const argv_execl[] = {"/Users/kangdroid/autohexuploader/bin/app.out", "--update", NULL};
                #else
                string script_file = "/home/pi/autohexuploader/assets/build_ahx.sh";
                string output_file = "/home/pi/autohexuploader/bin/app.out";
                char *const argv_execl[] = {"/home/pi/autohexuploader/bin/app.out", "--update", NULL};
                #endif
                
                wrm.callRequest(__LINE__, __func__, output, script_file);
                if (!filesystem::exists(output_file)) {
                    LOG_E("Error building/Updating files");
                } else {
                    // Back up latest info
                    um.backup_total();
                    Logger::close_stream();
                    execv(argv_execl[0], argv_execl);
                    LOG_E("Error executing new program");
                }
            }
            // sleep for desired seconds
            atomic< bool > run{ true };
            thread th{ Timer::update_file, &schedule_timer, &bvi, &printer_info, &parser, std::ref( run ) };
            schedule_timer.sleep_des();
            run = false;
            th.join();
        }
    }
    if (printer_info != nullptr) {
        delete[] printer_info;
    }
    Logger::close_stream();
    return 0;
}