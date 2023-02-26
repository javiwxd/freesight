#ifndef HTTP_GET_H
#define HTTP_GET_H

#include <json/json.h>
#include <cpr/cpr.h>

namespace http {
    inline Json::Value get_json(const std::string& url) {
        cpr::Response r = cpr::Get(cpr::Url{ url });

        Json::Reader reader;
        Json::Value obj;

        if (!reader.parse(r.text, obj))
        {
            logger::log_error(std::source_location::current(), "Error al parsear el JSON de la URL: ", url);
            return {};
        } else {
            return obj;
        }
    }

    inline cpr::Response get(const std::string& url) {
        cpr::Response r = cpr::Get(cpr::Url{ url });
        return r;
    }
}
#endif //HTTP_GET_H
