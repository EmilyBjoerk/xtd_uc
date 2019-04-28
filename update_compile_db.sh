#!/usr/bin/env bash

RC=rtags-rc

mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
cp compile_commands.json ../
$RC --clear
$RC -J ../compile_commands.json
