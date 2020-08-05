#ifndef __PRINTERINFO_H__
#define __PRINTERINFO_H__

#include <iostream>
#include <unistd.h>
#include <filesystem>
#include "WebRequestManager.h"
#include "Logger.h"
#include "json/json.h"

using namespace std;

class PrinterInfo {
private:
    string printer_type;
    // for command info
    string url;
    string apikey;
    string web_port;

    // for printing boolean
    bool is_printing;

    // for profile info
    string baudrate;
    string port;
    string profile_name;

    // For Web Request
    WebRequestManager wrm;
public:
    PrinterInfo();
    bool checkPrintingStatus();
    bool backupConnectionInfo();
    void printAllInfo();
    bool disconnect_server();

    bool upload_printer();

    bool getisPrinting();
    bool reconnect_server();
    void cleanup();
    void set_printer_type(string info);
    string get_printer_type();
    void set_command_info(string& u, string& a, string& wp);

    void get_command_info(string& url, string& api, string& web_port);
};
#endif // __PRINTERINFO_H__