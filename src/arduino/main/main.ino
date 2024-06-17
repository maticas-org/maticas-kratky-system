#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <Adafruit_SSD1306.h>

#include "InternetRTC.h"
#include "TimedOutput.h"
#include "HtmlPages.h"
#include "HandleConfigs.h"
#include "HandleWifi.h"

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_SYNC_INTERVAL 30000 // Sync screen every 30 seconds
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define sync interval
#define SYNC_INTERVAL 12*60*60*1000 // Sync time every 12 hours
#define SYNC_RETRY_INTERVAL 10000
#define SYNC_MAX_RETRIES 30
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
bool connected = false;
WebServer server(80);

// Tasks to run
TaskHandle_t Task1;
TaskHandle_t Task2;


// Method declarations
void mainControl(void *pvParameters);
void webServerTask(void *pvParameters);
void handleRoot();
void handleNotFound();
void handleWifiConfig();
void handleMakingChanges();
void handleUpdateActuatorConf();
void handleUpdateWifiConfig();


void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Starting Setup...");

    // Initialize display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
    }

    // Connect to WiFi
    connected = connectToWiFi(preferences);

    // Initialize server
    if (MDNS.begin(MDNS_NAME)) {
        Serial.println("MDNS responder started");
    }
    delay(250);
    server.on("/", handleRoot);
    server.on("/makingChanges", HTTP_GET, handleMakingChanges);
    server.on("/wifiConfig", HTTP_GET, handleWifiConfig);
    server.on("/updateWifiConf", HTTP_POST, handleUpdateWifiConfig);
    server.on("/updateActuatorConf", HTTP_POST, handleUpdateActuatorConf);
    server.onNotFound(handleNotFound);
    server.begin();

    // Create tasks for the main control loop and web server
    xTaskCreatePinnedToCore(mainControl, "mainControl", 20000, NULL, 1, &Task1, 0);
    delay(500); 
    xTaskCreatePinnedToCore(webServerTask, "webServerTask", 20000, NULL, 1, &Task2, 1);
    delay(500); 

    // Initialize RTC
    rtc.syncInternetTime(SYNC_RETRY_INTERVAL, SYNC_MAX_RETRIES);

    // Load previous configurations and set actuators
    updateDefaultConfigs(preferences, actuatorConfigs, 3);
    for (int i = 0; i < 3; i++) {
        actuators[i]->setFromConfig(actuatorConfigs[i]);
    }
}

void loop() {
}


// Method to control actuators based on configuration
void mainControl(void *pvParameters){

    // Synchronize time every 10 minutes
    static unsigned long previousSyncTime = 0;
    // Sync screen every 30 seconds
    static unsigned long previousScreenTime = 0;

    while (true)
    {
        // Update output state based on current time
        for (int i = 0; i < 3; i++) {
            actuators[i]->updateOutputState(rtc.getCurrentDateTime());
        }

        if (millis() - previousSyncTime > SYNC_INTERVAL) {
            connected = checkWiFiConnection(preferences);
            rtc.syncInternetTime(SYNC_RETRY_INTERVAL, SYNC_MAX_RETRIES);
            previousSyncTime = millis();
        }else{
            rtc.syncLocalTime();
        }

        if (millis() - previousScreenTime > OLED_SYNC_INTERVAL) {
            screenDisplay();
            previousScreenTime = millis();
        }else if ( previousScreenTime == 0){
            screenDisplay();
            previousScreenTime = millis();
        }

        delay(500); 
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

void screenDisplay() {

    // Display the current date and time
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Fecha-Hora");
    display.println(rtc.getCurrentDateTime().toString());
    display.display();
    delay(2000); // Display the time for 2 seconds

    // Display Maticas Tech URL
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Visitanos en:");
    display.println("maticas-tech.com");
    display.display();
    delay(2000); // Display the URL for 2 seconds

    // Display the current device IP
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Direccion IP: ");
    display.println(WiFi.localIP().toString());
    display.display();
    delay(2000); // Display the IP for 2 seconds

    // Display the logo
    display.clearDisplay();
    display.drawBitmap(0, 0, logo_bmp, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(2000); // Display the logo for 2 seconds
}

// **************************************************************
// * Web server handlers
// **************************************************************

//---------------------------
// GET Requests handlers
//---------------------------
void handleRoot() {
  Serial.println("Handling /...");

  //check if the module is connected to the internet
  // if not, redirect to the wifi config page
  if (!connected) {
    server.send(200, "text/html", updateWifiConfig());
  }else{
    server.send(200, "text/html", updateConfigHTML(actuators, 3, rtc));
  }
}

void handleWifiConfig() {
  Serial.println("Handling /wifiConfig...");
  server.send(200, "text/html", updateWifiConfig());
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

void handleMakingChanges() {
  Serial.println("Handling /makingChanges...");
  server.send(200, "text/html", makingChangesHTML());
    
  // Allow some time for the client to receive the response
  delay(2000);

  // Reset the ESP32 to apply changes
  ESP.restart();
}

//---------------------------
// POST Requests handlers
//---------------------------
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
    ActuatorConfig actuatorConf = {name, actuators[i.toInt()]->pin, type, DateTime(starttime), DateTime(endtime), minuteson.toInt(), minutesoff.toInt()};
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


void handleUpdateWifiConfig() {
  Serial.println("Handling /updateWifiConf...");
  // Check if the request has JSON data
  try{
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    // Update the configuration of the selected actuator
    saveWifiConfig(preferences, ssid, password);

    // Redirect to making changes page
    server.sendHeader("Location", "/makingChanges", true);
    server.send(302, "text/plain", "");

  } catch (const std::exception& e) {
    Serial.println("Error parsing JSON data");
    server.send(400, "text/plain", "Error parsing JSON data" + String(e.what()));
  }

}