#ifndef __PRINTERINFO_H__
#define __PRINTERINFO_H__

#include <iostream>
#include <unistd.h>
#include "WebRequestManager.h"
#include "Logger.h"
#include "json/json.h"

using namespace std;

class PrinterInfo {
private:
    // Printer count
    int printer_count;

    // Current working printer
    int current_printer_idx;

    // for command info
    string* url;
    string* apikey;
    string* web_port;

    // for printing boolean
    bool* is_printing;

    // for profile info
    string* baudrate;
    string* port;
    string* profile_name;

    // For Web Request
    WebRequestManager wrm;
public:
    PrinterInfo();
    ~PrinterInfo();
    bool checkPrintingStatus();
    bool backupConnectionInfo();
    void printAllInfo();
    bool disconnect_server();

    bool upload_printer();

    bool getisPrinting();
    bool reconnect_server();
    void cleanup();

    void create_info_dyn(int count);
    void set_current_printer(int idx);
    void set_command_info(string& u, string& a, string& wp);

    int getPrinterCount();
};
#endif // __PRINTERINFO_H__