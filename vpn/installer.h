#ifndef INSTALLER_H
#define INSTALLER_H

#include <utility>

#include "../http/manager.h"
#include "../lib/elevate.h"

namespace vpn {

    template<typename = void>
    void install(const char* file_path) {

        UINT result = MsiInstallProductA(file_path, nullptr);//"ACTION=INSTALL REBOOT=ReallySuppress QUIET=1");

        if (result == ERROR_SUCCESS)
        {
            logger::log(std::source_location::current(), "Instalación completada correctamente.");
        }
        else
        {
            logger::log_error(std::source_location::current(), "Error al instalar el programa, código de error: ", result);
        }
    }

    template<typename = void>
    void start_install() {

        if (!is_elevated()) {
            logger::log_error(std::source_location::current(), "You need to run this program as administrator");
            return;
        }

        std::string file_name = "downloads/vpn.msi";
        std::string url = "https://1111-releases.cloudflareclient.com/windows/Cloudflare_WARP_Release-x64.msi";

        if (!std::filesystem::exists(file_name))
        {
            int err = manager::download_file(nullptr, url, file_name, "Warp VPN");

            if (!err) {
                logger::log(std::source_location::current(),
                            "Instalador descargado correctamente, iniciando instalación...");
                install(file_name.c_str());
            }
        } else {
            logger::log(std::source_location::current(),
                        "Instalador ya descargado, iniciando instalación...");
            install(file_name.c_str());
        }
    }

}
#endif //INSTALLER_H
