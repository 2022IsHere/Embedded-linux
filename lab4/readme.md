# Time

Most embedded systems must operate with well-known schedules, because majority of processing tools (FIR/IIR digital filters, PID control etc) depend on uniformly sampled data points. Uniform sampling means that samples are taken at known regular intervals. Deviation from ideal timing is called jitter.

Different software and hardware architectures vary in timing accuracy:  
- Microcontrollers without cache memory can achieve cycle-accurate timing with proper programming. For example Arduino Uno @16MHz can time events and sequences to 62.5ns resolution (in single-threaded app).
- Microprocessors with instruction cache (like RasPi) are difficult to get running with repeatable timing. Any changes in program flow causes cache pages being loaded from/to main memory. Cache misses add random delay to any operation.
- Microprocessors with variable clock + VDD aim to optimise power consumption vs required load. Timings are different depending on the current clock frequency. This applies to RasPi as well.
- A **Bare metal application** runs directly on CPU without any operating system (OS) layer inbetween. Bare metal programs are easiest to analyse, as they contain only one main thread + interrupts. Measuring interrupt service run times and knowing interrupt rates, you can calculate all possible timing variations in interrupted main program (and interrupt latencies)
- **Real-time OS** (RTOS) architectures enable running the application in multiple threads. Often there are only fixed application threads, so OS runtime interference (and services) is minimal. RTOS timing can be analysed to 0.1-10us accuracy on typical platforms.
- Linux and other generic OS systems provide standardised platform for multiple independent applications and runtime command-line/graphical interface so that user can change running programs. To provide that functionality there are multiple system processes and threads running. Application CPU time is interleaved with other processes making accurate timing difficult. Standard Linux can achieve 100us-10ms timing accuracy on typical platforms (depending on system load).

## 1. Cron

Cron is the traditional tool for scheduling program execution to 1 minute resolution. Every user in the system has personal cron table (crontab), The user `pi` can list or edit cron table using commands
```
crontab -l
crontab -e
```
The cron table line syntax can get a bit complex. Each line in table can trigger one application at specified times.
```
5 4 * * * /home/pi/lab2
```
would run lab2 program every day at 04:05. For more details and examples see for example https://linuxhint.com/cron_jobs_complete_beginners_tutorial/ and https://crontab.guru/

### Exercise 1: cron

Lets assume your task is to control a water pump that removes the excess water from building basement. You need to check the level switch every 30 minutes, and if level switch is on you need to run the pump for 10 minutes. To make it simple to experiment and modify the start/stop rules, you decide to develop cron rules to turn on the pump and turn off the pump (instead of writing the complete application in C for example).

1. Create two small applications for Raspberry pi: lab4-ledon and lab4-ledoff (use led to indicate the pump motor). Use lab3 source code as template (copy C file to this repository), and in that file remove all extra lines to leave the necessary steps to reserve GPIO line, set the output state, write a timestamped message to application log file and release the GPIO line. Connect your GPIO pin to LED circuit. Edit the CMakeLists.txt file to set up source file and target names as required (copy and comment existing setup, you'll need that later). Build and debug both codes. 
2. Debug operation made a copy of the executable to RasPi /home/pi as well, so can start experimenting with cron rules. First, try blinking the led at maximum cron speed: turn on the led at every even minute, and turn it off every odd minute. Store your cron lines as `lab4/results/cron_blink.txt`
3. Modify software to check an GPIO input line, and design cron rules for the original task: Check the level switch every 30 minutes, and if level switch is on you need to run the pump for 10 minutes. Note that it is not a problem to turn off a motor that is not running. After a couple of testing hours to confirm correct operation, store and commit `lab4/results/cron_motor.txt` 
4. You notice that the pump noise disturbs your late night sleep between 04-06, so want to skip those hours in the schedule. Additionally, September is so rainy month, that you need to check the level switch every 15 minutes. After a couple of testing months (there must some way to test this faster?) to confirm correct operation, store and commit your cron lines as `lab4/results/cron_motor_extended.txt` 

## 2. Linux kernel timers

Cron mechanism provides high configurability, but is limited one minute resolution. Linux kernel timers can be applied from C code with sub-millisecond accuracy (some jitter is generated by OS processes taking CPU time). The original setup of the lab files provides an example of using the timers. Three timers are set up, and each timer event will trigger execution of handler code. Build, debug and study the example.

### Exercise 2: kernel timers

Modify kernel timer example to produce software generated PWM pulses for a servo motor. Servo pulses shall occur every 20 milliseconds and the nominal pulse length for servo middle position is 1.5ms. The angular position is controlled with pulse modulation, where pulse width can vary from 1.0 ms to 2.0 ms.  
- Create one timer task that activates every 20 milliseconds (PWM pulse start event). Bring in necessary header files and add GPIO output pin, and for testing, make it toggle every time event handler is called. Check with oscilloscope that output meets your expectations.
- In same event handler, create a new timer for PWM pulse length (between 1 and 2 milliseconds, the actual value does not matter at this phase). Create timer handler for this pulse stop timer. In handler, turn off the output (and in pulse start event replace the toggle with plain output activation to high state). Debug and verify with oscilloscope.
- Get a real servo (take a standard servo, there are continuous-running servos as well) and connect it to raspi. Red wire to +5V, Black wire to GND, and white wire to control pin. Test.

## 3. Case PWM

Software-generated PWM has so much jitter that it will wear out the servos very soon. Lets do the same using HW PWM capabilities of raspi (RasPi CPU has dedicated hardware for generating PWM signals).

### Enable HW PWM

In most linux systems, hardware setup is defined as **device tree** which is then given to kernel at boot time. In RasPi, the device tree resides in first SD card partition as has an overlay system, where additional features can be enabled in config file.  
To edit the config.txt file, it is not necessary to remove the SD card, because the first partition is already mounted on the running system as /boot folder. So,
```
sudo nano /boot/config.txt
```
add line to end of file (after [all])
```
dtoverlay=pwm
```
and reboot raspi. After reboot, check that pwm hardware is recognised by kernel and necessary pwm drivers are loaded:
```
pi@rpi0:~ $ lsmod | grep pwm
pwm_bcm2835            16384  0
```
You can nopw test the pwm functionality from GPIO pin 18 using instructions in  https://blog.oddbit.com/post/2017-09-26-some-notes-on-pwm-on-the-raspb/ like sequence
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

From userspace, the kernel PWM driver can be controlled via sysfs as above (in C code you will use fopen(), write() etc commands). In order to have smooth servo motion, you need to change the PWM setting from a precalculated table at constant rate so that the servo follows **S-curve** minimizing abrupt changes in acceleration.  For a deep dive, there is a very detailed explanation in https://www.pmdcorp.com/resources/type/articles/get/mathematics-of-motion-control-profiles-article  
To Do:
- Initialize hardawre PWM programmatically (replicating the operations from command line example above)
- Create a timer service that keeps triggering every 100 milliseconds
- In handler, read next profile position from a table and write it to PWM. When reaching end of table, continue from beginning.
- Create S-curve motion profile for the table
- Adjust timer rate (initially 100 msec) faster or slower, until servo movement feels (and sounds?) good (naturally, if you had mechanical loads connected to servo, the setting would be different)




