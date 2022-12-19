# Embedded Linux lab setup

## Introduction

Typically all embedded devices are developed in cross-development environment: The development system is a desktop PC with graphical IDE (Integrated Development Environment) providing editor for source code and debug capabilities, i.e. breakpoints, source code single line stepping, variable content visibility etc. The target system is an embedded device typically with very limited capabilities: low processing power, small memory size, no graphical user interfaces. Additionally the target runs on different CPU architecture, so it necessary to have cross-compilation toolchain in development system in order to produce machine code for target CPU.
To enable debugging, the development system needs to be able to set and monitor application states within target system. There are a couple of solutions:
- All modern CPUs have JTAG interface that provides hardware level access to processor registers and system memory. In practise you would attach JTAG debugger to target board, and connect debugger with USB to development system. Development system IDE has drivers for that specific JTAG debugger and so it is possible to do full debugging on target system. This method is suitable especially for bare-metal systems (those have no OS) and RTOS systems. 
- If target system is running capable OS like Linux it is possible to do software interrupt (SWI) based debugging for applications. Debugger process replaces instructions within target code with SWI commands, and takes control of the application in interrupt service. The same approach work both in native debugging (gdb) and cross-debugging (gdbserver).
- In both previous solutions the target system must be stopped in order to study variable contents, which makes it difficult to fully debug real-time systems. For example when debugging a washing machine motor control, the breakpoint will not stop the washing drum from rotating, only the program thread will stop. You debug with printf-lines, if your program can tolerate the additional printing delays. The ultimate solution is to use a processor emulator, which is a box of logic (FPGA) replicating CPU internals with all internal registers implemented as dual port RAM memory. So, while your application is running in CPU emulation logic connected target hardware (emulator is connected to a pod replacing the CPU on target circuit board), the debugger can read all system states and registers online, producing fulkl real-time system traces.

For Embedded Linux lab works you need to have 
1. Lab VM running on VirtualBox: VM provides cross-development toolchain and IDE.
2. TUAS gitlab account and working git skills (developed alongside the labs). Lab instructions and files are distributed from this lab instructions repository, and you will return your work into your personal return repository.
3. RasPi running gdbserver: this you will set up in the next lab.


