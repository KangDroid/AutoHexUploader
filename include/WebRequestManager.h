#ifndef __WEBREQUESTMANAGER_H__
#define __WEBREQUESTMANAGER_H__

#include <iostream>
#include <cstdlib>
#include "Logger.h"

using namespace std;

class WebRequestManager {
public:
    /**
     * Execute Command and put output
     * output: this will be returned(that's why we are using reference)
     * command: to execute
     */
    static void callRequest(int line, string fcode, string& output, string command);
};
#endif // __WEBREQUESTMANAGER_H__