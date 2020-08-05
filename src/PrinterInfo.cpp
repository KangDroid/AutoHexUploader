#include "PrinterInfo.h"
bool PrinterInfo::checkPrintingStatus() {
    LOG_V("Entered.");
    string output;
    if (web_port == "0") {
        output = CALL_REST_OCTO("GET", url + "/api/printer", apikey);
    } else {
        output = CALL_REST_OCTO("GET", url + ":" + web_port + "/api/printer", apikey);
    }
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::StyledWriter writer;
        string json_output = writer.write(main_json);
        LOG_V(json_output);
        Json::Value tmp_val = main_json["state"]["flags"];
        if (tmp_val["printing"].asString() == "true") {
            // It is printing
            this->is_printing = true;
        } else if (tmp_val["printing"].asString() == "false") {
            this->is_printing = false;
        } else {
            // Error
            LOG_E("Json is parsed, but cannot get property values.");
            LOG_E(output);
            return false;
        }
    }
    return true;
}

bool PrinterInfo::backupConnectionInfo() {
    string func_code = string(__func__);
    LOG_V("Entered.");
    string output;
    if (web_port == "0") {
        output = CALL_REST_OCTO("GET", url + "/api/connection", apikey);
    } else {
        output = CALL_REST_OCTO("GET", url + ":" + web_port + "/api/connection", apikey);
    }
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::StyledWriter writer;
        string json_output = writer.write(main_json);
        LOG_V(json_output);
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["baudrate"].isNull() || tmp_val["port"].isNull()) {
            // Error
            cout << "Printer seems like isn't connected to octoprinter" << endl;
            LOG_E("Printer seems like isn't connected to octoprinter");
            return false;
        }
        this->baudrate = tmp_val["baudrate"].asString();
        this->port = tmp_val["port"].asString();
        this->profile_name = tmp_val["printerProfile"].asString();
    }
    LOG_V("Successfully finished backing up information.");
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
    LOG_V("Entered.");
    string output;
    if (web_port == "0") {
        cout << "Web port is 0" << endl;
        LOG_V("URLINFO: " + url + "/api/connection" );
        CALL_REST_OCTO_DISCONNECT(url + "/api/connection", apikey);
        output = CALL_REST_OCTO("GET", url + "/api/connection", apikey);
    } else {
        LOG_V("URLINFO: " + url + ":" + web_port + "/api/connection" );
        CALL_REST_OCTO_DISCONNECT(url + ":" + web_port + "/api/connection", apikey);
        output = CALL_REST_OCTO("GET", url + ":" + web_port + "/api/connection", apikey);
    }

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Parse Failed
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::StyledWriter writer;
        string json_output = writer.write(main_json);
        LOG_V(json_output);
        Json::Value tmp_val = main_json["current"];
        if (tmp_val["state"].asString() == "Closed") {
            // Successfully disconnected from server
            cout << "Closed!" << endl;
            LOG_V("Octoprint server reports printer is closed");
        } else {
            // It may not successfully disconnected from server
            cout << "It is not properly closed!" << endl;
            LOG_E("Octoprint server reports printer isn't fully closed at all. Error!");
            // Disconnect failed
            return false;
        }
    }
    return true;
}

