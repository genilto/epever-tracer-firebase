#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <FS.h> // Include the SPIFFS library

ESP8266WebServer server(80);

void info();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    wifiManager.autoConnect("AutoConnectAP");
    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    SPIFFS.begin(); // Start the SPI Flash Files System

    server.onNotFound([]() {                              // If the client requests any URI
        server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.on("/", info);
    server.begin(); // Actually start the server
    Serial.println("HTTP server started");
}

void loop() {
    // put your main code here, to run repeatedly:
    server.handleClient();
    delay(100);
}

String htmlHeader(String title)
{
  return "<html><head><title>" + title + "</title></head><body>";
}

String htmlFooter()
{
  return "</body></html>";
}

void info()
{
  // calculate uptime
  long millisecs = millis() / 1000;
  int systemUpTimeSc = millisecs % 60;
  int systemUpTimeMn = (millisecs / 60) % 60;
  int systemUpTimeHr = (millisecs / (60 * 60)) % 24;
  int systemUpTimeDy = (millisecs / (60 * 60 * 24));

  // get SPIFFs info
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  // compose info string
  String info = htmlHeader("Info") + "<h1>Info</h1>" +
                "<b>ESP8266 Core Version:</b> " + ESP.getCoreVersion() + "</br>" +
                "<b>ESP8266 SDK Version:</b> " + ESP.getSdkVersion() + "</br></br>" +
                "<b>Reset Reason:</b> " + ESP.getResetReason() + "</br></br>" +
                "<b>Free Heap:</b> " + ESP.getFreeHeap() + "bytes (" +
                ESP.getHeapFragmentation() + "% fragmentation)</br></br>" +
                "<b>ESP8266 Chip ID:</b> " + ESP.getChipId() + "</br>" +
                "<b>ESP8266 Flash Chip ID:</b> " + ESP.getFlashChipId() + "</br></br>" +
                "<b>Flash Chip Size:</b> " + ESP.getFlashChipRealSize() + " bytes (" +
                ESP.getFlashChipSize() + " bytes seen by SDK) </br>" +
                "<b>Sketch Size:</b> " + ESP.getSketchSize() + " bytes used of " +
                ESP.getFreeSketchSpace() + " bytes available</br>" +
                "<b>SPIFFs Usage:</b> " + fs_info.usedBytes + " bytes of " +
                fs_info.totalBytes + " bytes available</br></br>" +
                "<b>WiFi SSID:</b> " + WiFi.SSID() + "</br>" +
                "<b>WiFi RSSI:</b> " + WiFi.RSSI() + "dBm</br></br>" +
                "<b>System Uptime:</b> " + String(systemUpTimeDy) + " day(s), " +
                systemUpTimeHr + " hour(s), " + systemUpTimeMn + " minute(s), " +
                systemUpTimeSc + " second(s)" + htmlFooter();

  server.send(200, "text/html", info);
}