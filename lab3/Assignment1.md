Question 2: Change the loop delay to 1 millisecond (1000 us). Google for usleep()
1. Task: Measure the cycle time | Answer: ~2,202-2,204 ms
2. Task: Measure the jitter (Duty cycle / high pulse width) | Answer: ~1,967-1,990 ms
3. Task: what would you say about the quality of such "generated clock"? | Answer: The managable jitter level really depends on the application purpose, therefore it is hard to say is it exactly good or bad. Based on application requirement, we shall see.

Change the loop delay to 1us and perform the same
1. Task: what is the true cycle time now? | Answer: ~147 - 178 us

Remove the printf()-call (comment it out)
1. Task: what is the true cycle time now? | Answer: 126,3-145,5 us

Remove the 1us delay (comment it out)
1. Task: what is the true cycle time now? | Answer: 3,803-3,855 us

Add the printf back
1. What are your observations? (have a look at the led as well) | Answer: 20,02 - 31,60 us

Add the following line after printf() 'fflush(stdout);'
1. How does the situation change? | Answer: ~21,06 us

Comment out the fflush()-call and now run the the code directly in RasPi
1. Task: What is the cycle time / jitter now? | Answer: ~21,06 us

SIDE NOTE: At third last step, vscode did it hard for me to go through the steps as it did not start building without closing-opening-powering off virtual machine so the results may be wrong due to memory usage on my laptop.



