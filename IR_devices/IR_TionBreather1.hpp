#pragma once
/*
    Tion breather control 

 */


#include "PR_IR_Sender.hpp"
#include "IR_protocols\PR_IR_protocol_NEC.hpp" 	//Tion protocol is NEC

#include "utils\PR_IR_Transiver_Utils.hpp"

typedef enum TionCommand: uint8_t {
	CMD_TION_REPEAT = 1,
	CMD_TION_POWER= 2,
	CMD_TION_SET= 3,
	CMD_TION_UP= 4,
	CMD_TION_DOWN= 5,
	CMD_TION_UNKNOWN= 6
} TionCommand_t;

const uint8_t TionAaddress = 0;

const uint8_t TionCmdPower	= 0b00100000;
const uint8_t TionCmdSet	= 0b00101000;
const uint8_t TionCmdUp		= 0b00110000;
const uint8_t TionCmdDown	= 0b00000000;


class IR_Device_TionBreather : public IR_protocol_NEC
{
	public:
		
		//void	send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd);
			
		bool	sendCommand(const TionCommand_t& tionCmd);
		bool	decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd);
	
	private:
		void sendTion(IRSender& IR, const TionCommand_t tionCmd);
	
};


bool	IR_Device_TionBreather::decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd)
{
	IR_protocol_NEC::decodeRaw(vectorRaw);	//decode by protocol
	
	if (! isDecoded() ) return false;
	
	if ( Data.isRepeat ) tionCmd = CMD_TION_REPEAT;
	else 
	{
		if ( Data.isExtendedVersion ) return false; //Tion use standard NEC 
		
		if ( Data.addressLow != TionAaddress ) return false;
		
		switch (Data.command)
		{
			case TionCmdPower:
				tionCmd = CMD_TION_POWER;
			break;
			case TionCmdSet:
				tionCmd = CMD_TION_SET;
			break;
			case TionCmdUp:
				tionCmd = CMD_TION_UP;
			break;
			case TionCmdDown:
				tionCmd = CMD_TION_DOWN;
			break;			
			default:
			break;
		}
		return true;
	}
}
