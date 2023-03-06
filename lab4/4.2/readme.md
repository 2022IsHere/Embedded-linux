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

### Exercise 4.2: kernel timers with HW PWM

From userspace, the kernel PWM driver can be controlled via sysfs as above (in C code you will use fopen(), write() etc commands). In order to have smooth servo motion, you need to change the PWM setting from a precalculated table at constant rate so that the servo follows **S-curve** minimizing abrupt changes in acceleration. S-curves, jerk (and snap, crackle and pop) are nicely explained in https://www.linearmotiontips.com/how-to-reduce-jerk-in-linear-motion-systems/. 

To Do:
- To initialize the hardware PWM programmatically, replicate the setup operations from command line example above. **NOTE** that opening a file, writing to a file, and closing the file does not quarantee that the file gets updated; you need to flush the stream! 
- Create a timer service that keeps triggering every 100 milliseconds
- Create S-curve motion profile for the table. You can start with the S-curve table below. Note that this is only 1-way movement from 0-100%, you must return to zero as well to be able to run the table data cyclically. You need to scale the percentage points to nanoseconds for servfo motor. Good starting point is 0% == 1.2ms and 100% == 1.8ms duty time. Looks like Excel work.
- In handler, read next profile position from a table and write it to PWM. When reaching end of table, continue from beginning.
- Adjust timer rate (initially 100 msec) faster or slower, until servo movement feels (and sounds?) good (naturally, if you had mechanical loads connected to servo, the setting would be different). It does not make sense to change duty time setting faster than every 20ms, because the output pulses aren't generated any faster. Making the steps too slow makes every step separate thus losing the benefits of S-curve.

```
0,0 %
0,4 %
1,3 %
3,0 %
5,6 %
9,3 %
14,2 %
20,3 %
27,0 %
34,3 %
41,9 %
49,7 %
57,6 %
65,2 %
72,5 %
79,2 %
85,2 %
90,3 %
94,3 %
97,0 %
98,6 %
99,6 %
100,0 %
```

