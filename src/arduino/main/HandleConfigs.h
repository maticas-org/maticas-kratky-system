#include <Arduino.h>
#include <Preferences.h>

/*
* Using preferences load the previous configurations
*/
void updateDefaultConfigs(Preferences &preferences, ActuatorConfig *actuatorConfigs[], int numActuators) {
    Serial.println("Loading previous configurations...");
    for (int i = 0; i < numActuators; i++) {
        preferences.begin(("actuator" + String(i) + "Config").c_str(), false);
        actuatorConfigs[i]->name = preferences.getString("name", actuatorConfigs[i]->name);
        actuatorConfigs[i]->pin = preferences.getInt("pin", actuatorConfigs[i]->pin);
        actuatorConfigs[i]->mode = preferences.getString("mode", actuatorConfigs[i]->mode);
        String onTime = preferences.getString("onTime", actuatorConfigs[i]->onTime.toTimeString());
        String offTime = preferences.getString("offTime", actuatorConfigs[i]->offTime.toTimeString());
        actuatorConfigs[i]->onTime = DateTime(onTime);
        actuatorConfigs[i]->offTime = DateTime(offTime);
        actuatorConfigs[i]->onDuration = preferences.getUInt("onDuration", actuatorConfigs[i]->onDuration);
        actuatorConfigs[i]->offDuration = preferences.getUInt("offDuration", actuatorConfigs[i]->offDuration);
        preferences.end(); 
    }
}

void saveActuatorConfig(Preferences &preferences,
                        ActuatorConfig *actuatorConfig,
                        int actuatorNum) {
    preferences.begin(("actuator" + String(actuatorNum) + "Config").c_str(), false);
    preferences.putString("name", actuatorConfig->name);
    preferences.putInt("pin", actuatorConfig->pin);
    preferences.putString("mode", actuatorConfig->mode);
    preferences.putString("onTime", actuatorConfig->onTime.toTimeString());
    preferences.putString("offTime", actuatorConfig->offTime.toTimeString());
    preferences.putUInt("onDuration", actuatorConfig->onDuration);
    preferences.putUInt("offDuration", actuatorConfig->offDuration);
    preferences.end();
}

void saveWifiConfig(Preferences &preferences,
                    String ssid,
                    String password) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
}