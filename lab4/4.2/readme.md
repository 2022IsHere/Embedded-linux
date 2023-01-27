## 4.2 Using HW PWM

Software-generated PWM (in lab 4.1) has so much jitter that it will wear out the servos very soon. Lets do the same using HW PWM capabilities of raspi. RasPi CPU has dedicated hardware for generating PWM signals: you need to set pulse period and duty time, and when enabled, hardware keeps generating that pulse train until disabled. Additionally, during pulse operation it is possible to adjust the period and duty time parameters, which allows us to to create better servo positioning.

### Enable HW PWM

In most linux systems, hardware setup is defined as **device tree** which is then given to kernel at boot time. In RasPi, the device tree resides in first SD card partition as an overlay system, where additional features can be enabled in config file.  
To edit the config.txt file, it is not necessary to remove the SD card, because the first partition is already mounted on the running system as /boot folder. So,
```
sudo nano /boot/config.txt
```
add line to end of file (after [all])
```
dtoverlay=pwm
```
and reboot raspi. After reboot, check that pwm hardware is recognised by kernel and necessary pwm drivers are loaded (an empty output would indicate no drivers are loaded):
```
pi@rpi0:~ $ lsmod | grep pwm
pwm_bcm2835            16384  0
```
You can now test the pwm functionality from GPIO pin 18 following instructions in  https://blog.oddbit.com/post/2017-09-26-some-notes-on-pwm-on-the-raspb/ like using sequence below (the values are in nanoseconds, i.e. 20000000ns == 20ms)
```
pi@rpi0:~ $ echo 0 > /sys/class/pwm/pwmchip0/export 
pi@rpi0:~ $ echo 20000000 > /sys/class/pwm/pwmchip0/pwm0/period 
pi@rpi0:~ $ echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle 
pi@rpi0:~ $ echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable 
pi@rpi0:~ $ echo 1700000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle 
pi@rpi0:~ $ echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle 
```
and checking with oscilloscope that there is specified output, and that it is jitter-free. You can connect servo as well.

### Exercise 3: kernel timers with HW PWM

From userspace, the kernel PWM driver can be controlled via sysfs as above (in C code you will use fopen(), write() etc commands). In order to have smooth servo motion, you need to change the PWM setting from a precalculated table at constant rate so that the servo follows **S-curve** minimizing abrupt changes in acceleration. S-curves, jerk (and snap, crackle and pop) are nicely explained in https://www.linearmotiontips.com/how-to-reduce-jerk-in-linear-motion-systems/. 

To Do:
- To initialize the hardware PWM programmatically, replicate the setup operations from command line example above
- Create a timer service that keeps triggering every 100 milliseconds
- Create a S-curve table
- In handler, read next profile position from a table and write it to PWM. When reaching end of table, continue from beginning.
- Create S-curve motion profile for the table
- Adjust timer rate (initially 100 msec) faster or slower, until servo movement feels (and sounds?) good (naturally, if you had mechanical loads connected to servo, the setting would be different)




