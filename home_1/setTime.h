#ifndef SETTIME_H
#define SETTIME_H

#include <Arduino.h>

// Structure to hold time settings
struct TimeRequest {
  unsigned int timeOn;
  unsigned int timeOff;
};

// Declare global variables
volatile unsigned int timeOn = 0, timeOff = 0;
volatile bool setTimeFlag = false; // Flag to indicate when "set time" button is pressed

const unsigned long debounceDelay = 200; // Debounce delay in milliseconds
volatile unsigned long lastDebounceTime = 0;

// Interrupt handlers for buttons
void IRAM_ATTR timeOnInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    timeOn += 5; // Increment timeOn by 5
    if (timeOn > 60) { // Cap at 60 minutes
      timeOn = 0; // Reset to 0
    }
    lastDebounceTime = currentTime;
  }
}

void IRAM_ATTR timeOffInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    timeOff += 5; // Increment timeOff by 5
    if (timeOff > 60) { // Cap at 60 minutes
      timeOff = 0; // Reset to 0
    }
    lastDebounceTime = currentTime;
  }
}

void IRAM_ATTR setTimeInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    setTimeFlag = true; // Indicate that the set time button has been pressed
    lastDebounceTime = currentTime;
  }
}

// Button setup function
void setupButtons(int timeOnButton, int timeOffButton, int setTimeButton) {
  pinMode(timeOnButton, INPUT_PULLUP);
  pinMode(timeOffButton, INPUT_PULLUP);
  pinMode(setTimeButton, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(timeOnButton), timeOnInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(timeOffButton), timeOffInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(setTimeButton), setTimeInterrupt, FALLING);
}

// Function to create a TimeRequest
TimeRequest createTimeRequest() {
  TimeRequest timing;
  timing.timeOn = timeOn;
  timing.timeOff = timeOff;
  return timing;
}

#endif
