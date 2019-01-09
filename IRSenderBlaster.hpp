#include <Arduino.h>
#include <IRSender.h>

// Send IR using the IR Blaster. The IR Blaster generates the 38 kHz carrier frequency

class IRSenderBlaster : public IRSender
{
  public:
    IRSenderBlaster(const uint8_t pin);
    void 	setFrequency(const uint16_t frequency);
    void 	space(const uint16_t spaceLength);
    void 	mark(const uint16_t markLength);
};


IRSenderBlaster::IRSenderBlaster(const uint8_t pin) : IRSender(pin)
{
  pinMode(_pin, OUTPUT);
}


void IRSenderBlaster::setFrequency(const int frequency)
{
  (void)frequency;
}


// Send an IR 'mark' symbol, i.e. transmitter ON
void IRSenderBlaster::mark(const int markLength)
{
  digitalWrite(_pin, HIGH);

  if (markLength < 16383) {
    delayMicroseconds(markLength);
  } else {
    delay(markLength/1000);
  }
}


// Send an IR 'space' symbol, i.e. transmitter OFF
void IRSenderBlaster::space(const int spaceLength)
{
  digitalWrite(_pin, LOW);

  if (spaceLength < 16383) {
    delayMicroseconds(spaceLength);
  } else {
    delay(spaceLength/1000);
  }
}