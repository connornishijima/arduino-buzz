![#Arduino Buzz Library](http://i.imgur.com/1mIkcUy.jpg)

**Arduino just got motion detection - with nothing but a wire!**

# [VIDEO DEMONSTRATION](https://www.youtube.com/watch?v=4KjB-HMuUs4)

By monitoring the amplitude of AC electricity waveforms in the air for changes, Buzz provides motion detection using only a wire! It's extremely easy to implement, and a perfect library for all experience levels.

----------
# Disclaimer

The Buzz library is just for experimental use only, and is **not intended for providing a home/business security solution**.

----------
# Contents
- [Explanation](#explanation)
- [Installation](#installation)
- [Usage](#usage)
- [Functions](#functions)
- [Limitations](#limitations)
- [Contributing](#contributing)
- [License and credits](#license-and-credits)

----------
# Explanation

Due to the ATMega328p's ADC being very high impedance, it can easily detect the AC electricity waves that leak into the air via open outlets, bad shielding, and more.

When something statically charged (human, pet, blanket, etc.) passes near the antenna, it increases or decreases the voltage perceived at the input. Even without rubbing a balloon on your head, you'll always have enough static charge to affect this value a measurable amount.

The Buzz library allows you to easily monitor these changes, and attach your own functions that will execute when motion exceeds a specified threshold.

----------
# Installation

~~With Arduino Library Manager:~~ **Coming soon!**

~~1. Open *Sketch > Include Library > Manage Libraries* in the Arduino IDE.~~

~~2. Search for "Buzz", (look for "Connor Nishijima") and select the latest version.~~

~~3. Click the Install button and Arduino will prepare the library and examples for you!~~

**Manual Install:**

1. Click "Clone or Download" above to get an "arduino-buzz-master.zip" file.
2. Extract it's contents to the libraries folder in your sketchbook.
3. Rename the folder from "arduino-buzz-master" to "Buzz".

----------
# Usage

Using the Buzz library is very simple, you only need the following to get started:

    #include "Buzz.h" // Include the Buzz library
    Buzz buzz;

    void setup() {
      Serial.begin(115200);
      buzz.begin(A0,60,3000);
    }
    void loop() {
      Serial.println(buzz.level());
      delay(1);
    }

Next, connect a wire/jumper (6-12") to pin A0, and open the Arduino IDE's Serial Plotter to see the current motion value! Try waving your hand near the antenna, or walking past it.

----------
# Functions

**Buzz buzz**;

This initializes the Buzz library after import. "buzz" can be any word you want, as long as it's reflected in the rest of your code.

**buzz.begin**(byte **pin**, byte **hz**, unsigned int **coolDown**);

This sets up a Timer Compare Interrupt on Timer1 for logging motion changes. It watches the ADC input defined by **pin**, does phase cancellation for **hz** AC to remove sine-wave artifacts from the data, and waits for **coolDown** milliseconds for the ADC to stabilize before triggering any alarms.

**buzz.end**();

This clears the Timer1 ISR that checks motion, essentially stopping all Buzz execution.

**buzz.level**();

Returns the current motion level as a signed integer, with a minor motion giving a value of ~10, and more major motions returning ~100 or more.

`buzz.level()` can be both positive or negative depending on if the motion was towards the antenna or away from it! Use `abs(buzz.level())` to get all values as positive.

**buzz.setAlarm**(void **action**, unsigned int **threshold**, unsigned int **hold**);

Used to set the user-provided function as the callback for an alarm trigger. If motion is >= to **threshold**, and an alarm hasn't been triggered for **hold** milliseconds, the function **action** will be called. If the function you wrote is this helloWorld() demo:

	void helloWorld(){
      Serial.println("Hello world!");
    }

You would write `setAlarm()` like this:

	buzz.setAlarm(helloWorld, 20, 500);
    
A current limitation is that arguments/parameters cannot be passed to the alarm function, and the function can't return data either - though a workaround is to set those values in global variables and read them wherever else your need to, inside or outside the alarm function. See [Limitations](#limitations).

**buzz.checkAlarm**();

This is used to see if the alarm flag has been set by Buzz. This function should be called as often as possible, and your code should avoid blocking functions like delay(). If the flag has been set true by motion exceding your custom threshold, the function defined in setAlarm() will be called.

**buzz.printData**()

This renders a graph to the Arduino IDE Serial Plotter containing current motion levels, your threshold for alarm, and marks when alarms were triggered. Nothing will appear in the plotter until the **coolDown** from `buzz.begin()` has passed.

----------
# Limitations
Unfortunately, a solution this simple has it's caveats:

**Buzz is susceptable to false positives**

Because we're relying on AC and static electricity for our readings, it's trivial to cause interference to the input by turning on power appliances nearby, or picking up local lighting strikes. (Though the latter is a cool use as well!)

**Buzz loves cats**

While a human like yourself always has enough static charge to cause a measurable shift in ADC voltage, cats are covered in electron-loving fur. It's much more sensitive to them! A solution is to make your cat wear an anti-static soldering strap at all times, but so far I've been very unsuccessful in implementing this.

**Phase cancellation has only been tested with 60Hz AC**

While it shouldn't make a huge difference without it, the phase cancellation has been **tested** for 60Hz AC, and **calculated** for 50Hz AC. 50Hz users may want to report results back to me if the defaults need changes.

**Buzz disables PWM on Pin 9 and 10**

Buzz uses Hardware Timer 1 to watch motion data "in the background" while your standard code runs. Timer1 is also responsible for PWM on 9 and 10, and it can't do both. If you desperately need to, call `buzz.end()`, run your PWM, then `buzz.begin()` again.

----------
# Contributing
Any advice or pull requests are welcome! :)

----------
# License and Credits
**Developed by Connor Nishijima (2016)**

**Released under the [GPLv3 license](http://www.gnu.org/licenses/gpl-3.0.en.html).**
