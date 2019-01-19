#pragma once
/*
    Ballu heatpump control 

 */


#include "PR_IR_Sender.hpp"
#include "PR_IR_protocol_Ballu.h" 	
#include "utils\PR_IR_Transiver_Utils.hpp"



const float tolPerc = 0.15;	//

bool _decodeByte(const std::vector <uint16_t>& vectorRaw, const uint16_t startPos, const bool isMSBfirst, uint8_t& decodedByte);

class IR_Protocol_Ballu 
{
	public:
		//IR_Protocol_Ballu();
		
		void	setSender(IRSender& IRsender);
		//void	send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd);
			
		//bool	sendCommand(const TionCommand_t& tionCmd);
		bool	decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd);
		bool	code(std::vector <uint16_t>& vectorRaw, );
	
	private:
		//void sendTion(IRSender& IR, const TionCommand_t tionCmd);
	
		//IRSender* _irSender = nullptr;
		
		uint8_t		_crc(const std::array<uint8_t, BalluCmdLenght-3>, const uint16_t begin,  const uint16_t begin);
		
		std::array <uint8_t, BalluCmdLenght-3> 	_data;
};

//IR_Protocol_Ballu::IR_Protocol_Ballu() {}

void	IR_Protocol_Ballu::setSender(IRSender& IRsender) { _irSender = &IRsender; }

bool	IR_Protocol_Ballu::sendCommand(const TionCommand_t& tionCmd)
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

bool	IR_Protocol_Ballu::decode(const std::vector <uint16_t>& vectorRaw, TionCommand_t& tionCmd)
{
	vectorRaw.cbegin();
	uint16_t rawLen = vectorRaw.size();
	
	TionCommand_t	_tmpCmd;
//Serial.print("d: len=");Serial.println(rawLen);	
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
//Serial.println("d: len ok");
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




