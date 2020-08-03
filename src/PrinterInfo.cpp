#include "PrinterInfo.h"
bool PrinterInfo::checkPrintingStatus() {
    LOG_V("Entered.");
    string output = CALL_REST_OCTO("GET", url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/printer", apikey[current_printer_idx]);
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
            this->is_printing[current_printer_idx] = true;
        } else if (tmp_val["printing"].asString() == "false") {
            this->is_printing[current_printer_idx] = false;
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
    string output = CALL_REST_OCTO("GET", url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/connection", apikey[current_printer_idx]);
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
        this->baudrate[current_printer_idx] = tmp_val["baudrate"].asString();
        this->port[current_printer_idx] = tmp_val["port"].asString();
        this->profile_name[current_printer_idx] = tmp_val["printerProfile"].asString();
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
    CALL_REST_OCTO_DISCONNECT(url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/connection", apikey[current_printer_idx]);
    string output = CALL_REST_OCTO("GET", url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/connection", apikey[current_printer_idx]);

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
    string command_upload = "\"" + avrdude_binary + "\" -D -C\"" + avrdude_configuration +"\" -patmega2560 -P" + this->port[current_printer_idx] + " -cwiring -b115200 -Uflash:w:" + to_upload + " 2>&1";
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
    CALL_REST_OCTO_CONNECT(url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/connection", apikey[current_printer_idx], port[current_printer_idx], baudrate[current_printer_idx], profile_name[current_printer_idx]);
    sleep(10); //Maximum timeout

    // Check printer status
    string output = CALL_REST_OCTO("GET", url[current_printer_idx] + ":" + web_port[current_printer_idx] + "/api/connection", apikey[current_printer_idx]);

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
    system("rm -rf /tmp/arduino")
    #endif
}

void PrinterInfo::create_info_dyn(int count) {
    this->printer_count = count;
    this->url = new string[count];
    this->apikey = new string[count];
    this->web_port = new string[count];
    this->is_printing = new bool[count];
    this->baudrate = new string[count];
    this->port = new string[count];
    this->profile_name = new string[count];
}

void PrinterInfo::set_command_info(string& u, string& a, string& wp) {
    url[current_printer_idx] = u;
    apikey[current_printer_idx] = a;
    web_port[current_printer_idx] = wp;
    LOG_V("Set basic information for printer " + to_string(current_printer_idx));
}

int PrinterInfo::getPrinterCount() {
    return this->printer_count;
}

void PrinterInfo::set_current_printer(int idx) {
    this->current_printer_idx = idx;
}

PrinterInfo::PrinterInfo() {
    current_printer_idx = 0;
}

PrinterInfo::~PrinterInfo() {
    if (url != nullptr) {
        delete[] url;
    }

    if (apikey != nullptr) {
        delete[] apikey;
    }

    if (web_port != nullptr) {
        delete[] web_port;
    }

    if (is_printing != nullptr) {
        delete[] is_printing;
    }

    if (baudrate != nullptr) {
        delete[] baudrate;
    }

    if (port != nullptr) {
        delete[] port;
    }

    if (profile_name != nullptr ) {
        delete[] profile_name;
    }
}
