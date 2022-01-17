#include "controller/TracerController.h"

TracerController::TracerController()
{
    // Runs the updates each 600 milliseconds
    this->_timer.setDelayTime(this->_delay);
}
void TracerController::begin()
{
    this->_node.begin(1, Serial);
    this->_node.preTransmission([]() {});
    this->_node.postTransmission([]() {});

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

void TracerController::updateNextRegistryEntry()
{
    if (this->_currentRegistryNumber == 0) {
        this->AddressRegistry_2000();
    }
    if (this->_currentRegistryNumber == 1) {
        this->AddressRegistry_200C();
    }
    if (this->_currentRegistryNumber == 2) {
        this->AddressRegistry_3000();
    }
    if (this->_currentRegistryNumber == 3) {
        this->AddressRegistry_300E();
    }    
    if (this->_currentRegistryNumber == 4) {
        this->AddressRegistry_3100();
    }
    if (this->_currentRegistryNumber == 5) {
        this->AddressRegistry_310C();
    }
    if (this->_currentRegistryNumber == 6) {
        this->AddressRegistry_3110();
    }
    if (this->_currentRegistryNumber == 7) {
        this->AddressRegistry_311A();
    }
    if (this->_currentRegistryNumber == 8) {
        this->AddressRegistry_311D();
    }
    if (this->_currentRegistryNumber == 9) {
        this->AddressRegistry_3200();
    }
    if (this->_currentRegistryNumber == 10) {
        this->AddressRegistry_3300();
    }
    if (this->_currentRegistryNumber == 11) {
        this->AddressRegistry_330A();
    }
    if (this->_currentRegistryNumber == 12) {
        this->AddressRegistry_3310();
    }
    if (this->_currentRegistryNumber == 13) {
        this->AddressRegistry_331B();
    }
    if (this->_currentRegistryNumber == 14) {
        this->readManualCoil();
    }
    if (this->_currentRegistryNumber == 15) {
        this->readLoadTestAndForceLoadCoil();
    }

    // better not use modulo, because after overlow it will start reading in incorrect order
    this->_currentRegistryNumber++;
    if (this->_currentRegistryNumber >= 15)
    {
        this->_currentRegistryNumber = 0;
        this->_data.canSend = true;
    }
}

// reads manual control state
void TracerController::readManualCoil()
{
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
    }
    else
    {
        DebugPrintln("Failed to read coils 0x05 & 0x06!");
    }
}

void TracerController::AddressRegistry_2000()
{
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
    this->_result = this->_node.readDiscreteInputs(0x200C, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.discreteInput.dayNight = this->_node.getResponseBuffer(0x00);
        DebugPrint("Day (0) or Night (1): ");
        DebugPrintln(this->_data.discreteInput.dayNight);
    }
    else
    {
        DebugPrintln("Read discrete input 0x200C failed!");
    }
}

void TracerController::AddressRegistry_3000()
{
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
    this->_result = this->_node.readInputRegisters(0x300E, 1);

    if (this->_result == this->_node.ku8MBSuccess)
    {
        this->_data.ratedData.loadCurrent = this->_node.getResponseBuffer(0x00) / 100.0f;
        DebugPrint("Load Current: ");
        DebugPrintln(this->_data.ratedData.loadCurrent);
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
