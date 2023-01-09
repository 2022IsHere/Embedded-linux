# Embedded Linux lab setup

## Introduction

Typically all embedded devices are developed in <b>cross-development environment</b>: The <b>development system</b> is a desktop PC with graphical IDE (Integrated Development Environment) providing editor for source code and debug capabilities, i.e. breakpoints, source code single line stepping, variable content visibility etc. The <b>target system</b> is an embedded device with very limited capabilities: low processing power, small memory size, no graphical user interfaces. Additionally the target runs on different CPU architecture, so it necessary to have <b>cross-compilation toolchain</b> in development system in order to produce machine code for target CPU.
To enable debugging, the development system needs to be able to set and monitor application states within target system. There are a couple of solutions:
- All modern CPUs have JTAG interface that provides hardware level access to processor registers and system memory. In practise you would attach <b>JTAG debugger</b> to target board, and connect debugger with USB to development system. Development system IDE has drivers for that specific JTAG debugger and so it is possible to do full debugging on target system. This method is suitable especially for bare-metal systems (those have no OS) and RTOS systems. 
- If target system is running capable OS like Linux it is possible to do application <b>software debugging</b> based on interrupt (SWI). Debugger process replaces instructions within target code with SWI commands, and takes control of the application in interrupt service. The same approach work both in native debugging (gdb) and cross-debugging (gdbserver).
- In both previous solutions the target system must be stopped in order to study variable contents, which makes it difficult to fully debug real-time systems. For example when debugging a washing machine motor control, the breakpoint will not stop the washing drum from rotating, only the program thread will stop. You debug with printf-lines, if your program can tolerate the additional printing delays. The ultimate solution is to use a <b>processor emulator</b>, which is a box of logic (FPGA) replicating CPU internals with all internal registers implemented as dual port RAM memory. Dual port RAM has two independent address/data buses, so it is possible to access the memory data from CPU emulation side and debugger side simultaneously. So, while your application is running in CPU emulation logic connected target hardware (emulator is connected to a pod replacing the CPU on target circuit board), the debugger can read all system states and registers online, producing full real-time system traces.

## Overview of lab setup

For Embedded Linux lab works you need to have 
1. Lab VM running on VirtualBox: VM provides cross-development toolchain and IDE.
2. TUAS gitlab account and working git skills (developed alongside the labs). Lab instructions and files are distributed from this lab instructions repository, and you will return your work into your personal return repository.
3. RasPi running gdbserver: this you will set up in the next lab.

## Lab assignment

### Step 1: Install VM

Download VM image from emblab ftp server ftp://172.27.0.40/Embedded-Linux/raspi-cross-emblab.ova (for this you need to be connected to lab Ethernet socket). Just copy the URL to Windows File Manager address bar. Browsers do not support insecure ftp any more.
Import VM to VirtualBox.
Modify configuration:
- "Bridged" network is recommended. You need to attach the bridge to your Ethernet network interface card.
- Check the amount of CPU cores and memory allocated for the VM. 2 cores + 2GB RAM should do, but double that amount is better.
- VM user/passwd is student/student
Check that you can start the VM, log in and have Ubuntu desktop running with internet connectivity.

### Step 2: Fork lab repository

Log into TUAS gilab https://git.dc.turkuamk.fi/ and in main menu, switch to Groups / Embedded Linux. In that group there is repository `embedded-linux-labs`. Create a private <b>fork</b> to your personal namespace. A fork is a copy of a project. Forking a repository allows you to make changes without affecting the original project. This forked project works as your return repository.  
Please add users `jppaalas` and `jarno.tuominen` to the project with "Reporter" role.

### Step 3: Customize VM

The installed VM is naturally the same for all students, but will want to use your own identity to sync with TUAS gitlab repository. 
- Configure git identity
```bash
   student@student-VirtualBox:~$ git config --global user.name "Mona Lisa"
   student@student-VirtualBox:~$ git config --global user.email "YOUR_EMAIL"
```
- Create keypair for the VM:
```bash
   student@student-VirtualBox:~$ ssh-keygen -t rsa -b 2048 -f /home/student/.ssh/id_rsa -q -N ""
```
- Copy your public key to your forked project in TUAS gitlab: print your public key on terminal, copy content and paste to TUAS gitlab in browser (top right corner, "Edit profile / SSH Keys").
```bash
   student@student-VirtualBox:~$ cat /home/student/.ssh/id_rsa.pub
```
- (Change passwd for student on VM)

### Step 4: Clone your remote repository to your VM

```bash
   student@student-VirtualBox:~$ git clone git@git.dc.turkuamk.fi:YOUR-NAMESPACE/embedded-linux-labs.git
```
If gitlab asks for credentials, then your SSH keys setting has failed.  
You should now have a local git repository containing all lab assignments and files, linked to a private remote repository in TUAS gitlab. 

### Step 5: git

All your lab work will end up in your git repository, so it is necessary develop git skills as well.  
Check the material and exercises [on this page](git_tutorial.md)

### Step 6: C native compilation

[continued on next page...](compilation_process.md)
