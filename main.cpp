#include "stdafx.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "msi.lib")

#define COMPILATION_DATE __DATE__ " " __TIME__
#define VERSION "0.0.3"

#include "firewall/windows_firewall.h"
#include "logger/logger.h"
#include "logger/banner.h"
#include "config/config.h"
#include "regedit/regedit.h"
#include "vpn/installer.h"

#include "updater/updater.h"

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType);

int main(int argc, char *argv[]) {
    SetConsoleTitle("freesight - " COMPILATION_DATE);
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, true);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 07);

    logger::log(std::source_location::current(), "Booting freesight...");
    print_banner();

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--version" || arg == "-v") {
            logger::log(std::source_location::current(), "freesight version: " COMPILATION_DATE);
            return 0;
        }
        if (arg == "--warranty" || arg == "-w") {
            print_warranty();
            return 0;
        }
    }

    std::string password;
    std::cout << " -- Este programa requiere una contraseña para ser ejecutado: ";
    int ch;
    ch = _getch();
    while (ch != 13) { //enter
        if (ch == 8) {
            if (!password.empty()) { //del
                std::cout << "\b \b";
                password.pop_back();
            }
            ch = _getch();
            continue;
        }
        if (ch == 27) { //est
            return 0;
        }
        password.push_back(ch);
        std::cout << '*';
        ch = _getch();
    }
    std::cout << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (password != "ww120")
    {
        MessageBox(FindWindowA("ConsoleWindowClass", nullptr),
                    "Contraseña incorrecta, cerrando...",
                   "freesight", MB_OK | MB_ICONERROR);

        logger::log_error(std::source_location::current(), "Contraseña incorrecta, cerrando...");
        return 0;
    }

    config config_data = config();
    config_data.load_config();

    bool do_update = config_data.get_bool("auto_update", true);
    bool do_firewall = config_data.get_bool("options.firewall", true);
    bool do_regedit = config_data.get_bool("options.regedit", true);
    bool do_vpn = config_data.get_bool("options.vpn", true);

    std::string message = "Se han ejecutado los siguientes cambios: ";
    if(do_update) {
        logger::log(std::source_location::current(), "Buscando actualizaciones... (auto_update = true)");
        updater::start_update();
    } else {
        logger::log(std::source_location::current(), "No se buscarán actualizaciones (auto_update = false)");
    }

    if (do_firewall) {
        logger::log(std::source_location::current(), "Iniciando firewall... (options.firewall = true)");
        windows_firewall::start_firewall_block();
        message += "firewall block, ";
    } else {
        logger::log(std::source_location::current(), "No se bloqueará el firewall (options.firewall = false)");
    }

    if (do_regedit) {
        logger::log(std::source_location::current(), "Iniciando regedit... (options.regedit = true)");
        regedit::start_regedit_config();
        message += "regedit config, ";
    } else {
        logger::log(std::source_location::current(), "No se modificará el registro (options.regedit = false)");
    }

    if (do_vpn) {
        logger::log(std::source_location::current(), "Instalando vpn... (options.vpn = true)");
        vpn::start_install();
        message += "vpn install.";
    } else {
        logger::log(std::source_location::current(), "No se instalará la vpn (options.vpn = false)");
    }

    MessageBox(FindWindowA("ConsoleWindowClass", nullptr),
                message.c_str(),
               "freesight", MB_OK | MB_ICONERROR);

    return 0;
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_CLOSE_EVENT) {
        logger::log(std::source_location::current(), "Cerrando freesight...");
        return true;
    }
    return false;
}
