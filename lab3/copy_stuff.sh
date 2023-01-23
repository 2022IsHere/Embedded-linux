#!/bin/bash

printf "\nCopying project stuff from ../lab2 to current directory\n\n"

cp -R ../lab2/.vscode .
cp -R ../lab2/cmake .
cp ../lab2/.gitignore .
cp ../lab2/CMakeLists.txt .

printf "\nDone.\n"
