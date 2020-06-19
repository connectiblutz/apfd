#!/usr/bin/env bash

set -eu

cd common/build
npm install
cd ../..

node common/build/index.js "$@"