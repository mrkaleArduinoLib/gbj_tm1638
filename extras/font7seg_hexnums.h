#ifndef GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_VERSION "GBJ_FONT7SEG_HEXNUMS 1.0.0"

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
// Digits
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
// Hexadecimal letters
, 0x41, 0b01110111 // A
, 0x42, 0b01111100 // B = b
, 0x43, 0b00111001 // C
, 0x44, 0b01011110 // D = d
, 0x45, 0b01111001 // E
, 0x46, 0b01110001 // F
, 0x61, 0b01110111 // a = A
, 0x62, 0b01111100 // b
, 0x63, 0b01011000 // c
, 0x64, 0b01011110 // d
, 0x65, 0b01111001 // e = E
, 0x66, 0b01110001 // f = F
};

#endif
