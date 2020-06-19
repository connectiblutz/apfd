# apfd - auto port forward daemon

Development Setup
===
 - Dependencies
   - node (>=10)
   - cmake (>=3.13)
   - ninja (>=1.8.2)
   - clang (>=10)
   - vcpkg
 - Create file `build-local.conf`
 ```
[vcpkg]
root = <path-to-vcpkg>
```

Building
===
 - `./build.[sh|ps1] [-x|--clean] [-a|--arch=[x64|x86]] [-c|--conf=[release|debug]] [-- targets..]`

Installing
===
 - Install service - `apfd.exe --install <windows account name> <windows account password>`
   - You must enable Log On As a Service for your account
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
      "protocol":"tcp", // only tcp
      "localIp":":Debian:lo", // ipv4 address | any | localhost | :<wsl distro>:<iface> (use lo for wsl1, eth0 for wsl2)
      "localPort":80,
      "remoteIp":"any", // ipv4 address | any | localhost
      "remotePort":80,
      "autoStart":true,
      "startCommand":"sudo service apache2 start" // set up your sudoers
    }
  ]
}
```