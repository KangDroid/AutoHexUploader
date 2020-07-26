#ifndef __BASICVARIABLEINFO_H__
#define __BASICVARIABLEINFO_H__

#include <iostream>

#define ARGS_COUNT 3

using namespace std;

class BasicVariableInfo {
public:
    string printer_type;
    string duration;
    int duration_number;
    bool is_force;
    /**
     * idx --> represents argument
     * 0 --> --printer_type
     * 1 --> --duration
     * 2 --> --force
     */
    bool is_used[ARGS_COUNT];
    bool isFullused();
};
#endif // __BASICVARIABLEINFO_H__