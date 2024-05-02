#include <Arduino.h>
#include "DateTime.h"

#ifndef TIMED_OUTPUT_H
#define TIMED_OUTPUT_H

// Our configuration structure.


struct ActuatorConfig {
  String name;
  int pin;
  String mode;
  DateTime onTime;
  DateTime offTime;
  unsigned long onDuration;
  unsigned long offDuration;
};

class TimedOutput {
  public:
    // Constructor
    TimedOutput(int pin, String name) {
      this->pin = pin;
      this->name = name;
      pinMode(pin, OUTPUT);
    }

    // Method to set operating mode to on/off (on during specified hours, off otherwise)
    void setOnOffMode(DateTime onTime, DateTime offTime) {
      this->mode = ON_OFF;
      this->onTime = onTime;
      this->offTime = offTime;
    }

    // Method to set operating mode to periodic (on for specified duration, off for extra duration)
    void setPeriodicMode(unsigned long onDuration, unsigned long offDuration) {
      this->mode = PERIODIC;
      this->onDuration = onDuration;
      this->offDuration = offDuration;
    }

    void setFromConfig (ActuatorConfig *config) {
      this->name = config->name;
      this->pin = config->pin;
      if (config->mode == "on/off") {
        this->mode = ON_OFF;
        this->onTime = config->onTime;
        this->offTime = config->offTime;
      } else {
        this->mode = PERIODIC;
        this->onDuration = config->onDuration;
        this->offDuration = config->offDuration;
      }
    }

    // Method to specify duration for actuator to be on
    void setOnDuration(unsigned long duration) {
      this->onDuration = duration;
    }

    // Method to update output state based on current time
    void updateOutputState(DateTime currentTime) {
      //Serial.println("ON_OFF onTime: " + onTime.toString()+ " currentTime: " + currentTime.toString() + " offTime: " + offTime.toString());
      switch (mode) {
        case ON_OFF:
            if (currentTime.timeGreaterThan(onTime) && currentTime.timeLessThan(offTime)) {
                digitalWrite(pin, HIGH); // Turn on
                Serial.print("ON/OFF " + name + " at pin ");
                Serial.print(pin);
                Serial.println(" is on");
            } else {
                digitalWrite(pin, LOW); // Turn off
                Serial.print("ON/OFF " + name + " at pin ");
                Serial.print(pin);
                Serial.println(" is off");
            }
            break;

        case PERIODIC:
            unsigned long cycleDuration = onDuration + offDuration;
            if (currentTime.toSeconds() % cycleDuration < onDuration) {
                digitalWrite(pin, HIGH); // Turn on
                Serial.print("Periodic " + name + " at pin ");
                Serial.print(pin);
                Serial.println(" is on");
            } else {
                digitalWrite(pin, LOW); // Turn off
                Serial.print("Periodic " + name + " at pin ");
                Serial.print(pin);
                Serial.println(" is off");
            }
            break;
      }
    }

    String exportConfig() {
      String config = "{";
      config += "\"name\":\"" + name + "\",";
      config += "\"pin\":" + String(pin) + ",";
      config += "\"mode\":\"" + String(mode == ON_OFF ? "ON_OFF" : "PERIODIC") + "\",";
      config += "\"onTime\":\"" + onTime.toString() + "\",";
      config += "\"offTime\":\"" + offTime.toString() + "\",";
      config += "\"onDuration\":" + String(onDuration) + ",";
      config += "\"offDuration\":" + String(offDuration);
      config += "}";
      return config;
    }

    enum Mode { ON_OFF, PERIODIC }; // Operating modes
    int pin; // Output pin
    String name; // Name of the actuator
    Mode mode; // Current operating mode
    DateTime onTime; // Hour to turn on (for ON_OFF mode)
    DateTime offTime; // Hour to turn off (for ON_OFF mode)
    unsigned long onDuration; // Duration to turn on (for PERIODIC mode)
    unsigned long offDuration; // Duration to turn off (for PERIODIC mode)

};

#endif