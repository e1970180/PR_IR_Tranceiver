#pragma once

#include <Arduino.h>

//Base class
class IR_Receiver {
	protected:
  		explicit IR_Receiver(const uint16_t recvpin);
		
	public:
		virtual void	begin(uint16_t *bufer, const uint16_t bufsize, const uint8_t timeoutMS);
		virtual void	end();
			
		void			setOnReceivedCallback( void (*func)(void) );
		
		virtual bool			isStoped();

	protected:
		uint8_t		_pin;   	// pin for IR data from detector
		
		void 	(*_onReceivedCallback)(void) = nullptr;
};

//if ( _onDblClickCallback != nullptr) _onDblClickCallback();

IR_Receiver::IR_Receiver(const uint16_t recvpin) : _pin(recvpin) {}

void	IR_Receiver::setOnReceivedCallback( void (*func)(void) ) { _onReceivedCallback = func;	}





