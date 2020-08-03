#include <iostream>
#include <fstream>
#include <unistd.h>
#include <queue>
#include <ctime>

#include "Logger.h"
#include "json/json.h"
#include "BasicVariableInfo.h"
#include "Timer.h"
#include "PrinterInfo.h"
#include "ArgumentParser.h"

class UpdateManager {
private:
    BasicVariableInfo* bvi;
    Timer* tmr;
    PrinterInfo** pi;
    ArgumentParser* ap;
    Json::Value to_backup;

    // Back-Up Inner function
    void backup_bvi();
    void backup_timer();
    void backup_printer_info();
    void backup_ap();

    // Restore Inner function
    void restore_bvi();
    void restore_timer();
    void restore_printer_info();
    void restore_ap();
public:
    // Timer
    // Printer Info
    // Arugment Parser
    UpdateManager(BasicVariableInfo* b, Timer* t, PrinterInfo** pinfo, ArgumentParser* arg);
    void backup_total();
    void restore_total();
};