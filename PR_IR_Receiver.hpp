 #pragma once

#include <Arduino.h>
#include <vector>

#define	IR_BUFFER_SIZE	1000

// receiver states
typedef enum IR_ReceiverState: int8_t {
	IR_R_OVERLENGH	=-1,
	IR_R_RECEIVED 	= 0,
	IR_R_WAIT 		= 1,
	IR_R_RECEIVING 	= 2,
	IR_R_STOP		= 3
} IR_ReceiverState_t;


//Base class
class IR_Receiver {
	protected:
  		explicit IR_Receiver(const uint16_t recvpin, const uint8_t timeoutMS);
		
	public:
		virtual void	begin();
		virtual void	end();
			
		void			setOnReceivedCallback( void (*func)(void) );
		void 			loop();
		
		virtual IR_ReceiverState_t	getState();

		virtual bool getReceived(std::vector <uint16_t>& vector);

		
	protected:
		uint8_t			_pin;   	// pin for IR data from detector
		
		void 	(*_onReceivedCallback)(void) = nullptr;
};



IR_Receiver::IR_Receiver(const uint16_t recvpin, const uint8_t timeoutMS) : _pin(recvpin) {}

void IR_Receiver::loop() {
	
	if ( getState() != IR_R_RECEIVED ) return;
	
	if ( _onReceivedCallback != nullptr) _onReceivedCallback();
	begin();	//re-arm receiver
}

void	IR_Receiver::setOnReceivedCallback( void (*func)(void) ) { _onReceivedCallback = func;	}





