#!/usr/bin/env pwsh

$ErrorActionPreference = "Stop"
function CheckLastExitCode {
    param ([int[]]$SuccessCodes = @(0), [scriptblock]$CleanupScript=$null)

    if ($SuccessCodes -notcontains $LastExitCode) {
        if ($CleanupScript) {
            "Executing cleanup script: $CleanupScript"
            &$CleanupScript
        }
        $msg = @"
EXE RETURNED EXIT CODE $LastExitCode
CALLSTACK:$(Get-PSCallStack | Out-String)
"@
               Write-Output $msg
        Pop-Location
        Exit 1
    }
}

try {
  Push-Location "common/build"
  npm install
  CheckLastExitCode 0
} finally {
  Pop-Location
}

node common/build/index.js $args
CheckLastExitCode 0