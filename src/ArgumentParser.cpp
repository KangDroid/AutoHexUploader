#include "ArgumentParser.h"

void ArgumentParser::call_error(const int errcode) { 
    switch (errcode) {
        case ERR_PRINTER_TYPE_ARGS:
            cout << "Invalid argument on --printer_type\nAvailable values are:\n\"SlideFast\" \"CoreM\", \"CoreM_Multi\", \"Lugo\"" << endl;
        break;
        case ERR_DURATION_ARGS:
            cout << "Invalid argument on —-duration\nAvailable values for first args are: 1 ~ 999,\nAvailable values for second args are \"weeks\", \"hour\", \"day\", \"month\"\nSpecifically, exactly \"one space\" should be inserted between first args and second args." << endl;
        break;
        case ERR_FORCE_ARGS:
            cout << "Invalid argument on --force\nAvailable values are: 0 or 1" << endl;
        break;
        case ERR_UNKNOWN_ARGS:
            cout << "Invalid argument detected.\nPlease check argument." << endl;
        break;
    }
}

bool ArgumentParser::parser_args(int argc, char** argv) {
    // Parse function
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--printer_type")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_PRINTER_TYPE_ARGS);
                return false;
            }
            
            if (!strcmp(argv[i], "SlideFast") || !strcmp(argv[i], "CoreM") || !strcmp(argv[i], "CoreM_Multi") || !strcmp(argv[i], "Lugo")) {
                shared_variable->printer_type = string(argv[i]);
                shared_variable->is_used[0] = true; // mark used
            } else {
                // ERROR - not supported argument
                call_error(ERR_PRINTER_TYPE_ARGS);
                return false;
            }
        } else if (!strcmp(argv[i], "--duration")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_DURATION_ARGS);
                return false;
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
                    return false;
                }

                // Check prefix 0
                if (number.at(0) == '0') {
                    call_error(ERR_DURATION_ARGS);
                    return false;
                }

                // Check whether this number is actual integer.
                int converted_number;
                try {
                    converted_number = stoi(number);
                } catch (const exception& expn) {
                    // Cannot parse as int
                    call_error(ERR_DURATION_ARGS);
                    return false;
                }

                if (converted_number < 1 || converted_number > 999) {
                    call_error(ERR_DURATION_ARGS);
                    return false;
                }

                // Check duration
                if (dur_specifier != "hour" && dur_specifier != "month" && dur_specifier != "week" && dur_specifier != "day") {
                    call_error(ERR_DURATION_ARGS);
                    return false;
                }

                // set it
                shared_variable->duration = dur_specifier;
                shared_variable->duration_number = converted_number;
                shared_variable->is_used[1] = true;
            } else {
                call_error(ERR_DURATION_ARGS);
                return false;
            }
        } else if (!strcmp(argv[i], "--force")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_FORCE_ARGS);
                return false;
            }

            if (!strcmp(argv[i], "0") || !strcmp(argv[i], "1")) {
                shared_variable->is_force = atoi(argv[i]);
                shared_variable->is_used[2] = true;
            }
        } else {
            // Need to handle "Unknown args"
            call_error(ERR_UNKNOWN_ARGS);
            return false;
        }
    }
    // Check whether all arguments are used
    if (shared_variable->isFullused()) {
        // All arguments are used
        return true;
    } else {
        // Maybe not all argumente are not used.
        cout << "All arguments are needed, supported arguments are: --printer_type, --duration, --force" << endl;
        return false;
    }
}

ArgumentParser::ArgumentParser(BasicVariableInfo* b) {
    this->shared_variable = b;
}
