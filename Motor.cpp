#include "Motor.h"

uint8_t Motor::steps[8] = {
  0b0001,
  0b0011,
  0b0010,
  0b0110,
  0b0100,
  0b1100,
  0b1000,
  0b1001
};

Motor::Motor(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3) {
  this->pin0 = p0;
  this->pin1 = p1;
  this->pin2 = p2;
  this->pin3 = p3;
  pinMode(p0, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
  pinMode(p3, OUTPUT);
}

void Motor::setDirSpeed(char d, uint16_t s) {
  this->dir = d;
  this->spd = s;
  this->currDelay = this->calculateDelay(s);
}

void Motor::loop() {
  unsigned long now = micros();
  unsigned long delta = now - this->lastMicros;
  if (now < lastMicros) { //millis counter overflowed in the meantime
    delta = now - ((long)(this->lastMicros - 4294967295)); //so we overflow lastMillis too...
  }
  if (delta < MIN_DELAY) { return; } //do not loop too often
  if (delta < this->currDelay) { //min delay according to the speed set
    return;
  }

  this->lastMicros = now;
  switch (this->dir) {
    case 'S':
    case 'N':
    case 'P': //Stop or Neutral or Park
      this->doStop();
      break;
    case 'F':
    case 'D': //Forward or Drive
      this->currStep = (this->currStep + 1) & 7;
      this->setStep(this->currStep);
      break;
    case 'B':
    case 'R': //Back or Reverse
      this->currStep = (this->currStep - 1) & 7;
      this->setStep(this->currStep);
      break;
  }
}

unsigned long Motor::calculateDelay(uint16_t s) {
  //s == MAX_SPEED => MIN_DELAY
  //s == MIN_SPEED => MAX_DELAY
  //interpolate the delay
  this->currDelay = map(s, MIN_SPEED, MAX_SPEED, MAX_DELAY, MIN_DELAY);
}

void Motor::setStep(uint8_t stp) {
  uint8_t s = Motor::steps[stp & 7];
  digitalWrite(this->pin0, (s & 0b0001) > 0);
  digitalWrite(this->pin1, (s & 0b0010) > 0);
  digitalWrite(this->pin2, (s & 0b0100) > 0);
  digitalWrite(this->pin3, (s & 0b1000) > 0);
}

void Motor::doStop() {
  digitalWrite(this->pin0, 0);
  digitalWrite(this->pin1, 0);
  digitalWrite(this->pin2, 0);
  digitalWrite(this->pin3, 0);
}

