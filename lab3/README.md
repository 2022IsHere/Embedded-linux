# The HW interface 
 
In this exercise we will create simple C-programs for RasPi, which utilize basic I/O-access through gpiod-library 
 
A few words on GPIO/GPIO-programming - see the pages below: 
 
https://www.ics.com/blog/gpio-programming-exploring-libgpiod-library 
https://www.ics.com/blog/introduction-gpio-programming 
      
## 1. Setting up the project environment 

- Copy cross-compilation setup (vscode / cmake-related files + makefiles) from lab2 using a script ```./copy_stuff.sh```: execute it in folder lab3
 
## 2. Get the example source codes 
 
```
student@student-VirtualBox:~/embedded-linux-labs/lab3$ git clone https://github.com/starnight/libgpiod-example
```

## 3. Organize the code

Create a directory "src" under lab3 
Copy the led-output example and input-example under src, so that you rename the files, like this: 
```
student@student-VirtualBox:~/embedded-linux-labs/lab3$ cp libgpiod-example/libgpiod-input/main.c src/libgpiod-input.c 
student@student-VirtualBox:~/embedded-linux-labs/lab3/src$ cp libgpiod-example/libgpiod-led/main.c src/libgpiod-led.c 
``` 

## 4. Configure the build 
 
Next, launch vscode (in directory lab3) 
Now we need to modify CMakeLists.txt.  
The copied example from lab2 defines an executable name "lab" and a single source file from which the executable is compiled from. We want to extend this to use multiple source files using a variable. Replace existing build dependency with
``` 
set(SRC_FILES  
        src/libgpiod-led.c         
        src/libgpiod-input.c 
) 

add_executable(lab3 ${SRC_FILES}) 
```
Additionally, replace all occurrences of lab2 to current executable. Also, if you share the RasPi with your fellow student, remember to use different names for executables. 
 
## 5. Build the project  

Select either c code file (it opens in main view), then press F7.  
Select kit GCC 13.2.0  

You should see a errors: 
```
[build] /home/student/embedded-linux-labs/lab3/src/libgpiod-input.c:1:10: fatal error: gpiod.h: No such file or directory
[build]     1 | #include <gpiod.h>
[build]       |          ^~~~~~~~~
```
This happens, because in order to access the raspi GPIO pins, our code has ```#include <gpiod.h>```, which is a header file for gpio handling functions. The header must be found somewhere for the compilation to succeed. Now we are missing the gpiod library (and libgpiod-dev package which contain the the header file gpiod.h). 
 
 
## 6. Installing the missing libraries 
 
We need to use a library providing us suitable functions to operate with GPIOs. (Such a library is libgpiod). Note that the last character "d" does not refer to daemon (=background process), it refers to device, as the library exploits the new GPIO character device. Somewhat confusing and misleading. 
We start by installing the gpiod –package on the raspberry pi. 
```
ssh rpi 
```
In raspi:  
```
sudo apt install gpiod 
```
(it is probably already installed) 
 
Gpiod will provide you a set of command line interface  (CLI) tools, which you can use in your scripts – or call from C programs ( using system() -call), Python programs ( using os.system() -call) etc.  
| command | operation
|-----------------|:-------------| 
|gpiodetect | List all GPIO chips present on the system, their names, labels and number of GPIO lines. 
|gpioinfo | List all lines of specified GPIO chips, their names, consumers, direction, active state and additional flags. 
|gpioget | Read values of specified GPIO lines. 
|gpioset | Set values of specified GPIO lines, and potentially keep the lines exported and wait until timeout, user input or signal. 
|gpiofind | Find the GPIO chip name and line offset given the line name. 
|gpiomon | Wait for events on GPIO lines, specifying which events to watch, how many events to process before exiting or if the events should be reported to the console 
 