bool PrinterInfo::upload_printer()  {
    string func_code = string(__func__);
    LOG_V("Entered.");
    string to_upload = "/Users/kangdroid/Desktop/Marlin.hex"; // This needs to be forwared from githubrequest.
    string command;
    // First disconnect from octoprint server
    bool succeed = disconnect_server();
    if (!succeed) {
        LOG_E("Disconnecting server has reported as failed.");
        // ERROR;
        return false;
    }
    #if defined(__APPLE__)
    // Download arduino
    command = "wget https://www.arduino.cc/download.php?f=/arduino-1.8.13-macosx.zip -O /tmp/arduino.zip 2>&1";
    string output;
    wrm.callRequest(__LINE__, __func__, output, command);

    // Unzip it
    command = "unzip -q /tmp/arduino.zip -d /tmp/arduino";
    wrm.callRequest(__LINE__, __func__, output, command);

    // Find avrdude.conf
    string avrdude_configuration = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf";
    string avrdude_binary = "/tmp/arduino/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude";
    string command_upload = "\"" + avrdude_binary + "\" -D -C\"" + avrdude_configuration +"\" -patmega2560 -P" + this->port + " -cwiring -b115200 -Uflash:w:" + to_upload + " 2>&1";
    wrm.callRequest(__LINE__, __func__, output, command_upload);
    #else
    // ARM-Raspberry Pi
    // Download arduino
    command = "wget https://www.arduino.cc/download.php?f=/arduino-1.8.13-linuxarm.tar.xz -O /tmp/arduino.tar.xz 2>&1";
    string output;
    wrm.callRequest(__LINE__, __func__, output, command);


    // Unzip it
    command = "mkdir /tmp/arduino";
    wrm.callRequest(__LINE__, __func__, output, command);
    command = "tar -xvf /tmp/arduino.tar.xz -C /tmp/arduino";
    wrm.callRequest(__LINE__, __func__, output, command);

    // Find avrdude.conf
    string avrdude_configuration = "/tmp/arduino/arduino-1.8.13/hardware/tools/avr/etc/avrdude.conf";
    string avrdude_binary = "/tmp/arduino/arduino-1.8.13/hardware/tools/avr/bin/avrdude";
    string command_upload = "\"" + avrdude_binary + "\" -D -C\"" + avrdude_configuration +"\" -patmega2560 -P" + this->port + " -cwiring -b115200 -Uflash:w:" + to_upload + " 2>&1";
    wrm.callRequest(__LINE__, __func__, output, command_upload);
    #endif

    LOG_V("Successfully uploaded hex file to printer");
    return true;
}

bool PrinterInfo::getisPrinting() {
    return this->is_printing;
}

bool PrinterInfo::reconnect_server() {
    string func_code = string(__func__);
    LOG_V("Entered.");

    // Issue Connection Request on Local Server
    if (web_port == "0") {
        CALL_REST_OCTO_CONNECT(url + "/api/connection", apikey, port, baudrate, profile_name);
    } else {
        CALL_REST_OCTO_CONNECT(url + ":" + web_port + "/api/connection", apikey, port, baudrate, profile_name);
    }
    
    sleep(10); //Maximum timeout

    // Check printer status
    string output = CALL_REST_OCTO("GET", url + ":" + web_port + "/api/connection", apikey);

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Cannot parse
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        Json::StyledWriter writer;
        string json_output = writer.write(main_json);
        LOG_V(json_output);
        Json::Value tmp_val = main_json["current"]["state"];
        if (tmp_val.asString() != "Operational") {
            // Error: Printer is not connected after all.
            LOG_E("It seems like printer is failed to reconnect server.");
            return false;
        }
    }
    // Return true
    LOG_V("Successfully finished reconnecting server.");
    return true;
}

void PrinterInfo::cleanup() {
    #if defined(__APPLE__)
    // Download arduino
    system("rm /tmp/arduino.zip");
    // Unzip it
    system("rm -rf /tmp/arduino");
    #else
    system("rm /tmp/arduino.tar.xz ");
    system("rm -rf /tmp/arduino");
    #endif
}

void PrinterInfo::set_printer_type(string info) {
    this->printer_type = info;
}

string PrinterInfo::get_printer_type() {
    return this->printer_type;
}

void PrinterInfo::set_command_info(string& u, string& a, string& wp) {
    this->url = u;
    this->apikey = a;
    this->web_port = wp;
}

void PrinterInfo::get_command_info(string& url_inc, string& api, string& web_port_inc) {
    url_inc = this->url;
    api = this->apikey;
    web_port_inc = this->web_port;
}

PrinterInfo::PrinterInfo() {
    this->apikey = "F43FAA8F2FB54AE998A42737D40CADE7";
    this->url = "http://localhost";
    this->web_port = "5000";
}
