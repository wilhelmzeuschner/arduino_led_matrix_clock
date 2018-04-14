# Arduino LED Matrix Clock

This is a simple Desk Clock made from five 8x8 LED matrices and an Arduino Nano / Pro Mini

This clock uses a DS3231 RTC IC, which is not only quite accurate, but does also provide a temperature reading (which isn't as accurate but good enough).
In order to switch between the normal Time-Display to the Date / Temperature Reading, a touch sensor (TTP223) or button can be used. This sensor triggers an interrupt inside the code.

Unfortunately the LEDs and / or their driver chip becomes quite warm which is obliviously a problem when trying to acquire an accurate temperature reading of the environment.
Therefore I decided to use a DHT11 sensor which is placed further away, on the end of the power cable. One of the former data lines in an old USB cable can be used to communicate with the sensor. 
Since I²C uses two wires for communication, it would be possible to place the RTC on the end of the cable too.

In addition to the code, I have created a 3D-printable model for this clock:
https://www.thingiverse.com/thing:2861401


The following libraries are needed:

https://github.com/Makuna/Rtc & https://github.com/MajicDesigns/MD_MAX72XX

Optional: https://github.com/adafruit/DHT-sensor-library

This code, combined with the enclosure is quite flexible, thanks to the Arduino IDE.
You could use an ESP8266 for example and get accurate time information via NTP.
Ultimately you can display whatever you want on these matrices...

Here is an image of the clock:
![Clock](https://github.com/wilhelmzeuschner/arduino_led_matrix_clock/blob/master/images/IMG_20180414_124421.jpg)
