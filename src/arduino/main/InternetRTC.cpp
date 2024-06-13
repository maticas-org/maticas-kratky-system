#include <WiFi.h> // Include the WiFi library for internet connection
#include <WiFiUdp.h> // Include the WiFi UDP library for NTP functionality
#include "InternetRTC.h"
#include "secrets.h"

// Method to synchronize time using millis
void InternetRTC::syncLocalTime() {
  // Update current time using millis()
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Increment time by interval
    currentTime += (currentMillis - previousMillis) / 1000;
    previousMillis = currentMillis;
  }

  // Print current time
  Serial.print("Local Time: ");
  Serial.println(getCurrentDateTime().toString());
}

// Method to synchronize time using internet time server
void InternetRTC::syncInternetTime(int wait_time, int max_retries) {
  Serial.println("Syncing time with internet time server...");
  timeClient.begin();

  bool result = timeClient.update();
  int retries = 0;
  
  while(!result && retries < max_retries) {
    result = timeClient.forceUpdate();
    retries++; 
    delay(wait_time);
  }

  if (!result) {
    Serial.println("Failed to update time from internet time server");
    Serial.println("Please check your internet connection, Rebooting now...");
    ESP.restart();
    return;
  }

  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Time: ");
  Serial.print(formattedTime);

  currentTime = timeClient.getEpochTime() - TIMEOFFSET - 20*60; //custom offset to calibrate
  Serial.print(" | Current time: ");
  Serial.println(currentTime);
}

// Getter method to return current time
unsigned long InternetRTC::getCurrentTime() {
  return currentTime;
}

// Getter method to return current time
DateTime InternetRTC::getCurrentDateTime(){
    DateTime dateTime;
    unsigned long time = getCurrentTime();
    unsigned long seconds_month = 2628288;
    unsigned long seconds_year = 31556952;

    dateTime.year = 1970 + (time / seconds_year); // 31556926 seconds per year
    dateTime.month = 1 + ((time % seconds_year) / seconds_month); // 2629743 seconds per month
    dateTime.day = 1 + (((time % seconds_year) % seconds_month) / 86400); // 86400 seconds per day
    dateTime.hour = ((time % seconds_year) % seconds_month) % 86400 / 3600; // 3600 seconds per hour
    dateTime.minute = (((time % seconds_year) % seconds_month) % 86400 % 3600) / 60; // 60 seconds per minute
    dateTime.second = (((time % seconds_year) % seconds_month) % 86400 % 3600) % 60; // 60 seconds per minute
    return dateTime;
}
