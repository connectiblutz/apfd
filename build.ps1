param (
  [switch]$all,
  [switch]$debug,
  [switch]$release,
  [switch]$x64,
  [switch]$x86,
  [switch]$clean
)

if (!$x64 -and !$x86) {
  $x64=$true
}
if (!$release -and !$debug) {
  $release=$true
}
if ($all) {
  $x64=$true
  $x86=$true
  $release=$true
  $debug=$true
}

Import-Module -Force $PSScriptRoot\common\pscommon.psm1

function clean {
  Param($arch, $config)

  Write-Output "Cleaning up $arch $config..."

  removeFolder _builds/$arch/$config
}
function setup {
  Param($arch, $config)

  Write-Output "Setting up $arch $config..."

  if (-Not (folderExists _builds/$arch/$config)) {
    createFolder _builds/$arch/$config
  }
  Set-Location _builds/$arch/$config
  $env:VSCMD_ARG_TGT_ARCH=$arch
  cmake ..\..\.. -GNinja -DCMAKE_BUILD_TYPE="$config" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
  CheckLastExitCode 0
  Set-Location ..\..\..
}
function build {
  Param($arch, $config)

  Write-Output "Building $arch $config..."

  $env:VSCMD_ARG_TGT_ARCH=$arch
  ninja -C _builds/$arch/$config
  CheckLastExitCode 0
}

function fullArchSteps {
  Param($arch, $config)
  if ($clean) { clean $arch $config }
  setup $arch $config
  build $arch $config
}

Push-Location

try {

  if ($x64) {
    if ($release) {
      fullArchSteps "x64" "Release"
    }
    if ($debug) {
      fullArchSteps "x64" "Debug"
    }
  }
  if ($x86) {
    if ($release) {
      fullArchSteps "x86" "Release"
    }
    if ($debug) {
      fullArchSteps "x86" "Debug"
    }
  }

} finally { Pop-Location }
