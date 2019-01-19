#pragma once

#include <PR_IR_Receiver.hpp>

#include <Arduino.h>
#include <algorithm>

#include <stddef.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>



extern "C" {
	#include <gpio.h>
	#include <user_interface.h>
}



// information for the interrupt handler
typedef struct {
	uint8_t 			timeoutMS;  	// Nr. of milliSeconds before we give up.
	IR_ReceiverState_t 	rcvstate;  		// state machine //TODO check if IR_ReceiverState_t increase IRAM
	uint16_t 			bufsize;  		// max. nr. of entries in the capture buffer.
	uint16_t 			received_lengh; // 	
	uint16_t 			*rawbuf;  		// raw data
								// uint16_t is used for rawlen as it saves 3 bytes of iram in the interrupt handler. Don't ask why, I don't know. It just does.
} ISRparams_t;

//******************** Globals

static ETSTimer 		timer;
volatile ISRparams_t	isrParams;
uint16_t				rawData[IR_BUFFER_SIZE];

void isrParams_resume();


class IRreceiver_ESP8266 : public IR_Receiver {
	public:
		explicit IRreceiver_ESP8266(const uint16_t recvpin, const uint8_t timeoutMS);

		virtual void		begin();
		virtual void		end();

		virtual IR_ReceiverState_t	getState() 	{ return isrParams.rcvstate; }
		
		bool 	getReceived(std::vector <uint16_t>& output);
	
	private:

};

static void ICACHE_RAM_ATTR read_timeout(void *arg __attribute__((unused))) {
	os_intr_lock();

	//if nothing was received yet  or if rcvstate == IR_R_OVERLENGH just continue
	//if something already got (mean received_lengh >0 ) then  rcvstate = IR_R_RECEIVED
	//if ( isrParams.received_lengh && (isrParams.rcvstate != IR_R_OVERLENGH ) ) isrParams.rcvstate = IR_R_RECEIVED;	

	if ( isrParams.rcvstate == IR_R_RECEIVING ) isrParams.rcvstate = IR_R_RECEIVED;	
	
	os_intr_unlock();
}

static void ICACHE_RAM_ATTR gpio_intr() {
	
	uint32_t	now = system_get_time();
	uint32_t	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	os_timer_disarm(&timer);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

	static uint32_t start = 0;

	uint16_t received = isrParams.received_lengh; // Grab a local copy of received to reduce instructions used in IRAM.  It saves about 13 bytes of IRAM.
	
	switch (isrParams.rcvstate) {
		case IR_R_WAIT:
			isrParams.rcvstate = IR_R_RECEIVING;
		break;
		case IR_R_RECEIVING: {

			uint32_t t = now - start;	
			if (now < start) t += UINT32_MAX;
			isrParams.rawbuf[received] = t;

		//if (now < start)
		//	isrParams.rawbuf[received] = (UINT32_MAX - start + now);
		//else
		//	isrParams.rawbuf[received] = (now - start);

			received++;
			isrParams.received_lengh = received;
			
			if (received >= isrParams.bufsize) isrParams.rcvstate = IR_R_OVERLENGH;			
		break;
		}
		default:		 //IR_R_OVERLENGH, IR_R_RECEIVED, IR_R_STOP
			return;	
		break;	
	}
	start = now;
	#define ONCE 0
	os_timer_arm(&timer, isrParams.timeoutMS, ONCE);
}


IRreceiver_ESP8266::IRreceiver_ESP8266(const uint16_t recvpin, const uint8_t timeoutMS) : IR_Receiver(recvpin, timeoutMS) {
	
	isrParams.bufsize = IR_BUFFER_SIZE;	  
	isrParams.rawbuf  = rawData;
	
	//isrParams.timeoutMS = std::min(timeoutMS, (uint8_t)(UINT16_MAX/1000) );	// [ms] uint8_t could store only (UINT16_MAX/1000) microSec 
	isrParams.timeoutMS = timeoutMS;
	isrParams.rcvstate = IR_R_STOP;
}


//   bufsize: Nr. of entries to have in the capture buffer.
//   timeoutMS: Nr. of milli-Seconds of no signal before we stop capturing data.  
void IRreceiver_ESP8266::begin() {

	//pinMode(_pin, INPUT); //TODO: does it necessary???

	isrParams_resume();  // initialize state machine variables

	os_timer_disarm(&timer);
	os_timer_setfn(&timer, reinterpret_cast<os_timer_func_t *>(read_timeout), NULL);

	attachInterrupt(_pin, gpio_intr, CHANGE);
}

void IRreceiver_ESP8266::end() {

	os_timer_disarm(&timer);
	detachInterrupt(_pin);
	isrParams.rcvstate = IR_R_STOP; 
}

void isrParams_resume() {

	isrParams.rcvstate 			= IR_R_WAIT;
	isrParams.received_lengh 	= 0;
	
	uint16_t len = isrParams.bufsize;			//using local var to reduce size 
	for (uint16_t i = 0; i < len ; isrParams.rawbuf[i++] = 0 );	//clear buffer	
	
//	for (uint16_t i = 0; i < len ; i++ ) {	//clear buffer	
//		isrParams.rawbuf[i] = 0;		
//	}
}

bool IRreceiver_ESP8266::getReceived(std::vector <uint16_t>& output) {
	
	if ( isrParams.rcvstate != IR_R_RECEIVED ) return false; 
	else {
		output.clear();
		uint16_t len = isrParams.received_lengh;			//using local var to reduce size 
		for (uint16_t i = 0; i < len ; output.push_back(isrParams.rawbuf[i++]) );
	}
	return true;	
}