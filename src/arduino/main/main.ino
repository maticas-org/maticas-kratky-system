#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include "InternetRTC.h"
#include "TimedOutput.h"
#include "HtmlPages.h"
#include "HandleConfigs.h"
#include "HandleWifi.h"

// Define sync interval
#define SYNC_INTERVAL 12*60*60*1000 // Sync time every 12 hours
InternetRTC rtc; // Create an instance of the InternetRTC class

// Define output pin
#define OUTPUT_PIN_1 4
#define OUTPUT_PIN_2 18
#define OUTPUT_PIN_3 19

// Create instances of TimedOutput class for each output
TimedOutput output1(OUTPUT_PIN_1, "Lights");
TimedOutput output2(OUTPUT_PIN_2, "Fan");
TimedOutput output3(OUTPUT_PIN_3, "Water oxygenator");

ActuatorConfig defaultConf1 = {"Lights", OUTPUT_PIN_1, "on/off", DateTime(0,0,0,6,0,0), DateTime(0,0,0,18,0,0), 0, 0};
ActuatorConfig defaultConf2 = {"Fan", OUTPUT_PIN_2, "periodic",  DateTime(0,0,0,0,0,0), DateTime(0,0,0,0,0,0), 65, 65*4};
ActuatorConfig defaultConf3 = {"Water oxygenator", OUTPUT_PIN_3, "periodic", DateTime(0,0,0,0,0,0), DateTime(0,0,0,0,0,0), 65, 65*4};

// Define actuator configurations and configuration files
ActuatorConfig *actuatorConfigs[3] = {&defaultConf1, &defaultConf2, &defaultConf3};
Preferences preferences;

// Store this timed outputs in an array of pointers to the TimedOutput instances
TimedOutput *actuators[] = {&output1, &output2, &output3};

// Create an instance of the AsyncWebServer class
WebServer server(80);

// Tasks to run
TaskHandle_t Task1;
TaskHandle_t Task2;

// Method declarations
void mainControl(void *pvParameters);
void webServerTask(void *pvParameters);
void handleNotFound();
void handleRoot();
void handleUpdateActuatorConf();


void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Starting Setup...");

    // Connect to WiFi
    connectToWiFi();

    // Initialize server
    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }
    delay(250);
    server.on("/", handleRoot);
    server.on("/updateActuatorConf", HTTP_POST, handleUpdateActuatorConf);
    server.onNotFound(handleNotFound);
    server.begin();

    // Initialize RTC
    rtc.syncInternetTime();

    // Load previous configurations and set actuators
    updateDefaultConfigs(preferences, actuatorConfigs, 3);
    for (int i = 0; i < 3; i++) {
        actuators[i]->setFromConfig(actuatorConfigs[i]);
    }

    // Create tasks for the main control loop and web server
    xTaskCreatePinnedToCore(mainControl, "mainControl", 20000, NULL, 1, &Task1, 0);
    delay(500); 
    xTaskCreatePinnedToCore(webServerTask, "webServerTask", 20000, NULL, 1, &Task2, 1);
    delay(500); 
}

void loop() {
}


// Method to control actuators based on configuration
void mainControl(void *pvParameters){
    while (true)
    {
        // Update output state based on current time
        for (int i = 0; i < 3; i++) {
            actuators[i]->updateOutputState(rtc.getCurrentDateTime());
        }

        // Synchronize time every 10 minutes
        static unsigned long previousSyncTime = 0;
        if (millis() - previousSyncTime > SYNC_INTERVAL) {
            checkWiFiConnection();
            rtc.syncInternetTime();
            previousSyncTime = millis();
        }else{
            rtc.syncLocalTime();
        }

        // Delay for 1 second
        delay(2500);
        Serial.println();
    }
}

void webServerTask(void *pvParameters){
    while (true)
    {
        server.handleClient();
        delay(2);//allow the cpu to switch to other tasks
    }
}

void handleNotFound() {
  Serial.println("Handling unknown path...");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRoot() {
  Serial.println("Handling /...");
  server.send(200, "text/html", updateConfigHTML(actuators, 3));
}

void handleUpdateActuatorConf() {
  Serial.println("Handling /updateActuatorConf...");
  // Check if the request has JSON data
  try{
    String i = server.arg("selected-actuator");
    String name = server.arg("name-" + i);
    String starttime = server.arg("starttime-" + i);
    String endtime = server.arg("endtime-" +i);
    String type = server.arg("type-" + i);
    String minuteson = server.arg("minuteson-" + i);
    String minutesoff = server.arg("minutesoff-" + i);

    // Update the configuration of the selected actuator
    ActuatorConfig actuatorConf = {name, 0, type, DateTime(starttime), DateTime(endtime), minuteson.toInt(), minutesoff.toInt()};
    saveActuatorConfig(preferences, &actuatorConf, i.toInt());
    actuators[i.toInt()]->setFromConfig(&actuatorConf);

    // Redirect to the root page
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");


  } catch (const std::exception& e) {
    Serial.println("Error parsing JSON data");
    server.send(400, "text/plain", "Error parsing JSON data" + String(e.what()));
  }
}
