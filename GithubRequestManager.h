#include <iostream>
#include <filesystem>
#include "json/json.h"
#include "WebRequestManager.h"

using namespace std;

class GithubRequestManager {
private:
    bool is_connected;
    string download_url;
    string save_directory = "/tmp/tmp.zip";
    WebRequestManager wrm;
public:
    /**
     * Check whether release exists
     * this function will set download url if release exists.
     */
    void checkRelease();

    /**
     * Download hex file from github
     */
    void download_hex();

    /**
     * Buld hex from local
     */
    void build_hex();

    // Remove file
    void remove_file();
};