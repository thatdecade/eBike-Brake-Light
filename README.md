# eBike-Brake-Light
Neopixel Ring Based Brake Light

This is a brake light I made for my RadPower ebike.  Connects to the 4 pin wire harness from the motor controller.

The mount was modeled to replace my old Meilan light (https://amzn.to/3edWSh7) and re-use the same attachment bolt.  The openscad file is included, so you can change the mount to fit your project.

![alt text](https://media4.giphy.com/media/5sMBCwhOdvm9IbYvTt/giphy.gif?cid=6c09b952261317ecaab570254baa62f059eea7e085dbe4c9&rid=giphy.gif)

We are a participant in the Amazon Services LLC Associates Program, an affiliate advertising program designed to provide a means for us to earn fees by linking to Amazon.com and affiliated sites.

# Part List:

* Bafang 4 pin extension cable https://amzn.to/36BLSG1
* 8-72V to 5V 4A DCDC Converter https://amzn.to/3i4d8lV
* Wemos Mini dev board https://amzn.to/3xJ0e3q
* Adafruit neopixel Ring 16 https://amzn.to/3hDBBj0
* Pushbutton https://amzn.to/3egmDxg
* Resistors (5M / 10M) for voltage divider

# Wiring:

* 4 pin cable Red Wire -> DCDC +V Input
* 4 pin cable Black Wire -> DCDC GND Input
* 4 pin cable Green Wire -> +4.8V Voltage Divider
* 4 pin cable Blue Wire -> NO CONNECTION

* Wemos Pin 5V -> DCDC +V Output -> Neopixel V+
* Wemos Pin GND -> DCDC GND Output -> Neopixel GND -> Button GND

* Wemos Pin D3 -> Button
* Wemos Pin A0 -> 3.2V Brake Signal
* Wemos Pin D5 -> Neopixel Data In

# Cable Harness Wiring Notes:

The motor controller's 4 pin wire harness has two +52V and two grounds.  The voltages from the red and green wires are not meant to be combined, they do not have a common ground.  The red/black wire is supposed to power your 52V running lights, and the green/blue wire is supposed to power your 52V brake light.  I want all these signals running to a single circuit.

Here are some of the voltages I measured:

|Wire Pair|Lights Off|Lights On|Brake|Notes|
| --- | --- |  --- |  --- |  --- | 
|Red / Black| 0| 52| 52| Correct ground|
|Green / Black| 4.8| 0| 4.8|Wrong ground|
|Blue / Black| 52| 52| 52|Wrong ground|
|Green / Blue| 0| 52| 0|Correct ground|
|Red / Blue| 52| 1.42| 1.55|Wrong ground|
|Red / Green| 4.8| 52| 52|Wrong ground|

**Correct ground**: These wires can sink current.
**Wrong ground**: Voltage drops to 0V if used for more than a few microamps.

There are a couple of ways to solve this.  
1. An optical isolator would be best.  Allowing the green/blue 52V brake signal to light up an LED, which then triggers a phototransistor to send a signal to our second 52V circuit.
2. Less good would be to try an read one circuit from the other using resistors and mixing the grounds.

I went with the second method.  The resistors are in the megaohm range, so less than 1uA is drawn from the green wires. The voltage does not drop and I can read it with an analog input set to detect a 0.2V threshold.

```

Green +4.8V ------- /\/\5M/\/\-+
                               |
                               +------ 3.2V to A0
                               |
Black GND   ------ /\/\10M/\/\-+

```

The wemos A0 pin has an additional voltage divider so the resulting signal is below 1V.  Measured 0.4V (idle) / 0.01V (brake)
