#include <WiFi.h>
#include <WiFiClient.h>
#include <Preferences.h>
#include "secrets.h"

// Method to connect to WiFi
void connectToWiFi(String &ssid, String &password){
    if (ssid.length() > 0 && password.length() > 0) {
        // Try to connect using stored credentials
        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.println("Connecting to WiFi...");

        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to WiFi");
            return;
        }
    }

    // If connection failed or no credentials stored, start AP mode
    Serial.println("\nStarting AP mode...");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.println("AP mode started. Connect to 'ESP32_Config' and go to http://192.168.4.1/config");
}


void connectToWiFi(Preferences &preferences){
    // Initialize Preferences
    preferences.begin("wifi", false);

    // Retrieve stored SSID and password
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");

    // Connect to WiFi
    connectToWiFi(ssid, password);
}

// Method to check WiFi connection, reconnect if necessary
void checkWiFiConnection(String &ssid, String &password){
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        connectToWiFi(ssid, password);
    }
}

void checkWiFiConnection(Preferences &preferences){
    // Initialize Preferences
    preferences.begin("wifi", false);

    // Retrieve stored SSID and password
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");

    checkWiFiConnection(ssid, password);
}