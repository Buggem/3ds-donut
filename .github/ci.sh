#!/bin/bash

sudo apt-get update

cd "$1"

make

mkdir release
mv *.cia release
mv *.3dsx release
mv *.smdh release
mv *.elf release
