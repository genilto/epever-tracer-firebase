#include <Arduino.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "domain/TracerData.h"
#include "debug.h"

#include "controller/OTAUpdateController.h"
#include "controller/TracerController.h"
#include "controller/ServerController.h"
#include "controller/FirebaseController.h"
//#include "controller/MqttController.h"

OTAUpdateController OTAUpdate;
TracerController tracer;
ServerController server( *tracer.getData() );
FirebaseController firebase( *tracer.getData() );
//MqttController mqttController;

void setup()
{
    DebugBegin(115200);
    Serial.begin(115200);

    pinMode(D3, OUTPUT);
    digitalWrite(D3, 0);

    // WiFiManager
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(debugOn);
    wifiManager.autoConnect(WIFI_CONFIG_AP_NAME);

    // Start MDNS and OTA Updates
    OTAUpdate.begin();

    // Init the readings of rs485 from Tracer
    tracer.begin();

    // Start Webserver
    server.begin();

    // Init Firebase
    firebase.begin();

    DebugPrintln("Setup OK!");
    DebugPrintln("----------------------------");
    DebugPrintln();

    WiFi.setSleepMode(WIFI_MODEM_SLEEP);
}

void loop()
{
    OTAUpdate.handle();
    tracer.update();
    server.handle();
    firebase.update();
    //mqttController.loop();
    delay(100);
}
