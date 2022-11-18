#include <windows.h>
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>
#include <stdio.h>
#include <string>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <Msi.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "msi.lib")

#include "logger.h"
#include "http_manager.h"

HRESULT WindowsFirewallInitialize(OUT INetFwProfile **fwProfile)
{
    HRESULT hr = S_OK;
    INetFwMgr *fwMgr = NULL;
    INetFwPolicy *fwPolicy = NULL;

    _ASSERT(fwProfile != NULL);

    *fwProfile = NULL;

    logger::log_info("Initializing Windows Firewall... ");
    hr = CoCreateInstance(
        __uuidof(NetFwMgr),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwMgr),
        (void **)&fwMgr);
    if (FAILED(hr))
    {
        logger::log_error("CoCreateInstance for INetFwMgr failed: 0x" + std::to_string(hr));
        if (fwPolicy != NULL)
        {
            fwPolicy->Release();
        }

        if (fwMgr != NULL)
        {
            fwMgr->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("CoCreateInstance ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwMgr->get_LocalPolicy(&fwPolicy);
    if (FAILED(hr))
    {
        logger::log_error("get_LocalPolicy failed: 0x" + std::to_string(hr));
        if (fwPolicy != NULL)
        {
            fwPolicy->Release();
        }

        if (fwMgr != NULL)
        {
            fwMgr->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("get_LocalPolicy ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    // Retrieve the firewall profile currently in effect.
    hr = fwPolicy->get_CurrentProfile(fwProfile);
    if (FAILED(hr))
    {
        logger::log_error("get_CurrentProfile failed: 0x" + std::to_string(hr));
        if (fwPolicy != NULL)
        {
            fwPolicy->Release();
        }

        if (fwMgr != NULL)
        {
            fwMgr->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("get_CurrentProfile ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }
}

void WindowsFirewallCleanup(IN INetFwProfile *fwProfile)
{
    if (fwProfile != NULL)
    {
        fwProfile->Release();
    }
}

HRESULT WindowsFirewallIsOn(IN INetFwProfile *fwProfile, OUT BOOL *fwOn)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwOn != NULL);

    *fwOn = FALSE;

    hr = fwProfile->get_FirewallEnabled(&fwEnabled);
    if (FAILED(hr))
    {
        logger::log_error("get_FirewallEnabled failed: 0x" + std::to_string(hr));
        return hr;
    }
    else
    {
        logger::log_ok("get_FirewallEnabled ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    if (fwEnabled != VARIANT_FALSE)
    {
        *fwOn = TRUE;
    }
}

HRESULT WindowsFirewallAppIsEnabled(
    IN INetFwProfile *fwProfile,
    IN const wchar_t *fwProcessImageFileName,
    OUT BOOL *fwAppEnabled)
{
    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication *fwApp = NULL;
    INetFwAuthorizedApplications *fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwAppEnabled != NULL);

    *fwAppEnabled = FALSE;

    hr = fwProfile->get_AuthorizedApplications(&fwApps);
    if (FAILED(hr))
    {
        logger::log_error("get_AuthorizedApplications failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);

        if (fwApp != NULL)
        {
            fwApp->Release();
        }

        if (fwApps != NULL)
        {
            fwApps->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("get_AuthorizedApplications ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
    if (fwBstrProcessImageFileName == NULL)
    {
        hr = E_OUTOFMEMORY;
        logger::log_error("SysAllocString failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);

        if (fwApp != NULL)
        {
            fwApp->Release();
        }

        if (fwApps != NULL)
        {
            fwApps->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
    if (SUCCEEDED(hr))
    {
        hr = fwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
            logger::log_error("get_Enabled failed: 0x" + std::to_string(hr));
            SysFreeString(fwBstrProcessImageFileName);

            if (fwApp != NULL)
            {
                fwApp->Release();
            }

            if (fwApps != NULL)
            {
                fwApps->Release();
            }

            return hr;
        }
        else
        {
            logger::log_ok("get_Enabled ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            *fwAppEnabled = TRUE;
        }
    }
    else
    {
        hr = S_OK;
    }
}

HRESULT WindowsFirewallBlockApp(
    IN INetFwPolicy2 *fwProfile,
    IN const wchar_t *fwProcessImageFileName,
    IN const wchar_t *fwName)
{

    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    BSTR fwBstrName = NULL;
    INetFwRule *fwRule = NULL;
    INetFwRules *fwRules = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwName != NULL);

    hr = fwProfile->get_Rules(&fwRules);
    if (FAILED(hr))
    {
        logger::log_error("get_Rules failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("get_Rules ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = CoCreateInstance(
        __uuidof(NetFwRule),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),
        (void **)&fwRule);
    if (FAILED(hr))
    {
        logger::log_error("CoCreateInstance failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("CoCreateInstance ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
    if (fwBstrProcessImageFileName == NULL)
    {
        hr = E_OUTOFMEMORY;
        logger::log_error("SysAllocString failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    fwBstrName = SysAllocString(fwName);
    if (fwBstrName == NULL)
    {
        hr = E_OUTOFMEMORY;
        logger::log_error("SysAllocString failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwRule->put_Name(fwBstrName);
    if (FAILED(hr))
    {
        logger::log_error("put_Name failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("put_Name ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwRule->put_ApplicationName(fwBstrProcessImageFileName);
    if (FAILED(hr))
    {
        logger::log_error("put_ApplicationName failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("put_ApplicationName ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwRule->put_Action(NET_FW_ACTION_BLOCK);
    if (FAILED(hr))
    {
        logger::log_error("put_Action failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("put_Action ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwRule->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr))
    {
        logger::log_error("put_Enabled failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("put_Enabled ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwRules->Add(fwRule);
    if (FAILED(hr))
    {
        logger::log_error("Add failed: 0x" + std::to_string(hr));
        SysFreeString(fwBstrProcessImageFileName);
        SysFreeString(fwBstrName);

        if (fwRule != NULL)
        {
            fwRule->Release();
        }

        if (fwRules != NULL)
        {
            fwRules->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("Add ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    SysFreeString(fwBstrProcessImageFileName);
    SysFreeString(fwBstrName);

    if (fwRule != NULL)
    {
        fwRule->Release();
    }

    if (fwRules != NULL)
    {
        fwRules->Release();
    }

    return hr;
}

HRESULT WindowsFirewallPortIsEnabled(
    IN INetFwProfile *fwProfile,
    IN LONG portNumber,
    IN NET_FW_IP_PROTOCOL ipProtocol,
    OUT BOOL *fwPortEnabled)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL fwEnabled;
    INetFwOpenPort *fwOpenPort = NULL;
    INetFwOpenPorts *fwOpenPorts = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwPortEnabled != NULL);

    *fwPortEnabled = FALSE;

    hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
    if (FAILED(hr))
    {
        logger::log_error("get_GloballyOpenPorts failed: 0x" + std::to_string(hr));
        if (fwOpenPort != NULL)
        {
            fwOpenPort->Release();
        }

        if (fwOpenPorts != NULL)
        {
            fwOpenPorts->Release();
        }

        return hr;
    }
    else
    {
        logger::log_ok("get_GloballyOpenPorts ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
    if (SUCCEEDED(hr))
    {
        hr = fwOpenPort->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
            logger::log_error("get_Enabled failed: 0x" + std::to_string(hr));
            if (fwOpenPort != NULL)
            {
                fwOpenPort->Release();
            }

            if (fwOpenPorts != NULL)
            {
                fwOpenPorts->Release();
            }

            return hr;
        }
        else
        {
            logger::log_ok("get_Enabled ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            *fwPortEnabled = TRUE;
        }
    }
    else
    {
        hr = S_OK;
    }
}

HRESULT WFCOMInitialize(INetFwPolicy2 **ppNetFwPolicy2)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void **)ppNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        return hr;
    }
}

BOOL is_elevated()
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
        {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken)
    {
        CloseHandle(hToken);
    }
    return fRet;
}

int __cdecl wmain(int argc, wchar_t *argv[])
{
	
	std::cout << "    ____                    _       __    __ " << std::endl;
	std::cout << "   / __/_______  ___  _____(_)___ _/ /_  / /_" << std::endl;
	std::cout << "  / /_/ ___/ _ \\/ _ \\/ ___/ / __ `/ __ \\/ __/" << std::endl;
	std::cout << " / __/ /  /  __/  __(__  ) / /_/ / / / / /_" << std::endl;
	std::cout << "/_/ /_/   \\___/\\___/____/_/\\__, /_/ /_/\\__/" << std::endl;
	std::cout << "                          /____/             " << std::endl;
	std::cout << "by @holasoyender" << std::endl;

	
    std::string password;
    std::cout << " -- Este programa requeiere una contraseña para ser ejecutado: ";
    std::cin >> password;

    if (password != "ww120")
    {
        MessageBox(FindWindowA("ConsoleWindowClass", NULL), L"Esa contraseña es incorrecta, no estas autorizado a usar este programa", L"No autorizado", MB_OK | MB_ICONERROR);
        return 0;
    }

    HRESULT hr = S_OK;
    HRESULT comInit = E_FAIL;
    INetFwProfile *fwProfile = NULL;

    comInit = CoInitializeEx(
        0,
        COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (comInit != RPC_E_CHANGED_MODE)
    {
        hr = comInit;
        if (FAILED(hr))
        {
            logger::log_error("CoInitializeEx failed: 0x" + std::to_string(hr));
            WindowsFirewallCleanup(fwProfile);

            if (SUCCEEDED(comInit))
            {
                CoUninitialize();
            }

            std::cin.get();
            return 0;
        }
        else
        {
            logger::log_ok("CoInitializeEx ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
        }
    }

    hr = WindowsFirewallInitialize(&fwProfile);
    if (FAILED(hr))
    {
        logger::log_error("WindowsFirewallInitialize failed: 0x" + std::to_string(hr));
        WindowsFirewallCleanup(fwProfile);

        if (SUCCEEDED(comInit))
        {
            CoUninitialize();
        }

        std::cin.get();
        return 0;
    }
    else
    {
        logger::log_ok("WindowsFirewallInitialize ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    BOOL fwEnabled;
    BOOL fwAppEnabled;

    hr = WindowsFirewallIsOn(fwProfile, &fwEnabled);
    if (FAILED(hr))
    {
        logger::log_error("WindowsFirewallIsOn failed: 0x" + std::to_string(hr));
        WindowsFirewallCleanup(fwProfile);

        if (SUCCEEDED(comInit))
        {
            CoUninitialize();
        }

        std::cin.get();
        return 0;
    }
    else
    {
        logger::log_ok("WindowsFirewallIsOn ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
    }

    if (fwEnabled)
    {

        logger::log_info("El firewall está habilitado, iniciando bloqueo...");

        const wchar_t *p_name = L"%programfiles(x86)%\\Faronics\\Insight\\Student.exe";
        hr = WindowsFirewallAppIsEnabled(fwProfile, p_name, &fwAppEnabled);

        if (FAILED(hr))
        {
            /*logger::log_error("WindowsFirewallAppIsEnabled failed: 0x" + std::to_string(hr));
            WindowsFirewallCleanup(fwProfile);

            if (SUCCEEDED(comInit))
            {
                CoUninitialize();
            }

            std::cin.get();
            return 0;*/
            fwAppEnabled = true;
        }
        else
        {
            logger::log_ok("WindowsFirewallAppIsEnabled ha sido iniciado correctamente y ha devuelto el código 0x" + std::to_string(hr));
        }

        printf("\n");
        if (fwAppEnabled)
        {
            logger::log_info("Parece que la aplicación sigue habilitada dentro del firewall, intentando bloquear...");

            if (!is_elevated())
            {
                MessageBox(FindWindowA("ConsoleWindowClass", NULL), L"Para que el programa funcione debes de iniciarlo como administrador", L"eres tonto?", MB_OK | MB_ICONERROR);
                return 0;
            }

            INetFwPolicy2* pNetFwPolicy2 = NULL;
            WFCOMInitialize(&pNetFwPolicy2);

            int random_number = rand() % 65535 + 1;

            const wchar_t *rule_name = L"que te den ";
            std::wstring rule_name_wstring = std::wstring(rule_name);
            std::wstring rule_name_wstring_final = rule_name_wstring + std::to_wstring(random_number);
            const wchar_t *rule_name_final = rule_name_wstring_final.c_str();

            hr = WindowsFirewallBlockApp(
                pNetFwPolicy2,
                p_name,
                rule_name_final);
            if (FAILED(hr))
            {
                logger::log_error("WindowsFirewallAddApp failed: 0x" + std::to_string(hr));
                WindowsFirewallCleanup(fwProfile);

                if (SUCCEEDED(comInit))
                {
                    CoUninitialize();
                }

                std::cin.get();
                return 0;
            }
            else
            {
                logger::log_ok("La aplicación se ha bloqueado correctamente, intentando instalar 1.1.1.1...");
                int response = MessageBox(
                    NULL,
                    L"El programa ha sido bloqueado correctamente\n\n¿Quieres instalar una VPN?",
                    L"Bloqueado correctamente",
                    MB_YESNOCANCEL | MB_ICONINFORMATION);

                std::cout << "msgbox code: " << response << std::endl;

                switch (response)
                {

                case IDYES:
                    logger::log_ok("El usuario ha aceptado instalar 1.1.1.1, iniciando instalación...");

                    if (std::filesystem::exists("temp/vpn_installer.msi"))
                    {
                        logger::log_info("El archivo vpn_installer.msi existe, iniciando instalación...");
						
                        int run = system(("runas /user:administrator \"msiexec /a \"" + std::filesystem::absolute("temp/vpn_installer.msi").string() + "\" /qb+ \"").c_str());
                        if (run == 0)
                        {
                            logger::log_ok("El archivo vpn_installer.msi se ha instalado correctamente");
                        }
                        else
                        {
                            logger::log_error("El archivo vpn_installer.msi no se ha instalado correctamente");
                        }
                        // ShellExecute(NULL, L"open", L"temp/vpn_installer.msi", NULL, NULL, SW_SHOW);
                    }
                    else
                    {
                        logger::log_info("No se ha encontrado el instalador, descargando...");
                        http_manager::download_file(NULL, "https://1111-releases.cloudflareclient.com/windows/Cloudflare_WARP_Release-x64.msi", "temp/vpn_installer.msi", "1.1.1.1");
                        logger::log_ok("Instalador descargado correctamente, iniciando instalación...");
                        int run = system(("runas /user:administrator \"msiexec /a \"" + std::filesystem::absolute("temp/vpn_installer.msi").string() + "\" /qb+ \"").c_str());
                        if (run == 0)
                        {
                            logger::log_ok("El archivo vpn_installer.msi se ha instalado correctamente");
                        }
                        else
                        {
                            logger::log_error("El archivo vpn_installer.msi no se ha instalado correctamente");
                        }
                    }

                    std::cin.get();
                    return 0;

                    break;

                case IDCANCEL:
                case IDNO:
                default:
                    logger::log_info("El usuario ha cancelado la instalación de la VPN, cerrando programa...");
                    std::cin.get();
                    return 0;
                    break;
                }

                std::cin.get();
                return 0;
            }
        }
        else
        {
            logger::log_info("La aplicación ya está bloqueada, no se necesita reiniciar este proceso.");
        }
    }
    else
    {
        logger::log_info("El firewall está deshabilitado, este debe de estar encendido para que el bloqueo surga efecto\nUna vez habilitado por favor, inicia este programa otra vez.");

        std::cin.get();
        return 1;
    }
}
