#pragma once

#include <Arduino.h>
#include <vector>

#include <PR_IR_protocol_NEC.h>	//protocol parameters



class IRSenderNEC : public IR_Protocol 
{
	protected:
		IRSenderNEC(); // Cannot create generic IR_Protocol instances

	public:
		virtual bool	sendToRaw(std::vector <uint16_t>& vector);
		virtual bool	send(IRSender& sender);

		virtual bool	decodeRaw(const std::vector <uint16_t>& vector);

	protected:
    
};


bool	IRSenderNEC::sendToRaw(std::vector <uint16_t>& vector)
{
	
}
bool	IRSenderNEC::send(IRSender& sender)
{
	
}

bool	IRSenderNEC::decodeRaw(const std::vector <uint16_t>& vector)
{
	
}