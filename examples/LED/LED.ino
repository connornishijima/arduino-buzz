/*
 * Buzz LED Example
 * 
 * This sketch maps motion readings to 8-bit LED brightness
 * (0-255) for an easy display.
 */

#include "Buzz.h" // Include the Buzz library
Buzz buzz;

int led = 11; // Can't use PWM on pins 9 / 10, Buzz uses Timer1 for readings

void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  buzz.begin(A0,60,3000);
}
void loop() {
  Serial.println(buzz.level());
  int motion = constrain((abs(buzz.level())*5)-20,0,255); // Used to map motion to 8-bit brightness value
  analogWrite(led,motion);
  delay(1);
}
