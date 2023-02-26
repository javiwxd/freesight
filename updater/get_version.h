#ifndef GET_VERSION_H
#define GET_VERSION_H

#include "../http/http_get.h"

inline std::string get_latest_version() {
    Json::Value latest_data = http::get_json("https://api.github.com/repos/javiwxd/freesight/releases/latest");
    std::string version = latest_data["tag_name"].asString();
    return version;
}

inline std::string get_latest_version_url() {
    Json::Value latest_data = http::get_json("https://api.github.com/repos/javiwxd/freesight/releases/latest");
    std::string url = latest_data["assets"][0]["browser_download_url"].asString();
    return url;
}

#endif //GET_VERSION_H
