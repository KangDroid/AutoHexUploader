#include "PrinterInfo.h"
void PrinterInfo::checkPrintingStatus() {
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/printer --header \"X-Api-Key:" + apikey + "\""; 
    string output = "";
    wrm.callRequest(output, command);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        return;
    } else {
        Json::Value tmp_val = main_json["state"]["flags"];
        if (tmp_val["printing"].asString() == "true") {
            // It is printing
            this->is_printing = true;
        } else if (tmp_val["printing"].asString() == "false") {
            this->is_printing = false;
        } else {
            // Error
            return;
        }
    }
}

void PrinterInfo::backupConnectionInfo() {
    string command = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\""; 
    string output = "";
    wrm.callRequest(output, command);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        return;
    } else {
        Json::Value tmp_val = main_json["current"];
        this->baudrate = tmp_val["baudrate"].asString();
        this->port = tmp_val["port"].asString();
        this->profile_name = tmp_val["printerProfile"].asString();
    }
}

void PrinterInfo::printAllInfo() {
    cout << "IS_PRINTING: " << is_printing << endl;
    cout << "bausrate: " << baudrate << endl;
    cout << "port: " << port << endl;
    cout << "profile: " << profile_name << endl;
}

void PrinterInfo::disconnect_server() {
    string command = "curl -s --request POST " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\" --header \"Content-Type: application/json; charset=utf-8\" -d \'{\"command\": \"disconnect\"}\'";
    string command_check = "curl -s --request GET " + url + ":" + web_port + "/api/connection --header \"X-Api-Key:" + apikey + "\"";
    string output = "";
    wrm.callRequest(output, command);
    wrm.callRequest(output, command_check);
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        return;
    } else {
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["state"].asString() == "Closed") {
            // Successfully disconnected from server
            cout << "Closed!" << endl;
        } else {
            // It may not successfully disconnected from server
            cout << "It is not properly closed!" << endl;
        }
    }
}

void PrinterInfo::upload_printer()  {
    string to_upload = "/tmp/CompiledHex.hex"; // This needs to be forwared from githubrequest.
    string command;
    // First disconnect from octoprint server
    disconnect_server();
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
    system(command_upload.c_str());
    #else
    #endif
}

bool PrinterInfo::getisPrinting() {
    return this->is_printing;
}

PrinterInfo::PrinterInfo() {
    this->apikey = "F43FAA8F2FB54AE998A42737D40CADE7";
    this->url = "http://localhost";
    this->web_port = "5000";
}
