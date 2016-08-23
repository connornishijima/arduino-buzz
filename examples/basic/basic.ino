/*
 * Buzz Motion Detection Example
 * 
 * This sketch provides basic Buzz library functionality,
 * allowing the user to detect motion using an open wire
 * attached to an ADC pin.
 * 
 * Once uploaded, try waving your hands near the antenna
 * and watching the output on Arduino IDE's Serial Plotter.
 * Nothing will appear until 3 seconds after boot.
 * 
 * How it works:
 * 
 * Due to the ATMega328p's ADC being very high impedance,
 * it can easily detect the AC electricity waves that leak
 * into the air via open outlets, bad sheilding, and more.
 * 
 * When something statically charged (human, pet, blanket,
 * etc.) passes near the antenna, it increases or decreases
 * the voltage perceived at the input. Even without rubbing
 * a balloon on your head, you'll always have enough static
 * charge to affect this value a measurable amount.
 * 
 * The Buzz library allows you to easily monitor these
 * changes, and attach your own functions that will execute
 * when motion excedes a specified threshold.
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
  buzz.begin(antenna,acFreq,coolDown); 

  // Set alarm function to soundTheAlarm() below
  buzz.setAlarm(soundTheAlarm, alarmThreshold, alarmWait);
}

void loop() {
  buzz.checkAlarm(); // Used to see if alarm flag has been triggered
  buzz.printData();  // Prints a graph to Arduino IDE's Serial Plotter
                     // showing motion against the slarm threshold
  delay(1);
}

void soundTheAlarm(){
  // Generates an alarm sound
  for(byte i = 0; i < 128; i++){
    tone(speaker,880);
    delay(1);
    tone(speaker,440);
    delay(1);
  }
  noTone(speaker);
}
