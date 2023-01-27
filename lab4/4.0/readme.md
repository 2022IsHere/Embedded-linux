
## 4.1 Cron

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

### Exercise 4.1: cron

Lets assume your task is to control a water pump that removes the excess water from building basement. You need to check the level switch every 30 minutes, and if level switch is on you need to run the pump for 10 minutes. To make it simple to experiment and modify the start/stop rules, you decide to develop cron rules to turn on the pump and turn off the pump (instead of writing the complete application in C for example).

1. Create two small applications for Raspberry pi: lab4-ledon and lab4-ledoff (use led to indicate the pump motor). Use lab3 source code as template (copy C file to this repository), and in that file remove all extra lines to leave the necessary steps to reserve GPIO line, set the output state, write a timestamped message to application log file and release the GPIO line. Connect your GPIO pin to LED circuit. Edit the CMakeLists.txt file to set up source file and target names as required (copy and comment existing setup, you'll need that later). Build and debug both codes. 
2. Debug operation made a copy of the executable to RasPi /home/pi as well, so can start experimenting with cron rules. First, try blinking the led at maximum cron speed: turn on the led at every even minute, and turn it off every odd minute. Store your cron lines as `lab4/results/cron_blink.txt`
3. Modify software to check an GPIO input line, and design cron rules for the original task: Check the level switch every 30 minutes, and if level switch is on you need to run the pump for 10 minutes. Note that it is not a problem to turn off a motor that is not running. After a couple of testing hours to confirm correct operation, store and commit `lab4/results/cron_motor.txt` 
4. You notice that the pump noise disturbs your late night sleep between 04-06, so want to skip those hours in the schedule. Additionally, September is so rainy month, that you need to check the level switch every 15 minutes. After a couple of testing months (there must some way to test this faster?) to confirm correct operation, store and commit your cron lines as `lab4/results/cron_motor_extended.txt` 

