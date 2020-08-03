#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "json/json.h"
#include "WebRequestManager.h"
#include "PrinterInfo.h"
#include "Logger.h"

using namespace std;

class GithubRequestManager {
private:
    bool is_connected;
    string download_url;
    string save_directory = "/tmp/tmp.zip";
    string file_store = "/tmp/CompiledHex.hex";
    WebRequestManager wrm;
    PrinterInfo** pi;

    /**
     * Check whether release exists
     * this function will set download url if release exists.
     */
    bool checkRelease();

    /**
     * Buld hex from local
     */
    bool build_hex();
public:
    GithubRequestManager(PrinterInfo** b) {
        this->pi = b;
    }

    /**
     * Download hex file from github
     */
    bool download_hex(int idx);

    // Remove file
    void cleanup();
};