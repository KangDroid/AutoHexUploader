#include <iostream>
#include <cstdlib>

using namespace std;

class WebRequestManager {
public:
    /**
     * Execute Command and put output
     * output: this will be returned(that's why we are using reference)
     * command: to execute
     */
    static void callRequest(string& output, string command);
};