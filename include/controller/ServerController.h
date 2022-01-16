#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "domain/TracerData.h"
#include "debug.h"

#ifndef ServerController_h
#define ServerController_h

/**************************************************
* Gerencia as requisições HTTP do painel web local
**************************************************/
class ServerController {
  private:
    ESP8266WebServer _server;
    TracerData *_tracerData;

    String htmlHeader(String title);
    String htmlFooter();

    void reset();
    void info();
    void getRatedData();
    void getRealtimeData();
    void getRealtimeStatus();
    void getStatisticalData();
    void getCoils();
    void getDiscrete();

  public:
    ServerController(TracerData &_tracerData);
    void begin();
    void handle();
};

#endif
