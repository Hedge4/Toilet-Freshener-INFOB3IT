#include "sprayFunctions.h"

/////////////////////////
//    CONFIGURATION    //
/////////////////////////

int sprayDuration = 25 * 1000; // 25 seconds
int pauseDuration = 5 * 1000; // 5 seconds

// current spray information
bool spraying = false;
bool waiting = false;
int plannedSpraysLeft = 0;
unsigned long sprayTimestamp = 0;
//bool buttonPressed = false;


////////////////////////////
//          LOOP          //
////////////////////////////

void sprayLoop(unsigned long curTime) {
  if (spraying) {
    // stop spraying if we've reached sprayDuration
    if (compareTimestamps(curTime, sprayTimestamp, sprayDuration)) {
      spraying = false;
      waiting = true;
      digitalWrite(sprayPin, LOW);
      sprayTimestamp = millis();
      Serial.println("Spraying stopped, now waiting.");

      // if there's another spray coming, we indicate that with a yellow led
      if (plannedSpraysLeft > 0) yellowLed = 1;
      // if there's less than 5% of sprays left in the device, we show a constantly burning yellow led
      else if (spraysLeft / defaultTotalSprays < 0.05) yellowLed = 1;
      // and if neither, turn the lef off
      else yellowLed = 0;
    }
  } else if (waiting) {
    // stop waiting if we've reached pauseDuration
    if (compareTimestamps(curTime, sprayTimestamp, pauseDuration)) {
      waiting = false;
      if (plannedSpraysLeft > 0) {
        if (spraysLeft > 0) plannedSpraysLeft--;
        startSpray();
      } else {
        Serial.println("No more sprays, now idle.");
      }
    }
  }
}


/////////////////////////////
//        FUNCTIONS        //
/////////////////////////////

void startSpray() {
  if (waiting || spraying) {
    // when still waiting or spraying, schedule another spray
    plannedSpraysLeft = (1 > plannedSpraysLeft) ? 1 : plannedSpraysLeft; // set to 1 if 0
    Serial.println("Remaining sprays set to: " + String(plannedSpraysLeft));
  } else {
    spraying = true;
    yellowLed = 3; // the yellow led indicates the device is active with a fast blinking led
    if (spraysLeft > 0) spraysLeft--;
    // TODO EEPROM UPDATE LOGIC HERE
    digitalWrite(sprayPin, HIGH);
    sprayTimestamp = millis();
    Serial.println("Spray started! Sprays left: " + String(plannedSpraysLeft));
  }
}

void startSpray(int amount) {
  amount--; // first spray is immediately activated so doesn't count
  plannedSpraysLeft = (amount > plannedSpraysLeft) ? amount : plannedSpraysLeft; // whichever value is higher
  startSpray();
}

// stop ongoing and cancel upcoming sprays
void cancelSprays() {
  plannedSpraysLeft = 0;
  if (spraying) {
    spraying = false;
    waiting = true;
    digitalWrite(sprayPin, LOW);
    sprayTimestamp = millis();
    Serial.println("Spraying stopped, now waiting.");
  }
}

void sprayButtonUpdate(bool pressed) {
  // handle spray button change
  // we don't have to check current state, startSpray() does this
  if (!pressed) {
    // only do something if button was released
    startSpray();
  }
}


/////////////////////////////
//         GETTERS         //
/////////////////////////////

// whether a spray is ongoing or coming
bool sprayComing() {
  return (spraying || plannedSpraysLeft > 0);
}
