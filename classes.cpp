#include "classes.h"

// BUTTON CLASS
// constructor for button class
Knop::Knop(int l, int h) {
  lowVolt = l;
  highVolt = h;
}

// update function for buttons
void Knop::update(int volt, unsigned long curTime) {
  //reading is HIGH if button is pressed, LOW if not
  //get voltage from analog pin A5
  int reading = LOW;
  if (volt >= lowVolt && volt <= highVolt) {
    reading = HIGH;
  }

  // if debounced state changed last iteration, longPress is no longer true
  if (changed && longPress) {
    longPress = false;
  }

  // reset debounceTime if buttonstate changed
  if (reading != lastButtonState) {
    lastDebounceTime = curTime;
  }

  //compare last time button is pressed, skip if too fast (this is debounce part)
  if (compareTimestamps(curTime, lastDebounceTime, debounceDelay)) {
    //if the buttonstate has changed, update buttonState
    if (reading != buttonState) {
      buttonState = reading;
      changed = true;
      // if the buttonstate has changed to HIGH (button is pressed), update status
      if (buttonState == HIGH) {
        pressed = true;
      } else {
        pressed = false;
      }
    } else {
      changed = false;
      if ((curTime - lastDebounceTime) > longPressDelay) {
        // should be used together with pressed, to detect long pressed or long not pressed
        // on release of button, stays true for one more iteration so release after long press can be detected
        longPress = true;
      }
    }
  }

  // save the reading. Next update call, it'll be the lastButtonState:
  lastButtonState = reading;
}

DistanceSensor::DistanceSensor(int interval) {
  senseInterval = interval;
  closeByThreshold = 100;
  farAwayThreshold = 400;
}

void DistanceSensor::update(unsigned long curTime) {
  //check if sensor can do its sensing
  if (!(compareTimestamps(curTime, lastSensed, senseInterval) && active)) {
    return;
  }
  lastSensed = curTime;
    
  int reading = sonar.ping_cm();
  //only update lastreading if differnece is greater than the readsensitivity
  lastReading = (abs(reading - lastReading) > readSensitivity) ? reading : lastReading;
}

LightSensor::LightSensor(int interval){
  senseInterval = interval;
}

void LightSensor::update(unsigned long curTime) {
  //check if sensor can do its sensing
  if (!(compareTimestamps(curTime, lastSensed, senseInterval) && active)) {
    return;
  }
  lastSensed = curTime;
    
  int reading = analogRead(ldrPin);
  //only update lastreading if differnece is greater than the readsensitivity
  lastReading = (abs(reading - lastReading) > readSensitivity) ? reading : lastReading;
}


MotionSensor::MotionSensor(int interval){
  senseInterval = interval;
}

void MotionSensor::update(unsigned long curTime){
  //check if sensor can do its sensing
  if (!(compareTimestamps(curTime, lastSensed, senseInterval) && active)) {
    return;
  }
  lastSensed = curTime;
  //read the sensor
  lastReading = digitalRead(motionSensorPin);  
}

TemperatureSensor::TemperatureSensor(int interval){
  senseInterval = interval;
}

void TemperatureSensor::update(unsigned long curTime){
  //check if sensor can do its sensing
  if (!(compareTimestamps(curTime, lastSensed, senseInterval) && active)) {
    return;
  }
  lastSensed = curTime;
  
  temperatureSensor.requestTemperatures(); // Send the command to get temperatures
  float tempC = temperatureSensor.getTempCByIndex(0);
  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    lastReading = (int)tempC;
  }
  else {
    lastReading = 0;
  }  
}





