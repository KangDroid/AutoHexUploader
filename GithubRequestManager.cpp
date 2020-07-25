#include "GithubRequestManager.h"
void GithubRequestManager::checkRelease() {   
    string command = " curl -s -H \"Accept: application/vnd.github.v3+json\" https://api.github.com/repos/KangDroid/Marlin/releases";
    string output;
    wrm.callRequest(output, command);

    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error
        return;
    } else {
        if (main_json.size() < 1) {
            // Error: Release is not found
            // Need to fall back compilation
            this->is_connected = false;
            return;
        } else {
            // Newest one comes first.
            Json::Value tmp_json = main_json[0]["assets"];
            if (tmp_json.size() < 1) {
                // Error: Archive not found
                // Need to fall back compilation
                this->is_connected = false;
                return;
            }
            this->download_url = tmp_json[0]["browser_download_url"].asString();
            this->is_connected = true;
        }
    }
}

void GithubRequestManager::download_hex() {
    if (!this->is_connected) {
        // Need to call build_hex()
        cout << "Calling build_hex" << endl;
        return;
    }
    cout << "Downloading Files..." << endl;
    string command = "curl -s -L "  + this->download_url + " > " + this->save_directory;
    system(command.c_str());

    if (!filesystem::exists(save_directory)) {
        cout << "It does not exists!" << endl;
        // Need to call build_hex()
        return;
    }
}

void GithubRequestManager::remove_file() {
    string command = "rm " + save_directory;
    system(command.c_str());
}