# RPi Cross Development


## Setup:
Have a Raspberry Pi with 32bit Raspbian OS (optionally Lite). If you prepare the Raspi boot SD card from image, use 'Raspberry Pi Imager' application and set the following image customisation settings before flashing:
- set a unique hostname to help connecting your device in lab environment
- keep default user 'pi' but set a private password to prevent hassle in lab
- in "services" tab: enable ssh with password access 

Connect raspi to network and boot it. If you are in same LAN network, mDNS does magic and you can access your raspi using ```hostname.local``` where hostname is your above set hostname. This case applies when you have set up this VBox VM network in 'bridged' mode and are connected to same LAN as Raspi. If you are in NAT network mode, you need to use Raspi IP address for connections.

```
ssh pi@hostname.local
```

For convenient remote debugging, you will want to set up ssh keys, configure ssh shortname for the connection, install gdbserver to raspi, and synchronize raspi libraries to be similar to development libraries on this VM.

For your eternal happiness this all can be achieved using a script in this lab2 folder:
```
./raspi-init.sh -t target -u username -p passwd
```
where  
```target```: raspi IP address / hostname.local / ssh-config shortcut  
```username```: raspi username that you previously set to image (default 'pi')  
```passwd```: raspi password that you previously set to image

### Examples
Case where you have set up fresh Raspbian OS 32-bit image with hostname ```XXX```, user ```pi```, and passwd ```myraspipasswd```, and have the VM bridged to same ethernet LAN where raspi is connected:
```
./raspi-init.sh -t XXX -p myraspipasswd
```
Case where you have same raspi setup but your VM is in different LAN so that mDNS does not work (for example NATted VM or using Tailscale remote connection). You need to find out the raspi IP using nmap scan or attach display and keyboard to see the IP or some other way. 
```
./raspi-init.sh -t 172.27.242.233 -p mypasswd
```

After setup in both cases you have ssh configuration in ```~/.ssh/config``` setting a connection name rpi --> your device. This ssh connection name is then used in vscode setups. 

After successful setup you can connect to raspi using just ```ssh rpi``` (test it) command because ssh config maps 'rpi' to 'pi@yourdevicename', and the script has generated ssh keypair to VM and installed the public key to raspi ```~/.ssh/authorized_keys``` file.

You can run the raspi-init script multiple times (in case you want to adjust your setup or change pi device, or you want to debug the connection). For existing mapping you can just run ```raspi.init.sh rpi```.

## vscode usage
Navigate to hello folder and start vscode:
```
student@student-VirtualBox:~$ cd embeddedxxx/lab2/
student@student-VirtualBox:~/embeddedxxx/lab2$ code .
```
- Open hello.c to editor (select explorer on top-left icon and click hello.c)
- Press 'F5' key to cross-build the C code to armv6 binary, to copy the file to raspi, and to start debugger process on raspi. 
A successful debug session will result in code stopped on first code line and having full visibility to all variables in code. You can now single step through the code.  

Possible problem case:  
- Wrong or undefined "kit" (toolchain). Click around the middle of blue bar in screen bottom and choose toolchain "GCC 13.2.0 armv8-rpi3-linux-gnueabihf". If that kit is not listed, select "scan" option from the list and retry. Also see general instructions for new vscode projects [here](howto-create-vscode-projects.md)

An unwanted feature of this setup is that there is no way to see the stdout stream. This is not really a problem, since we are focused in developing embedded devices where there are no operators looking at displays; instead we want to have log files for later inspection. So this example demonstrates log file output. You can open a ssh terminal to rpi and view the output file using command `tail -f lab2.out`. The log file does not update in real-time, since the output buffer is not flushed to file after every write. You could add flush commands to C code make this happen.   

## Lab assignment

First, make sure that the existing demo code compiles and runs as above.

You did a test run for native compilation in lab1. Your task is to cross-compile that code for raspi.
- Replace existing demo code with your lab1 code
- Open another raspi terminal to `tail -f` your code output
- Debug and test
- commit to local repo, and push to your TUAS gitlab repo

Where is the executable located (file path) in VM and in raspi?  

After checking that you can get the debugger working proceed to [cross-compilation and libraries](Compilation_process_and_libraries.md)

## Some references

The setup is based on website
[Ubuntu to Raspberry Pi OS Cross C++ Development](https://tttapa.github.io/Pages/Raspberry-Pi/C++-Development-RPiOS/index.html)

Toolchains: [tttapa/docker-arm-cross-toolchain](https://github.com/tttapa/docker-arm-cross-toolchain)
