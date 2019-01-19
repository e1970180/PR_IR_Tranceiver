#pragma once
/*
    Tion breather control 

 */


#include "PR_IR_Sender.hpp"
#include "IR_protocols\PR_IR_protocol_NEC.h" 	//Tion protocol is NEC
#include "utils\PR_IR_Transiver_Utils.hpp"

typedef enum TionCommand: uint8_t {
	CMD_TION_REPEAT = 1,
	CMD_TION_POWER= 2,
	CMD_TION_SET= 3,
	CMD_TION_UP= 4,
	CMD_TION_DOWN= 5,
	CMD_TION_UNKNOWN= 6
} TionCommand_t;

const uint16_t TionPower = 	0b0000000000100000;
const uint16_t TionSet = 	0b0000000000101000;
const uint16_t TionUp = 	0b0000000000110000;
const uint16_t TionDown = 	0b0000000000000000;
const uint8_t TionRepeat = 1;

const float tolPerc = 0.15;	//

bool _decodeByte(const std::vector <uint16_t>& vectorRaw, const uint16_t startPos, const bool isMSBfirst, uint8_t& decodedByte);

class IR_Device_TionBreather 
{
	public:
		//IR_Device_TionBreather();
		
		void	setSender(IRSender& IRsender);
		//void	send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd);
			
		bool	sendCommand(const TionCommand_t& tionCmd);
		bool	decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd);
	
	private:
		void sendTion(IRSender& IR, const TionCommand_t tionCmd);
	
		IRSender* _irSender = nullptr;
};

//IR_Device_TionBreather::IR_Device_TionBreather() {}

void	IR_Device_TionBreather::setSender(IRSender& IRsender) { _irSender = &IRsender; }

bool	IR_Device_TionBreather::sendCommand(const TionCommand_t& tionCmd)
{
	if ( _irSender == nullptr ) return false;
	
	uint32_t	data;
	uint16_t	nbits;
	switch (tionCmd) 
	{
		case CMD_TION_REPEAT:
			data = TionRepeat;
			nbits = 1;
		break;
		case CMD_TION_POWER:
			data = TionPower;
			nbits = 32;		
		break;		
		case CMD_TION_SET:
			data = TionSet;
			nbits = 32;		
		break;		
		case CMD_TION_UP:
			data = TionUp;
			nbits = 32;		
		break;		
		case CMD_TION_DOWN:
			data = TionDown;
			nbits = 32;		
		break;			
	}
	_irSender->begin();
	
	if (tionCmd == CMD_TION_REPEAT) 
	{
		_irSender->mark(NecRptHdrMarkTicks*NecTick);
		_irSender->space(NecRptHdrSpaceTicks*NecTick);
	}
	else
	{
		_irSender->mark(NecCmdHdrMarkTicks*NecTick);
		_irSender->space(NecCmdHdrSpaceTicks*NecTick);
	}
	_irSender->sendIRnbits(data, nbits, NecMSBfirst, NecOneMarkTicks*NecTick, NecZeroMarkTicks*NecTick, NecOneSpaceTicks*NecTick, NecZeroSpaceTicks*NecTick);

	_irSender->end();
}

bool	IR_Device_TionBreather::decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd)
{
	vectorRaw.cbegin();
	uint16_t rawLen = vectorRaw.size();
	
	TionCommand_t	_tmpCmd;
Serial.print("d: len=");Serial.println(rawLen);	
	switch (rawLen) {
		case NecRptLenght:
			_tmpCmd = CMD_TION_REPEAT;
		break;
		case NecCmdLenght:
			_tmpCmd = CMD_TION_UNKNOWN;
		break;	
		default:
			return false;
	}
Serial.println("d: len ok");
	switch (_tmpCmd) {
		case CMD_TION_REPEAT:
		
			if ( !isMatch(vectorRaw.at(0), NecRptHdrMarkTicks*NecTick, tolPerc) ) return false;
			if ( !isMatch(vectorRaw.at(1), NecRptHdrSpaceTicks*NecTick, tolPerc) ) return false;
			
			if ( !isMatch(vectorRaw.at(2), NecOneMarkTicks*NecTick, tolPerc) ) return false;
			
			tionCmd = _tmpCmd;

			break;
		case CMD_TION_UNKNOWN:
		
			if ( !isMatch(vectorRaw.at(0), NecCmdHdrMarkTicks*NecTick, tolPerc) ) return false;
			if ( !isMatch(vectorRaw.at(1), NecCmdHdrSpaceTicks*NecTick, tolPerc) ) return false;
 
			uint8_t addressLow = 0;
			uint8_t addressHigh = 0;
			uint8_t command = 0;
			uint8_t commandInv = 0;

			if (!_decodeByte(vectorRaw, 2, NecMSBfirst, addressLow)) return false;
			if (!_decodeByte(vectorRaw, 2+16, NecMSBfirst, addressHigh)) return false;

			if (!_decodeByte(vectorRaw, 2+16+16, NecMSBfirst, command)) return false;		
			if (!_decodeByte(vectorRaw, 2+16+16+16, NecMSBfirst, commandInv)) return false;					

			if ( command != (uint8_t)(~commandInv) ) return false;
			
			if ( addressLow != (uint8_t)(~addressHigh) ) return false;	//if different it is ExtendedNEC protocol, but not the Tion case
			
			uint16_t decoded = addressLow;
			decoded = (decoded << 8) + command;
		
			switch (decoded) {
				case TionPower:
					tionCmd = CMD_TION_POWER;
				break;
				case TionSet:
					tionCmd = CMD_TION_SET;
				break;
				case TionUp:
					tionCmd = CMD_TION_UP;
				break;
				case TionDown:
					tionCmd = CMD_TION_DOWN;
				break;				
				default:
					return false;
			} //switch (decoded)
	} //switch (_tmpCmd)
	
	return true;
}

bool _decodeByte(const std::vector <uint16_t>& vectorRaw, const uint16_t startPos, const bool isMSBfirst, uint8_t& decodedByte) {
		
	uint8_t data = 0;

	for (uint16_t i = startPos ; i < (startPos + 16); i += 2 ) {
		
		if ( isMatch(vectorRaw.at(i), NecOneMarkTicks*NecTick, tolPerc) &&
			 isMatch(vectorRaw.at(i+1), NecOneSpaceTicks*NecTick, tolPerc) )
			
			data = (data << 1) | 1;	//"1"
			
		else if ( isMatch(vectorRaw.at(i), NecZeroMarkTicks*NecTick, tolPerc) &&
				  isMatch(vectorRaw.at(i+1), NecZeroSpaceTicks*NecTick, tolPerc) )
				  
				data = (data << 1);	//"0"
			
			else return false;
	}
	if (isMSBfirst) data = bitReverse(data);
	
	decodedByte = data;
	return true;
}




