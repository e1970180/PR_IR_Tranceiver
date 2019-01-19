#pragma once

#include <Arduino.h>
#include <vector>


//virtual class
class IR_Protocol {
	protected:
		IR_Protocol(); // Cannot create generic IR_Protocol instances

	public:
		virtual bool	sendToRaw(std::vector <uint16_t>& vector);
		virtual bool	send(IRSender& sender);

		virtual bool	decodeRaw(const std::vector <uint16_t>& vector);

	protected:
    
};