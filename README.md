# e-pulse
Development of telemedical (telehealth) system for remote monitoring of patient's pulse and oxygen levels in blood.
The project is developed for a university course 'Team Project : SEL029' @ University of Split - University Department of Professional Studies

#### e-pulse online platform:
![start-position](https://github.com/mateax/laser-detection-system/blob/main/final-prototype/start-position.jpg)

#### schematic:
![first-laser-detection](https://github.com/mateax/laser-detection-system/blob/main/final-prototype/first-laser-detection.jpg)

#### Second laser detection - final position:
![second-laser-detection](https://github.com/mateax/laser-detection-system/blob/main/final-prototype/second-laser-detection.jpg)


## What it is?
**opiši 

## How it works?

Special Mecanum wheels allow movement in all directions. For example, a prototype first searches for one laser beam, when the first laser beam is detected, then the entire assembly moves to the left. At the moment of detection of the photodiode beam, its corresponding LED lights up. In this way, the accuracy and precision of detection is ensured. 

This mode of movement is not possible with normal wheels used on cars, so special Mecanum wheels are used that allow movement in all directions.

## Circuit
According to the circuit diagram, attach resistors, photodiodes and LEDs ,and hook them up to an Arduino board.

**popisati komponente
Components used: 
* Arduino UNO
* Photodiode OSRAM PIN SFH203
* LED diodes
* resistors (1kOhm, 100kOhm)
* wires 

## Code
As the project uses 3 NodeMCU platforms, each has its own code. The codes used are named as:
• Online_PPG_DHT11.ino - code for streaming PPG waveform and DHT information (temperature and humidity)
• Online_HR_ SpO2.ino - code for stream pulse and blood oxygen saturation
• Local_EN.ino - code for local pulse display

#### Streaming PPG waveform, temperature and humidity (Online_PPG_DHT11.ino)
Streaming PPG waveform is done through the IoT platform Thinger.IO. The ESP8266 collects infrared data from the MAX30102 sensor using the getIR () function. This function is actually called by the C ++ lambda function of the Thinger.io platform, the return value is stored in the "out" parameter of the "pulse" field. In the main loop of the program there is a call to the handle () function and the stream () function to which the "pulse" field is forwarded.

The code sends the PPG waveform to the control panel of the Thinger.io platform, and sends data on the temperature and humidity of the room in which the patient is. The images show the PPG waveform, and the data from the DHT11 sensor on the dashboard and a magnified view when the graph is open full screen.
**staviti u italic određene nazive fje i slike

#### Sending pulse and blood oxygen saturation values (Online_EN_ SpO2)
As the algorithm is adapted for Arm Cortex M0 there are non-periodic jumps in measurement but they can be ignored in the code by setting conditions when data is flowing.
Simultaneous data flow and calculation of pulse and blood oxygen saturation on the same module results in distorted waveform and incorrect calculation since sampling takes a minimum of 4 seconds. There is a minimum wavelength transmission delay of 4 seconds. But the problem is not that the stream is 4 seconds late but that samples that don’t have to be exactly one after the other come 4 seconds apart.
A display of the code for calculating heart rate, blood oxygen saturation and sending data to the Thinger.io platform is at this link.
**staviti tekst opisa i link na kod
![android-app](https://github.com/mateax/laser-detection-system/blob/main/android-app-screen.jpg) 

#### Local pulse reading (Local_HR)
Local pulse reading uses a more precise algorithm implemented within the SparkFun library for MAX3010x sensors within the "heartRate.h" header file. This algorithm is different from the one that reads SpO2 and pulse at the same time, and is more accurate because the simultaneous reading of pulse and SpO2 introduces large oscillations of the measured values, so the calculation is more inaccurate.
Local pulse reading was performed in combination with an external display. You can find the code at this link




