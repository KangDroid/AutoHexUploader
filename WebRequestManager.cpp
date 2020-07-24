#include "WebRequestManager.h"
void WebRequestManager::callRequest(string& output, string command) {
    char tmp_buffer[4096];
    // Initiate output
    output = "";

    FILE* file_output;
    file_output = popen(command.c_str(), "r");
    while (fgets(tmp_buffer, sizeof(tmp_buffer), file_output) != NULL) {
        output += string(tmp_buffer);
    }
    pclose(file_output);
}
