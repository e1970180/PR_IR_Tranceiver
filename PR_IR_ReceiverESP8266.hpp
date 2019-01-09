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

// receiver states
enum ReceiveState: uint8_t {
	
	kIdleState 	= 2,
	kMarkState 	= 3,
	kSpaceState = 4,
	kStopState 	= 5,
} ReceiveState_t;

// information for the interrupt handler
typedef struct {
	uint8_t 	timeoutMS;   	// Nr. of milliSeconds before we give up.
	uint8_t 	rcvstate;  	// state machine
	uint16_t 	bufsize;  	// max. nr. of entries in the capture buffer.
	uint16_t 	*rawbuf;  	// raw data
							// uint16_t is used for rawlen as it saves 3 bytes of iram in the interrupt
							// handler. Don't ask why, I don't know. It just does.

	//bool	 	overflow;  	// Buffer overflow indicator.

} ISRparams_t;


// Globals

static ETSTimer 		timer;

volatile ISRparams_t	isr_params;


class IRreceiver_ESP8266 : public IR_Receiver {
	public:
		explicit IRreceiver_ESP8266(const uint16_t recvpin);

		virtual void		begin(uint16_t *bufer, const uint16_t bufsize, const uint8_t timeoutMS);
		virtual void		end();
		void				resumeIsrParams();
		
		bool				isStoped() 		{ return (isr_params.rcvstate == kStopState); }

	private:

};

IRreceiver_ESP8266::IRreceiver_ESP8266(const uint16_t recvpin) : IR_Receiver(recvpin)_pin(recvpin) {}

static void ICACHE_RAM_ATTR read_timeout(void *arg __attribute__((unused))) {
	os_intr_lock();

	if ( isr_params.rawbuf[0] ) isr_params.rcvstate = kStopState;	
	
	os_intr_unlock();
}

static void ICACHE_RAM_ATTR gpio_intr() {
	
	uint32_t	now = system_get_time();
	uint32_t	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	os_timer_disarm(&timer);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

	static uint32_t start = 0;

	// Grab a local copy of rawlen to reduce instructions used in IRAM.
	// This is an ugly premature optimisation code-wise, but we do everything we can to save IRAM.
	// It seems referencing the value via the structure uses more instructions.
	// Less instructions means faster and less IRAM used.
	// N.B. It saves about 13 bytes of IRAM.
	
	//uint16_t rawlen = isr_params.rawlen;
	
	uint16_t rawlen = isr_params.rawbuf[0] + 1;
	
	if (rawlen >= isr_params.bufsize) {
		//isr_params.overflow = true;
		isr_params.rcvstate = kStopState;
	}

	if (isr_params.rcvstate == kStopState) return;

	if (isr_params.rcvstate == kIdleState) {
		isr_params.rcvstate = kMarkState;
	} 
	else {
		//isr_params.rawlen++;
		isr_params.rawbuf[0]++;
		
		if (now < start)
			isr_params.rawbuf[rawlen] = (UINT32_MAX - start + now);
		else
			isr_params.rawbuf[rawlen] = (now - start);
	}
	start = now;
#define ONCE 0
	os_timer_arm(&timer, isr_params.timeoutMS, ONCE);
}



//   bufsize: Nr. of entries to have in the capture buffer.
//   timeoutMS: Nr. of milli-Seconds of no signal before we stop capturing data.  
void IRreceiver_ESP8266::begin(uint16_t *bufer, const uint16_t bufsize, const uint8_t timeoutMS) {

	//pinMode(_pin, INPUT); //TODO: does it necessary???
	
	isr_params.bufsize = bufsize;	  // Ensure we are going to be able to store all possible values in the capture buffer.
	isr_params.rawbuf = bufer;
	
	isr_params.timeoutMS = std::min(timeoutMS, (uint8_t)(UINT16_MAX/1000) );	// [ms] uint8_t could store only (UINT16_MAX/1000) microSec 
	
	resumeIsrParams();  // initialize state machine variables

	// Initialize timer
	os_timer_disarm(&timer);
	os_timer_setfn(&timer, reinterpret_cast<os_timer_func_t *>(read_timeout), NULL);

	attachInterrupt(_pin, gpio_intr, CHANGE);
}

void IRreceiver_ESP8266::end() {

	os_timer_disarm(&timer);
	detachInterrupt(_pin);
}

void IRreceiver_ESP8266::resumeIsrParams() {
	
	isr_params.rcvstate 	= kIdleState;

	isr_params.rawbuf[0]	= 0; // using [0] to store length of received 
	
  	//isr_params.overflow 	= false;
}



