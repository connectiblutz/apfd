#include "firewallutil.h"
#include <windows.h>
#include <stdio.h>
#include <netfw.h>
#include "stringutil.h"
#include <memory>
#include "logutil.h"
#include <functional>

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

namespace apfd::common {

void FirewallUtil::Open(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& localIp, uint16_t localPort, const std::wstring& remoteIp, uint16_t remotePort) {
  if (direction==Direction::ANY) {
    FirewallUtil::Open(name,Direction::INBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    FirewallUtil::Open(name,Direction::OUTBOUND,protocol,localIp,localPort,remoteIp,remotePort);
    return;
  }

  HRESULT hr = S_OK;

  // Initialize COM.
  hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
  // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
  // initialized with a different mode. Since we don't care what the mode is,
  // we'll just use the existing mode.
  if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
  {
    common::LogUtil::Debug()<<"CoInitializeEx failed: "<< hr;
  } else {    
    INetFwPolicy2 *pNetFwPolicy2 = NULL;

    // Retrieve INetFwPolicy2
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    if (FAILED(hr))
    {
      common::LogUtil::Debug()<<"CoCreateInstance failed:"<< hr;
    }
    else {
      auto spNetFwPolicy=std::unique_ptr<INetFwPolicy2,std::function<void(INetFwPolicy2*)>>(pNetFwPolicy2,[](INetFwPolicy2* p) {
        p->Release();
      });

      // Retrieve INetFwRules
      INetFwRules *pFwRules = NULL;
      hr = pNetFwPolicy2->get_Rules(&pFwRules);
      if (FAILED(hr))
      {
        common::LogUtil::Debug()<<"get_Rules failed:"<< hr;
      } else {
        auto spFwRules=std::unique_ptr<INetFwRules,std::function<void(INetFwRules*)>>(pFwRules,[](INetFwRules* p) {
          p->Release();
        });

        // Create a new Firewall Rule object.
        INetFwRule *pFwRule = NULL;
        hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
        if (FAILED(hr))
        {
          common::LogUtil::Debug()<<"CoCreateInstance for Firewall Rule failed: "<<hr;
        } else {          
          auto spFwRule=std::unique_ptr<INetFwRule,std::function<void(INetFwRule*)>>(pFwRule,[](INetFwRule* p) {
            p->Release();
          });

          auto bstrRuleName = StringUtil::toBSTR(L"APFD "+name);
          auto bstrRuleGroup = StringUtil::toBSTR(L"APFD");
          auto bstrRuleLPorts = StringUtil::toBSTR(std::to_wstring(remotePort));

          // Populate the Firewall Rule object
          pFwRule->put_Name(bstrRuleName.get());
          if (protocol==L"tcp") {
            pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
          } else if (protocol==L"udp") {
            pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_UDP);
          }
          pFwRule->put_LocalPorts(bstrRuleLPorts.get());
          if (direction==Direction::INBOUND) {
              pFwRule->put_Direction(NET_FW_RULE_DIR_IN);
          } else if (direction==Direction::OUTBOUND) {
              pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
          }
          pFwRule->put_Grouping(bstrRuleGroup.get());
          pFwRule->put_Profiles(NET_FW_PROFILE2_DOMAIN | NET_FW_PROFILE2_PRIVATE);
          pFwRule->put_Action(NET_FW_ACTION_ALLOW);
          pFwRule->put_Enabled(VARIANT_TRUE);

          // Add the Firewall Rule
          hr = pFwRules->Add(pFwRule);
          if (FAILED(hr))
          {
            common::LogUtil::Debug()<<"Firewall Rule Add failed: "<<hr;
          }
        }
      }
    }
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