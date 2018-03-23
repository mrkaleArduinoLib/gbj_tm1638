#ifndef GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_VERSION "GBJ_FONT7SEG_DECNUMS 1.0.0"

#if defined(__AVR__)
	#include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

const uint8_t gbjFont7segTable[] PROGMEM =
{
  // ASCII code, Font mask
  0x20, 0b00000000 // Space
, 0x2d, 0b01000000 // -
, 0x30, 0b00111111 // 0
, 0x31, 0b00000110 // 1
, 0x32, 0b01011011 // 2
, 0x33, 0b01001111 // 3
, 0x34, 0b01100110 // 4
, 0x35, 0b01101101 // 5
, 0x36, 0b01111101 // 6
, 0x37, 0b00000111 // 7
, 0x38, 0b01111111 // 8
, 0x39, 0b01101111 // 9
};

#endif
