# Time

Most embedded systems must operate with well-known schedules, because majority of processing tools (FIR/IIr difgital filters, PID control etc) depend on uniformly sampled data points. Uniform sampling means that samples are taken at known regular intervals. Deviation from ideal timing is called jitter.

Different software and hardware architectures vary in timing accuracy:  
- Microcontrollers without cache memory can achieve cycle-accurate timing with proper programming. For example Arduino Uno @16MHz can time events and sequences to 62.5ns resolution (in single-threaded app).
- Microprocessors with cache (like RasPi) are difficult to get running with repeatable timing. Any changes in program flow causes cache pages being loaded from/to main memory. Cache misses add random delay to any operation.
- Bare metal application runs directly on CPU without any operating system (OS) layer inbetween. Bare metal programs are easiest to analyse, as they contain only one main thread + interrupts. Measuring interrupt service run times and knowing interrupt rates, you can calculate all possible timing variations in interrupted main program (and interrupt latencies)
- Real-time OS (RTOS) architectures enable running the application in multiple threads. Often there are only fixed application threads, so OS runtime interference (and services) is minimal. RTOS timing can be analysed to 0.1-10us accuracy on typical platforms.
- Linux and other generic OS systems provide standardised platform for multiple independent applications and runtime command-line/graphical interface so that user can change running programs. To provide that functionality there are multiple system processes and threads running. Application CPU time is interleaved with other processes making accurate timing difficult. Standard Linux can achieve 100us-10ms timing accuracy on typical platforms (depending on system load).

## Cron

Cron is the traditional tool for scheduling program execution to 1 minute resolution. Every user in the system has personal cron table (crontab), The user `pi` can list or edit cron table using commands
```
crontab -l
crontab -e
```
The cron table line syntax is a bit complex. Each line in table can trigger one application at specified times.
```
5 4 * * * /home/pi/lab2
```
would run lab2 program every day at 04:05. For more examples see (https://crontab.guru/)


  
