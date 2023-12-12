# MQTT

MQTT is a popular publish-subscribe protocol for trasmitting sensor data in IoT networks.  

This time you want to create an amazing MQTT demo on RasPi.  
In step A you demonstrate that it is quite straightforward to install mosquitto package to VM and see that you can publish and subscribe messages from command line.  
In step B you do a native build on VM to create C language MQTT client to run on VM.  
In step C you do cross-build on VM to publish your GPIO pin data from raspi. Quite immediately you will see a couple of problems:
- paho prebuilt libraries are distributed for x86 architecture only, not for raspi. You need to cross-build the libraries.
- paho libraries have dependencies on other libraries (openssl), and you need to install those in our development system as target architecture binary, and install those to target system too.

After recognizing the work ahead, you may want to question the original project targets
1. to develop the system in C language (Python solution is one `pip install` away), and 
2. to build the dependent libraries yourself (sure there is a prebuilt binary somewhere).  

Your boss states that often in embedded realm you are very concerned about energy and memory usage --> you need to use C language. And in case of eventual security vulnerability in paho library, you need to be able to generate newest patched binaries for your target architecture --> you need to be able to build the binaries yourself.  
Next you realize that it would be much easier to do a native build in raspi. Just clone the paho repository into raspi, and do a make. The build is slower, but there are no problems with library dependencies and stuff.  
Your boss states that the target system is not a managed build environment. You will have no control over toolchain versions or library versions (and there is also a secret business plan to transfer this design to a lowest-cost product with non-existent build capabilities).  
OK, lets cross-build.

## A. MQTT command-line demo in VM

We'll start with native builds in VM to see that they work OK. For a MQTT system we need a MQTT broker, one publisher and one subscriber to get a running system. There are multiple guides in net for this, lets pick this one [https://www.arubacloud.com/tutorial/how-to-install-and-secure-mosquitto-on-ubuntu-20-04.aspx]  
Please follow the guide up to the point of Configuring MQTT password, we are happy with unsecured demo at this point.  
Outcome: you have demonstrated message publishing from command line, and you have a broker and subscriber running. Next you want to publish messages from C code client.


## B. Native built C language client in VM

Again, there are multiple candidates for a c MQTT library. We'll choose paho [https://www.eclipse.org/paho/index.php?page=clients/c/index.php], because it has SSL/TLS support and both synchronous (blocking) and asynchronous (non-blocking) APIs.

Follow the instructions (clone - make - sudo make install), and test CLI utility paho_c_pub to send a message to your still running mosquitto broker (have subscriber running as well.)   

>>>
Many linux libraries are makefile based and they support the basic commands  
`make` builds the application/library according to configuration in project local Makefile  
`make clean` deletes all build artefacts  
`(sudo) make install` copies the built executables into system library folders (so that running applications can find them)  
>>>

**NOTE1** I recommend you to clone the paho repository into your home folder. It is possible to clone it under {repo}/lab7 as well and add the paho folder to .gitignore (and maybe use git submodules feature to manage all), but don't.  
**NOTE2** If compilation step fails with missing include file, it means you need to install dependent library. For most linux libraries, there are two related install packages. The library package itself contains executable libraries. The dev-package contains include files as well, making it possible to build other programs that depend on installed library. If `ssl.h` is missing, then after some googling you should find out that the VM ubuntu system is missing `libssl-dev` package. Sudo apt install it.  

After successful build you can check that the library included cli utility runs OK. Just send a test message and see that your mosquitto subscriber gets it:
```
paho_c_pub -c localhost -t /home/sensors/temp/kitchen -m "Hello from x86 paho.mqtt.c"
```
Successful publish operation proves our native compilation was successful, so we have the courage to start tinkering with example source code.

