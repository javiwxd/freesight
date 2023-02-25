#ifndef LOGGER_H
#define LOGGER_H

#include "color.h"

namespace logger {

    template<typename = std::string>
    std::string get_date_with_seconds_formatted() {

        time_t rawtime;
        struct tm timeinfo;
        char buffer[80];


        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
        std::string str(buffer);

        return str;
    }

    template<typename... Args>
    void log(const std::source_location &location, Args &&... args) {
        const char *file_name = location.file_name();
        file_name = strrchr(file_name, '\\') + 1;

        std::cout << dye::green(get_date_with_seconds_formatted()) << " - " << file_name << "(" << location.line() << ") " << dye::yellow("info") << ": ";
        (std::cout << ... << std::forward<Args>(args)) << '\n';
    }

    template <typename ...Args>
    void log_error(const std::source_location& location, Args&& ...args) {
        const char *file_name = location.file_name();
        file_name = strrchr(file_name, '\\') + 1;

        std::cout << dye::green(get_date_with_seconds_formatted()) << " - " << file_name << "(" << location.line() << ") " << dye::red("error") << ": ";
        (std::cout << ... << std::forward<Args>(args)) << '\n';
    }

    template <typename ...Args>
    void log_curl(const std::source_location& location, Args&& ...args) {
        const char *file_name = location.file_name();
        file_name = strrchr(file_name, '\\') + 1;

        std::cout << dye::green(get_date_with_seconds_formatted()) << " - " << file_name << "(" << location.line() << ") " << dye::red("error") << ": ";
        (std::cout << ... << std::forward<Args>(args)) << '\n';
    }
};


#endif //LOGGER_H
