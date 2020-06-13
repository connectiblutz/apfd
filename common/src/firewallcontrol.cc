#include "common/firewallcontrol.h"
#include <stdio.h>
#include "common/stringutil.h"
#include "common/logutil.h"

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

namespace common {

FirewallControl::FirewallControl(const std::string& name, Direction direction, const std::string& protocol, const std::string& remoteIp, uint16_t remotePort) 
  : _name(name), _direction(direction), _protocol(protocol), _remoteIp(remoteIp), _remotePort(remotePort)
{

}
void FirewallControl::open() {
  open(_direction);
}
void FirewallControl::open(Direction direction) {
  if (direction==Direction::ANY) {
    open(Direction::INBOUND);
    open(Direction::OUTBOUND);
    return;
  }

  commonSetup([this,direction](std::shared_ptr<INetFwRules> pFwRules) {
    HRESULT hr = S_OK;
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

      auto nameEnd = "";
      if (direction==Direction::INBOUND) {
          nameEnd=" (IN)";
      } else if (direction==Direction::OUTBOUND) {
          nameEnd=" (OUT)";
      }
      auto bstrRuleName = StringUtil::toBSTR("APFD "+_name+nameEnd);
      auto bstrRuleGroup = StringUtil::toBSTR("APFD");
      auto bstrRuleLPorts = StringUtil::toBSTR(std::to_string(_remotePort));

      // Populate the Firewall Rule object
      pFwRule->put_Name(bstrRuleName.get());
      if (_protocol=="tcp") {
        pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP);
      } else if (_protocol=="udp") {
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
  });
}

void FirewallControl::close() {
  close(_direction);
}
void FirewallControl::close(Direction direction) {
  if (direction==Direction::ANY) {
    close(Direction::INBOUND);
    close(Direction::OUTBOUND);
    return;
  }
  commonSetup([this,direction](std::shared_ptr<INetFwRules> pFwRules) {
    HRESULT hr = S_OK;
    auto nameEnd = "";
    if (direction==Direction::INBOUND) {
        nameEnd=" (IN)";
    } else if (direction==Direction::OUTBOUND) {
        nameEnd=" (OUT)";
    }
    auto bstrRuleName = StringUtil::toBSTR("APFD "+_name+nameEnd);
    hr = pFwRules->Remove(bstrRuleName.get());
    if (FAILED(hr)) {
      common::LogUtil::Debug()<<"Firewall Rule Remove failed: "<<hr;
    }
  });
}


void FirewallControl::commonSetup(std::function<void(std::shared_ptr<INetFwRules>)> cb) {
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
        auto spFwRules=std::shared_ptr<INetFwRules>(pFwRules,[](INetFwRules* p) {
          p->Release();
        });

        cb(spFwRules);
      }
    }
    CoUninitialize();
  }

}

}