#include <WiFiUdp.h> // Include the WiFi UDP library for NTP functionality
#include <NTPClient.h>

#ifndef INTERNET_RTC_H
#define INTERNET_RTC_H
#include "DateTime.h"
#include "secrets.h"

class InternetRTC {
  public:
    // Constructor
    InternetRTC() {
      // Define NTP Client to get time
    }
        
    // Method to synchronize time using millis
    void syncLocalTime();

    // Method to synchronize time using internet time server
    void syncInternetTime();

    // Getter method to return current time
    unsigned long getCurrentTime();

    // Getter method to return current time
    DateTime getCurrentDateTime();

  private:
    WiFiUDP ntpUDP;
    NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", TIMEOFFSET);
    unsigned long currentTime = 0; // Current time in seconds since startup
    unsigned long previousMillis = 0;
    const long interval = 1000; // Interval for local time sync (1 second)
};
#endif