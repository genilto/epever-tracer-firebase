#include "controller/FirebaseController.h"
#include <addons/TokenHelper.h>

FirebaseController::FirebaseController(TracerData &tracerData)
{
    this->_tracerData = &tracerData;
    this->_timer.setDelayTime(3000);
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
void FirebaseController::handle()
{
    if (this->_userId.isEmpty())
    {
        this->_userId = this->_auth.token.uid.c_str();

        DebugPrint("UID: ");
        DebugPrintln(this->_userId);

        if (this->_userId.isEmpty())
        {
            return;
        }
    }

    if (Firebase.ready() && this->_tracerData->canSend && this->_timer.expired())
    {
        // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
        FirebaseJson content;
        String documentPath = "users/" + this->_userId +
                              "/devices/" + DEVICE_ID +
                              "/readings";

        content.set("fields/pvVoltage/doubleValue", this->_tracerData->realtimeData.pvVoltage);
        content.set("fields/pvCurrent/doubleValue", this->_tracerData->realtimeData.pvCurrent);
        content.set("fields/pvPower/integerValue", this->_tracerData->realtimeData.pvPower);
        content.set("fields/batteryVoltage/doubleValue", this->_tracerData->realtimeData.batteryVoltage);
        content.set("fields/batteryChargingCurrent/doubleValue", this->_tracerData->realtimeData.batteryChargingCurrent);
        content.set("fields/batteryChargingPower/integerValue", this->_tracerData->realtimeData.batteryChargingPower);
        content.set("fields/loadVoltage/doubleValue", this->_tracerData->realtimeData.loadVoltage);
        content.set("fields/loadCurrent/doubleValue", this->_tracerData->realtimeData.loadCurrent);
        content.set("fields/loadPower/integerValue", this->_tracerData->realtimeData.loadPower);
        content.set("fields/batteryTemp/doubleValue", this->_tracerData->realtimeData.batteryTemp);
        content.set("fields/equipmentTemp/doubleValue", this->_tracerData->realtimeData.equipmentTemp);
        content.set("fields/heatsinkTemp/doubleValue", this->_tracerData->realtimeData.heatsinkTemp);
        content.set("fields/batterySoC/integerValue", this->_tracerData->realtimeData.batterySoC);
        content.set("fields/batteryRemoteTemp/doubleValue", this->_tracerData->realtimeData.batteryRemoteTemp);

        DebugPrint("Create a document... ");

        if (Firebase.Firestore.createDocument(
                &this->_fbdo,
                FIREBASE_PROJECT_ID,
                "",
                documentPath.c_str(),
                content.raw()))
        {
            DebugPrintf("ok\n%s\n\n", this->_fbdo.payload().c_str());
            this->_tracerData->canSend = false;
        }
        else
        {
            DebugPrintln(this->_fbdo.errorReason());
        }
    }
}
