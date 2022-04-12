# e-pulse
Development of telemedical (telehealth) system for remote monitoring of patient's pulse and oxygen levels in blood.

The project is developed for a university course 'Team Project : SEL029' @ University of Split - University Department of Professional Studies

### Circuit
According to the circuit diagram, attach microcontroller, sensors, power-supply and display.

Components used: 
* NodeMCU microcontroller (ESP8266 WiFi SoC microchip)
* MAX30102 sensor
* OLED display
* DHT11 sensor
* power-supply
* wires 

### Code - How it works?
As the project uses 3 NodeMCU platforms, each has its own code. The codes used are named as:
*  Online_PPG_DHT11.ino - code for streaming PPG waveform and DHT information (temperature and humidity)
*  Online_HR_ SpO2.ino - code for stream pulse and blood oxygen saturation
*  Local_EN.ino - code for local pulse display

#### Streaming PPG waveform, temperature and humidity (Online_PPG_DHT11.ino)
Streaming PPG waveform is done through the IoT platform Thinger.IO. The ESP8266 collects infrared data from the MAX30102 sensor using the _getIR()_ function. This function is actually called by the C ++ lambda function of the Thinger.io platform, the return value is stored in the "out" parameter of the "pulse" field. In the main loop of the program there is a call to the _handle()_ function and the _stream()_ function to which the "pulse" field is forwarded.

![PPG-waveform](https://github.com/mateax/e-pulse/blob/main/Online_PPG_DHT11/images%20-%20PPG%20waveform%2C%20temperature%2C%20humidity/PPG%20waveform%20-%20full%20screen.png)

![temperature-and-humidity](https://github.com/mateax/e-pulse/blob/main/Online_PPG_DHT11/images%20-%20PPG%20waveform%2C%20temperature%2C%20humidity/Temperature%20and%20humidity%20-%20dashboard.png)

The code sends the PPG waveform to the control panel of the Thinger.io platform, and sends data on the temperature and humidity of the room in which the patient is. The images show the PPG waveform, and the data from the DHT11 sensor on the dashboard and a magnified view when the graph is open full screen. You can find the code at this [link](https://github.com/mateax/e-pulse/blob/main/Online_PPG_DHT11/Online_PPG_DHT11.ino).


#### Sending pulse and blood oxygen saturation values (Online_HR_ SpO2)
As the algorithm is adapted for Arm Cortex M0 there are non-periodic jumps in measurement but they can be ignored in the code by setting conditions when data is streaming.
Simultaneous data stream and calculation of pulse and blood oxygen saturation on the same module results in distorted waveform and incorrect calculation since sampling takes a minimum of 4 seconds.

There is a minimum wavelength transmission delay of 4 seconds. But the problem is not that the stream is 4 seconds late but that samples that don’t have to be exactly one after the other come 4 seconds apart.
You can find code for display calculating heart rate, blood oxygen saturation and sending data to the Thinger.io platform at this [link](https://github.com/mateax/e-pulse/blob/main/Online_HR__SpO2/Online_HR__SpO2.ino).

![pulse-and-blood-oxygen-saturation](https://github.com/mateax/e-pulse/blob/main/Online_HR__SpO2/images%20-%20pulse%20and%20blood%20oxygen%20saturation/pulse%20and%20blood%20oxygen%20saturation.png)

#### Local pulse reading (Local_HR)
Local pulse reading uses a more precise algorithm implemented within the SparkFun library for MAX3010x sensors within the "heartRate.h" header file. This algorithm is different from the one that reads SpO2 and pulse at the same time, and is more accurate because the simultaneous reading of pulse and SpO2 introduces large oscillations of the measured values, so the calculation is more inaccurate.
Local pulse reading was performed in combination with an external display. You can find the code at this  [link](https://github.com/mateax/e-pulse/blob/main/Local_HR/Local_HR.ino).







