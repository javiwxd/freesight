#ifndef WINDOWS_FIREWALL_H
#define WINDOWS_FIREWALL_H

#include "../logger/logger.h"
#include "../lib/elevate.h"

namespace windows_firewall {

    HRESULT WindowsFirewallInitialize(OUT INetFwProfile **fwProfile);
    void WindowsFirewallCleanup(IN INetFwProfile *fwProfile);
    HRESULT WindowsFirewallIsOn(IN INetFwProfile *fwProfile, OUT BOOL *fwOn);

    HRESULT WindowsFirewallAppIsEnabled(
            IN INetFwProfile *fwProfile,
            IN const wchar_t *fwProcessImageFileName,
            OUT BOOL *fwAppEnabled);

    HRESULT WindowsFirewallBlockApp(
            IN INetFwPolicy2 *fwProfile,
            IN const wchar_t *fwProcessImageFileName,
            IN const wchar_t *fwName);

    HRESULT WFCOMInitialize(INetFwPolicy2 **ppNetFwPolicy2);

    int start_firewall_block();
}

#endif //WINDOWS_FIREWALL_H
