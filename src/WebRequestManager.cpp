#include "WebRequestManager.h"
void WebRequestManager::callRequest(int line, string fcode, string& output, string command) {
    Logger::log_v(line, fcode, "Executing Command: " + command, false);
    char tmp_buffer[4096];
    // Initiate output
    output = "";

    FILE* file_output;
    file_output = popen(command.c_str(), "r");
    while (fgets(tmp_buffer, sizeof(tmp_buffer), file_output) != NULL) {
        string to_output = string(tmp_buffer);
        Logger::log_v(line, fcode, to_output, true);
        output += to_output;
    }
    pclose(file_output);
}

string WebRequestManager::call_rest_api(int line, string fcode, string request, string url, string apikey, bool is_octo) {
    using namespace web::http; 
    using namespace web::http::client; 
    web::http::method request_type;
    string ret_val;
    if (request == "GET") {
        request_type = methods::GET;
    } else if (request == "POST") {
        request_type = methods::POST;
    } else if (request == "CONNECT") {
        request_type = methods::CONNECT;
    } else if (request == "DEL") {
        request_type = methods::DEL;
    } else if (request == "HEAD") {
        request_type = methods::HEAD;
    } else if (request == "MERGE") {
        request_type = methods::MERGE;
    } else if (request == "OPTIONS") {
        request_type = methods::OPTIONS;
    } else if (request == "PATCH") {
        request_type = methods::PATCH;
    } else if (request == "PUT") {
        request_type = methods::PUT;
    } else if (request == "TRCE") {
        request_type = methods::TRCE;
    } else {
        // unsupported request
        ret_val = "KDR: Error Ocurred";
        Logger::log_e(line, fcode, "Request Failed: Unsupported Request!", false);
        Logger::log_e(line, fcode, "Request Type: " + request, false);
        return ret_val;
    }
    Logger::log_v(line, fcode, "Rest API CALLED!", false);
    Logger::log_v(line, fcode, "Request Type: " + request, false);
    Logger::log_v(line, fcode, "Request Address: " + url, false);
    http_client client(url);
    http_request request_tpr (request_type);
    if (is_octo) {
        // Octoprint request
        Logger::log_v(line, fcode, "Octoprint Request Detected.", false);
        request_tpr.headers().add("X-Api-Key", apikey);
    } else {
        // Github Reuqest
        Logger::log_v(line, fcode, "Github Request Detected.", false);
        request_tpr.headers().add("Accept", "application/vnd.github.v3+json");
    }

    try {
        client.request(request_tpr).then([line, fcode, &ret_val] (http_response hr) {
            Logger::log_v(line, fcode, "Requested Status: " + to_string(hr.status_code()), false);
            Logger::log_v(line, fcode, "Requested Content-Type: " + hr.headers().content_type(), false);
            
            if (hr.status_code() < 500 && hr.status_code() >= 400) {
                // Request Failed
                ret_val = "KDR: Error Ocurred";
                Logger::log_e(line, fcode, "Request Failed: Status is: !" + to_string(hr.status_code()), false);
            }
            ret_val = hr.extract_string(true).get();
        }).wait();
    } catch (const exception& e) {
        ret_val = "KDR: Error Ocurred";
        Logger::log_e(line, fcode, "Request Failed: Exception Occured in CPPRESTSDK!", false);
        Logger::log_e(line, fcode, e.what(), true);
        return ret_val;
    }
    return ret_val;
}

void WebRequestManager::call_rest_connect_octo(int line, string fcode, string url, string apikey, string port, string baudrate, string profile_name) {
    using namespace web::http; 
    using namespace web::http::client; 
    web::http::method request_type = methods::POST;
    string ret_val;
    Logger::log_v(line, fcode, "Rest API CALLED[Connect]!", false);
    Logger::log_v(line, fcode, "Request Type: POST", false);
    Logger::log_v(line, fcode, "Request Address: " + url, false);
    http_client client(url);
    http_request request_tpr (request_type);
    Logger::log_v(line, fcode, "Octoprint Request Detected.", false);
    request_tpr.headers().add("X-Api-Key", apikey);
    json::value post_data = json::value::object();
    post_data["command"] = json::value::string("connect");
    post_data["port"] = json::value::string(port);
    post_data["baudrate"] = json::value::number(stoi(baudrate));
    post_data["printerProfile"] = json::value::string(profile_name);
    post_data["save"] = json::value::boolean(true);
    post_data["autoconnect"] = json::value::boolean(false);
    request_tpr.set_body(post_data);
    string output = post_data.serialize();
    Json::Value main_json;
    Json::Reader tmp_reader;
    if (!tmp_reader.parse(output, main_json, false)) {
        // Error - Cannot parse
        LOG_E("Cannot parse json file, Please see detailed information: \n" + tmp_reader.getFormattedErrorMessages());
    } else {
        Json::StyledWriter writer;
        string json_output = writer.write(main_json);
        LOG_V(json_output);
    }

    try {
        client.request(request_tpr).then([line, fcode, &ret_val] (http_response hr) {
            Logger::log_v(line, fcode, "Requested Status: " + to_string(hr.status_code()), false);
            Logger::log_v(line, fcode, "Requested Content-Type: " + hr.headers().content_type(), false);
            if (hr.status_code() < 500 && hr.status_code() >= 400) {
                // Request Failed
                ret_val = "KDR: Error Ocurred";
                Logger::log_e(line, fcode, "Request Failed: Status is: !" + to_string(hr.status_code()), false);
            }
            ret_val = hr.extract_string(true).get();
        }).wait();
    } catch (const exception& e) {
        ret_val = "KDR: Error Ocurred";
        Logger::log_e(line, fcode, "Request Failed: Exception Occured in CPPRESTSDK!", false);
        Logger::log_e(line, fcode, e.what(), true);
    }
    return;
}

void WebRequestManager::call_rest_disconnect_octo(int line, string fcode, string url, string apikey) {
    using namespace web::http; 
    using namespace web::http::client; 
    web::http::method request_type = methods::POST;
    string ret_val;
    Logger::log_v(line, fcode, "Rest API CALLED[Disconnect]!", false);
    Logger::log_v(line, fcode, "Request Type: POST", false);
    Logger::log_v(line, fcode, "Request Address: " + url, false);
    http_client client(url);
    http_request request_tpr (request_type);
    Logger::log_v(line, fcode, "Octoprint Request Detected.", false);
    request_tpr.headers().add("X-Api-Key", apikey);
    json::value post_data = json::value::object();
    post_data["command"] = json::value::string("disconnect");
    request_tpr.set_body(post_data);

    try {
        client.request(request_tpr).then([line, fcode, &ret_val] (http_response hr) {
            Logger::log_v(line, fcode, "Requested Status: " + to_string(hr.status_code()), false);
            Logger::log_v(line, fcode, "Requested Content-Type: " + hr.headers().content_type(), false);
            if (hr.status_code() < 500 && hr.status_code() >= 400) {
                // Request Failed
                ret_val = "KDR: Error Ocurred";
                Logger::log_e(line, fcode, "Request Failed: Status is: !" + to_string(hr.status_code()), false);
                return;
            }
            ret_val = hr.extract_string(true).get();
        }).wait();
    } catch (const exception& e) {
        ret_val = "KDR: Error Ocurred";
        Logger::log_e(line, fcode, "Request Failed: Exception Occured in CPPRESTSDK!", false);
        Logger::log_e(line, fcode, e.what(), true);
        return;
    }
    return;
}
