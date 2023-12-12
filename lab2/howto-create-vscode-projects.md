# Native and cross build projects in vscode

C source code is portable in the sense that it does not dictate what platform or architecture it is compiled to. Native build results in executable code in same architecture as the build architecture (in our x86 VM we build native x86 executables). Cross-build refers to code built for another architecture (in our x86 VM we build executables for arm architecture).

Vscode can be configured to both targets. 

## Native C projects

### Global setup

The first step is specific to our lab VM setup and needs to be done only once. The problem with fresh VM start is that it is created for generic cross-build support, with both gcc and clang compilers. For native builds, the two extensions prevent running intellisense commands (which would make your life easier). So the first step is to remove clang compiler support (we will not use it for cross-builds anyway) and enable intellisense engine: 

- Shift-Ctrl-X: Find clang extension and uninstall it. Reload.
- File > Preferences > Settings : find Intellisense Engine; change "disabled" --> "default"

### Project setup (one source file)

A minimal setup consists of setting up two files in the project:
- ```.vscode/tasks.json``` to define build task (build task is binded to hotkey Shift-Ctrl-B) so that gcc is invoked on currently active code tab
- ```.vscode/launch.json``` to define debug task (F5 key) to launch gdb

The base version for tasks.json
```
//tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "gcc",
            "args": [
                "-g",
                "${file}",
                "-o",
                "${fileBasenameNoExtension}"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```
If you need to add external libraries or paths, add those to argument list (remember to append a comma to previous item to keep list format valid)
```
                "-l", "paho-mqtt3c"
```

The base version for launch
```
//launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "gcc - Build and debug active file",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}/${fileBasenameNoExtension}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```

Alternatively you can follow the instructions in [https://code.visualstudio.com/docs/cpp/config-linux] which relies to vscode extensions which could break well-operating cross-compilation setup...
In nutshell:
- Open an empty folder for the new project (File > Open Folder...)
- add New File (hello.c or whatever)
- Shift-ctrl-P: C/C++: Edit Configurations (JSON)   --> creates file .vscode/c_cpp_properties.json
- select your source file tab again
- to build: Shift-Ctrl-B: choose "gcc build active file"
- to debug F5: choose "C++ (GDB/LLDB)", choose "gcc build and debug active file"
The tool selections are stored in .vscode/tasks.json
You can now set breakpoints into source file and start debugging the code (F5).

### Project setup (multiple source files)

You would need to set up make or CMake project for this. Easiest way to go is to copy .vscode/tasks.json configuration (and possibly other files) from an other existing project.

## Cross-build C projects

There are no prebuilt templates for specific cross-development setups like our arm-raspi lab setup. The easiest way to proceed is to 
- copy .vscode folder and contents from existing project (lab2 for example) to a new project 
- copy cmake folder and contents from existing project (lab2 for example) to a new project
- copy CMakeLists.txt file from existing project (lab2 for example) to a new project 
- edit CMakeLists.txt so that source files and library dependencies match your new project 
- Shift-Ctrl-P: "CMake: Delete Cache and Reconfigure" to rebuild CMake cache. Choose kit (toolchain) "GCC 13.2.0 armv8-rpi3-linux-gnueabihf"
You should now be able to do cross-builds and remote debugging with the new project.  
Note that key shortcut are different for CMake projects:  
`F7` to build (especially note that Shift-Ctrl-B builds with wrong parameters... annoying)  
`F5` to debug


