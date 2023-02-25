#ifndef MANAGER_H
#define MANAGER_H

#include "../logger/logger.h"

namespace manager {
    extern int download_file(_In_opt_ LPUNKNOWN, const std::string& url, const std::string& output_file, const std::string& file_name);
}

#endif //MANAGER_H
