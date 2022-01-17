#include "controller/OTAUpdateController.h"

OTAUpdateController::OTAUpdateController () 
{
    pinMode(OTA_LED_PIN, OUTPUT);
    digitalWrite(OTA_LED_PIN, HIGH); //inverted, off

    ArduinoOTA.setHostname(MDNS_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_SPIFFS
            type = "filesystem";
        }
        DebugPrintln("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        DebugPrintln("\nEnd of update");
        digitalWrite(OTA_LED_PIN, HIGH);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      int percentageComplete = (progress / (total / 100));
      DebugPrintf("Progress: %u%%\r", percentageComplete);
      digitalWrite(OTA_LED_PIN, percentageComplete % 2);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        DebugPrintf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            DebugPrintln("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            DebugPrintln("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            DebugPrintln("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            DebugPrintln("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            DebugPrintln("End Failed");
        }
    });
}
void OTAUpdateController::begin() 
{
    // Start the mDNS responder for esp8266.local
    if (MDNS.begin(MDNS_HOSTNAME)) {
        DebugPrint("mDNS responder started with hostname ");
    } else {
        DebugPrint("Error setting up mDNS responder with hostname ");
    }
    DebugPrintln(MDNS_HOSTNAME);

    ArduinoOTA.begin();
    DebugPrintln("ArduinoOTA running.");
}
void OTAUpdateController::handle() 
{
    MDNS.update();
    ArduinoOTA.handle();
}