#include "controller/TracerController.h"

TracerController::TracerController()
{
    // Runs the updates each 600 milliseconds
    this->_timer.setDelayTime(this->_delayTime);
}
void TracerController::begin()
{
    this->_node.begin(1, Serial);
    //this->_node.preTransmission([]() {});
    //this->_node.postTransmission([]() {});

    // Read the static information from Tracer
}
void TracerController::update()
{
    // Just continue when it's time
    if (!this->_timer.expired())
        return;

    // Read the realtime information from Tracer
    this->updateNextRegistryEntry();

    digitalWrite(D3, 1);

    if (this->_result != this->_node.ku8MBSuccess)
    {
        delay(100);
        digitalWrite(D3, 0);
        delay(100);
        digitalWrite(D3, 1);
        delay(100);
        digitalWrite(D3, 0);
    }
}

TracerData *TracerController::getData()
{
    return &this->_data;
}

void TracerController::updateCurrentRegistryNumber () {
    this->_data.currentRegistryNumber++;
    if (this->_data.currentRegistryNumber > 16)
    {
        this->_data.currentRegistryNumber = 0;
        this->_data.canSend = true;
    }
}

void TracerController::updateNextRegistryEntry()
{
    if (this->_data.currentRegistryNumber == 0) {
        this->AddressRegistry_2000();
    }
    if (this->_data.currentRegistryNumber == 1) {
        this->AddressRegistry_200C();
    }
    if (this->_data.currentRegistryNumber == 2) {
        this->AddressRegistry_3000();
    }
    if (this->_data.currentRegistryNumber == 3) {
        this->AddressRegistry_300E();
    }    
    if (this->_data.currentRegistryNumber == 4) {
        this->AddressRegistry_3100();
    }
    if (this->_data.currentRegistryNumber == 5) {
        this->AddressRegistry_310C();
    }
    if (this->_data.currentRegistryNumber == 6) {
        this->AddressRegistry_3110();
    }
    if (this->_data.currentRegistryNumber == 7) {
        this->AddressRegistry_311A();
    }
    if (this->_data.currentRegistryNumber == 8) {
        this->AddressRegistry_311D();
    }
    if (this->_data.currentRegistryNumber == 9) {
        this->AddressRegistry_3200();
    }
    if (this->_data.currentRegistryNumber == 10) {
        this->AddressRegistry_3300();
    }
    if (this->_data.currentRegistryNumber == 11) {
        this->AddressRegistry_330A();
    }
    if (this->_data.currentRegistryNumber == 12) {
        this->AddressRegistry_3310();
    }
    if (this->_data.currentRegistryNumber == 13) {
        this->AddressRegistry_331B();
    }
    if (this->_data.currentRegistryNumber == 14) {
        this->readManualCoil();
    }
    if (this->_data.currentRegistryNumber == 15) {
        this->readLoadTestAndForceLoadCoil();
    }
    if (this->_data.currentRegistryNumber == 16) {
        this->AddressRegistry_9013();
    }

    this->updateCurrentRegistryNumber();
}

// reads manual control state
void TracerController::readManualCoil()
{
    if (this->_data.switchValues.read) {
        this->updateCurrentRegistryNumber();
        return;
    }
    
    DebugPrint("Reading coil 0x02.. ");
    delay(10);

    this->_result = this->_node.readCoils(0x0002, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.switchValues.manualControl = (this->_node.getResponseBuffer(0x00) > 0);

        DebugPrint("Manual Load Control State: ");
        DebugPrintln(this->_data.switchValues.manualControl);
    }
    else
    {
        DebugPrintln("Failed to read coil 0x02!");
    }
}

// reads Load Enable Override coil
void TracerController::readLoadTestAndForceLoadCoil()
{
    if (this->_data.switchValues.read) {
        this->updateCurrentRegistryNumber();
        return;
    }

    DebugPrint("Reading coil 0x05 & 0x06.. ");
    delay(10);

    this->_result = this->_node.readCoils(0x0005, 2);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.switchValues.loadTest = (this->_node.getResponseBuffer(0x00) > 0);
        DebugPrint("Enable Load Test Mode: ");
        DebugPrintln(this->_data.switchValues.loadTest);

        this->_data.switchValues.forceLoad = (this->_node.getResponseBuffer(0x01) > 0);
        DebugPrint("Force Load On/Off: ");
        DebugPrintln(this->_data.switchValues.forceLoad);

        this->_data.switchValues.read = true;
    }
    else
    {
        DebugPrintln("Failed to read coils 0x05 & 0x06!");
    }
}

