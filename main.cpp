#include <iostream>
#include <string>

// Local include
#include "BasicVariableInfo.h"
#include "ArgumentParser.h"

#define ARGS_COUNT 3

using namespace std;

int main(int argc, char** argv) {
    BasicVariableInfo bvi;
    ArgumentParser parser(bvi);
    bool is_parsed = parser.parser_args(argc, argv);
    return 0;
}