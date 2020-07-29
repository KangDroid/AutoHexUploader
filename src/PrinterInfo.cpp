#include "PrinterInfo.h"
bool PrinterInfo::checkPrintingStatus() {
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/printer --header \"X-Api-Key:" + apikey + "\""; 
    string output = "";
    wrm.callRequest(output, command);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
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
            return false;
        }
    }
    return true;
}

bool PrinterInfo::backupConnectionInfo() {
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\""; 
    string output = "";
    wrm.callRequest(output, command);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        return false;
    } else {
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["baudrate"].isNull() || tmp_val["port"].isNull()) {
            // Error
            cout << "Printer seems like isn't connected to octoprinter" << endl;
            return false;
        }
        this->baudrate = tmp_val["baudrate"].asString();
        this->port = tmp_val["port"].asString();
        this->profile_name = tmp_val["printerProfile"].asString();
    }
    return true;
}

void PrinterInfo::printAllInfo() {
    cout << "IS_PRINTING: " << is_printing << endl;
    cout << "bausrate: " << baudrate << endl;
    cout << "port: " << port << endl;
    cout << "profile: " << profile_name << endl;
}

bool PrinterInfo::disconnect_server() {
    string command = "curl -s --request POST " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\" --header \"Content-Type: application/json; charset=utf-8\" -d \'{\"command\": \"disconnect\"}\'";
    string command_check = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\"";
    string output = "";
    wrm.callRequest(output, command);
    wrm.callRequest(output, command_check);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Parse Failed
        return false;
    } else {
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["state"].asString() == "Closed") {
            // Successfully disconnected from server
            cout << "Closed!" << endl;
        } else {
            // It may not successfully disconnected from server
            cout << "It is not properly closed!" << endl;
            // Disconnect failed
            return false;
        }
    }
    return true;
}

bool PrinterInfo::upload_printer()  {
    string to_upload = "/Users/kangdroid/Desktop/Marlin.hex"; // This needs to be forwared from githubrequest.
    string command;
    // First disconnect from octoprint server
    bool succeed = disconnect_server();
    if (!succeed) {
        // ERROR;
        return false;
    }
    #if defined(__APPLE__)
    // Download arduino
    command = "wget https://www.arduino.cc/download.php?f=/arduino-1.8.13-macosx.zip -O /tmp/arduino.zip";
    system(command.c_str());
    // Unzip it
    command = "unzip /tmp/arduino.zip -d /tmp/arduino";
    system(command.c_str());
    // Find avrdude.conf
    string avrdude_configuration = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf";
    string avrdude_binary = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude";
    string command_upload = "\"" + avrdude_binary + "\" -D -C\"" + avrdude_configuration +"\" -patmega2560 -P" + this->port + " -cwiring -b115200 -Uflash:w:" + to_upload;
    cout << command_upload << endl;
    system(command_upload.c_str());
    #else
    #endif

    return true;
}

bool PrinterInfo::getisPrinting() {
    return this->is_printing;
}

bool PrinterInfo::reconnect_server() {
    // Issue Connection Request on Local Server
    string command = "curl -s --request POST " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\" --header \"Content-Type: application/json; charset=utf-8\" -d \'{\"command\": \"connect\", \"port\": \"" + port +"\", \"baudrate\": "+ baudrate +", \"printerProfile\": \"" + profile_name + "\", \"save\": true, \"autoconnect\": false}\'";
    system(command.c_str());

    // Check printer status
    command = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey +"\"";
    string output;
    wrm.callRequest(output, command);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Cannot parse
        return false;
    } else {
        Json::Value tmp_val = main_json["current"]["state"];
        if (tmp_val.asString() != "Operational") {
            // Error: Printer is not connected after all.
            return false;
        }
    }
    // Return true
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
