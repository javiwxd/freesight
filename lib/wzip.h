#ifndef WZIP_H
#define WZIP_H

#include "zip/libzippp.h"

namespace wzip {

    bool unzip(std::string file, std::string folder, std::string file_name)
    {
        libzippp::ZipArchive zf(file);
        zf.open(libzippp::ZipArchive::ReadOnly);

        std::vector<libzippp::ZipEntry> entries = zf.getEntries();
        std::vector<libzippp::ZipEntry>::iterator it;

        int i = 0;
        int last = 0;
        for (it = entries.begin(); it != entries.end(); ++it) {
            i++;
            libzippp::ZipEntry entry = *it;
            std::string name = entry.getName();
            int size = entry.getSize();

            void* binaryData = entry.readAsBinary();
            std::string textData = entry.readAsText();

            int total = entries.size();
            float perc = ((float)i / (float)total) * 100;
            int rounded = (int)round(perc);

            if (last != rounded) {
                last = rounded;
                if (rounded % 2 == 0) {
                    std::cout << ".";

                    if (rounded % 5 == 0) {
                        std::cout << rounded;
                    }
                }
            }

            std::string path = folder + name;

            std::ofstream o;
            std::filesystem::create_directories(std::filesystem::path(path).parent_path());
            o.open(path, std::ios::binary);
            o.write((char*)binaryData, size);
            o.close();
        }

        zf.close();
        return true;
    }
}

#endif //WZIP_H
