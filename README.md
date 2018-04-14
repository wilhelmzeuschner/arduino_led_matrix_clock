# Arduino LED Matrix Clock

This is a simple Desk Clock made from five 8x8 LED matrices and an Arduino Nano / Pro Mini

This clock uses a DS3231 RTC IC, which is not only quite accurate, but does also provide a temperature reading (which isn't as accurate but good enough).
In order to switch between the normal Time-Display to the Date / Temperature Reading, a touch sensor (TTP223) or button can be used. This sensor triggers an interrupt inside the code.

Unfortunately the LEDs and / or their driver chip becomes quite warm which is obliviously a problem when trying to acquire an accurate temperature reading of the environment.
Therefore I decided to use a DHT11 sensor which is placed further away, on the end of the power cable. One of the former data lines in an old USB cable can be used to communicate with the sensor. 
Since I²C uses two wires for communication, it would be possible to place the RTC on the end of the cable too.

In addition to the code, I have created a 3D-printable model for this clock.