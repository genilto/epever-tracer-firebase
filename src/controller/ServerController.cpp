#include "controller/ServerController.h"

ServerController::ServerController(TracerData &tracerData)
{
    this->_tracerData = &tracerData;
}
void ServerController::begin()
{
    this->_server.onNotFound([this]() {                          // If the client requests any URI
        this->_server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    // Routes
    this->_server.on("/", [this]()
                     { this->info(); });

    this->_server.on("/getRatedData", [this]()
                     { this->getRatedData(); });
    this->_server.on("/getRealtimeData", [this]()
                     { this->getRealtimeData(); });
    this->_server.on("/getRealtimeStatus", [this]()
                     { this->getRealtimeStatus(); });
    this->_server.on("/getStatisticalData", [this]()
                     { this->getStatisticalData(); });
    this->_server.on("/getCoils", [this]()
                     { this->getCoils(); });
    this->_server.on("/getDiscrete", [this]()
                     { this->getDiscrete(); });
    this->_server.on("/reset", [this]()
                     { this->reset(); });

    // Actually start the server, defaults to port 80
    this->_server.begin();
}
void ServerController::handle()
{
    this->_server.handleClient();
}

String ServerController::htmlHeader(String title)
{
    return "<html><head><title>" + title + "</title></head><body>";
}

String ServerController::htmlFooter()
{
    return "</body></html>";
}

void ServerController::reset()
{
    // Reset saved settings
    //wifiManager.resetSettings();
    DebugPrintln("Reseting Settings...");
    WiFi.disconnect(true);
    ESP.eraseConfig();
    DebugPrintln("Rebooting...");
    delay(2000);
    ESP.reset();
}

void ServerController::info()
{
    // Calculate uptime
    long millisecs = millis() / 1000;
    int systemUpTimeSc = millisecs % 60;
    int systemUpTimeMn = (millisecs / 60) % 60;
    int systemUpTimeHr = (millisecs / (60 * 60)) % 24;
    int systemUpTimeDy = (millisecs / (60 * 60 * 24));

    // compose info string
    String info = htmlHeader("Tracer Reader Info") + "<h1>Tracer Reader Info</h1>" +
                  "<b>Firmware Version:</b> " + VERSION + "</br></br>" +
                  "<b>ESP8266 Core Version:</b> " +
                  ESP.getCoreVersion() + "</br>" +
                  "<b>ESP8266 SDK Version:</b> " + ESP.getSdkVersion() + "</br>" +
                  "<b>ESP8266 Chip ID:</b> " + ESP.getChipId() + "</br>" +
                  "<b>ESP8266 Flash Chip ID:</b> " + ESP.getFlashChipId() + "</br></br>" +

                  "<b>Free Heap:</b> " + ESP.getFreeHeap() + "bytes (" +
                  ESP.getHeapFragmentation() + "% fragmentation)</br>" +
                  "<b>Flash Chip Size:</b> " + ESP.getFlashChipRealSize() + " bytes (" +
                  ESP.getFlashChipSize() + " bytes seen by SDK) </br>" +
                  "<b>Sketch Size:</b> " + ESP.getSketchSize() + " bytes used of " +
                  ESP.getFreeSketchSpace() + " bytes available</br></br>" +

                  "<b>WiFi SSID:</b> " + WiFi.SSID() + "</br>" +
                  "<b>WiFi RSSI:</b> " + WiFi.RSSI() + "dBm</br>" +
                  "<b>WiFi Local IP:</b> " + WiFi.localIP().toString() + "</br></br>" +

                  "<b>Reset Reason:</b> " + ESP.getResetReason() + "</br>" +
                  "<b>System Uptime:</b> " + String(systemUpTimeDy) + " day(s), " +
                  systemUpTimeHr + " hour(s), " + systemUpTimeMn + " minute(s), " +
                  systemUpTimeSc + " second(s)" + htmlFooter();

    this->_server.send(200, "text/html", info);
}

void ServerController::getRatedData()
{
    this->_server.send(200, "application/json",
                       "{\"pvVoltage\":" + String(this->_tracerData->ratedData.pvVoltage) +
                           ", \"pvCurrent\":" + String(this->_tracerData->ratedData.pvCurrent) +
                           ", \"pvPower\":" + String(this->_tracerData->ratedData.pvPower) +
                           ", \"batteryVoltage\":" + String(this->_tracerData->ratedData.batteryVoltage) +
                           ", \"batteryCurrent\":" + String(this->_tracerData->ratedData.batteryCurrent) +
                           ", \"batteryPower\":" + String(this->_tracerData->ratedData.batteryPower) +
                           ", \"chargingMode\":" + String(this->_tracerData->ratedData.chargingMode) +
                           ", \"loadCurrent\":" + String(this->_tracerData->ratedData.loadCurrent) + "}");
}

void ServerController::getRealtimeData()
{
    this->_server.send(200, "application/json",
                       "{\"pvVoltage\":" + String(this->_tracerData->realtimeData.pvVoltage) +
                           ", \"pvCurrent\":" + String(this->_tracerData->realtimeData.pvCurrent) +
                           ", \"pvPower\":" + String(this->_tracerData->realtimeData.pvPower) +
                           ", \"batteryVoltage\":" + String(this->_tracerData->realtimeData.batteryVoltage) +
                           ", \"batteryChargingCurrent\":" + String(this->_tracerData->realtimeData.batteryChargingCurrent) +
                           ", \"batteryChargingPower\":" + String(this->_tracerData->realtimeData.batteryChargingPower) +
                           ", \"loadVoltage\":" + String(this->_tracerData->realtimeData.loadVoltage) +
                           ", \"loadCurrent\":" + String(this->_tracerData->realtimeData.loadCurrent) +
                           ", \"loadPower\":" + String(this->_tracerData->realtimeData.loadPower) +
                           ", \"batteryTemp\":" + String(this->_tracerData->realtimeData.batteryTemp) +
                           ", \"equipmentTemp\":" + String(this->_tracerData->realtimeData.equipmentTemp) +
                           ", \"heatsinkTemp\":" + String(this->_tracerData->realtimeData.heatsinkTemp) +
                           ", \"batterySoC\":" + String(this->_tracerData->realtimeData.batterySoC) +
                           ", \"batteryRemoteTemp\":" + String(this->_tracerData->realtimeData.batteryRemoteTemp) +
                           ", \"batteryRatedPower\":" + String(this->_tracerData->realtimeData.batteryRatedPower) + "}");
}

void ServerController::getRealtimeStatus()
{
    this->_server.send(200, "application/json",
                       "{\"batteryStatus\":" + String(this->_tracerData->realtimeStatus.batteryStatus) +
                           ", \"chargeEquipmentStatus\":" + String(this->_tracerData->realtimeStatus.chargeEquipmentStatus) +
                           ", \"dischargeEquipmentStatus\":" + String(this->_tracerData->realtimeStatus.dischargeEquipmentStatus) +
                           ", \"currentRegistryNumber\":" + String(this->_tracerData->currentRegistryNumber) +
                           ", \"realTimeClock\":" + String(this->_tracerData->settingParameters.realTimeClock) + 
                           ", \"realTimeClock2\":" + String(this->_tracerData->settingParameters.realTimeClock2) + 
                           ", \"realTimeClock3\":" + String(this->_tracerData->settingParameters.realTimeClock3) + 
                           ", \"error\":\"" + String(this->_tracerData->settingParameters.error) + "\"}");
}

void ServerController::getStatisticalData()
{
    this->_server.send(200, "application/json",
                       "{\"todayMaxPvVoltage\":" + String(this->_tracerData->statisticalParameters.todayMaxPvVoltage) +
                           ", \"todayMinPvVoltage\":" + String(this->_tracerData->statisticalParameters.todayMinPvVoltage) +
                           ", \"todayMaxBattVoltage\":" + String(this->_tracerData->statisticalParameters.todayMaxBattVoltage) +
                           ", \"todayMinBattVoltage\":" + String(this->_tracerData->statisticalParameters.todayMinBattVoltage) +
                           ", \"todayConsumedEnergy\":" + String(this->_tracerData->statisticalParameters.todayConsumedEnergy) +
                           ", \"monthConsumedEnergy\":" + String(this->_tracerData->statisticalParameters.monthConsumedEnergy) +
                           ", \"yearConsumedEnergy\":" + String(this->_tracerData->statisticalParameters.yearConsumedEnergy) +
                           ", \"totalConsumedEnergy\":" + String(this->_tracerData->statisticalParameters.totalConsumedEnergy) +
                           ", \"todayGeneratedEnergy\":" + String(this->_tracerData->statisticalParameters.todayGeneratedEnergy) +
                           ", \"monthGeneratedEnergy\":" + String(this->_tracerData->statisticalParameters.monthGeneratedEnergy) +
                           ", \"yearGeneratedEnergy\":" + String(this->_tracerData->statisticalParameters.yearGeneratedEnergy) +
                           ", \"totalGeneratedEnergy\":" + String(this->_tracerData->statisticalParameters.totalGeneratedEnergy) +
                           ", \"CO2reduction\":" + String(this->_tracerData->statisticalParameters.CO2reduction) +
                           ", \"batteryCurrent\":" + String(this->_tracerData->statisticalParameters.batteryCurrent) +
                           ", \"batteryTemp\":" + String(this->_tracerData->statisticalParameters.batteryTemp) +
                           ", \"ambientTemp\":" + String(this->_tracerData->statisticalParameters.ambientTemp) + "}");
}

void ServerController::getCoils()
{
    this->_server.send(200, "application/json",
                       "{\"manualControl\":" + String(this->_tracerData->switchValues.manualControl) +
                           ", \"loadTest\":" + String(this->_tracerData->switchValues.loadTest) +
                           ", \"forceLoad\":" + String(this->_tracerData->switchValues.forceLoad) + "}");
}

void ServerController::getDiscrete()
{
    this->_server.send(200, "application/json",
                       "{\"overTemp\":" + String(this->_tracerData->discreteInput.overTemp) +
                           ", \"dayNight\":" + String(this->_tracerData->discreteInput.dayNight) + "}");
}
