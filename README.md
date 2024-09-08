
![](Images/PCB_WaterQualitySensor.png)
![](Arduino/Animation2.gif)

# Water Quality Sensor - Documentary/Cookbook
The development of the sensor was funded as part of the ParKli research project.
The ParKli research project is funded by the Baden-Württemberg Foundation in the programme “Innovations for Adaptation to Climate Change”.

## Further information
https://datahub.openscience.eu/dataset/parkli-boje <br>
https://parkli.de/

# Arduino ProMini - Connecting & programming

As a USB to TTL converter is required for programming, this must be wired beforehand.
Arduino ProMini USB to TTL converter.
 GND-GND
 RxD-TxD
 TxD-RxD
 VCC-nc
 
Connecting VCC only makes sense if the TTL converter is set to 3.3 V and the Arduino is not on the board or the switch (B1 - BTN on/off) is set to OFF. To program the Arduino, a power supply from the battery makes sense and VCC of the TTL remains unconnected.

(Programming via the USB port of the tp4056 module is not possible)

# Programming using the Arduino IDE

The libraries required for this are as follows:

[https://github.com/mcci-catena/arduino-lorawan](https://www.arduinolibraries.info/libraries/mcci-lo-ra-wan-lmic-library)

You should not forget to adjust the config file 
The default directory is: C:\Users\Administrator\Documents\Arduino\libraries\MCCI_LoRaWAN_LMIC_library\project_config
The following entries must be commented in for Europe and the Lora module used:

#define CFG_eu868 1

#define CFG_sx1276_radio 1

[https://github.com/rocketscream/Low-Power](https://www.arduinolibraries.info/libraries/low-power) <br>
[https://www.arduinolibraries.info/libraries/one-wire](https://www.arduinolibraries.info/libraries/one-wire) <br>
https://www.arduinolibraries.info/libraries/dallas-temperature


# Schematics and components used 
 
<img src="https://github.com/os4os-repo/WaterQualitySensor/blob/main/PcbResources/Schematic_WasserQualliSensor%20V1.1_2022-12-06.svg" width="1000">

1.)  The TP4056 module enables the built-in Li-ion batteries to be charged either via USB and a standard power supply unit or power bank or via a 6V solar panel, which is connected to X1. The batteries can be charged in the on and off position of switch B1.

2.) The circuit board is designed for use with two 18650 Li-Ion batteries. Operation with other rechargeable batteries in the voltage range of 3-4.2V is also possible. Additional rechargeable batteries can also be connected via connector X1 to enable a longer runtime. It is essential to ensure correct polarity when connecting!

3.) Switch B1 is used to switch the sensor on and off. 

4.) The MT3608 module increases the variable input voltage of the Li-ion batteries to 5V to supply the sensors with a constant voltage. The voltage must be set to 5V for the first use via the trim potentiometer and a multimeter. 

5.) The NPN mosfet Q1 can be used to control the power supply to the sensors and the LoraWan module. The connected modules are only supplied with power when pin D4 of the Arduino is set to HIGH. This allows the power consumption to be optimized. 
- During operation, the Arduino is in deep sleep most of the time and consumes approx. 30-40 μA.  
- To measure and send the values, the modules are only active for as short a time as possible.

6.) Switch B3 is connected to one of the Arduino's WakeUp pins. It offers the possibility of triggering any function of the Arduino without having to reset it.

7.) U5, U6, U7 and U9 are designed as JST XH2.54 3-pin connectors and thus prevent incorrect connection of the sensor modules. The pin mapping is systematic as follows: VCC-P1 Value-P2 GND-P3
- U5, U7 and U9 allow the connection of any 5v analog sensor.
- In conjunction with R5, U6 is designed for the use of one or more 18b20 temperature sensors.
- The RFM95W LoRa Radio Transceiver Module is used as a transmitter module. 
- The component group C1, R2 and R3 enable the Arduino to measure the voltage and thus the remaining capacity of the batteries. As all batteries are connected in parallel, their voltage is identical. The remaining capacity can be calculated based on the voltage curve of the installed batteries.



















 
 
