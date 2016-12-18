//Motor.h

#ifndef Motor_h
#define Motor_h

#define MIN_SPEED 0
#define MAX_SPEED 100
#define MIN_DELAY 8000
#define MAX_DELAY 32000

#include <Arduino.h>
#include <stdint.h>

class Motor {
  public:
    //constructor defining default step callback (setting pins directly)
    Motor(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);
    //constructor passing user supplied callback
    Motor(void (*func)(uint8_t), bool rev);
    
    void setDirSpeed(char d, uint16_t s);
    void loop();

  private:
    void (*stepCallback)(uint8_t);
    bool reverse = false;
    uint8_t pin0, pin1, pin2, pin3;
    char dir = 'S';
    uint16_t spd = 0;
    unsigned long lastMicros = 0;
    unsigned long currDelay = 0;
    static const uint8_t steps[8];
    static const uint8_t stopState = 0;
    uint8_t currStep = 0;
    
    unsigned long calculateDelay(uint16_t s);
    void setStep(uint8_t stp);
    void doStop();
    //default pins callback
    void setPins(uint8_t state);
};

#endif


