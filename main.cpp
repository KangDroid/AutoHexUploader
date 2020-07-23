#include <iostream>
#include <string>
#include <cstring>

// Local include
#include "errorcode.h"
#include "BasicVariableInfo.h"

#define ARGS_COUNT 3

using namespace std;

void call_error(const int errcode) {
    switch (errcode) {
        case ERR_PRINTER_TYPE_ARGS:
            cout << "Invalid argument on --printer_type\nAvailable values are:\n\"SlideFast\" \"CoreM\", \"CoreM_Multi\", \"Lugo\"" << endl;
        break;
        case ERR_DURATION_ARGS:
            cout << "Invalid argument on —duration\nAvailable values for first args are: 1 ~ 999,\nAvailable values for second args are \"weeks\", \"hour\", \"day\", \"month\"\nSpecifically, exactly \"one space\" should be inserted between first args and second args." << endl;
        break;
        case ERR_FORCE_ARGS:
            cout << "Invalid argument on --force\nAvailable values are: 0 or 1" << endl;
        break;
    }
}

int main(int argc, char** argv) {
    BasicVariableInfo bvi;
    // Parse function
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--printer_type")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_PRINTER_TYPE_ARGS);
                break;
            }
            
            if (!strcmp(argv[i], "SlideFast") || !strcmp(argv[i], "CoreM") || !strcmp(argv[i], "CoreM_Multi") || !strcmp(argv[i], "Lugo")) {
                bvi.printer_type = string(argv[i]);
                bvi.is_used[0] = true; // mark used
            } else {
                // ERROR - not supported argument
                call_error(ERR_PRINTER_TYPE_ARGS);
                break;
            }
        } else if (!strcmp(argv[i], "--duration")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_DURATION_ARGS);
                break;
            }
            string number;
            string dur_specifier;
            string argv_to_string = string(argv[i]);
            int to_cut = argv_to_string.find(" ");
            if (to_cut != string::npos) {
                number = argv_to_string.substr(0, to_cut);
                dur_specifier = argv_to_string.substr(to_cut+1, argv_to_string.length());

                // Check length of number(Should be 0 to 999);
                if (number.length() > 3) {
                    call_error(ERR_DURATION_ARGS);
                    break;
                }

                // Check prefix 0
                if (number.at(0) == '0') {
                    call_error(ERR_DURATION_ARGS);
                    break;
                }

                // Check whether this number is actual integer.
                int converted_number;
                try {
                    converted_number = stoi(number);
                } catch (const exception& expn) {
                    // Cannot parse as int
                    call_error(ERR_DURATION_ARGS);
                    break;
                }

                if (converted_number < 1 || converted_number > 999) {
                    call_error(ERR_DURATION_ARGS);
                    break;
                }

                // Check duration
                if (dur_specifier != "hour" && dur_specifier != "month" && dur_specifier != "week" && dur_specifier != "day") {
                    call_error(ERR_DURATION_ARGS);
                    break;
                }

                // set it
                bvi.duration = dur_specifier;
                bvi.duration_number = converted_number;
                bvi.is_used[1] = true;
            } else {
                call_error(ERR_DURATION_ARGS);
                break;
            }
        } else if (!strcmp(argv[i], "--force")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_FORCE_ARGS);
                break;
            }

            if (!strcmp(argv[i], "0") || !strcmp(argv[i], "1")) {
                bvi.is_force = atoi(argv[i]);
                bvi.is_used[2] = true;
            }
        } else {
            // Need to handle "Unknown args"
        }
    }
    // Check whether all arguments are used
    if (bvi.isFullused()) {
        // All arguments are used
    } else {
        // Maybe not all argumente are not used.
    }
    return 0;
}