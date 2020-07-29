#include <cstring>

#include "BasicVariableInfo.h"
#include "errorcode.h"
#include "ErrorLogger.h"

class ArgumentParser {
private:
    BasicVariableInfo* shared_variable;
public:
    ArgumentParser(BasicVariableInfo* b);
    bool parser_args(int argc, char** argv);
    void call_error(const int errcode);
};