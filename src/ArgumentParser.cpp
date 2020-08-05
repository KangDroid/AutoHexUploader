#include "ArgumentParser.h"

string ArgumentParser::getfile_path() {
    return this->file_path;
}

void ArgumentParser::set_cur_modfile(time_t time_us) {
    this->cur_mod_file = time_us;
}

time_t ArgumentParser::get_mod_file() {
    return this->cur_mod_file;
}

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

int ArgumentParser::parser_args(int argc, char** argv) {
    string function_code = string(__func__);
    string tmp_logger = "";
    for (int i = 0; i < argc; i++) {
        tmp_logger += string(argv[i]) + " ";
    }
    LOG_V("Argument Count: " + to_string(argc) + "\n" + "Arguments: " + tmp_logger);
    // Parse function
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--web_info")) {
            if (i < argc-1) {
                i++;
            } else {
                call_error(ERR_WEB_INFO_ARGS);
                LOG_E("Error on web info argument: " + string(argv[i]));
                return -1;
            }
            this->file_path = string(argv[i]);
            if (!filesystem::exists(file_path)) {
                // File does not exists
                LOG_E("Specified file does not exists!\nSpecified File Path: " + file_path);
                return -1;
            }
            filesystem::path path_in_fs = this->file_path;
            auto file_time = filesystem::last_write_time(path_in_fs);
            this->cur_mod_file = decltype(file_time)::clock::to_time_t(file_time);

            // Parse it
            ifstream input_stream(file_path);
            Json::Value main_json;
            Json::Reader tmp_reader;
            if (!tmp_reader.parse(input_stream, main_json)) {
                // error
                LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
                return -1;
            }

            // Parse duration information
            Json::Value duration_info = main_json[0];
            if (duration_info["duration_int"].isNull() || duration_info["duration_string"].isNull()) {
                LOG_E("Cannot find information about timer");
                return -1;
            }
            int duration_num = duration_info["duration_int"].asInt();
            string duration_specifier = duration_info["duration_string"].asString();

            if (duration_num < 1 || duration_num > 999) {
                call_error(ERR_DURATION_ARGS);
                LOG_E("Duration: Number should be between 0 ~ 999, input was: " + to_string(duration_num));
                return -1;
            }

            // Check duration
            if (duration_specifier != "hour" && duration_specifier != "month" && duration_specifier != "week" && duration_specifier != "day" && duration_specifier != "minute")  {
                call_error(ERR_DURATION_ARGS);
                LOG_E("Duration: Unsupported duration: " + duration_specifier);
                return -1;
            }

            shared_variable->duration = duration_specifier;
            shared_variable->duration_number = duration_num;

            // For now, support single one.
            prt_count = main_json.size() - 1;
            LOG_V("Total " + to_string(prt_count) + " printers are detected");
            *printer_info = new PrinterInfo[prt_count];
            for (int i = 1; i < prt_count+1; i++) {
                Json::Value cur_printer = main_json[i];

                // Printer Type
                string printer_type_tmp = cur_printer["printer_type"].asString();
                if (printer_type_tmp != "CoreM" && printer_type_tmp != "SlideFast" && printer_type_tmp != "CoreM_Multi" && printer_type_tmp != "Lugo") {
                    // Error - not supported
                    LOG_E("Unsupported printer type detected, specified printer was: " + printer_type_tmp);
                    return -1;
                }

                // ApiKey
                string apikey = cur_printer["api_key"].asString();
                if (apikey.length() != 32) {
                    // Something happened with apkey
                    LOG_E("!!!APIKEY MUST BE 32-BIT LENGTH!!!");
                    LOG_E("SOMEONE MIGHT MANIPULATED PROGRAM!");
                    LOG_E("ABORTING PROGRAM TO PROTECT OVERALL SYSTEM!!!");
                    LOG_E("Specified Apikey: " + apikey);
                    LOG_E("Apikey Length: " + to_string(apikey.length()));
                    return -1;
                }
                
                // URL
                string url = cur_printer["main_url"].asString();
                if (url.find("http") == string::npos) {
                    // HTTP Protocol not found.
                    LOG_E("Something wrong about main_url, It usally happens when main_url its syntax is wrong.");
                    LOG_E("Specified URL: " + url);
                    return -1;
                }

                // PORT
                string port = to_string(cur_printer["port"].asInt());

                // force - skip for now.

                // Set printer information
                LOG_V("Setting Value for PI: " + to_string(i-1));
                (*printer_info)[i-1].set_command_info(url, apikey, port);
                (*printer_info)[i-1].set_printer_type(printer_type_tmp);
            }
            shared_variable->is_used[0] = true;
        } else if (!strcmp(argv[i], "--update")) {
            // Update detected.
            return 0;
        } else {
            // Need to handle "Unknown args"
            call_error(ERR_UNKNOWN_ARGS);
            LOG_E("Unknown Argument Detected" + string(argv[i]));
            return -1;
        }
    }
    // Check whether all arguments are used
    if (shared_variable->isFullused()) {
        // All arguments are used
        LOG_V("Successfully initiated arguments.");
        return 1;
    } else {
        // Maybe not all argumente are not used.
        cout << "All arguments are needed, supported arguments are: --printer_type, --duration, --force" << endl;
        LOG_E("Arguments are NOT used at all");
        return -1;
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
