#include "UpdateManager.h"
void UpdateManager::backup_bvi() {
    LOG_V("Backing up BasicVariableInformation...");
    to_backup["BasicVariableInformation"]["duration"] = bvi->duration;
    to_backup["BasicVariableInformation"]["duration_number"] = bvi->duration_number;
    to_backup["BasicVariableInformation"]["is_force"] = bvi->is_force;
    // Is web info needed?
    // to_backup["BasicVariableInformation"]["web_info"] = bvi.web_info_file;
}

void UpdateManager::backup_timer() {
    LOG_V("Backing up Timer...");
    int year, month, day, hour, min;
    string duration_string;
    queue<time_t> queue_backup;
    tmr->get_backup_info(year, month, day, hour, min, duration_string, queue_backup);
    to_backup["Timer"]["year_ref"] = year;
    to_backup["Timer"]["month_ref"] = month;
    to_backup["Timer"]["day_ref"] = day;
    to_backup["Timer"]["hour_ref"] = hour;
    to_backup["Timer"]["min_ref"] = min;
    to_backup["Timer"]["duration_string"] = duration_string;

    int queue_size = queue_backup.size();
    queue<time_t> tmp_q(queue_backup);
    for (int i = 0; i < queue_size; i++) {
        int time_val = tmp_q.front();
        to_backup["Timer"]["time_info"][i] = time_val;
        tmp_q.pop();
    }
}

void UpdateManager::backup_printer_info() {
    LOG_V("Backing up PrinterInfo...");
    int printer_count = ap->getPrinterCount();
    for (int i = 0; i < printer_count; i++) {
        string url, api, wp;
        (*pi)[i].get_command_info(url, api, wp);
        to_backup["PrinterInfo"][i]["url"] = url;
        to_backup["PrinterInfo"][i]["apikey"] = api;
        to_backup["PrinterInfo"][i]["web_port"] = wp;
        to_backup["PrinterInfo"][i]["printer_type"] = (*pi)[i].get_printer_type();
    }
}

void UpdateManager::backup_ap() {
    LOG_V("Backing up ArgumentParser...");
    to_backup["ArgumentParser"]["printer_count"] = ap->getPrinterCount();
}

void UpdateManager::restore_bvi() {
    LOG_V("Restoring BasicVariableInfo");
    bvi->duration = (to_backup["BasicVariableInformation"]["duration"]).asString();
    bvi->duration_number = (to_backup["BasicVariableInformation"]["duration_number"]).asInt();
    bvi->is_force = (to_backup["BasicVariableInformation"]["is_force"]).asBool();
}

void UpdateManager::restore_timer() {
    LOG_V("Restoring Timer");
    queue<time_t> queue_backup;

    int queue_size = to_backup["Timer"]["time_info"].size();
    for (int i = 0; i < queue_size; i++) {
        queue_backup.push(to_backup["Timer"]["time_info"][i].asInt());
    }
    tmr->restore_info(to_backup["Timer"]["year_ref"].asInt(), to_backup["Timer"]["month_ref"].asInt(), to_backup["Timer"]["day_ref"].asInt(), to_backup["Timer"]["hour_ref"].asInt(), to_backup["Timer"]["min_ref"].asInt(), to_backup["Timer"]["duration_string"].asString(), queue_backup);
}

void UpdateManager::restore_printer_info() {
    LOG_V("Restoring PrinterInfo");
    int printer_count = to_backup["PrinterInfo"].size();
    if (*pi == nullptr) {
        (*pi) = new PrinterInfo[printer_count];
    }
    for (int i = 0; i < printer_count; i++) {
        string url = to_backup["PrinterInfo"][i]["url"].asString();
        string api = to_backup["PrinterInfo"][i]["apikey"].asString();
        string wp = to_backup["PrinterInfo"][i]["web_port"].asString();
        string printer_type = to_backup["PrinterInfo"][i]["printer_type"].asString();
        (*pi)[i].set_command_info(url, api, wp);
        (*pi)[i].set_printer_type(printer_type);
    }
}

void UpdateManager::restore_ap() {
    LOG_V("Restoring ArgumentParser");
    ap->setprt_count(to_backup["ArgumentParser"]["printer_count"].asInt());
}

UpdateManager::UpdateManager(BasicVariableInfo* b, Timer* t, PrinterInfo** pinfo, ArgumentParser* arg) {
    this->bvi = b;
    this->tmr = t;
    this->pi = pinfo;
    this->ap = arg;
}

void UpdateManager::backup_total() {
    this->backup_bvi();
    this->backup_timer();
    this->backup_printer_info();
    this->backup_ap();
    ofstream backup_json("/tmp/backup.json");
    string json_str = to_backup.toStyledString();
    LOG_V("Backup Json: ");
    LOG_V(json_str);
    backup_json << json_str;
    backup_json.close();

    LOG_V("Successfully backed-up program profile to /tmp/backup.json");
}

void UpdateManager::restore_total() {
    ifstream input_ifs("/tmp/backup.json");
    Json::Reader json_parser;
    if (!json_parser.parse(input_ifs, to_backup)) {
        // Something wrong.
        LOG_E("Cannot parse json file, Please see detailed information: \n" + json_parser.getFormattedErrorMessages());
        return;
    }
    this->restore_bvi();
    this->restore_timer();
    this->restore_printer_info();
    this->restore_ap();
    input_ifs.close();
}

bool UpdateManager::update_checker() {
    ifstream update_json("/Users/kangdroid/Desktop/update.json"); // need fetch data from github TOOD
    string command = "curl https://raw.githubusercontent.com/KangDroid/AutoHexUploader/master/update_info.json";
    string output;
    WebRequestManager wrm;
    wrm.callRequest(__LINE__, __func__, output, command);
    Json::Reader json_parser;
    Json::Value root_val;
    if (!json_parser.parse(output, root_val)) {
        LOG_E("Cannot parse json file, Please see detailed information: \n" + json_parser.getFormattedErrorMessages());
        return false;
    }

    int version_web = root_val["version"].asInt();
    if (version_web > VERSION) {
        // Needs update
        LOG_V("Needs Update");
        LOG_V("Current Version: " + to_string(VERSION));
        LOG_V("Remote Version: " + to_string(version_web));
        return true;
    } else {
        LOG_V("Don't Need Update");
        LOG_V("Current Version: " + to_string(VERSION));
        LOG_V("Remote Version: " + to_string(version_web));
        return false;
    }
}