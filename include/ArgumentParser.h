#include <cstring>
#include <filesystem>

#include "BasicVariableInfo.h"
#include "errorcode.h"
#include "Logger.h"
#include "PrinterInfo.h"

class ArgumentParser {
private:
    BasicVariableInfo* shared_variable;
    PrinterInfo** printer_info;
    int prt_count;
public:
    ArgumentParser(BasicVariableInfo* b, PrinterInfo** p);
    //~ArgumentParser();
    bool parser_args(int argc, char** argv);
    void call_error(const int errcode);
    int getPrinterCount();
};