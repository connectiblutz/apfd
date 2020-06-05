#include "firewallutil.h"
#include <windows.h>
#include <stdio.h>
#include <netfw.h>
#include "stringutil.h"

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

namespace apfd::common {

// Instantiate INetFwPolicy2
HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2), 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        __uuidof(INetFwPolicy2), 
        (void**)ppNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;        
    }

Cleanup:
    return hr;
}


void FirewallUtil::Open(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& localIp, uint16_t localPort, const std::wstring& remoteIp, uint16_t remotePort) {
  if (direction==Direction::ANY) {
    FirewallUtil::Open(name,Direction::INBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    FirewallUtil::Open(name,Direction::OUTBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    return;
  }

  HRESULT hrComInit = S_OK;
  HRESULT hr = S_OK;
  INetFwPolicy2 *pNetFwPolicy2 = NULL;
  INetFwRules *pFwRules = NULL;
  INetFwRule *pFwRule = NULL;
  long CurrentProfilesBitMask = 0;

  auto bstrRuleName = StringUtil::toBSTR(L"APFD "+name);
  //auto bstrRuleDescription = SysAllocString(L"Allow outbound network traffic from my Application over TCP port 4000");
  auto bstrRuleGroup = StringUtil::toBSTR(L"APFD");
  auto bstrRuleLPorts = StringUtil::toBSTR(L"4000");

    // Initialize COM.
    hrComInit = CoInitializeEx(
                    0,
                    COINIT_APARTMENTTHREADED
                    );

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(hrComInit))
        {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr))
    {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve Current Profiles bitmask
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&CurrentProfilesBitMask);
    if (FAILED(hr))
    {
        printf("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile, we remove it from the bitmask
    if ((CurrentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (CurrentProfilesBitMask != NET_FW_PROFILE2_PUBLIC))
    {
        CurrentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(
                __uuidof(NetFwRule),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwRule),
                (void**)&pFwRule);
    if (FAILED(hr))
    {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object
    pFwRule->put_Name(bstrRuleName.get());
    //pFwRule->put_Description(bstrRuleDescription);
    //pFwRule->put_ApplicationName(bstrRuleApplication);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
    pFwRule->put_LocalPorts(bstrRuleLPorts.get());
    pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    pFwRule->put_Grouping(bstrRuleGroup.get());
    pFwRule->put_Profiles(CurrentProfilesBitMask);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr))
    {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:

    // Release the INetFwRule object
    if (pFwRule != NULL)
    {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL)
    {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL)
    {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit))
    {
        CoUninitialize();
    }
}

void FirewallUtil::Close(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& localIp, uint16_t localPort, const std::wstring& remoteIp, uint16_t remotePort) {
  if (direction==Direction::ANY) {
    FirewallUtil::Close(name,Direction::INBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    FirewallUtil::Close(name,Direction::OUTBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    return;
  }
}

}