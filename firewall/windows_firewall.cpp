#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc50-cpp"
#include "windows_firewall.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
#pragma ide diagnostic ignored "ConstantConditionsOC"
#pragma ide diagnostic ignored "modernize-use-nullptr"
namespace windows_firewall {

    HRESULT WindowsFirewallInitialize(OUT INetFwProfile **fwProfile)
    {
        HRESULT hr = S_OK;
        INetFwMgr *fwMgr = NULL;
        INetFwPolicy *fwPolicy = NULL;

        _ASSERT(fwProfile != NULL);

        *fwProfile = NULL;

        logger::log(std::source_location::current(), "Initializing Windows Firewall... ");
        hr = CoCreateInstance(
                __uuidof(NetFwMgr),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwMgr),
                (void **)&fwMgr);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "CoCreateInstance for INetFwMgr failed: 0x", hr);

            if (fwMgr != NULL)
            {
                fwMgr->Release();
            }

            return hr;
        }
        else
        {
            logger::log(std::source_location::current(), "CoCreateInstance ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwMgr->get_LocalPolicy(&fwPolicy);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "get_LocalPolicy failed: 0x", hr);
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
            logger::log(std::source_location::current(), "get_LocalPolicy ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        // Retrieve the firewall profile currently in effect.
        hr = fwPolicy->get_CurrentProfile(fwProfile);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "get_CurrentProfile failed: 0x", hr);
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
            logger::log(std::source_location::current(), "get_CurrentProfile ha sido iniciado correctamente y ha devuelto el código 0x", hr);
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
            logger::log_error(std::source_location::current(), "get_FirewallEnabled failed: 0x", hr);
            return hr;
        }
        else
        {
            logger::log(std::source_location::current(), "get_FirewallEnabled ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            *fwOn = TRUE;
        }
        return hr;
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
            logger::log_error(std::source_location::current(), "get_AuthorizedApplications failed: 0x", hr);
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
            logger::log(std::source_location::current(), "get_AuthorizedApplications ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL)
        {
            hr = E_OUTOFMEMORY;
            logger::log_error(std::source_location::current(), "SysAllocString failed: 0x", hr);
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
            logger::log(std::source_location::current(), "SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
        if (SUCCEEDED(hr))
        {
            hr = fwApp->get_Enabled(&fwEnabled);
            if (FAILED(hr))
            {
                logger::log_error(std::source_location::current(), "get_Enabled failed: 0x", hr);
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
                logger::log(std::source_location::current(), "get_Enabled ha sido iniciado correctamente y ha devuelto el código 0x", hr);
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
        return hr;
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
            logger::log_error(std::source_location::current(), "get_Rules failed: 0x", hr);
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
            logger::log(std::source_location::current(), "get_Rules ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = CoCreateInstance(
                __uuidof(NetFwRule),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwRule),
                (void **)&fwRule);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "CoCreateInstance failed: 0x", hr);
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
            logger::log(std::source_location::current(), "CoCreateInstance ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL)
        {
            hr = E_OUTOFMEMORY;
            logger::log_error(std::source_location::current(), "SysAllocString failed: 0x", hr);
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
            logger::log(std::source_location::current(), "SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        fwBstrName = SysAllocString(fwName);
        if (fwBstrName == NULL)
        {
            hr = E_OUTOFMEMORY;
            logger::log_error(std::source_location::current(), "SysAllocString failed: 0x", hr);
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
            logger::log(std::source_location::current(), "SysAllocString ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwRule->put_Name(fwBstrName);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "put_Name failed: 0x", hr);
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
            logger::log(std::source_location::current(), "put_Name ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwRule->put_ApplicationName(fwBstrProcessImageFileName);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "put_ApplicationName failed: 0x", hr);
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
            logger::log(std::source_location::current(), "put_ApplicationName ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwRule->put_Action(NET_FW_ACTION_BLOCK);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "put_Action failed: 0x", hr);
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
            logger::log(std::source_location::current(), "put_Action ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwRule->put_Enabled(VARIANT_TRUE);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "put_Enabled failed: 0x", hr);
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
            logger::log(std::source_location::current(), "put_Enabled ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        hr = fwRules->Add(fwRule);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "Add failed: 0x", hr);
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
            logger::log(std::source_location::current(), "Add ha sido iniciado correctamente y ha devuelto el código 0x", hr);
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
        return hr;
    }

    int start_firewall_block() {
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
                logger::log_error(std::source_location::current(), "CoInitializeEx failed: 0x", hr);
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
                logger::log(std::source_location::current(), "CoInitializeEx ha sido iniciado correctamente y ha devuelto el código 0x", hr);
            }
        }

        hr = WindowsFirewallInitialize(&fwProfile);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "WindowsFirewallInitialize failed: 0x", hr);
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
            logger::log(std::source_location::current(), "WindowsFirewallInitialize ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        BOOL fwEnabled;
        BOOL fwAppEnabled;

        hr = WindowsFirewallIsOn(fwProfile, &fwEnabled);
        if (FAILED(hr))
        {
            logger::log_error(std::source_location::current(), "WindowsFirewallIsOn failed: 0x", hr);
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
            logger::log(std::source_location::current(), "WindowsFirewallIsOn ha sido iniciado correctamente y ha devuelto el código 0x", hr);
        }

        if (fwEnabled)
        {

            logger::log(std::source_location::current(), "El firewall está habilitado, iniciando bloqueo...");

            const wchar_t *p_name = L"%programfiles(x86)%\\Faronics\\Insight\\Student.exe";
            hr = WindowsFirewallAppIsEnabled(fwProfile, p_name, &fwAppEnabled);

            if (FAILED(hr))
            {
                /*logger::log_error(std::source_location::current(), "WindowsFirewallAppIsEnabled failed: 0x", hr);
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
                logger::log(std::source_location::current(), "WindowsFirewallAppIsEnabled ha sido iniciado correctamente y ha devuelto el código 0x", hr);
            }

            printf("\n");
            if (fwAppEnabled)
            {
                logger::log(std::source_location::current(), "Parece que la aplicación sigue habilitada dentro del firewall, intentando bloquear...");

                if (!is_elevated())
                {
                    MessageBox(FindWindowA("ConsoleWindowClass", NULL), "Para que el programa funcione debes de iniciarlo como administrador", "freesight", MB_OK | MB_ICONERROR);
                    return 0;
                }

                INetFwPolicy2* pNetFwPolicy2 = NULL;
                WFCOMInitialize(&pNetFwPolicy2);

                int random_number = rand() % 65535 + 1;

                const wchar_t *rule_name = L"freesight ";
                std::wstring rule_name_wstring = std::wstring(rule_name);
                std::wstring rule_name_wstring_final = rule_name_wstring + std::to_wstring(random_number);
                const wchar_t *rule_name_final = rule_name_wstring_final.c_str();

                hr = WindowsFirewallBlockApp(
                        pNetFwPolicy2,
                        p_name,
                        rule_name_final);
                if (FAILED(hr))
                {
                    logger::log_error(std::source_location::current(), "WindowsFirewallAddApp failed: 0x", hr);
                    WindowsFirewallCleanup(fwProfile);

                    if (SUCCEEDED(comInit))
                    {
                        CoUninitialize();
                    }

                    std::cin.get();
                    return 0;
                }
                else {
                    logger::log(std::source_location::current(), "WindowsFirewallAddApp ha sido iniciado correctamente y ha devuelto el código 0x", hr);
                }
            }
            else
            {
                logger::log(std::source_location::current(), "La aplicación ya está bloqueada, no se necesita reiniciar este proceso.");
            }
        }
        else
        {
            logger::log(std::source_location::current(), "El firewall está deshabilitado, este debe de estar encendido para que el bloqueo tenga efecto\nUna vez habilitado por favor, inicia este programa otra vez.");

            std::cin.get();
            return 1;
        }
        return 0;
    }

}

#pragma clang diagnostic pop
#pragma clang diagnostic pop