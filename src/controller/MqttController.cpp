#include "controller/MqttController.h"

MqttController::MqttController()
{
    this->MQTT.setClient(this->wifiClient);
    this->MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    this->MQTT.setCallback(MqttController::receiveTopic);
}
bool MqttController::setConnected()
{
    bool connected = (WiFi.isConnected() && MQTT.connected());

    // Envia notificação para o Display quando ocorre alguma mudança
    if ((this->_connected && !connected) || (!this->_connected && connected))
    {
        String cs = (connected ? "Connected" : "Disconnected");

        DebugPrint("MQTT ");
        DebugPrintln(cs);
    }

    this->_connected = connected;
    return connected;
}
void MqttController::connect()
{
    if (this->setConnected()) return;

    // Apenas tenta conexão quando o Wifi está conectado
    if (WiFi.isConnected())
    {
        //if (MQTT.connect(CLIENT_ID, USER_MQTT, PWD_MQTT))
        if (MQTT.connect(CLIENT_ID))
        {
            this->setConnected();
            this->subscribeTopic("updates");
            this->publishTopic("helloworld", "I am back!");
        }
    }
}
void MqttController::loop()
{
    connect();
    MQTT.loop();
}
// Se inscreve em algum tópico
void MqttController::subscribeTopic(String topicCode)
{
    if (this->setConnected())
    {
        boolean success = this->MQTT.subscribe(topicCode.c_str());

        if (success) {
            DebugPrint("Successfully Subscribed on Topic ");
        } else {
            DebugPrint("Error to Subscribe on Topic ");
        }
        DebugPrintln(topicCode);
    }
}
// Método estático
// Recebe o pacote dos tópicos o qual está inscrito
void MqttController::receiveTopic(char *code, byte *payload, unsigned int length)
{
    if (sizeof(code) <= 0) return;

    // Monta a struct para envio dos dados
    String value = "";
    // obtem a string do payload recebido
    for (unsigned int i = 0; i < length; i++)
    {
        char c = (char) payload[i];
        value += c;
    }

    DebugPrint("Received Topic ");
    DebugPrint(code);
    DebugPrint(": ");
    DebugPrintln(value);
}
// Publica um valor no Topico
void MqttController::publishTopic(String topicCode, String topicValue)
{
    if (this->setConnected())
    {
        // Envia o dado para o MQTT
        if(MQTT.publish(topicCode.c_str(), topicValue.c_str())) {
            DebugPrint("Topic Sent ");
        } else {
            DebugPrint("Error to send Topic ");
        }
        DebugPrintln(topicValue);
    }
}