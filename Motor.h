//Motor.h

#ifndef Motor_h
#define Motor_h

#define MIN_SPEED 0
#define MAX_SPEED 100
#define MIN_DELAY 750
#define MAX_DELAY 12000

#include <Arduino.h>
#include <stdint.h>

class Motor {
  public:
    Motor(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);
    void setDirSpeed(char d, uint16_t s);
    void loop();

  private:
    uint8_t pin0, pin1, pin2, pin3;
    char dir = 'S';
    uint16_t spd = 0;
    unsigned long lastMicros = 0;
    unsigned long currDelay = 0;
    unsigned long calculateDelay(uint16_t s);
    static uint8_t steps[8];
    uint8_t currStep = 0;
    void setStep(uint8_t stp);
    void doStop();
};

#endif