1. Create two project folders under {repo}/lab7:  
{repo}/lab7/mqtt-vm  
{repo}/lab7/mqtt-raspi
2. Copy the example code from paho web page bottom as it is into `{repo}/lab7/mqtt-vm/mqtt-client.c`` 
3. Set up native C project into {repo}/lab7/mqtt-vm/ (see instructions [on this page](../lab2/howto-create-vscode-projects.md)).
4. Select mqtt-client.c as active tab and build (Shift-Ctrl-B). The build should fail because paho libraries are not linked to the project.
5. Edit .vscode/tasks.json to add argument `"-lpaho-mqtt3c"` for the build (append it, i.e. insert as last element in argument list).

>>>
Note: Library names and file names are somewhat confusing. The library file name is "libpaho-mqtt3c.so", but library name (not file) is filename without prefix "lib" and without suffix. The file itself is usually not a file, but a symlink to main version file, which is symlink to actual library file... so, to link to actual "libpaho-mqtt3c.so.1.3" file we add argument "-lpaho-mqtt3c".
>>>

6. Select mqtt-client.c as active tab and build (Shift-Ctrl-B). The build should be successful.
7. Edit the mqtt topic in source to match topic your subscriber is watching. Rebuild and test (F5).

You now have a C language MQTT client native project, with full debug support.

## C. Cross built C language client in RasPi

### Cross-build paho libraries

First, in paho.mqtt.c folder do a `make clean` to remove x86 build results from paho folder. Removing the built executables does not break anything, because the previous `make install` copied the important files into system libraries already.  

So you need to cross-build paho libraries. For cross-build to be successful, you need to tell to make build process that
1. it must use cross-compiler `armv8-rpi3-linux-gnueabihf-gcc` instead of native `gcc`
2. it must search all dependent libraries from our arm library folder `/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/` instead of the standard native library paths like `/usr/lib`
Fortunately most makefiles are generally constructed in similar fashion, they use environment variables for conveying this information. "CC" specifies the C compiler, and LDFLAGS can be used to give extra parameters for the linker:
```
student@student-VirtualBox:~/paho.mqtt.c$ CC=armv8-rpi3-linux-gnueabihf-gcc LDFLAGS=--sysroot=/var/lib/schroot/chroots/rpi3-bookworm-armhf/ make

```
Once more there is trouble with missing <openssl/ssl.h> include file. When doing native build, you added the `libssl-dev` package to VM native libraries. Now you need to do the same again for arm sysroot `/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/`. To add libraries there you use the utility tool to fetch the required package from Raspbian repository
```
sudo sbuild-apt rpi3-bookworm-armhf apt-get install libssl-dev
```
Now the make build fails with different error:
```
...
/home/student/opt/x-tools/armv8-rpi3-linux-gnueabihf/bin/../lib/gcc/armv8-rpi3-linux-gnueabihf/13.2.0/../../../../armv8-rpi3-linux-gnueabihf/bin/ld.bfd: warning: libdl.so.2, needed by build/output/libpaho-mqtt3cs.so, not found (try using -rpath or -rpath-link)
...
```
This smells like cross-development setup with nested libary dependencies in nonstandard locations. The fix is found with following steps: first we should check does the missing library exist and where is it located. Using ```sudo find / -name libdl.so.2``` we see that the missing library exists in folder /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/ but is not found by the linker. Looking at gcc linker options we see that we could pass extra paths using rpath-link (as was suggested in original error message). In the next try we pass two linker options via LDFLAGS so we need to use quotes around them:
```
student@student-VirtualBox:~/paho.mqtt.c$ CC=armv8-rpi3-linux-gnueabihf-gcc LDFLAGS="--sysroot=/var/lib/schroot/chroots/rpi3-bookworm-armhf/ -Wl,-rpath-link,/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/" make

```
Thi sbuild should be successful. You can find the built executables in folder `paho.mqtt.c/build/output`. Use ```readelf -h``` command to check that output files are for arm architecture.
```
student@student-VirtualBox:~/paho.mqtt.c$ readelf -h build/output/libpaho-mqtt3c.so.1.3
```
You need to install these libraries to arm sysroot in VM, so that application building and debugging is possible.
```
sudo DESTDIR=/var/lib/schroot/chroots/rpi3-bookworm-armhf LDCONFIG=echo make install
```
Next you need to install these files to raspi. `make install` does not help us here, it has no idea where the target is. You do the install manually (set the raspi ip and user; unfortunately ssh config does not help with scp):
```
student@student-VirtualBox:~/paho.mqtt.c$ scp build/output/libpaho-mqtt3* pi@172.x.x.x:~
student@student-VirtualBox:~/paho.mqtt.c$ ssh rpi 'sudo cp libpaho* /usr/local/lib/'
```
Finally, we have paho mqtt libraries cross-built and installed both in debug environment and raspi. Next we can build applications that use those libraries.


### Create MQTT client application for raspi

1. To create a new cross-build project into {repo}/lab7/mqtt-raspi, revisit instructions [on this page](../lab2/howto-create-vscode-projects.md)): copy lab2 configs and rebuild CMake cache. Add paho-mqtt3c library to CMakeLists.txt:
2. Copy your C source to {repo}/lab7/mqtt-raspi/mqtt-client.c (edit the file: change broker address from localhost to VM IP assuming VM has bridged connection to lab net, and change payload message too)
```
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/)
target_link_libraries(lab7 /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/lib/libpaho-mqtt3c.so )
```
3. Build `F7` and Debug `F5`

You should have same functionality that you had with native application.

### Final task: The amazing demo

Add some GPIO functionality to complete the demo. For example, have a pushbutton connected to input GPIO so that button presses generate MQTT publish messages.

