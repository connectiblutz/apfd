const shelljs = require('shelljs');
const path = require('path');

var argv = require('minimist')(process.argv.slice(2));
if (!argv.x64 && !argv.$x86) {
  argv.x64=true
}
if (!argv.release && !argv.debug) {
  argv.release=true
}
if (argv.all) {
  argv.x64=true
  argv.x86=true
  argv.release=true
  argv.debug=true
}

function clean(arch, config) {
  console.log("Cleaning up "+arch+" "+config+"...");
  shelljs.rm("-rf",path.join("_builds",arch,config));
}

function setup(arch, config) {
  console.log("Setting up "+arch+" "+config+"...");
  shelljs.mkdir("-p",path.join("_builds",arch,config));
  process.env.VSCMD_ARG_TGT_ARCH=arch
  if (shelljs.exec("cmake -B "+path.join("_builds",arch,config)+" -GNinja -DCMAKE_BUILD_TYPE="+config+" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++").code != 0) {
    exit(1);
  }
}

function build(arch, config, targets) {
  console.log("Building "+arch+" "+config+"...");
  process.env.VSCMD_ARG_TGT_ARCH=arch
  if (shelljs.exec("ninja -C "+path.join("_builds",arch,config)+" "+targets.join(" ")).code != 0) {
    exit(1);
  }
}
  
function fullArchSteps(arch, config, targets) {
  if (argv.clean) {
    clean(arch, config);
  }
  setup(arch, config);
  build(arch, config, targets);
}

if (argv.x64 && argv.release) {
  fullArchSteps("x64", "Release", argv._);
}
if (argv.x64 && argv.debug) {
  fullArchSteps("x64", "Debug", argv._);
}
if (argv.x86 && argv.release) {
  fullArchSteps("x86", "Release", argv._);
}
if (argv.x86 && argv.debug) {
  fullArchSteps("x86", "Debug", argv._);
}