#pragma once
/*
    Tion breather control 

 */

#include <HeatpumpIR.h>

#include <ir_NEC.h> 	//Tion protocol is NEC

typedef enum TionCommand: uint8_t {
	TION_REPEAT,
	TION_POWER,
	TION_SET,
	TION_UP,
	TION_DOWN
} TionCommand_t;



class TionBreatherIR : public HeatpumpIR
{
  public:
    TionBreatherIR();
    void	send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd);
	
	bool	decode(const &raw, TionCommand_t& tionCmd);
	
	
  private:
    void sendTion(IRSender& IR, const TionCommand_t tionCmd);
};

