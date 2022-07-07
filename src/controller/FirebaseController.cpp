#include "controller/FirebaseController.h"
#include <addons/TokenHelper.h>

FirebaseController::FirebaseController(TracerData &tracerData)
{
    this->_tracerData = &tracerData;
    this->_timer.setDelayTime(this->_delayTime);
}
void FirebaseController::begin()
{
    /* Assign the api key (required) */
    this->_config.api_key = FIREBASE_API_KEY;

    /* Assign the user sign in credentials */
    this->_auth.user.email = FIREBASE_USER;
    this->_auth.user.password = FIREBASE_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    this->_config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&this->_config, &this->_auth);
    Firebase.reconnectWiFi(true);
}
bool FirebaseController::update()
{
    if (this->_userId.isEmpty())
    {
        this->_userId = this->_auth.token.uid.c_str();

        DebugPrint("UID: ");
        DebugPrintln(this->_userId);

        if (this->_userId.isEmpty()) {
            return false;
        }
    }

    if (this->_timer.expired()) 
    {
        this->_canSend = true;
    }
    if (!this->_tracerData->everythingRead || !this->_canSend) 
    {
        return true;
    }
    if (!Firebase.ready()) 
    {
        return false;
    }

    String tracerDocument = "users/" + this->_userId +
                            "/devices/" + DEVICE_ID;
    
    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
    FirebaseJson content;
    String mapPath = "fields/realtimeData/mapValue/fields/";
    
    content.set(mapPath + "pvVoltage/doubleValue", this->_tracerData->realtimeData.pvVoltage);
    content.set(mapPath + "pvCurrent/doubleValue", this->_tracerData->realtimeData.pvCurrent);
    content.set(mapPath + "pvPower/integerValue", this->_tracerData->realtimeData.pvPower);
    content.set(mapPath + "batteryVoltage/doubleValue", this->_tracerData->realtimeData.batteryVoltage);
    content.set(mapPath + "batteryChargingCurrent/doubleValue", this->_tracerData->realtimeData.batteryChargingCurrent);
    content.set(mapPath + "batteryChargingPower/integerValue", this->_tracerData->realtimeData.batteryChargingPower);
    content.set(mapPath + "loadVoltage/doubleValue", this->_tracerData->realtimeData.loadVoltage);
    content.set(mapPath + "loadCurrent/doubleValue", this->_tracerData->realtimeData.loadCurrent);
    content.set(mapPath + "loadPower/integerValue", this->_tracerData->realtimeData.loadPower);
    content.set(mapPath + "batteryTemp/doubleValue", this->_tracerData->realtimeData.batteryTemp);
    content.set(mapPath + "equipmentTemp/doubleValue", this->_tracerData->realtimeData.equipmentTemp);
    content.set(mapPath + "heatsinkTemp/doubleValue", this->_tracerData->realtimeData.heatsinkTemp);
    content.set(mapPath + "batterySoC/integerValue", this->_tracerData->realtimeData.batterySoC);
    content.set(mapPath + "batteryRemoteTemp/doubleValue", this->_tracerData->realtimeData.batteryRemoteTemp);

    mapPath = "fields/statisticalParameters/mapValue/fields/";
    
    content.set(mapPath + "todayMaxPvVoltage/doubleValue", this->_tracerData->statisticalParameters.todayMaxPvVoltage);
    content.set(mapPath + "todayMinPvVoltage/doubleValue", this->_tracerData->statisticalParameters.todayMinPvVoltage);
    content.set(mapPath + "todayMaxBattVoltage/doubleValue", this->_tracerData->statisticalParameters.todayMaxBattVoltage);
    content.set(mapPath + "todayMinBattVoltage/doubleValue", this->_tracerData->statisticalParameters.todayMinBattVoltage);
    content.set(mapPath + "todayConsumedEnergy/doubleValue", this->_tracerData->statisticalParameters.todayConsumedEnergy);
    content.set(mapPath + "monthConsumedEnergy/doubleValue", this->_tracerData->statisticalParameters.monthConsumedEnergy);
    content.set(mapPath + "yearConsumedEnergy/doubleValue", this->_tracerData->statisticalParameters.yearConsumedEnergy);
    content.set(mapPath + "totalConsumedEnergy/doubleValue", this->_tracerData->statisticalParameters.totalConsumedEnergy);
    content.set(mapPath + "todayGeneratedEnergy/doubleValue", this->_tracerData->statisticalParameters.todayGeneratedEnergy);
    content.set(mapPath + "monthGeneratedEnergy/doubleValue", this->_tracerData->statisticalParameters.monthGeneratedEnergy);
    content.set(mapPath + "yearGeneratedEnergy/doubleValue", this->_tracerData->statisticalParameters.yearGeneratedEnergy);
    content.set(mapPath + "totalGeneratedEnergy/doubleValue", this->_tracerData->statisticalParameters.totalGeneratedEnergy);
    content.set(mapPath + "CO2reduction/doubleValue", this->_tracerData->statisticalParameters.CO2reduction);
    content.set(mapPath + "batteryCurrent/doubleValue", this->_tracerData->statisticalParameters.batteryCurrent);
    content.set(mapPath + "batteryTemp/doubleValue", this->_tracerData->statisticalParameters.batteryTemp);
    content.set(mapPath + "ambientTemp/doubleValue", this->_tracerData->statisticalParameters.ambientTemp);

    content.set("fields/localTime/stringValue", this->_tracerData->settingParameters.realTimeClock);
    /*
    DebugPrint("Create a document... ");

    if (Firebase.Firestore.createDocument(
            &this->_fbdo,
            FIREBASE_PROJECT_ID,
            "",
            (tracerDocument + "/readings").c_str(),
            content.raw()))
    {
        DebugPrintf("ok\n%s\n\n", this->_fbdo.payload().c_str());
        this->_tracerData->canSend = false;
        this->_canSend = false;
    }
    else
    {
        DebugPrintln(this->_fbdo.errorReason());
    }
    */
    if (Firebase.Firestore.patchDocument (
        &this->_fbdo,
            FIREBASE_PROJECT_ID,
            "",
            tracerDocument.c_str(),
            content.raw(),
            ""))
    {
        DebugPrintf("ok\n%s\n\n", this->_fbdo.payload().c_str());
        this->_tracerData->everythingRead = false;
        this->_canSend = false;

        digitalWrite(D3, 0);
        delay(100);
        digitalWrite(D3, 1);
        delay(100);
        digitalWrite(D3, 0);
        delay(100);
        digitalWrite(D3, 1);
        delay(100);

        return true;
    }
    
    DebugPrintln(this->_fbdo.errorReason());
    return false;
}
