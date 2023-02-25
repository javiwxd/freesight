#include "regedit.h"

namespace regedit {

    int start_regedit_config() {

        if (!is_elevated()) {
            logger::log_error(std::source_location::current(), "You need to run this program as administrator");
            return 1;
        }

        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, R"(Software\Microsoft\Windows\CurrentVersion\Policies\System)", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
            logger::log_error(std::source_location::current(), "Failed to open registry key");
            logger::log(std::source_location::current(), "El administrador de tareas no está deshabilitado, saltando paso...");
            return 1;
        }

        DWORD value;
        DWORD size = sizeof(DWORD);
        if (RegQueryValueEx(hKey, "DisableTaskMgr", nullptr, nullptr, (LPBYTE)&value, &size) != ERROR_SUCCESS) {
            logger::log_error(std::source_location::current(), "Failed to read registry value");
            logger::log(std::source_location::current(), "El administrador de tareas no está deshabilitado, saltando paso...");
            RegCloseKey(hKey);
            return 1;
        }

        if (value == 1) {
            logger::log(std::source_location::current(), "Habilitando el administrador de tareas...");

            value = 0;

            if (RegSetValueEx(hKey, "DisableTaskMgr", 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS) {
                logger::log_error(std::source_location::current(), "Failed to write registry value");
                RegCloseKey(hKey);
                return 1;
            }

            logger::log(std::source_location::current(), "Administrador de tareas habilitado correctamente desde el registro de Windows.");

            return 0;
        } else {
            logger::log(std::source_location::current(), "El administrador de tareas no está deshabilitado, saltando paso...");
            RegCloseKey(hKey);
            return 1;
        }
    }
}