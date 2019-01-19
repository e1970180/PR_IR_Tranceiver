#pragma once


// See http://www.nrtm.org/index.php/2013/07/25/reverse-bits-in-a-uint8_t/
uint8_t bitReverse(const uint8_t x)
{
	uint8_t y = x;
  //          01010101  |         10101010
  y = ((y >> 1) & 0x55) | ((y << 1) & 0xaa);
  //          00110011  |         11001100
  y = ((y >> 2) & 0x33) | ((y << 2) & 0xcc);
  //          00001111  |         11110000
  y = ((y >> 4) & 0x0f) | ((y << 4) & 0xf0);
  return y;
}

bool	isMatch(const uint16_t received, const uint16_t expected, const float tolerancePerc) 
{
	uint16_t delta;
	if (received > expected) delta = received-expected; else delta = expected-received; 
	return (delta <= (expected * tolerancePerc));
}