void TracerController::AddressRegistry_2000()
{
    if (this->_data.discreteInput.read) {
        this->updateCurrentRegistryNumber();
        return;
    }

    this->_result = this->_node.readDiscreteInputs(0x2000, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.discreteInput.overTemp = this->_node.getResponseBuffer(0x00);
        DebugPrint("Over temperature inside device (1) or Normal (0): ");
        DebugPrintln(this->_data.discreteInput.overTemp);
    }
    else
    {
        DebugPrintln("Read discrete input 0x2000 failed!");
    }
}

void TracerController::AddressRegistry_200C()
{
    if (this->_data.discreteInput.read) {
        this->updateCurrentRegistryNumber();
        return;
    }

    this->_result = this->_node.readDiscreteInputs(0x200C, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.discreteInput.dayNight = this->_node.getResponseBuffer(0x00);
        DebugPrint("Day (0) or Night (1): ");
        DebugPrintln(this->_data.discreteInput.dayNight);

        this->_data.discreteInput.read = true;
    }
    else
    {
        DebugPrintln("Read discrete input 0x200C failed!");
    }
}

void TracerController::AddressRegistry_3000()
{
    if (this->_data.ratedData.read) {
        this->updateCurrentRegistryNumber();
        return;
    }

    this->_result = this->_node.readInputRegisters(0x3000, 9);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.ratedData.pvVoltage = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("PV Voltage: ");
        DebugPrintln(this->_data.ratedData.pvVoltage);

        this->_data.ratedData.pvCurrent = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("PV Current: ");
        DebugPrintln(this->_data.ratedData.pvCurrent);

        this->_data.ratedData.pvPower = (this->_node.getResponseBuffer(0x02) | this->_node.getResponseBuffer(0x03) << 16) / 100.0f;
        DebugPrint("PV Power: ");
        DebugPrintln(this->_data.ratedData.pvPower);

        this->_data.ratedData.batteryVoltage = this->_node.getResponseBuffer(0x04) / 100.0f;
        DebugPrint("Battery Voltage: ");
        DebugPrintln(this->_data.ratedData.batteryVoltage);

        this->_data.ratedData.batteryCurrent = this->_node.getResponseBuffer(0x05) / 100.0f;
        DebugPrint("Battery Current: ");
        DebugPrintln(this->_data.ratedData.batteryCurrent);

        this->_data.ratedData.batteryPower = (this->_node.getResponseBuffer(0x06) | this->_node.getResponseBuffer(0x07) << 16) / 100.0f;
        DebugPrint("Battery Power: ");
        DebugPrintln(this->_data.ratedData.batteryPower);

        this->_data.ratedData.chargingMode = this->_node.getResponseBuffer(0x08);
        DebugPrint("Charging mode: ");
        DebugPrintln(this->_data.ratedData.chargingMode);
    }
    else
    {
        DebugPrintln("Read register 0x3000 failed!");
    }
}

void TracerController::AddressRegistry_300E()
{
    if (this->_data.ratedData.read) {
        this->updateCurrentRegistryNumber();
        return;
    }

    this->_result = this->_node.readInputRegisters(0x300E, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.ratedData.loadCurrent = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Load Current: ");
        DebugPrintln(this->_data.ratedData.loadCurrent);

        this->_data.ratedData.read = true;
    }
    else
    {
        DebugPrintln("Read register 0x311D failed!");
    }
}

