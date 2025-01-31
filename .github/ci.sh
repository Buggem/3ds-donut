#!/bin/bash

sudo apt-get update

cd "$1"

if [ -d "release" ]; then
  # Remove the file
  rm -rf "release"
fi

make

mkdir release
mv *.cia release
mv *.3dsx release
mv *.smdh release
mv *.elf release
