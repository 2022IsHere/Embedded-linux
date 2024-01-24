The benefits of shared libraries
1. You can save free up memory when each executable use shared libraries rather than compiling the required labriaries of each file into executable of the file, which would require more memory.
2. It's easier to update libraries as this action does not require to remake every executable file and the updates are available without remaking executables. 
3. Sharing files allows multiple versions to coexist and therefore wider range of compatibility is met. 



check for what architecture the executable was built for
The command used: file lab2 
1. ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV)



What shared libraries your 'lab2' executable requires?
The command used: ldd lab2
1. linux-vdso.so.1
2. /usr/lib/arm-linux-gnueabihf/libarmmem-${PLATFORM}.so
3. libc.so.6
4. /lib/ld-linux-armhf.so.3



where is your executable located in VM, and where in raspi is it copied to?
VM ('pwd' command is used in both cases), Raspberry pi: /home/student/EmbeddedLinux/embedded-linux-labs/lab2/build/lab2, /home/pi/lab2



Identify your development setup (VM and raspi) 
Get all files named libc.so.6 in VM and raspi
The command used: sudo find /lib /usr/lib /usr/lib32 /usr/lib64  -type l -name 'libc.so.6'
FOR VM BELOW ------------------------------------------------------------------------------------
LIBRARY libc.so.6 (Build-time library and its version)
FILE PATH(s): /usr/lib/x86_64-linux-gnu/libc.so.6    /usr/lib32/libc.so.6
ARCHITECHTURE: x86_64                                ELF 32-bit LSB shared object, Intel 80386
LIBRARY VERSION: libc-2.31.so                        libc-2.31.so
DEBUG LIBRARY: GNU gdb (Ubuntu 9.2-0ubuntu1~20.04.1) 9.2 (Debug library and its version) | COMMAND: gdb server --version 
-------------------------------------------------------------------------------------------------
FOR RASPBERRY PI BELOW ..........................................................................
LIBRARY libc.so.6 (Run-time library and its version)
FILE PATH(s): /usr/lib/arm-linux-gnueabihf/libc.so.6
ARCHITECHTURE: aarch64
LIBRARY VERSION: libc-2.36.so
.................................................................................................



COMPILE TIME ¦ RUN TIME GLIBC VERSIONS
VM    | Compile-time glibc version: 2.31	Runtime glibc version: 2.31
Raspi | Compile-time glibc version: 2.36	Runtime glibc version: 2.36



Can you be sure that executables built on this setup and target Debian Bookworm will run on Debian Bullseye (glibc version 2.31) and/or Debian Buster (glibc version 2.28)?
To be sure, I would need to test run a program compiled with glibc-2.36 version with both older versions. Generally new added features may prevent the program to run on older versions. 



Reflection: What are the key benefits of cross-development setup?
The setup allows to test fairly easily embedded system software on target system. The setup is fairly agile as well so source files, executables, etc can be manipulated and system software versions modified if needed. 


