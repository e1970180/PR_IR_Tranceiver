#pragma once

#include <stdint.h>

const uint16_t BalluTick = 1;		//[uS]
//COMMAND

const uint16_t BalluCmdLenght			= 227;						
//header
const uint16_t BalluCmdHdrMarkTicks		= 2960;
const uint16_t BalluCmdHdrSpaceTicks	= 1750;
//mark
const uint16_t BalluOneMarkTicks 		= 407;
const uint16_t BalluZeroMarkTicks 		= BalluOneMarkTicks;
//space
const uint16_t BalluOneSpaceTicks 		= BalluOneMarkTicks;
const uint16_t BalluZeroSpaceTicks 		= 1150;




	 

