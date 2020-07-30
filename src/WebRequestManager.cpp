#include "WebRequestManager.h"
void WebRequestManager::callRequest(int line, string fcode, string& output, string command) {
    Logger::log_v(line, fcode, "Executing Command: " + command, false);
    char tmp_buffer[4096];
    // Initiate output
    output = "";

    FILE* file_output;
    file_output = popen(command.c_str(), "r");
    while (fgets(tmp_buffer, sizeof(tmp_buffer), file_output) != NULL) {
        string to_output = string(tmp_buffer);
        Logger::log_v(line, fcode, to_output, true);
        output += to_output;
    }
    pclose(file_output);
}
