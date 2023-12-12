# LAb3 HW setup

## 1. The PicoScope 

We will be using PicoScope during this course, see:  
https://www.picotech.com/oscilloscope/2000/picoscope-2000-overview 

Download and install PicoScope 7 software for Picoscope 2205A 
Bonus: If you want to try PicoScope under Linux, feel free. (No experiences so far) 

## 2. Building the HW 

WARNING: NEVER TRY TO CONNECT OSCILLOSCOPE GROUND DIRECTLY TO RASPI GPIO PIN HEADER 
It is simply too large, you will cause short circuits. Use jumper wires. 

You can do this with power on but BE CAREFUL! 

For this lab you need: 
- A led ( C3024 component tower, G53-55) 
- 220ohm resistor (to be put in series) (C3024 component tower, E17) 
- A prototyping board 
- Jumper wires (male-to-female) 

Connect the LED + resistor (in series) between GPIO23 and Ground.  
Connect the oscilloscope to GPIO23 (on breakout board) and GND (on breakout board as well.  

If you are unsure how to connect oscilloscope, ask before you burn anything. 
See Hot oscilloscope tips
https://tuas365.sharepoint.com/sites/EmbeddedLinux/SiteAssets/Embedded%20Linux%20Notebook/_Content%20Library/Lab%20instructions.one 

see https://pinout.xyz/ for Raspi pin mappings. 

![pins](/lab3/images/pin-locations.png){width=80%} 

In the code we refer to GPIO 23. Find it from the pin map - check what's its location in the pin header. 

![Example setup](/lab3/images/example_setup.jpeg){width=40%}*Example setup*
 

 

## Assignment 1 

1. Change the program so that it blinks in an endless loop 
    - See the activity/frequency with an oscillosope. 
2. Change the loop delay to 1 millisecond (1000 us). Google for usleep() 
    - Measure the cycle time 
    - Measure the jitter (average / peak) using persistence view/ measurement tools (Duty cycle / high pulse width). 
    - what would you say about the quality of such "generated clock"? 
3. Change the loop delay to 1us and perform the same 
    - what is the true cycle time now? 
4. Remove the printf()-call (comment it out) 
    - what is the true cycle time now? 
5. Remove the 1us delay (comment it out) 
    - what is the true cycle time now? 
6. Add the printf back 
    - What are your observations? (have a look at the led as well) 
7. Add the following line after printf()
    - How does the situation change? 
``` 
fflush(stdout); 
```
    
8. Comment out the fflush()-call and now run the the code directly in RasPi 
    - What is the cycle time / jitter now? 

 The behaviour is explained in this post: 
https://stackoverflow.com/questions/39536212/what-are-the-rules-of-automatic-stdout-buffer-flushing-in-c 

In our case, during debugging, there is no terminal attached to the printf-command, so it is not automatically flushing after every printf, even if we have newline (/n)-character these. It is flushed when the buffer is full. BUT, if we run the same program directly in RasPi (that is, not via  debugger), we do have a terminal connected to the our application, so each printf(... \n ) will cause the actual printout, which will take some time.  

**A key point here is**: If you have timing-critical applications, NEVER use printf() or fprintf() for debugging, because it changes the timing dramatically. 

## Submission of assignment 1:   

- Commit your source files to git
- Create a text-file: Lab3_answers.txt containing the answers to questions 2-8 , and commit that to git as well 

## Assignment 2 

Build a HW that has a Button input and a LED output. You can expand the circuit of previous step.  

  
[A few words about the HW and input pins](configuring-gpio-pin-pullup.md)

When you press the button, your program measures the length of a button press, prints it into a log file and blinks a LED in following way: 
- 10 ms pulse (high), followed by 90 ms pause (low) 
- The total lenght of the pulse train equals lenght of the button press (100ms accuracy is enough)  

Example: The button press is 730 ms. Your program will output 7 pulses. 


## Submission of assignment 2: 

- Commit your source files to git 
- Grab the button input + led output with an oscilloscope, save it (select "current waveform only") as CSV-file and push it to git. 
