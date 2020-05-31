# apfd - auto port forward daemon

Building
===
 - x64 release - `.\build.ps1`
 - x64 debug - `.\build.ps1 -debug`
 - x86 release - `.\build.ps1 -x86`
 - x86 debug - `.\build.ps1 -x86 -debug`
 - Everything - `.\build.ps1 -all`
 - Clean and build - `.\build.ps1 -clean`
Installing
===
 - Install service - `apfd.exe --install`
 - Remove service - `apfd.exe --remove`

Running manually
===
 - `apfd.exe --debug`

Config
===
Example `apfd.json`
```
{
  "services": [
    {
      "enabled":true,
      "name":"Debian Apache",
      "protocol":"tcp",
      "localIp":":Debian",
      "localPort":80,
      "remoteIp":"any",
      "remotePort":80,
      "autoStart":true,
      "startCommand":"sudo service apache2 start"
    }
  ]
}
```