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
