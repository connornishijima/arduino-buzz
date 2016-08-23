/*
  Buzz.h - Library for detecting motion via AC electricity
  Created by Connor Nishijima, August 22nd 2016.
  Released under the GPLv3 license.
*/

#include "Buzz.h"

uint16_t b_reads[16] = {};       // Takes in raw ADC readings for phase cancellation
uint16_t b_averages[32] = {};    // Smoothes the b_roughMotion output of b_reads[]
uint16_t b_sensor = 0;           // ADC pin to use
uint8_t b_phaseShift = 0;        // Offset in index of b_reads[] used for phase cancellation
uint32_t b_buzzStart = 0;        // Used to timestamp beginning of usage for b_buzzWait
int16_t b_motionLevel = 0;       // Most current reading into b_record[]
uint16_t b_buzzThreshold = 20;   // Minimum amount of motion required to trigger alarm
uint16_t b_buzzHold = 1000;      // Minimum amount of time between alarms
uint32_t b_buzzWait = 0;         // Timestamp millis() must pass before another alarm can trigger
uint32_t b_lastAlarm = 0;        // Timestamp when last alarm triggered
bool b_buzzReady = false;        // False before millis() reaches time in b_buzzWait to stop alarms
bool b_alarm = false;            // Alarm flag, set to true by ISR, checked by checkAlarm()
int16_t b_alarmLine = -100;      // Used by printData() to produce "vertical" lines in the plotter

static void (*b_alarmFunc)();    // The function to be called in an alarm state, defined by setAlarm()

int16_t b_buzzRead = 0;          // Difference between most recent read ond the one 128ms ago,
                                 // used to derive current motion measurement

int16_t b_record[64] = {};       // Used to compare most current reading with one from 128ms ago
                                 // to produce b_buzzRead value

Buzz::Buzz() {}

void Buzz::begin(uint8_t pin, uint8_t hz, uint16_t coolDown) {
  // pin = ADC pin to use for measurement
  // hz = AC electricity frequency for your region used for phase cancellation
  // coolDown = Amount of time to "cool down" - ADC is too sensitive at sketch
  //            start and needs to stabilize

  b_buzzStart = ::millis();
  b_buzzWait = b_buzzStart + coolDown;
  b_sensor = pin;

  if (hz == 60) {
    b_phaseShift = 2;
  }
  else if (hz == 50) {
    b_phaseShift = 1;
  }

  // Inititalize storage arrays to zeros
  for (byte i = 0; i < 64; i++) {
    b_record[i] = 0;
  }
  for (byte i = 0; i < 16; i++) {
    b_reads[i] = 0;
  }
  for (byte i = 0; i < 32; i++) {
    b_averages[i] = 0;
  }

  // TIMER 1 for interrupt frequency 500 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 500 Hz increments
  OCR1A = 31999; // = 16000000 / (1 * 500) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}

void Buzz::end() {
  // Clear timer1 ISR to stop all Buzz execution
  TIMSK1 &= (0 << OCIE1A); // Disable timer1 ISR
}

void Buzz::printData() {
  if (Serial) { // If user has started Serial
    if (b_buzzReady == true) { // If the ADC is ready, display chart - if not, display a single line
      Serial.print(F("100\t"));
      Serial.print(F("-100\t"));
      Serial.print(b_buzzThreshold);
      Serial.print(F("\t"));
      Serial.print(int(b_buzzThreshold) * -1);
      Serial.print(F("\t"));
      Serial.print(b_alarmLine);
      Serial.print(F("\t"));
      Serial.println(b_buzzRead);
    }
    else {
      Serial.println("0\t0\t0\t0\t0\t0\t");
    }
  }
}

void Buzz::setAlarm(void (*action)(), uint16_t thresh, uint16_t hold) {
  // action() = user-defined function to call during alarm state - can be any function
  // thresh = sets value of buzzThreshold
  // hold = sets value of buzzHold

  b_alarmFunc = action;
  b_buzzThreshold = thresh;
  b_buzzHold = hold;
}

void Buzz::checkAlarm() {
  if (b_alarm == true) { // If alarm flag has been set in the ISR:

    TIMSK1 &= (0 << OCIE1A); // Disable timer1 ISR:
    b_alarmFunc();             // Call the alarm function
    TIMSK1 |= (1 << OCIE1A); // Enable timer1 ISR:
    b_alarm = false;           // Reset alarm flag

    // show "vertical" line in printData()
    if (b_alarmLine == -100) {
      b_alarmLine = 100;
    }
    else if (b_alarmLine == 100) {
      b_alarmLine = -100;
    }
  }
}

int16_t Buzz::level() { // Returns value of buzzRead
  return b_buzzRead;
}

// Interrupt Service Routine called at 500 Hz
ISR(TIMER1_COMPA_vect) {
  // Check if buzzWait is over:
  if (millis() >= b_buzzWait && b_buzzReady == false) {
    b_buzzReady = true;
  }

  uint16_t b_reading = analogRead(b_sensor); // Take reading

  // keep moving average of b_reading in reads[]
  for (byte i = 0; i < 15; i++) {
    b_reads[i] = b_reads[i + 1];
  }
  b_reads[15] = b_reading;

  // phase cancellation to remove AC sine effects
  // by comparing current value to an older one 180
  // degrees out of phase
  uint16_t b_roughMotion = b_reads[b_phaseShift] + b_reading;

  // keep moving average of b_roughMotion in b_averages[];
  for (byte i = 0; i < 31; i++) {
    b_averages[i] = b_averages[i + 1];
  }
  b_averages[31] = b_roughMotion;

  // b_motionLevel is the average of all in the b_averages[] array
  uint16_t b_sum = 0;
  for (byte i = 0; i < 32; i++) {
    b_sum += b_averages[i];
  }
  b_motionLevel = abs(b_sum / 32);

  // Add b_motionLevel to b_record[] for comparing current motion data
  // against past data to calculate the shift (amount of change = b_buzzRead)
  for (byte i = 0; i < 63; i++) {
    b_record[i] = b_record[i + 1];
  }
  b_record[63] = b_motionLevel;
  b_buzzRead = (b_motionLevel - b_record[0])*-1; // inverted so that movement towards antenna
                                                 // is a positive shift in the log

  // If buzzRead is >= buzzThreshold, we're past the buzzWait, and
  //we haven't had another alarm too recently, set the alarm flag to true.
  if (abs(b_buzzRead) >= b_buzzThreshold && b_buzzReady == true && millis() >= b_lastAlarm + b_buzzHold) {
    b_lastAlarm = millis();
    b_alarm = true;
  }
}
