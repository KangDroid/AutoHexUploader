#ifndef __BASICVARIABLEINFO_H__
#define __BASICVARIABLEINFO_H__

#include <iostream>
#include <cpprest/http_client.h>
#include <json/json.h>

#define ARGS_COUNT 2

using namespace std;

class BasicVariableInfo {
private:
    bool parse_web_info();
public:
    string duration;
    int duration_number;
    bool is_force;
    string web_info_file;
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