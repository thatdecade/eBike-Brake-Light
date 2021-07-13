# eBike-Brake-Light
Neopixel Ring Based Brake Light

This is a brake light I made for my RadPower ebike.  Connects to the 4 pin wire harness from the motor controller.

# Part List:

* Bafang 4 pin extension cable https://amzn.to/36BLSG1
* 8-72V to 5V 4A DCDC Converter https://amzn.to/3i4d8lV
* Wemos Mini dev board https://amzn.to/3xJ0e3q
* Adafruit neopixel Ring 16 https://amzn.to/3hDBBj0
* Pushbutton https://amzn.to/3egmDxg
* Resistors (1K / 15K)

We are a participant in the Amazon Services LLC Associates Program, an affiliate advertising program designed to provide a means for us to earn fees by linking to Amazon.com and affiliated sites.

# Wiring:

Be sure to measure the voltages from the cable harness.  These colors / voltages may not match your cable.

* 4 pin cable Red Wire -> DCDC +V Input
* 4 pin cable Black Wire -> DCDC GND Input

* 4 pin cable Blue Wire -> V+ Voltage Divider
* 4 pin cable Green Wire -> GND Voltage Divider

* Wemos Pin 5V -> DCDC +V Output -> Neopixel V+
* Wemos Pin GND -> DCDC GND Output -> Neopixel GND -> Button GND

* Wemos Pin D3 -> Button
* Wemos Pin D6 -> Brake Signale After Voltage Divider
* Wemos Pin D5 -> Neopixel Data In

# Voltage Divider:

Same as the light voltage, the Brake signal is also 52V.  To use as an input signal, we just have to divide it over two resistors.

```
52 V Input ------/\/\15k/\/\---x---/\/\1k/\/\------GND
                               |
                               |
                               |
                               3.25V Output
```
