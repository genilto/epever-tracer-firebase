#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "debug.h"

#ifndef OTAUpdateController_h
#define OTAUpdateController_h

/**************************************************
* Gerencia os updates OTA
**************************************************/
class OTAUpdateController {
  private:
  public:
    OTAUpdateController();
    void begin();
    void handle();
};

#endif
