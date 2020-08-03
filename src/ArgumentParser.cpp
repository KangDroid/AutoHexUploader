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
        case ERR_WEB_INFO_ARGS:
            cout << "Invalid argument on --web_info\nPlease provide appropriate FULL path to file." << endl;
        break;
    }
}

int ArgumentParser::getPrinterCount() {
    return this->prt_count;
}

void ArgumentParser::setprt_count(int ctr) {
    this->prt_count = ctr;
}

bool ArgumentParser::parser_args(int argc, char** argv) {
    string function_code = string(__func__);
    string tmp_logger = "";
    for (int i = 0; i < argc; i++) {
        tmp_logger += string(argv[i]) + " ";
    }
    LOG_V("Argument Count: " + to_string(argc) + "\n" + "Arguments: " + tmp_logger);
    // Parse function
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--duration")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_DURATION_ARGS);
                LOG_E("Value is not specified for --duration");
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
                    LOG_E("Duration: Number should be between 0 ~ 999, input was: " + number);
                    return false;
                }

                // Check prefix 0
                if (number.at(0) == '0') {
                    call_error(ERR_DURATION_ARGS);
                    LOG_E("Duration: Prefix 0 is not supported");
                    return false;
                }

                // Check whether this number is actual integer.
                int converted_number;
                try {
                    converted_number = stoi(number);
                } catch (const exception& expn) {
                    // Cannot parse as int
                    call_error(ERR_DURATION_ARGS);
                    LOG_E("Duration: Cannot convert number: " + number);
                    return false;
                }

                if (converted_number < 1 || converted_number > 999) {
                    call_error(ERR_DURATION_ARGS);
                    LOG_E("Duration: Number should be between 0 ~ 999, input was: " + number);
                    return false;
                }

                // Check duration
                if (dur_specifier != "hour" && dur_specifier != "month" && dur_specifier != "week" && dur_specifier != "day" && dur_specifier != "minute")  {
                    call_error(ERR_DURATION_ARGS);
                    LOG_E("Duration: Unsupported duration: " + dur_specifier);
                    return false;
                }

                // set it
                shared_variable->duration = dur_specifier;
                shared_variable->duration_number = converted_number;
                shared_variable->is_used[0] = true;
            } else {
                call_error(ERR_DURATION_ARGS);
                LOG_E("Duration: Specifier not found: " + argv_to_string);
                return false;
            }
        } else if (!strcmp(argv[i], "--web_info")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_WEB_INFO_ARGS);
                LOG_E("Error on web info argument: " + string(argv[i]));
                return false;
            }
            string tmp_store = string(argv[i]);
            if (!filesystem::exists(tmp_store)) {
                // File does not exists
                LOG_E("Specified file does not exists!\nSpecified File Path: " + tmp_store);
                return false;
            }

            // Parse it
            ifstream input_stream(tmp_store);
            Json::Value main_json;
            Json::Reader tmp_reader;
            if (!tmp_reader.parse(input_stream, main_json)) {
                // error
                LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
                return false;
            }

            // For now, support single one.
            prt_count = main_json.size();
            LOG_V("Total " + to_string(prt_count) + " printers are detected");
            *printer_info = new PrinterInfo[prt_count];
            for (int i = 0; i < prt_count; i++) {
                Json::Value cur_printer = main_json[i];

                // Printer Type
                string printer_type_tmp = cur_printer["printer_type"].asString();
                if (printer_type_tmp != "CoreM" && printer_type_tmp != "SlideFast" && printer_type_tmp != "CoreM_Multi" && printer_type_tmp != "Lugo") {
                    // Error - not supported
                    LOG_E("Unsupported printer type detected, specified printer was: " + printer_type_tmp);
                    return false;
                }
                shared_variable->printer_type = printer_type_tmp;

                // ApiKey
                string apikey = cur_printer["api_key"].asString();
                if (apikey.length() != 32) {
                    // Something happened with apkey
                    LOG_E("!!!APIKEY MUST BE 32-BIT LENGTH!!!");
                    LOG_E("SOMEONE MIGHT MANIPULATED PROGRAM!");
                    LOG_E("ABORTING PROGRAM TO PROTECT OVERALL SYSTEM!!!");
                    LOG_E("Specified Apikey: " + apikey);
                    LOG_E("Apikey Length: " + to_string(apikey.length()));
                    return false;
                }
                
                // URL
                string url = cur_printer["main_url"].asString();
                if (url.find("http") == string::npos) {
                    // HTTP Protocol not found.
                    LOG_E("Something wrong about main_url, It usally happens when main_url its syntax is wrong.");
                    LOG_E("Specified URL: " + url);
                    return false;
                }

                // PORT
                string port = to_string(cur_printer["port"].asInt());

                // force - skip for now.

                // Set printer information
                (*printer_info)[i].set_command_info(url, apikey, port);
            }
            shared_variable->is_used[1] = true;
        } else {
            // Need to handle "Unknown args"
            call_error(ERR_UNKNOWN_ARGS);
            LOG_E("Unknown Argument Detected" + string(argv[i]));
            return false;
        }
    }
    // Check whether all arguments are used
    if (shared_variable->isFullused()) {
        // All arguments are used
        LOG_V("Successfully initiated arguments.");
        return true;
    } else {
        // Maybe not all argumente are not used.
        cout << "All arguments are needed, supported arguments are: --printer_type, --duration, --force" << endl;
        LOG_E("Arguments are NOT used at all");
        return false;
    }
}

ArgumentParser::ArgumentParser(BasicVariableInfo* b, PrinterInfo** p) {
    this->shared_variable = b;
    this->printer_info = p;
}

// ArgumentParser::~ArgumentParser() {
//     if (printer_info != nullptr) {
//         delete[] printer_info;
//     }
// }
