const shelljs = require('shelljs');
const path = require('path');
const fs = require('fs');
const os = require('os');

let props = require('properties-reader')("build.conf");
if (fs.existsSync('build-local.conf')) props=props.append("build-local.conf");
props=props.path()

const options = require('getopts')(process.argv.slice(2), {
  alias: {
    conf: "c",
    arch: "a",
    clean
  },
  default: {
    conf: props.default.conf?props.default.conf:"release",
    arch: props.default.arch?props.default.arch:os.arch(),
    clean: false
  }
});

let vcpkgPlatform = os.platform();
if (vcpkgPlatform=="win32") {
  vcpkgPlatform="windows";
} else if (vcpkgPlatform=="darwin") {
  vcpkgPlatform="osx";
}

function getVcpkgTriplet(arch) {
  var vcpkgTriplet = arch+"-"+vcpkgPlatform;
  if (vcpkgPlatform=="windows" && props.vcpkg.static===true) {
    vcpkgTriplet+="-static";
  } else if (vcpkgPlatform=="osx" && props.vcpkg.static===false) {
    vcpkgTriplet+="-dynamic";
  }
  return vcpkgTriplet;
}

function clean(arch, config) {
  console.log("Cleaning up "+arch+" "+config+"...");
  shelljs.rm("-rf",path.join("_builds",arch,config));
}

function setup(arch, config) {
  console.log("Setting up "+arch+" "+config+"...");
  shelljs.mkdir("-p",path.join("_builds",arch,config));
  process.env.VSCMD_ARG_TGT_ARCH=arch
  var vcpkgTriplet = getVcpkgTriplet(arch);
  if (shelljs.exec("cmake -B "+path.join("_builds",arch,config)+" -GNinja -DCMAKE_BUILD_TYPE="+config+" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_TOOLCHAIN_FILE="+props.vcpkg.root+"/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET="+vcpkgTriplet).code != 0) {
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

let walk = function(dir) {
  let results = [];
  let list = fs.readdirSync(dir);
  list.forEach(function(file) {
    file = dir + '/' + file;
    let stat = fs.statSync(file);
    if (stat && stat.isDirectory()) { 
      /* Recurse into a subdirectory */
      results = results.concat(walk(file));
    } else { 
      /* Is a file */
      results.push(file);
    }
  });
  return results;
}
function depends(arch) {
  let files = walk(".");
  files.forEach(function (file) {
    if (file.endsWith("deps.json")) {
      processDepends(arch, file);
    }
  });
}
function processDepends(arch, file) {
  let deps = JSON.parse(fs.readFileSync(file));
  let currentPkg = processVcpkgInstall();
  for (let [key, value] of Object.entries(deps)) {
    var pkgName = key+":"+getVcpkgTriplet(arch);
    if (!currentPkg[pkgName]) {
      console.log("Installing "+pkgName+"...")
      if (shelljs.exec(path.join(props.vcpkg.root,"vcpkg")+" install "+pkgName).code != 0) {
        console.error("failed to install dependency "+pkgName + " " + value.version);
        process.exit(1);
      }
    } else if (value.version && currentPkg[pkgName]!=value.version) {
      console.warn("Package "+pkgName+" at version "+currentPkg[pkgName]+", wanted version "+value.version);
    }
  }
}
function processVcpkgInstall() {
  let installed = {};
  let output = shelljs.exec(path.join(props.vcpkg.root,"vcpkg")+" list", {silent:true}).stdout;
  let lines = output.split('\n');
  lines.forEach(line => {
    var parts = line.split(/(\s+)/).filter( e => e.trim().length > 0);
    installed[parts[0]]=parts[1];
  })
  return installed;
}
  
function fullArchSteps(doClean, arch, config, targets) {
  if (doClean) {
    clean(arch, config);
  }
  depends(arch)
  setup(arch, config);
  build(arch, config, targets);
}

// validate args
let arch = options.arch;
if (arch=="x64"||arch=="amd64"||arch=="64") {
  arch="x64";
} else if (arch=="x86"||arch=="86"||arch=="x32"||arch=="32") {
  arch="x86"
} else {
  console.error("invalid arch");
  process.exit(1);
}
let conf = options.conf;
if (conf=="release"||conf=="Release"||conf=="r") {
  conf="Release";
} else if (conf=="debug"||conf=="Debug"||conf=="d") {
  conf="Debug"
} else {
  console.error("invalid conf");
  process.exit(1);
}

fullArchSteps(options.clean, arch, conf, options._);
