#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#ifndef MqttController_h
#define MqttController_h

/**************************************************
* Mantém e gerencia a conexão com Mqtt
**************************************************/
class MqttController {
  private:
    WiFiClient wifiClient;
    PubSubClient MQTT;
    bool _connected = false;

    bool setConnected();
  public:
    MqttController();
    void connect();
    void loop();
    void subscribeTopic (String topicCode);
    static void receiveTopic(char* topic, byte* payload, unsigned int length);
    void publishTopic (String topicCode, String topicValue);
};

#endif
