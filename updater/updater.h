#ifndef UPDATER_H
#define UPDATER_H

#include "get_version.h"
#include "../lib/split.h"
#include "../lib/wzip.h"

namespace updater {

    inline void start_update() {

        std::string latest_version = get_latest_version();
        std::string current_version = VERSION;

        int latest_version_int = std::stoi(split_str(latest_version, ".").at(0) + split_str(latest_version, ".").at(1) + split_str(latest_version, ".").at(2));
        int current_version_int = std::stoi(split_str(current_version, ".").at(0) + split_str(current_version, ".").at(1) + split_str(current_version, ".").at(2));

        if (latest_version_int > current_version_int) {
            logger::log(std::source_location::current(), "Hay una nueva versión disponible: ", latest_version);
            logger::log(std::source_location::current(), "Descargando actualización...");

            std::string download_url = get_latest_version_url();

            int err = manager::download_file(nullptr, download_url, "downloads/latest.zip", "v" + latest_version);

            if (!err) {
                logger::log(std::source_location::current(), "Actualización descargada correctamente, instalando...");

                std::cout << std::endl;
                bool res = wzip::unzip("downloads/latest.zip", "temp/update/", "v" + latest_version);
                std::cout << std::endl << std::endl;

                if (res) {
                    logger::log(std::source_location::current(), "Actualización instalada correctamente, reiniciando...");

                    if(!std::filesystem::exists("elevate.bat")) {
                        std::ofstream file("elevate.bat");
                        file << "@echo off\n";
                        file << "timeout /t 3 /nobreak >nul\n";

                        file << "del downloads\\latest.zip\n";
                        file << "xcopy temp\\update\\* . /s /y /i\n";
                        file << "rmdir /s /q temp\\update\n";

                        file << "start /b /wait /high /affinity 0x00000001 /wait freesight.exe\n";
                        file << "exit\n";
                        file.close();
                    }

                    ShellExecuteA(nullptr, "open", "elevate.bat", nullptr, nullptr, SW_HIDE);
                    exit(0);
                } else {
                    logger::log_error(std::source_location::current(), "Error al instalar la actualización.");
                }

            } else {
                logger::log_error(std::source_location::current(), "Error al descargar la actualización, código de error: ", err);
            }
        } else {
            logger::log(std::source_location::current(), "No hay actualizaciones disponibles.");
        }
    }

}
#endif //UPDATER_H
