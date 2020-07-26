#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "json/json.h"
#include "WebRequestManager.h"
#include "BasicVariableInfo.h"

using namespace std;

class GithubRequestManager {
private:
    bool is_connected;
    string download_url;
    string save_directory = "/tmp/tmp.zip";
    string file_store = "/tmp/CompiledHex.hex";
    WebRequestManager wrm;
    BasicVariableInfo* bvi;

    /**
     * Check whether release exists
     * this function will set download url if release exists.
     */
    void checkRelease();

    /**
     * Buld hex from local
     */
    void build_hex();
public:
    GithubRequestManager(BasicVariableInfo* b) {
        this->bvi = b;
    }

    /**
     * Download hex file from github
     */
    void download_hex();

    // Remove file
    void cleanup();
};