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
		echo $msg
        POPD
        Exit 1
    }
}

function copyFolder {
  Param($source,$target)
  Copy-Item $source $target -Force -Recurse
}

function copySingle {
  Param($source,$target)
  Copy-Item $source $target -Force
}

function removeFolderPartial {
  Param($target,$match)
  if (Test-Path $target) { Remove-Item $target -Force -Recurse -include $match }
}
function removeFolder {
  Param($target)
  if (Test-Path $target) { Remove-Item $target -Force -Recurse  }
}

function removeSingle {
  Param($target)
  if (Test-Path $target) { Remove-Item $target -Force }
}
function createFolder {
  Param($target)
  $null = New-Item -Path . -Name $target -ItemType "directory"
}

function folderExists {
  Param($target)
  if (Test-Path $target) { return $true; }
  return $false;
}
