# Configuring the input pin to have internal pull-up resistor 

The GPIO-pins of raspi have a few important properties: 

- Direction – is it input or output? 
- Bias – if it is an input, does it have pull-up or pull-down resistor, or neither. 

In Raspi terminal, type: gpioinfo 
```
pi@JarnoRaspi1:~ $ gpioinfo 
gpiochip0 - 58 lines: 
line   0:     "ID_SDA"       unused   input  active-high  
line   1:     "ID_SCL"       unused   input  active-high  
line   2:       "SDA1"       unused   input  active-high  
line   3:       "SCL1"       unused   input  active-high  
line   4:  "GPIO_GCLK"       unused   input  active-high  
line   5:      "GPIO5"       unused   input  active-high  
line   6:      "GPIO6"       unused   input  active-high  
line   7:  "SPI_CE1_N"       unused   input  active-high  
line   8:  "SPI_CE0_N"       unused   input  active-high  
line   9:   "SPI_MISO"       unused   input  active-high  
line  10:   "SPI_MOSI"       unused   input  active-high  
line  11:   "SPI_SCLK"       unused   input  active-high  
line  12:     "GPIO12"       unused   input  active-high  
line  13:     "GPIO13"       unused   input  active-high  
line  14:       "TXD1"       unused   input  active-high  
line  15:       "RXD1"       unused   input  active-high  
line  16:     "GPIO16"       unused   input  active-high  
line  17:     "GPIO17"       unused   input  active-high  
line  18:     "GPIO18"       unused   input  active-high  
line  19:     "GPIO19"       unused   input  active-high  
line  20:     "GPIO20"       unused   input  active-high  
line  21:     "GPIO21"       unused   input  active-high  
line  22:     "GPIO22"       unused   input  active-high  
line  23:     "GPIO23"       unused  output  active-high  
line  24:     "GPIO24"       unused  output  active-high  
line  25:     "GPIO25"       unused  output  active-high  
line  26:     "GPIO26"       unused   input  active-high  
line  27:     "GPIO27"       unused   input  active-high  
line  28: "RGMII_MDIO"       unused   input  active-high  
line  29:  "RGMIO_MDC"       unused   input  active-high  
line  30:       "CTS0"       unused   input  active-high  
line  31:       "RTS0"       unused   input  active-high  
line  32:       "TXD0"       unused   input  active-high  
line  33:       "RXD0"       unused   input  active-high  
line  34:    "SD1_CLK"       unused   input  active-high  
line  35:    "SD1_CMD"       unused   input  active-high  
line  36:  "SD1_DATA0"       unused   input  active-high  
line  37:  "SD1_DATA1"       unused   input  active-high  
line  38:  "SD1_DATA2"       unused   input  active-high  
line  39:  "SD1_DATA3"       unused   input  active-high  
line  40:  "PWM0_MISO"       unused   input  active-high  
line  41:  "PWM1_MOSI"       unused   input  active-high  
line  42: "STATUS_LED_G_CLK" "led0" output active-high [used] 
line  43: "SPIFLASH_CE_N" unused input active-high  
line  44:       "SDA0"       unused   input  active-high  
line  45:       "SCL0"       unused   input  active-high  
line  46: "RGMII_RXCLK" unused input active-high  
line  47: "RGMII_RXCTL" unused input active-high  
line  48: "RGMII_RXD0"       unused   input  active-high  
line  49: "RGMII_RXD1"       unused   input  active-high  
line  50: "RGMII_RXD2"       unused   input  active-high  
line  51: "RGMII_RXD3"       unused   input  active-high  
line  52: "RGMII_TXCLK" unused input active-high  
line  53: "RGMII_TXCTL" unused input active-high  
line  54: "RGMII_TXD0"       unused   input  active-high  
line  55: "RGMII_TXD1"       unused   input  active-high  
line  56: "RGMII_TXD2"       unused   input  active-high  
line  57: "RGMII_TXD3"       unused   input  active-high  
gpiochip1 - 8 lines: 
line   0:      "BT_ON"       unused  output  active-high  
line   1:      "WL_ON"       unused  output  active-high  
line   2: "PWR_LED_OFF" "led1" output active-low [used] 
line   3: "GLOBAL_RESET" unused output active-high  
line   4: "VDD_SD_IO_SEL" "vdd-sd-io" output active-high [used] 
line   5:   "CAM_GPIO" "cam1_regulator" output active-high [used] 
line   6:  "SD_PWR_ON" "sd_vcc_reg"  output  active-high [used] 
line   7:    "SD_OC_N"       unused   input  active-high  
``` 

In case the I/O-pins direction is output, you can set the value and properties of that pin with gpioset.  
In case of input, use gpioget to read its value and configure its properties 

```
pi@JarnoRaspi1:~ $ gpioget -h 
Usage: gpioget [OPTIONS] <chip name/number> <offset 1> <offset 2> ... 

Read line value(s) from a GPIO chip 
  
Options: 
  -h, --help: display this message and exit 
  -v, --version: display the version and exit 
  -l, --active-low: set the line active state to low 
  -B, --bias=[as-is|disable|pull-down|pull-up] (defaults to 'as-is'): 
set the line bias 
 
Biases: 
  as-is: leave bias unchanged 
  disable: disable bias 
  pull-up: enable pull-up 
  pull-down: enable pull-down 
```
 
Now, if you want to have a button input, the most common (and best way) way is to connect a button between a GPIO-pin and ground, and provide a pull-up resistor. When the button is unpressed, the pull-up resistor "pull" the input high = to vcc = logical '1'. When the button is pressed, it connects the gpio-pin to ground = logical '0'. 

![twoways](/lab3/images/two-ways-to-wire-switch.jpg)

Image source: https://www.dummies.com/article/technology/computers/hardware/raspberry-pi/recognizing-and-coping-with-contact-bounce-on-the-raspberry-pi-143678/ 

Now, to make our circuit easier, we can use the built-in pull-up resistor of raspberry pi GPIO cell. 

For example, setting pull-up resistor for pin 22: 
```
pi@JarnoRaspi1:~ $ gpioget --bias=pull-up gpiochip0 22 
```
 
Now we can get rid of the external 3k3 resistor, plus we don't have to route 3.3V Vcc to the breadboard. 
How to do this in your program? 

Well, there might be a function in gpiod-library for that, but at least I couldn't find it. So let's use a work-around: 

You can use system()-function 
```
system("gpioget --bias=pull-up gpiochip0 22"); 
```
 
Place this after you have configured the line as an input ( with gpiod_chip_get_line() -function call ) 
 