void TracerController::AddressRegistry_3100()
{
    this->_result = this->_node.readInputRegisters(0x3100, 8);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeData.pvVoltage = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("PV Voltage: ");
        DebugPrintln(this->_data.realtimeData.pvVoltage);

        this->_data.realtimeData.pvCurrent = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("PV Current: ");
        DebugPrintln(this->_data.realtimeData.pvCurrent);

        this->_data.realtimeData.pvPower = (this->_node.getResponseBuffer(0x02) | this->_node.getResponseBuffer(0x03) << 16) / 100.0f;
        DebugPrint("PV Power: ");
        DebugPrintln(this->_data.realtimeData.pvPower);

        this->_data.realtimeData.batteryVoltage = this->_node.getResponseBuffer(0x04) / 100.0f;
        DebugPrint("Battery Voltage: ");
        DebugPrintln(this->_data.realtimeData.batteryVoltage);

        this->_data.realtimeData.batteryChargingCurrent = this->_node.getResponseBuffer(0x05) / 100.0f;
        DebugPrint("Battery Charge Current: ");
        DebugPrintln(this->_data.realtimeData.batteryChargingCurrent);

        this->_data.realtimeData.batteryChargingPower = (this->_node.getResponseBuffer(0x06) | this->_node.getResponseBuffer(0x07) << 16) / 100.0f;
        DebugPrint("Battery Charge Power: ");
        DebugPrintln(this->_data.realtimeData.batteryChargingPower);
    }
    else
    {
        DebugPrintln("Read register 0x3100 failed!");
    }
}

void TracerController::AddressRegistry_310C()
{
    this->_result = this->_node.readInputRegisters(0x310C, 4);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeData.loadVoltage = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Load Voltage: ");
        DebugPrintln(this->_data.realtimeData.loadCurrent);

        this->_data.realtimeData.loadCurrent = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("Load Current: ");
        DebugPrintln(this->_data.realtimeData.loadCurrent);

        this->_data.realtimeData.loadPower = (this->_node.getResponseBuffer(0x02) | this->_node.getResponseBuffer(0x03) << 16) / 100.0f;
        DebugPrint("Load Power: ");
        DebugPrintln(this->_data.realtimeData.loadPower);
    }
    else
    {
        DebugPrintln("Read register 0x310C failed!");
    }
}

void TracerController::AddressRegistry_3110()
{
    this->_result = this->_node.readInputRegisters(0x3110, 3);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeData.batteryTemp = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Battery Temp: ");
        DebugPrintln(this->_data.realtimeData.batteryTemp);

        this->_data.realtimeData.equipmentTemp = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("Equipment Temp: ");
        DebugPrintln(this->_data.realtimeData.equipmentTemp);

        this->_data.realtimeData.heatsinkTemp = this->_node.getResponseBuffer(0x02) / 100.0f;
        DebugPrint("Heatsink Temp: ");
        DebugPrintln(this->_data.realtimeData.heatsinkTemp);
    }
    else
    {
        DebugPrintln("Read register 0x3110 failed!");
    }
}

void TracerController::AddressRegistry_311A()
{
    this->_result = this->_node.readInputRegisters(0x311A, 2);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeData.batterySoC = this->_node.getResponseBuffer(0x00) / 1.0f;
        DebugPrint("Battery State of Charge %: ");
        DebugPrintln(this->_data.realtimeData.batterySoC);

        this->_data.realtimeData.batteryTemp = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("Battery Temperature: ");
        DebugPrintln(this->_data.realtimeData.batteryTemp);
    }
    else
    {
        DebugPrintln("Read register 0x311A failed!");
    }
}

void TracerController::AddressRegistry_311D()
{
    this->_result = this->_node.readInputRegisters(0x311D, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeData.batteryRatedPower = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Battery Rated Power: ");
        DebugPrintln(this->_data.realtimeData.batteryRatedPower);
    }
    else
    {
        DebugPrintln("Read register 0x311D failed!");
    }
}

