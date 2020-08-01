#ifndef __WEBREQUESTMANAGER_H__
#define __WEBREQUESTMANAGER_H__

#include <iostream>
#include <cstdlib>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <json/json.h>
#include "Logger.h"

using namespace std;
using namespace web;

#define CALL_REST_GITHUB(request_type, url) WebRequestManager::call_rest_api(__LINE__, __func__, request_type, url, "", false)
#define CALL_REST_OCTO(request_type, url, apikey) WebRequestManager::call_rest_api(__LINE__, __func__, request_type, url, apikey, true)
#define CALL_REST_OCTO_DISCONNECT(url, apikey) WebRequestManager::call_rest_disconnect_octo(__LINE__, __func__, url, apikey)
#define CALL_REST_OCTO_CONNECT(url, apikey, port, baudrate, profile_name) WebRequestManager::call_rest_connect_octo(__LINE__, __func__, url, apikey, port, baudrate, profile_name)

class WebRequestManager {
public:
    /**
     * Execute Command and put output
     * output: this will be returned(that's why we are using reference)
     * command: to execute
     */
    static void callRequest(int line, string fcode, string& output, string command);

    static string call_rest_api(int line, string fcode, string request, string url, string apikey, bool is_octo);

    static void call_rest_disconnect_octo(int line, string fcode, string url, string apikey);
    static void call_rest_connect_octo(int line, string fcode, string url, string apikey, string port, string baudrate, string profile_name);
};
#endif // __WEBREQUESTMANAGER_H__