#include "GithubRequestManager.h"
bool GithubRequestManager::checkRelease() {   
    string func_code = string(__func__);
    LOG_V("Entered.");
    string command = " curl -s -H \"Accept: application/vnd.github.v3+json\" https://api.github.com/repos/KangDroid/Marlin/releases";
    LOG_V("Executing Command: " + command);
    string output;
    wrm.callRequest(output, command);

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error -- Cannot parse json properly
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
        return false;
    } else {
        if (main_json.size() < 1) {
            // Error: Release is not found
            // Need to fall back compilation
            this->is_connected = false;
            LOG_E("Successfully parsed json, but Release is NOT Found, falling back to compilation process..");
            return false;
        } else {
            // Newest one comes first.
            Json::Value tmp_json = main_json[0]["assets"];
            if (tmp_json.size() < 1) {
                // Error: Archive not found
                // Need to fall back compilation
                this->is_connected = false;
                LOG_E("Successfully parsed json, release, but Archive is NOT Found, falling back to compilation process..");
                return false;
            }
            this->download_url = tmp_json[0]["browser_download_url"].asString();
            this->is_connected = true;
        }
    }
    LOG_V("Sucessfully checked release from GitHub.");
    return true;
}

bool GithubRequestManager::download_hex() {
    string func_code = string(__func__);
    LOG_V("Entered.");
    // Call CheckRelease for connectivity and availability
    checkRelease();
    if (!this->is_connected) {
        cout << "Calling build_hex" << endl;
        bool succ = build_hex();
        if (!succ) {
            // Build also failed
            LOG_E("Build Hexfile failed detected, Cannot run program more.");
            return false;
        }
    }
    cout << "Downloading Files..." << endl;
    LOG_V("Downloading files...");
    string command = "wget -O " + this->save_directory + " " + this->download_url + " 2>&1";

    LOG_V("Executing Command: " + command);
    string output;
    wrm.callRequest(output, command);
    LOG_V(output);

    if (!filesystem::exists(save_directory)) {
        cout << "It does not exists!" << endl;
        LOG_V("File is not downloaded somehow.");
        // Need to call build_hex()
        bool succ = build_hex();
        if (!succ) {
            // Build also failed
            LOG_E("Build Hexfile failed detected, Cannot run program more.");
            return false;
        }
    }
    
    // Unzip it!
    command = "unzip " + this->save_directory + " -d /tmp/tmp";
    LOG_V("Executing Command: " + command);
    wrm.callRequest(output, command);
    LOG_V(output);

    // Move it!
    command = "mv /tmp/tmp/" + bvi->printer_type + "*.hex " + file_store;
    LOG_V("Executing Command: " + command);
    wrm.callRequest(output, command);
    LOG_V(output);

    LOG_V("Successfully downloaded hex file.");
    return true;
}

bool GithubRequestManager::build_hex() {
    string func_code = string(__func__);
    // Since we are building hex from local device.
    system("./test.sh >> /tmp/buildlog");

    if (!filesystem::exists(file_store)) {
        // Build also failed
        LOG_E("Build failed somehow.");
        return false;
    } else {
        cout << "Build Complete!" << endl;
        LOG_V("Build Completed");
        return true;
    }
}

void GithubRequestManager::cleanup() {
    // remove fw directory
    string command = "rm " + save_directory;
    system(command.c_str());
    system("rm -rf /tmp/tmp");

    // remove file_store
    command = "rm " + file_store;
    system(command.c_str());
}