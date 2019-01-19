#pragma once

#include <Arduino.h>
#include <vector>

#include <IR_protocols\PR_IR_protocol_NEC_timings.h>	//protocol parameters
#include "utils\PR_IR_Transiver_Utils.hpp"

typedef struct NEC_data {
	bool	isGood;	
	bool	isRepeat;
	bool	isExtendedVersion;
	uint8_t	addressLow;
	uint8_t	addressHigh;
	uint8_t	command;
	
} NEC_data_t;

const float tolPerc = 0.15;	//

class IR_protocol_NEC
{
	public:
		bool		sendToRaw(std::vector <uint16_t>& vector);
		bool		send(IRSender& sender);

		bool		decodeRaw(const std::vector <uint16_t>& vRaw);
		
		bool		isDecoded()		{ return Data.isGood; }
		
		NEC_data_t	Data;
		
	protected:
		bool		_decodeByte(const std::vector <uint16_t>& vRaw, const uint16_t startPos, const bool isMSBfirst, uint8_t& decodedByte);
		bool 		_pushEncodedByte(std::vector <uint16_t>& vRaw, const bool NecMSBfirst, const uint8_t b);
		void		_clearData();
};


bool	IR_protocol_NEC::decodeRaw(const std::vector <uint16_t>& vRaw)
{
	_clearData();

	uint16_t rawLen = vRaw.size();
//Serial.print("d: len=");Serial.println(rawLen);		
	switch (rawLen) {
		case NecRptLenght:
			Data.isRepeat = true;
		break;
		case NecCmdLenght:
			//do nothing
		break;	
		default:
			return false;
	}
//Serial.println("d: lengh ok");

	vRaw.cbegin();
	switch (Data.isRepeat) {
		case true:	//checking for Repeat sequence
		
			if ( !isMatch(vRaw.at(0), NecRptHdrMarkTicks*NecTick, tolPerc) ) return false;
			if ( !isMatch(vRaw.at(1), NecRptHdrSpaceTicks*NecTick, tolPerc) ) return false;
			
			if ( !isMatch(vRaw.at(2), NecOneMarkTicks*NecTick, tolPerc) ) return false;
			
			//isRepeat = true;		// set isRepeat as check length may be switched off

			break;
		case false:	//checking for Command sequence
		
			if ( !isMatch(vRaw.at(0), NecCmdHdrMarkTicks*NecTick, tolPerc) ) return false;
			if ( !isMatch(vRaw.at(1), NecCmdHdrSpaceTicks*NecTick, tolPerc) ) return false;

			if (!_decodeByte(vRaw, 2, NecMSBfirst, Data.addressLow)) 			return false;
			if (!_decodeByte(vRaw, 2+16, NecMSBfirst, Data.addressHigh)) 		return false;

			if (!_decodeByte(vRaw, 2+16+16, NecMSBfirst, Data.command)) 		return false;
			
			uint8_t	inversed;
			if (!_decodeByte(vRaw, 2+16+16+16, NecMSBfirst, inversed)) 			return false;					

			if ( Data.command != (uint8_t)(~inversed) ) return false;
			
			if ( Data.addressLow != (uint8_t)(~Data.addressHigh) ) Data.isExtendedVersion = true;	
			
			//uint16_t decoded = Data.addressLow;
			//decoded = (decoded << 8) + Data.command;
		} //switch 
	Data.isGood 	= true;
	return true;
}

bool IR_protocol_NEC::sendToRaw(std::vector <uint16_t>& vRaw) 
{
	if ( !Data.isGood ) return false;
	
	vRaw.clear();
	
	switch (Data.isRepeat) {
		case true:	//checking for Repeat sequence
		
			vRaw.push_back( NecRptHdrMark);
			vRaw.push_back( NecRptHdrSpace);
			//vRaw.push_back( NecOneMark);

			break;
		case false:	//checking for Command sequence
			

			vRaw.push_back( NecCmdHdrMark);
			vRaw.push_back( NecCmdHdrSpace);

			_pushEncodedByte(vRaw, NecMSBfirst, Data.addressLow);
			
			uint8_t tmp = Data.addressLow;
			if ( Data.isExtendedVersion ) tmp = ~tmp;
			_pushEncodedByte(vRaw, NecMSBfirst, tmp);
			
			_pushEncodedByte(vRaw, NecMSBfirst, Data.command);
			_pushEncodedByte(vRaw, NecMSBfirst, (uint8_t)(~Data.command));
				
		} 
	vRaw.push_back( NecOneMark);
	return true;
}

bool IR_protocol_NEC::send(IRSender& sender)
{
	if ( !Data.isGood ) return false;
	
	sender.begin();

	switch (Data.isRepeat) {
		case true:	//Repeat sequence
		
			sender.mark(NecRptHdrMark);
			sender.space(NecRptHdrSpace);
			//sender.mark(NecOneMark);
		break;
		case false:	//Command sequence
			
			uint16_t nbits = 32;	
			uint64_t dataComplex = Data.addressLow;
			dataComplex = (dataComplex << 8) + Data.addressHigh;
			dataComplex = (dataComplex << 8) + Data.command;
			dataComplex = (dataComplex << 8) + (uint8_t)(~Data.command);
			
			sender.mark(NecCmdHdrMark);
			sender.space(NecCmdHdrSpace);
			sender.sendIRnbits(dataComplex, nbits, false, NecOneMark, NecZeroMark, NecOneSpace, NecZeroSpace);				
	} 
	sender.mark(NecOneMark);
	sender.end();
	return true;
}

/////////	PROTECTED members

bool IR_protocol_NEC::_decodeByte(const std::vector <uint16_t>& vRaw, const uint16_t startPos, const bool isMSBfirst, uint8_t& decodedByte) 
{		
	uint8_t decoded = 0;

	for (uint16_t i = startPos ; i < (startPos + 16); i += 2 ) {
		
		if ( isMatch(vRaw.at(i),   NecOneMark, tolPerc) &&
			 isMatch(vRaw.at(i+1), NecOneSpace, tolPerc) )
			
			decoded = (decoded << 1) | 1;	//"1"
			
		else if ( isMatch(vRaw.at(i),   NecZeroMark, tolPerc) &&
				  isMatch(vRaw.at(i+1), NecZeroSpace, tolPerc) )
				  
				decoded = (decoded << 1);	//"0"
			
			else return false;
	}
	if (isMSBfirst) decoded = bitReverse(decoded);
	
	decodedByte = decoded;
	return true;
}

bool IR_protocol_NEC::_pushEncodedByte(std::vector <uint16_t>& vRaw, const bool MSBfirst, const uint8_t val)
{	
	const uint8_t nbits = 8;
	uint8_t mask = 1;

	if (MSBfirst) mask <<= (nbits-1);	// Send the MSB first.

	for (uint8_t i = 0; i < nbits ; i++) {
		if (val & mask)
		{
			vRaw.push_back( NecOneMark );
			vRaw.push_back( NecOneSpace );
		}
		else
		{
			vRaw.push_back( NecZeroMark );
			vRaw.push_back( NecZeroSpace );
		}
		MSBfirst ? mask >>= 1 :  mask <<= 1;
	}
}

void IR_protocol_NEC::_clearData() {
	Data.isGood 	= false;
	Data.isRepeat 	= false;
	Data.isExtendedVersion = false;
	Data.addressLow 	= 0;
	Data.addressHigh = 0;
	Data.command 	= 0;
}