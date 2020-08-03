#ifndef __ARGUMENTPARSER_H__
#define __ARGUMENTPARSER_H__

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
    int parser_args(int argc, char** argv);
    void call_error(const int errcode);
    int getPrinterCount();
    void setprt_count(int ctr);
};
#endif // __ARGUMENTPARSER_H__