(Source: https://www.ics.com/blog/gpio-programming-exploring-libgpiod-library) 
 
However, calling these functions via system calls would be very ineffective – we want to call them directly from C, using the APIs provided by a shared (static) library libgpiod. It was installed together with gpiod (via dependencies). To be able to use these library functions for cross-compilation in your host machine, you need to provide the header files, plus you need to provide the pre-compiled static libraries. Install those (with docs), again, on the raspberry pi 
``` 
sudo apt install libgpiod-dev libgpiod-doc 
```
To see more info on the installed package: 
``` 
apt info libgpiod-dev 
 
pi@JarnoRaspi1:~ $ apt info libgpiod-dev 
Package: libgpiod-dev
Version: 1.6.3-1+rpi1+b2
Priority: optional
Section: libdevel
Source: libgpiod (1.6.3-1+rpi1)
Maintainer: SZ Lin (林上智) <szlin@debian.org>
Installed-Size: 295 kB
Depends: libgpiod2 (= 1.6.3-1+rpi1+b2)
Suggests: libgpiod-doc
Homepage: https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git
Download-Size: 52.3 kB
APT-Manual-Installed: yes
APT-Sources: http://raspbian.raspberrypi.com/raspbian bookworm/main armhf Packages
Description: C library for interacting with Linux GPIO device - static libraries and headers
 libgpiod encapsulates the ioctl calls and data structures
 behind a straightforward API. This new character device
 interface guarantees all allocated resources are freed after
 closing the device file descriptor and adds several new
 features that are not present in the obsolete sysfs interface
 (like event polling, setting/reading multiple values at once or
 open-source and open-drain GPIOs).
 .
 This package contains the required static libraries, headers, and C++ bindings.
``` 
To see, where the files of the package are installed, type: 
``` 
pi@JarnoRaspi1:~ $ dpkg-query -L libgpiod-dev 
 
/. 
/usr 
/usr/include 
/usr/include/gpiod.h 
/usr/include/gpiod.hpp 
/usr/lib 
/usr/lib/arm-linux-gnueabihf 
/usr/lib/arm-linux-gnueabihf/libgpiod.a 
/usr/lib/arm-linux-gnueabihf/libgpiodcxx.a 
/usr/lib/arm-linux-gnueabihf/pkgconfig 
/usr/lib/arm-linux-gnueabihf/pkgconfig/libgpiod.pc 
/usr/lib/arm-linux-gnueabihf/pkgconfig/libgpiodcxx.pc 
/usr/share 
/usr/share/doc 
/usr/share/doc/libgpiod-dev 
/usr/share/doc/libgpiod-dev/README.gz 
/usr/share/doc/libgpiod-dev/changelog.Debian.gz 
/usr/share/doc/libgpiod-dev/copyright 
/usr/lib/arm-linux-gnueabihf/libgpiod.so 
/usr/lib/arm-linux-gnueabihf/libgpiodcxx.so 
pi@JarnoRaspi1:~ $  
```

So, the library we have installed is version xxxxxx1.6.3-1+rpi1  and the actual file is under /usr/lib/arm...  which makes sense because we are operating on an arm platform. 
Now, we need to synchronize the build environment of your host PC to have the same library (and headers) as the target. 
 
```
student@student-VirtualBox:~/embedded-linux-labs/lab3$ sudo sbuild-apt rpi3-bookworm-armhf apt-get update 
student@student-VirtualBox:~/embedded-linux-labs/lab3$ sudo sbuild-apt rpi3-bookworm-armhf apt-get upgrade 
student@student-VirtualBox:~/embedded-linux-labs/lab3$ sudo sbuild-apt rpi3-bookworm-armhf apt-get install gpiod 
student@student-VirtualBox:~/embedded-linux-labs/lab3$ sudo sbuild-apt rpi3-bookworm-armhf apt-get install libgpiod-dev 
``` 
 
Now you have installed the gpiod lib and related development packages to both, RasPi and your Build environment. 
 
## 7. Compilation ( second try ) 

In vscode, select project file and press F7.   
You will still get the same errors.  
 
Now that we have installed the libraries and header files, they are in the build system, but you need you need tell Cmake where to find them.     
Find them first manually by typing: 
```
sudo find / -name gpiod.h 
``` 
Add the following lines to CMakeLists.txt 
``` 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/arm-linux-gnueabihf/) 
``` 
In VSCode, press Shift-Ctrl-P and select "Delete Cache and reconfigure" 
 
## 8. Third compilation try

Now again select C source file and try to rebuild (F7). 
 
Different errors this time. But now you can see that the compilation is successful. 
``` 
[build] [ 33%] Building C object CMakeFiles/lab3.dir/src/libgpiod-led.c.o 
[build] [ 66%] Building C object CMakeFiles/lab3.dir/src/libgpiod-input.c.o 
[build] [100%] Linking C executable lab3 
 
Linking fails.  
 
[build] /home/student/opt/x-tools/armv6-rpi-linux-gnueabihf/bin/../lib/gcc/armv6-rpi-linux-gnueabihf/12.1.0/../../../../armv6-rpi-linux-gnueabihf/bin/ld.bfd: CMakeFiles/lab3.dir/src/libgpiod-input.c.o: in function `main': 
[build] /home/student/projects/embedded-linux-labs/lab3/src/libgpiod-input.c:10: multiple definition of `main'; CMakeFiles/lab3.dir/src/libgpiod-led.c.o:/home/student/projects/embedded-linux-labs/lab3/src/libgpiod-led.c:10: first defined here 
[build] /home/student/opt/x-tools/armv6-rpi-linux-gnueabihf/bin/../lib/gcc/armv6-rpi-linux-gnueabihf/12.1.0/../../../../armv6-rpi-linux-gnueabihf/bin/ld.bfd: CMakeFiles/lab3.dir/src/libgpiod-led.c.o: in function `main': 
``` 
 
The problem is that we have now several source codes, which each have main()-function. Every C-program must have one – and only one - main()-function. That is an built-in entry point to your application. Make sure that you have only one main()-function – considering ALL the project files. If you have several, rename (or delete) the unnecessary ones. 
 
A handy trick is to use C preprocessor directives (which start with a #) 
 
Open libgpiod-input.c and modify it like this: 
```
#if 0 
 
< the code > 
 
#endif 
```
You will see that the code changes to green (thanks to VSCode's syntax coloring / ability to understand these preprocessor directives). 
Instead of using "0" as confition for #if, you can also use compile flags to pass information to preprocessor. This is especially useful if/when you need to make build variants of your program. 
 
## 9. Compilation ( 4th ) 
 
Now try to rebuild (F7). 
 
Again, it fails.  
``` 
[build] /home/student/opt/x-tools/armv6-rpi-linux-gnueabihf/bin/../lib/gcc/armv6-rpi-linux-gnueabihf/12.1.0/../../../../armv6-rpi-linux-gnueabihf/bin/ld.bfd: CMakeFiles/lab3.dir/src/libgpiod-led.c.o: in function `main': 
[build] /home/student/projects/embedded-linux-labs/lab3/src/libgpiod-led.c:18: undefined reference to `gpiod_chip_open_by_name' 
``` 
This time the reason is that it cannot find the library, which contains the gpiod-functions, which were introduced in "gpiod.h" 
 
Add the following line to CMakeLists.txt 
``` 
target_link_libraries(lab3 /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so) 
``` 
## 10. Try to recompile
 
``` 
[build] [ 33%] Linking C executable lab3 
[build] [100%] Built target lab3 
[build] Build finished with exit code 0 
``` 
 
Finally! :) 

Now it is a good time to commit all changes in your lab3 files to repository. A proper commit message would be something like "lab3 project set up for successful build, not tested yet" which would be helpful info if you need to revert back to earlier commits in future.
 
## 11. Debugging 
 
Start debugging by pressing F5. Play with breakpoints, see how the program works.  Now, obviously, you cannot see the LED blinking, because you don't have the LED there yet, so we need to build the HW first, continue to

[HW setup for lab3](lab3_hw_setup.md)
