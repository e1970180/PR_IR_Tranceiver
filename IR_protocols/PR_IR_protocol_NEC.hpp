#pragma once

#include <stdint.h>
//#include "IRremoteESP8266.h"

//                           N   N  EEEEE   CCCC
//                           NN  N  E      C
//                           N N N  EEE    C
//                           N  NN  E      C
//                           N   N  EEEEE   CCCC

// NEC originally added from https://github.com/shirriff/Arduino-IRremote/

// Constants
// Ref:
//  http://www.sbprojects.com/knowledge/ir/nec.php
const uint16_t kNecTick = 560;									//[uS]
//header
const uint16_t kNecHdrMarkTicks		= 16;
const uint16_t kNecHdrSpaceTicks	= 8;
//mark
const uint16_t kNecOneMarkTicks 	= 1;
const uint16_t kNecZeroMarkTicks 	= 1;
//space
const uint16_t kNecOneSpaceTicks 	= 3;
const uint16_t kNecZeroSpaceTicks 	= 1;
//repeat
const uint16_t kNecRptMarkTicks		= 16;
const uint16_t kNecRptSpaceTicks 	= 4;
const uint16_t kNecRptLength 		= 4;

//header
const uint16_t kNecHdrMark		= kNecHdrMarkTicks * kNecTick;
const uint16_t kNecHdrSpace		= kNecHdrSpaceTicks * kNecTick;
//mark
const uint16_t kNecOneMark		= kNecOneMarkTicks * kNecTick;
const uint16_t kNecZeroMark		= kNecZeroMarkTicks * kNecTick;
//space
const uint16_t kNecOneSpace		= kNecOneSpaceTicks * kNecTick;
const uint16_t kNecZeroSpace	= kNecZeroSpaceTicks * kNecTick;
//repeat
const uint16_t kNecRptMark	 	= kNecRptMarkTicks * kNecTick;
const uint16_t kNecRptSpace 	= kNecRptSpaceTicks * kNecTick;


const uint16_t kNecMinCommandLengthTicks = 193;
const uint32_t kNecMinCommandLength = kNecMinCommandLengthTicks * kNecTick;
const uint32_t kNecMinGap = kNecMinCommandLength - (kNecHdrMark + kNecHdrSpace + kNECBits * (kNecBitMark + kNecOneSpace) + kNecBitMark);
const uint16_t kNecMinGapTicks =
    kNecMinCommandLengthTicks -
    (kNecHdrMarkTicks + kNecHdrSpaceTicks +
     kNECBits * (kNecBitMarkTicks + kNecOneSpaceTicks) + kNecBitMarkTicks);

	 

