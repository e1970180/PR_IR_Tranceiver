#pragma once

#include <stdint.h>

//                           N   N  EEEEE   CCCC
//                           NN  N  E      C
//                           N N N  EEE    C
//                           N  NN  E      C
//                           N   N  EEEEE   CCCC

// NEC originally added from https://github.com/shirriff/Arduino-IRremote/

// Constants
// Ref:
//  http://www.sbprojects.com/knowledge/ir/nec.php

const uint16_t NecTick = 560;		//[uS]
//COMMAND

const uint16_t NecCmdLenght			= 67;						
//header
const uint16_t NecCmdHdrMarkTicks	= 16;
const uint16_t NecCmdHdrSpaceTicks	= 8;
//mark
const uint16_t NecOneMarkTicks 		= 1;
const uint16_t NecZeroMarkTicks 	= 1;
//space
const uint16_t NecOneSpaceTicks 	= 3;
const uint16_t NecZeroSpaceTicks 	= 1;

//REPEAT
const uint16_t NecRptLenght			= 3;
//header
const uint16_t NecRptHdrMarkTicks	= 16;
const uint16_t NecRptHdrSpaceTicks	= 4;

const bool	   NecMSBfirst 			= false;

const uint16_t NecCommandLength = 193;
const uint16_t NecRptLength = 3;



	 

