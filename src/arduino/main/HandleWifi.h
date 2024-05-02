#include <WiFi.h>
#include <WiFiClient.h>
#include "secrets.h"

// Method to connect to WiFi
void connectToWiFi(){
    WiFi.mode(WIFI_STA);

    // Start WiFi connection
    WiFi.begin(SECRET_SSID, SECRET_PASS);

    // Wait for WiFi connection
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    // Print local IP address
    Serial.print("Connected to WiFi. IP address: ");
    Serial.println(WiFi.localIP());
}

// Method to check WiFi connection, reconnect if necessary
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        connectToWiFi();
    }
}
