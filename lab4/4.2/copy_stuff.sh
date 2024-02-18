#!/bin/bash

printf "\nCopying project stuff from ../lab4 to current directory\n\n"

cp -R ../lab4/.vscode .
cp -R ../lab4/cmake .
cp ../lab4/.gitignore .
cp ../lab4/CMakeLists.txt .

printf "\nDone.\n"
