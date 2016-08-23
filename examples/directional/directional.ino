/*
   Buzz Directional Motion Example

   Buzz allows a user to detect whether the motion was
   towards or away from the antenna. To do this, we simply
   check if the motion value is positive (towards) or
   negative (away). A higher tone is positive, a lower tone
   is negative.

   Once uploaded, try waving your hands near the antenna
   and watching the output on Arduino IDE's Serial Plotter.
   Nothing will appear until 3 seconds after boot.
*/

#include "Buzz.h" // Include the Buzz Library

// ----------------------------------------------------------------
// buzz.begin() parameters ----------------------------------------

int speaker = 13; // Connect a speaker or piezo to this pin
int antenna = A0; // Connect a 6-12in length of wire to this input
int acFreq = 60;  // This is used for phase-cancellation to remove
// AC sine waves from the reading

int coolDown = 3000; // When the microcontroller first starts, the
// ADC is very sensitive and needs about 3
// seconds to "cool down" to a baseline value.
// Buzz will wait this amount of milliseconds
// before triggering any alarms

// ----------------------------------------------------------------
// buzz.setAlarm() parameters -------------------------------------

int alarmThreshold = 20; // Call alarm function (soundTheAlarm())
// if motion reading is >= this value.

int alarmWait = 500; // To avoid an alarm retriggering too soon,
// Buzz will wait this long to trigger again.

// ----------------------------------------------------------------
// ----------------------------------------------------------------

Buzz buzz; // Declare library alias

void setup() {
  Serial.begin(115200); // Start Serial for printData()
  pinMode(speaker, OUTPUT);

  // Start watching motion
  buzz.begin(antenna, acFreq, coolDown);

  // Set alarm function to soundTheAlarm() below
  buzz.setAlarm(soundTheAlarm, alarmThreshold, alarmWait);
}

void loop() {
  buzz.checkAlarm(); // Used to see if alarm flag has been triggered
  buzz.printData();  // Prints a graph to Arduino IDE's Serial Plotter
  // showing motion against the slarm threshold
  delay(1);
}

void soundTheAlarm() {
  // Generates an alarm sound

  // Outgoing motion
  if (buzz.level() <= 0) {
    for (byte i = 0; i < 128; i++) {
      tone(speaker, 440);
      delay(1);
      tone(speaker, 220);
      delay(1);
    }
    noTone(speaker);
  }

  // Incoming motion
  else {
    for (byte i = 0; i < 128; i++) {
      tone(speaker, 880);
      delay(1);
      tone(speaker, 440);
      delay(1);
    }
    noTone(speaker);
  }
}
