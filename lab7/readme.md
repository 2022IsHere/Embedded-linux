# MQTT

This time you want to create an amazing MQTT demo on RasPi. In step A you demonstrate that it is quite straightforward to install mosquitto package to VM and see that you can publish and subscribe messages from command line.
In step B you aim to apply existing C code example  to publish our GPIO pin data from raspi. Quite immediately you will see a couple of problems:
- paho prebuilt libraries are distributed for x86 architecture only, not for raspi. You need to cross-build the libraries.
- paho libraries have dependencies on other libraries (openssl), and you need to install those in our development system as target architecture binary, and install those to target system too.
(If you need to refresh on concepts of publish-subscribe architecture and MQTT, please check these links: )

After recognizing the work ahead, you will want to question the original project targets
1. to develop the system in C language (Python solution is one pip install away), and 
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

Follow the instructions (clone - make - sudo make install), and test CLI utility paho_c_pub to send a message to your still running mosquitto broker (have subscriber running as well.) NOTE that I recommend you to clone the paho repository into your home folder. It is possible to clone it under labs/lab7 as well and add the paho folder to .gitignore (and maybe use git submodules feature to manage all), but don't.

After build you can check that the library included cli utility runs OK. Just send a test message and see that your mosquitto subscriber gets it:


paho_c_pub -c localhost -t /home/sensors/temp/kitchen -m "Hello from x86 paho.mqtt.c"

Successful publish operation proves our native compilation was successful, so we have the courage to start tinkering with example source code.

1. Create two project folders under labs/lab7:
labs/lab7/mqtt-vm
labs/lab7/mqtt-raspi
2. Copy the example code from paho web page bottom as it is into labs/lab7/mqtt-vm/mqtt-client.c 
3. Set up native C project into labs/lab7/mqtt-vm/ (see instructions in ).
4. Select mqtt-client.c as active tab and build (Shift-Ctrl-B). The build should fail because paho libraries are not linked to the project.
5. Edit .vscode/tasks.json to add argument "-lpaho-mqtt3c" for the build (append it, i.e. insert as last element in argument list).
Note: Library names and file names are somewhat confusing. The library file name is "libpaho-mqtt3c.so", but library name (not file) is filename without prefix "lib" and without suffix. The file itself is usually not a file, but a symlink to main version file, which is symlink to actual library file... so, to link to actual "libpaho-mqtt3c.so.1.3" file we add argument "-lpaho-mqtt3c".
6. Select mqtt-client.c as active tab and build (Shift-Ctrl-B). The build should be successful.
7. Edit the mqtt topic in source to match topic your subscriber is watching. Rebuild and test (F5).

## C. Cross built C language client in RasPi





