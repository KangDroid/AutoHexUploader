#include <iostream>
#include "WebRequestManager.h"
#include "ErrorLogger.h"
#include "json/json.h"

using namespace std;

class PrinterInfo {
private:
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
};