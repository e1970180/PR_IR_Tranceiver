#pragma once

/*
    from 
	Class to send IR signals using the Arduino PWM
*/

#include <Arduino.h>

#ifdef ESP8266
					
#endif

class IRSender {
  protected:
    IRSender(const uint8_t pin); // Cannot create generic IRSender instances

  public:
	
	void 			sendIRbyte(const uint8_t sendByte, const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
							   const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength);
	
	void	 		sendIRnbits(const uint64_t data, const const uint8_t nbits, 
								const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
								const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength);
	
	void 			sendIRraw(uint16_t raw[], const uint16_t lenght)
    
	uint8_t 		bitReverse(const uint8_t x);
    
	virtual void	begin();
	virtual void 	setFrequency(const uint16_t frequency);
	virtual void 	space(const uint16_t spaceLength);
    virtual void 	mark(const uint16_t markLength);

  protected:
    uint8_t _pin;
};

// The generic functions of the abstract IRSender class

IRSender::IRSender(const uint8_t pin)	{ _pin = pin; }

void	IRSender::begin() {
	  pinMode(_pin, OUTPUT);
}

// Send a uint8_t (8 bits) over IR
void	IRSender::sendIRbyte(const uint8_t sendByte, const bool MSBfirst, 
							 const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
							 const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength)
{
	const uint8_t nbits = 8;

	uint8_t mask = 1ULL;

	if (MSBfirst) mask <<= (nbits-1);	// Send the MSB first.

	for (uint8_t i = 0; i < nbits ; i++) {
		if (data & mask)
		{
			mark(oneMarkLength);
			space(oneSpaceLength);
		}
		else
		{
			mark(zeroMarkLength);
			space(zeroSpaceLength);
		}
		MSBfirst ? mask >>= 1 :  mask <<= 1;
	}
}

void	IRSender::sendIRnbits(const uint64_t data, const uint8_t nbits, const bool MSBfirst
							const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
							const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength)
	{
		if (nbits == 0) return;	// If we are asked to send nothing, just return.
		
		uint64_t mask = 1ULL;
		
		if (MSBfirst) mask <<= (nbits-1);	// Send the MSB first.

		for (uint8_t i = 0; i < nbits ; i++) {
			if (data & mask)
			{
				mark(oneMarkLength);
				space(oneSpaceLength);
			}
			else
			{
				mark(zeroMarkLength);
				space(zeroSpaceLength);
			}
			MSBfirst ? mask >>= 1 :  mask <<= 1;
		}
	}	
	


void	IRSender::sendIRraw(uint16_t raw[], const uint16_t lenght) 
	{
		for (uint16_t i = 0; i < lenght; i++)
		{
			if ( i & 0x01 ) mark(raw[i]); else space(raw[i]);
		}
		space(0);
	}





// The Carrier IR protocol has the bits in a reverse order (compared to the other heatpumps)
// See http://www.nrtm.org/index.php/2013/07/25/reverse-bits-in-a-uint8_t/
uint8_t IRSender::bitReverse(const uint8_t x)
	{
	  //          01010101  |         10101010
	  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
	  //          00110011  |         11001100
	  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
	  //          00001111  |         11110000
	  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
	  return x;
	}


// Definitions of virtual functions
void IRSender::setFrequency(const uint16_t) {};
void IRSender::space(const uint16_t) {};
void IRSender::mark(const uint16_t) {};