void TracerController::AddressRegistry_3200()
{
    this->_result = this->_node.readInputRegisters(0x3200, 3);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.realtimeStatus.batteryStatus = this->_node.getResponseBuffer(0x00);
        DebugPrint("Battery Status: ");
        DebugPrintln(this->_data.realtimeStatus.batteryStatus);

        this->_data.realtimeStatus.chargeEquipmentStatus = this->_node.getResponseBuffer(0x01);
        DebugPrint("Charge Equipment Status: ");
        DebugPrintln(this->_data.realtimeStatus.chargeEquipmentStatus);

        this->_data.realtimeStatus.dischargeEquipmentStatus = this->_node.getResponseBuffer(0x02);
        DebugPrint("Discharge Equipment Status: ");
        DebugPrintln(this->_data.realtimeStatus.dischargeEquipmentStatus);
    }
    else
    {
        DebugPrintln("Read register 0x3200 failed!");
    }
}

void TracerController::AddressRegistry_3300()
{
    this->_result = this->_node.readInputRegisters(0x3300, 16);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.statisticalParameters.todayMaxPvVoltage = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Maximum PV today: ");
        DebugPrintln(this->_data.statisticalParameters.todayMaxPvVoltage);

        this->_data.statisticalParameters.todayMinPvVoltage = this->_node.getResponseBuffer(0x01) / 100.0f;
        DebugPrint("Minimum PV today: ");
        DebugPrintln(this->_data.statisticalParameters.todayMinPvVoltage);

        this->_data.statisticalParameters.todayMaxBattVoltage = this->_node.getResponseBuffer(0x02) / 100.0f;
        DebugPrint("Maximum Battery today: ");
        DebugPrintln(this->_data.statisticalParameters.todayMaxBattVoltage);

        this->_data.statisticalParameters.todayMinBattVoltage = this->_node.getResponseBuffer(0x03) / 100.0f;
        DebugPrint("Minimum Battery today: ");
        DebugPrintln(this->_data.statisticalParameters.todayMinBattVoltage);

        this->_data.statisticalParameters.todayConsumedEnergy = (this->_node.getResponseBuffer(0x04) | this->_node.getResponseBuffer(0x05) << 16) / 100.0f;
        DebugPrint("Consumed energy today: ");
        DebugPrintln(this->_data.statisticalParameters.todayConsumedEnergy);

        this->_data.statisticalParameters.monthConsumedEnergy = (this->_node.getResponseBuffer(0x06) | this->_node.getResponseBuffer(0x07) << 16) / 100.0f;
        DebugPrint("Consumed energy this month: ");
        DebugPrintln(this->_data.statisticalParameters.monthConsumedEnergy);

        this->_data.statisticalParameters.yearConsumedEnergy = (this->_node.getResponseBuffer(0x08) | this->_node.getResponseBuffer(0x09) << 16) / 100.0f;
        DebugPrint("Consumed energy this year: ");
        DebugPrintln(this->_data.statisticalParameters.yearConsumedEnergy);
    }
    else
    {
        DebugPrintln("Read register 0x3300 failed!");
    }
}

void TracerController::AddressRegistry_330A()
{
    this->_result = this->_node.readInputRegisters(0x330A, 6);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.statisticalParameters.totalConsumedEnergy = (this->_node.getResponseBuffer(0x00) | this->_node.getResponseBuffer(0x01) << 16) / 100.0f;
        DebugPrint("Total consumed energy: ");
        DebugPrintln(this->_data.statisticalParameters.totalConsumedEnergy);

        this->_data.statisticalParameters.todayGeneratedEnergy = (this->_node.getResponseBuffer(0x02) | this->_node.getResponseBuffer(0x03) << 16) / 100.0f;
        DebugPrint("Generated energy today: ");
        DebugPrintln(this->_data.statisticalParameters.todayGeneratedEnergy);

        this->_data.statisticalParameters.monthGeneratedEnergy = (this->_node.getResponseBuffer(0x04) | this->_node.getResponseBuffer(0x05) << 16) / 100.0f;
        DebugPrint("Generated energy this month: ");
        DebugPrintln(this->_data.statisticalParameters.monthGeneratedEnergy);
    }
    else
    {
        DebugPrintln("Read register 0x330A failed!");
    }
}

