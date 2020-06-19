const shelljs = require('shelljs');
const path = require('path');

const options = require('getopts')(process.argv.slice(2), {
  alias: {
    conf: "c",
    arch: "a",
    clean
  },
  default: {
    conf: "release",
    arch: "x64",
    clean: false
  }
});

function clean(arch, config) {
  console.log("Cleaning up "+arch+" "+config+"...");
  shelljs.rm("-rf",path.join("_builds",arch,config));
}

function setup(arch, config) {
  console.log("Setting up "+arch+" "+config+"...");
  shelljs.mkdir("-p",path.join("_builds",arch,config));
  process.env.VSCMD_ARG_TGT_ARCH=arch
  if (shelljs.exec("cmake -B "+path.join("_builds",arch,config)+" -GNinja -DCMAKE_BUILD_TYPE="+config+" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++").code != 0) {
    process.exit(1);
  }
}

function build(arch, config, targets) {
  console.log("Building "+arch+" "+config+"...");
  process.env.VSCMD_ARG_TGT_ARCH=arch
  if (shelljs.exec("ninja -C "+path.join("_builds",arch,config)+" "+targets.join(" ")).code != 0) {
    process.exit(1);
  }
}
  
function fullArchSteps(arch, config, targets) {
  if (options.clean) {
    clean(arch, config);
  }
  setup(arch, config);
  build(arch, config, targets);
}

// validate args
console.log(options);
var arch = options.arch;
if (arch=="x64"||arch=="64") {
  arch="x64";
} else if (arch=="x86"||arch=="86"||arch=="32") {
  arch="x86"
} else {
  console.error("invalid arch");
  process.exit(1);
}
var conf = options.conf;
if (conf=="release"||conf=="Release"||conf=="r") {
  conf="Release";
} else if (conf=="debug"||conf=="Debug"||conf=="d") {
  conf="Debug"
} else {
  console.error("invalid conf");
  process.exit(1);
}
fullArchSteps(arch, conf, options._);
