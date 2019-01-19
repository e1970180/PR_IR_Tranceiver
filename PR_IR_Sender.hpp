#pragma once

/*
    from 
	Class to send IR signals using the Arduino PWM
*/

#include <Arduino.h>
#include <vector>

#ifdef ESP8266
					
#endif

class IRSender {
	protected:
		IRSender(const uint8_t pin); // Cannot create generic IRSender instances

	public:
	
		void 			sendIRbyte(const uint8_t sendByte, const bool MSBfirst, 
								   const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
								   const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength);
		
		void	 		sendIRnbits(const uint64_t data, const uint16_t nbits, const bool MSBfirst,
									const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
									const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength);
		
		void 			sendIRraw(uint16_t raw[], const uint16_t lenght);
		void 			sendIRraw(const std::vector <uint16_t>& vectorRaw);
		
		uint8_t 		bitReverse(const uint8_t x);
		
		virtual void	begin();
		virtual void	end();
		virtual void 	setFrequency(const uint16_t frequency);
		virtual void 	space(const uint16_t spaceLength);
		virtual void 	mark(const uint16_t markLength);

	protected:
		uint8_t _pin;
};

// The generic functions of the abstract IRSender class

IRSender::IRSender(const uint8_t pin)	{ _pin = pin; }

void	IRSender::begin() 
{
	pinMode(_pin, OUTPUT);
}
void	IRSender::end() 
{
	pinMode(_pin, INPUT);
}

// Send a uint8_t (8 bits) over IR
void	IRSender::sendIRbyte(const uint8_t data, const bool MSBfirst, 
							 const uint16_t oneMarkLength, const uint16_t zeroMarkLength, 
							 const uint16_t oneSpaceLength, const uint16_t zeroSpaceLength)
{
	const uint8_t nbits = 8;

	uint8_t mask = 1;

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

void	IRSender::sendIRnbits(const uint64_t data, const uint16_t nbits, const bool MSBfirst,
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

void 	IRSender::sendIRraw(const std::vector <uint16_t>& vectorRaw) 
{
	uint16_t	lenght = vectorRaw.size();
	
	for (uint16_t i = 0; i < lenght; i++)
	{
		if ( i & 0x01 ) mark( vectorRaw.at(i) ); else space( vectorRaw.at(i) );
	}
	space(0);

}


// Definitions of virtual functions
void IRSender::setFrequency(const uint16_t) {};
void IRSender::space(const uint16_t) {};
void IRSender::mark(const uint16_t) {};