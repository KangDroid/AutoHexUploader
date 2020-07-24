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

bool PrinterInfo::getisPrinting() {
    return this->is_printing;
}

PrinterInfo::PrinterInfo() {
    this->apikey = "F43FAA8F2FB54AE998A42737D40CADE7";
    this->url = "http://localhost";
    this->web_port = "5000";
}