void TracerController::AddressRegistry_3310()
{
    this->_result = this->_node.readInputRegisters(0x3310, 6);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.statisticalParameters.yearGeneratedEnergy = (this->_node.getResponseBuffer(0x00) | this->_node.getResponseBuffer(0x01) << 16) / 100.0f;
        DebugPrint("Generated energy this year: ");
        DebugPrintln(this->_data.statisticalParameters.yearGeneratedEnergy);

        this->_data.statisticalParameters.totalGeneratedEnergy = (this->_node.getResponseBuffer(0x02) | this->_node.getResponseBuffer(0x03) << 16) / 100.0f;
        DebugPrint("Total generated energy: ");
        DebugPrintln(this->_data.statisticalParameters.totalGeneratedEnergy);

        this->_data.statisticalParameters.CO2reduction = (this->_node.getResponseBuffer(0x04) | this->_node.getResponseBuffer(0x05) << 16) / 100.0f;
        DebugPrint("Carbon dioxide reduction: ");
        DebugPrintln(this->_data.statisticalParameters.CO2reduction);
    }
    else
    {
        DebugPrintln("Read register 0x3310 failed!");
    }
}

void TracerController::AddressRegistry_331B()
{
    this->_result = this->_node.readInputRegisters(0x331B, 4);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.statisticalParameters.batteryCurrent = (this->_node.getResponseBuffer(0x00) | this->_node.getResponseBuffer(0x01) << 16) / 100.0f;
        DebugPrint("Battery Discharge Current: ");
        DebugPrintln(this->_data.statisticalParameters.batteryCurrent);

        this->_data.statisticalParameters.batteryTemp = this->_node.getResponseBuffer(0x02) / 100.0f;
        DebugPrint("Battery Temperature: ");
        DebugPrintln(this->_data.statisticalParameters.batteryTemp);

        this->_data.statisticalParameters.ambientTemp = this->_node.getResponseBuffer(0x03) / 100.0f;
        DebugPrint("Ambient Temperature: ");
        DebugPrintln(this->_data.statisticalParameters.ambientTemp);
    }
    else
    {
        DebugPrintln("Read register 0x331B failed!");
    }
}

void TracerController::AddressRegistry_9013()
{
    this->_result = this->_node.readHoldingRegisters (0x9013, 3);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        Rtc rtc;
        memset(rtc.buf,0,sizeof(rtc.buf));

        rtc.buf[0]  = this->_node.getResponseBuffer(0x00);
        rtc.buf[1]  = this->_node.getResponseBuffer(0x01);
        rtc.buf[2]  = this->_node.getResponseBuffer(0x02);

        sprintf(this->_data.settingParameters.realTimeClock, "20%02d-%02d-%02d %02d:%02d:%02dZ-3",
            rtc.r.y, rtc.r.M, rtc.r.d, rtc.r.h, rtc.r.m, rtc.r.s
        );

        DebugPrint("RTC: ");
        DebugPrintln(this->_data.settingParameters.realTimeClock);
        /*
        tm t = {0};
        t.tm_sec = rtc.r.s;
        t.tm_min = rtc.r.m;
        t.tm_hour = rtc.r.h;
        t.tm_mday = rtc.r.d;
        t.tm_mon = (rtc.r.M) - 1; // 2-1, not 2!
        t.tm_year = (2000 + rtc.r.y) - 1900; // 2022-1900, not 1990!
        t.tm_isdst = -1;

        time_t time = mktime(&t);
        this->_data.settingParameters.realTimeClockTimestamp = time;

        DebugPrint("RTC Timestamp: ");
        DebugPrintln(this->_data.settingParameters.realTimeClockTimestamp);*/

        /*
        this->_data.settingParameters.realTimeClock = this->_node.getResponseBuffer(0x00);
        DebugPrint("RTC: ");
        DebugPrintln(this->_data.settingParameters.realTimeClock);


        this->_data.settingParameters.realTimeClock2 = this->_node.getResponseBuffer(0x01);
        DebugPrint("RTC2: ");
        DebugPrintln(this->_data.settingParameters.realTimeClock2);

        this->_data.settingParameters.realTimeClock3 = this->_node.getResponseBuffer(0x02);
        DebugPrint("RTC3: ");
        DebugPrintln(this->_data.settingParameters.realTimeClock3);*/
    }
    else
    {
        DebugPrintln("Read register 0x9013 failed!");
    }
}