#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "domain/TracerData.h"
#include "debug.h"
#include "Timer.h"

#ifndef FirebaseController_h
#define FirebaseController_h

/**************************************************
* Gerencia o envio de dados para o firebase
**************************************************/
class FirebaseController {
  private:
    TracerData *_tracerData;
    Timer _timer;
    int _delayTime = 60000;
    bool _canSend = false;

    // Define Firebase Data object
    FirebaseData _fbdo;
    FirebaseAuth _auth;
    FirebaseConfig _config;

    // UserId to facilitate
    String _userId;
  public:
    FirebaseController(TracerData &_tracerData);
    void begin();
    bool update();
};

#endif
