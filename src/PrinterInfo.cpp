#include "PrinterInfo.h"
bool PrinterInfo::checkPrintingStatus() {
    string func_code = string(__func__);
    Logger::log_v(func_code, "Entered.");
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/printer --header \"X-Api-Key:" + apikey + "\""; 
    Logger::log_v(func_code, "Executing Command: " + command);
    string output = "";
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        Logger::log_e(func_code, "Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::Value tmp_val = main_json["state"]["flags"];
        if (tmp_val["printing"].asString() == "true") {
            // It is printing
            this->is_printing = true;
        } else if (tmp_val["printing"].asString() == "false") {
            this->is_printing = false;
        } else {
            // Error
            Logger::log_e(func_code, "Json is parsed, but cannot get property values.");
            Logger::log_e(func_code, output);
            return false;
        }
    }
    return true;
}

bool PrinterInfo::backupConnectionInfo() {
    string func_code = string(__func__);
    Logger::log_v(func_code, "Entered.");
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\""; 
    Logger::log_v(func_code, "Executing Command: " + command);
    string output = "";
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        Logger::log_e(func_code, "Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["baudrate"].isNull() || tmp_val["port"].isNull()) {
            // Error
            cout << "Printer seems like isn't connected to octoprinter" << endl;
            Logger::log_e(func_code, "Printer seems like isn't connected to octoprinter");
            return false;
        }
        this->baudrate = tmp_val["baudrate"].asString();
        this->port = tmp_val["port"].asString();
        this->profile_name = tmp_val["printerProfile"].asString();
    }
    Logger::log_v(func_code, "Successfully finished backing up information.");
    return true;
}

void PrinterInfo::printAllInfo() {
    cout << "IS_PRINTING: " << is_printing << endl;
    cout << "bausrate: " << baudrate << endl;
    cout << "port: " << port << endl;
    cout << "profile: " << profile_name << endl;
}

bool PrinterInfo::disconnect_server() {
    string func_code = string(__func__);
    Logger::log_v(func_code, "Entered.");
    string command = "curl -s --request POST " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\" --header \"Content-Type: application/json; charset=utf-8\" -d \'{\"command\": \"disconnect\"}\'";
    string command_check = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\"";
    string output = "";

    Logger::log_v(func_code, "Executing Command: " + command);
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);

    Logger::log_v(func_code, "Executing Command: " + command_check);
    wrm.callRequest(output, command_check);
    Logger::log_v(func_code, output);

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Parse Failed
        Logger::log_e(func_code, "Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["state"].asString() == "Closed") {
            // Successfully disconnected from server
            cout << "Closed!" << endl;
            Logger::log_v(func_code, "Octoprint server reports printer is closed");
        } else {
            // It may not successfully disconnected from server
            cout << "It is not properly closed!" << endl;
            Logger::log_e(func_code, "Octoprint server reports printer isn't fully closed at all. Error!");
            // Disconnect failed
            return false;
        }
    }
    return true;
}

bool PrinterInfo::upload_printer()  {
    string func_code = string(__func__);
    Logger::log_v(func_code, "Entered.");
    string to_upload = "/Users/kangdroid/Desktop/Marlin.hex"; // This needs to be forwared from githubrequest.
    string command;
    // First disconnect from octoprint server
    bool succeed = disconnect_server();
    if (!succeed) {
        Logger::log_e(func_code, "Disconnecting server has reported as failed.");
        // ERROR;
        return false;
    }
    #if defined(__APPLE__)
    // Download arduino
    command = "wget https://www.arduino.cc/download.php?f=/arduino-1.8.13-macosx.zip -O /tmp/arduino.zip 2>&1";
    Logger::log_v(func_code, "Executing Command: " + command);
    string output;
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);


    // Unzip it
    command = "unzip /tmp/arduino.zip -d /tmp/arduino";
    Logger::log_v(func_code, "Executing Command: " + command);
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);

    // Find avrdude.conf
    string avrdude_configuration = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf";
    string avrdude_binary = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude";
    string command_upload = "\"" + avrdude_binary + "\" -D -C\"" + avrdude_configuration +"\" -patmega2560 -P" + this->port + " -cwiring -b115200 -Uflash:w:" + to_upload + " 2>&1";
    Logger::log_v(func_code, "Executing Command: " + command_upload);
    wrm.callRequest(output, command_upload);
    Logger::log_v(func_code, output);
    #else
    #endif

    Logger::log_v(func_code, "Successfully uploaded hex file to printer");
    return true;
}

bool PrinterInfo::getisPrinting() {
    return this->is_printing;
}

bool PrinterInfo::reconnect_server() {
    string func_code = string(__func__);
    Logger::log_v(func_code, "Entered.");

    // Issue Connection Request on Local Server
    string command = "curl --request POST " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\" --header \"Content-Type: application/json; charset=utf-8\" -d \'{\"command\": \"connect\", \"port\": \"" + port +"\", \"baudrate\": "+ baudrate +", \"printerProfile\": \"" + profile_name + "\", \"save\": true, \"autoconnect\": false}\'";
    Logger::log_v(func_code, "Executing Command: " + command);
    string output;
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);
    sleep(10); //Maximum timeout

    // Check printer status
    command = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey +"\"" + " 2>&1";
    Logger::log_v(func_code, "Executing Command: " + command);
    wrm.callRequest(output, command);
    Logger::log_v(func_code, output);

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Cannot parse
        Logger::log_e(func_code, "Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::Value tmp_val = main_json["current"]["state"];
        if (tmp_val.asString() != "Operational") {
            // Error: Printer is not connected after all.
            Logger::log_e(func_code, "It seems like printer is failed to reconnect server.");
            return false;
        }
    }
    // Return true
    Logger::log_v(func_code, "Successfully finished reconnecting server.");
    return true;
}

void PrinterInfo::cleanup() {
    #if defined(__APPLE__)
    // Download arduino
    system("rm /tmp/arduino.zip");
    // Unzip it
    system("rm -rf /tmp/arduino");
    #else
    #endif
}

PrinterInfo::PrinterInfo() {
    this->apikey = "F43FAA8F2FB54AE998A42737D40CADE7";
    this->url = "http://localhost";
    this->web_port = "5000";
}
