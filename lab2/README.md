# RPi Cross Development


## Quick setup:
Have a Raspberry Pi with 32bit Raspbian OS (optionally Lite). If you prepare the Raspi boot SD card from image, use 'Raspberry Pi Imager' application and set the following options before flashing:
- set a unique hostname to help connecting your device in lab environment
- keep default user 'pi' but set a private password to prevent hassle in lab
- enable ssh
Connect raspi to network and boot it. If you are in same LAN network, mDNS does magic and you can access your raspi using ```hostname.local``` where hostname is your above set hostname. This case applies when you have set up this VBox VM network in 'bridged' mode and are connected to same LAN as Raspi. If you are in NAT network mode, you need to use Raspi IP for connections.

For convenient remote debugging, you will want to set up ssh keys, configure ssh shortname for the connection, install gdbserver to raspi, and synchronize raspi libraries to be similar to development libraries on this VM.

For your eternal happiness this all can be achieved using a script in home folder:
```
raspi-init.sh -t target -u username -p passwd
```
where \
```target```: raspi IP address / hostname.local / ssh-config shortcut \
```username```: raspi username (default 'pi') \
```passwd```: raspi password (default 'raspberry' ...you should not use this...)\
### Examples
Case where you have set up fresh Raspbian OS 32-bit image with hostname ```XXX```, user ```pi```, and passwd ```myraspipasswd```, and have the VM bridged to same ethernet LAN where raspi is connected:
```
raspi-init.sh -t XXX -p myraspipasswd
```
Case where you have same raspi setup but your VM is in different LAN so that mDNS does not work (for example NATted VM or using Tailscale remote connection). You need to find out the raspi IP using nmap scan or attach display and keyboard to see the IP or some other way. 
```
raspi-init.sh -t 172.27.242.233 -p mypasswd
```

After setup in both cases you have ssh configuration in ```~/.ssh/config``` setting a connection name rpi --> your device. This ssh connection name is then used in vscode setups. 

After successful setup you can connect to raspi using just ```ssh rpi``` (test it) command because ssh config maps 'rpi' to 'pi@yourdevicename', and the script has generated ssh keypair to VM and installed the public key to raspi ```~/.ssh/authorized_keys``` file.

You can run the raspi-init script multiple times (in case you want to adjust your setup or change pi device, or you want to debug the connection). For existing mapping you can just run ```raspi.init.sh rpi```.

## vscode usage
Navigate to hello folder and start vscode:
```
student@student-VirtualBox:~$ cd projects/hello/
student@student-VirtualBox:~/projects/hello$ code .
```
Open hello.c to editor (select explorer on top-left icon and click hello.c)
Press 'F5' key to cross-build the C code to armv6 binary, copy the file to raspi, and start debugger process on raspi. 
A successful debug session will result in code stopped on first code line and having full visibility to all variables in code. You can now single step through the code.
A feature of this setup is that there is no way to see the stdout stream. This is not really a problem, since we a focused in developing embedded devices where there are no operators looking at displays; instead we want to have log files for later inspection. So this example demonstrates log file output. You can open a ssh terminal to rpi and view the output file using command ```tail -f lab1.out```. The log file does not update in real-time, since the output buffer is not flushed to file after every write. You could add flush commands to C code make this happen.   


## Some references

The setup is based on website
[Ubuntu to Raspberry Pi OS Cross C++ Development](https://tttapa.github.io/Pages/Raspberry-Pi/C++-Development-RPiOS/index.html)

Toolchains: [tttapa/docker-arm-cross-toolchain](https://github.com/tttapa/docker-arm-cross-toolchain)
