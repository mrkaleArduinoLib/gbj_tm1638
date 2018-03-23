/*
  NAME:
  gbj_tm1638

  DESCRIPTION:
  Library for 8 digital 7-segment tubes, 8 two-color LEDs, and 8 keys on a display
  module controlled by the driver TM1638.
  - The library controls the driver as a state machine with screen buffer in the
    microcontroller's operating memory, which is transmitted to the controller
    for displaying.
    - Screen buffer is considered as an image of controller's graphical memory.
    - Graphical library methods (prefixed with "print") performs all graphical
      manipulation in the screen buffer, which state reflects the desired image
      for display.
    - Finally the dedicated method transmits the content of the screen buffer
      to the driver and it causes to display the image on the attached digital
      tubes display and LEDs.
  - The driver TM1638 can control up to 8 digital 7-segment tubes, 8 two-color
    LEDs, and 24 keys.
  - The library controls 7-segment glyphs (digits) independently from radix 8th
    segments of glyphs.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_tm1638.git
 */
#ifndef GBJ_TM1638_H
#define GBJ_TM1638_H
#define GBJ_TM1638_VERSION "GBJ_TM1638 1.0.0"

#if defined(__AVR__)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
  #include <inttypes.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif

// Result and error codes
#define GBJ_TM1638_SUCCESS    0
#define GBJ_TM1638_ERR_PINS   255
#define GBJ_TM1638_ERR_ACK    254


class gbj_tm1638 : public Print
{
public:
//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------
/*
  Initialize display geometry

  DESCRIPTION:
  The constructor methodsanitizes and stores physical features of the display
  to the class instance object.

  PARAMETERS:
  pinClk - Microcontroller pin's number utilized as a serial clock.
           - Data type: non-negative integer
           - Default value: 2
           - Limited range: 0 ~ 255 (by microcontroller datasheet)

  pinDio - Microcontroller pin's number utilized as a data input and output.
           - Data type: non-negative integer
           - Default value: 3
           - Limited range: 0 ~ 255 (by microcontroller datasheet)

  pinStb - Microcontroller pin's number utilized as a strobe (chip select).
           Potentially it might be an array for more controllers.
          - Data type: non-negative integer
          - Default value: 4
          - Limited range: 0 ~ 255 (by microcontroller datasheet)

  RETURN:
  Result code.
*/
gbj_tm1638(uint8_t pinClk = 2, uint8_t pinDio = 3, uint8_t pinStb = 4);


/*
  Initialize display

  DESCRIPTION:
  The method sets the microcontroller's pins dedicated for the driver and perfoms
  initial sequence recommended by the data sheet for the controller.
  - It clears the display and sets it to the normal operating mode.

  PARAMETERS: none

  RETURN:
  Result code.
*/
uint8_t begin();


/*
  Transmit screen buffer to driver

  DESCRIPTION:
  The method transmits current content of the screen buffer to the driver, so that
  it content is displayed immediatelly and stays unchanged until another transmission.
  - The method utilizes automatic addressing mode of the driver.

  PARAMETERS: none

  RETURN:
  Result code.
*/
uint8_t display();


/*
  Manipulate digital tubes' radixes of a display module

  DESCRIPTION:
  The particular method performs corresponding manipulation with radix segment
  (usually 8th one) of particular glyph without influence on its glyph segments
  (first 7 segments) and on LEDs in the screen buffer.

  PARAMETERS:
  grid - Driver's digit tube number counting from 0, which radix segment
         should be manipulated.
         - Data type: non-negative integer
         - Default value: 0
         - Limited range: 0 ~ 7

  RETURN: none
*/
inline void printRadixOn(uint8_t grid = 0) { if (grid < DIGITS) _print.buffer[addrGrid(grid)] |= 0x80; };
inline void printRadixOff(uint8_t grid = 0) { if (grid < DIGITS) _print.buffer[addrGrid(grid)] &= ~0x80; };
inline void printRadixToggle(uint8_t grid = 0) { if (grid < DIGITS) _print.buffer[addrGrid(grid)] ^= 0x80; };
inline void printRadixOnAll() { for (uint8_t grid = 0; grid < DIGITS; grid++) printRadixOn(grid); };
inline void printRadixOffAll() { for (uint8_t grid = 0; grid < DIGITS; grid++) printRadixOff(grid); };
inline void printRadixToggleAll() { for (uint8_t grid = 0; grid < DIGITS; grid++) printRadixToggle(grid); };


/*
  Manipulate red LEDs of a display module

  DESCRIPTION:
  The particular method performs corresponding manipulation with red LEDs without
  influence on green LEDs and digital tubes in the screen buffer.

  PARAMETERS:
  led - Driver's red LED number counting from 0.
        - Data type: non-negative integer
        - Default value: 0
        - Limited range: 0 ~ 7

  RETURN: none
*/
inline void printLedRedOn(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] |= LED_RED; };
inline void printLedRedOff(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] &= ~LED_RED; };
inline void printLedRedToggle(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] ^= LED_RED; };
inline void printLedRedOnAll() { for (uint8_t led = 0; led < LEDS; led++) printLedRedOn(led); };
inline void printLedRedOffAll() { for (uint8_t led = 0; led < LEDS; led++) printLedRedOff(led); };
inline void printLedRedToggleAll() { for (uint8_t led = 0; led < LEDS; led++) printLedRedToggle(led); };


/*
  Manipulate green LEDs of the display module

  DESCRIPTION:
  The particular method performs corresponding manipulation with green LEDs
  without influence on red LEDs and digital tubes in the screen buffer.
  - Some display modules does not have green LEDs present.

  PARAMETERS:
  led - Driver's green LED number counting from 0.
        - Data type: non-negative integer
        - Default value: 0
        - Limited range: 0 ~ 7

  RETURN: none
*/
inline void printLedGreenOn(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] |= LED_GREEN; };
inline void printLedGreenOff(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] &= ~LED_GREEN; };
inline void printLedGreenToggle(uint8_t led = 0) { if (led < LEDS) _print.buffer[addrLed(led)] ^= LED_GREEN; };
inline void printLedGreenOnAll() { for (uint8_t led = 0; led < LEDS; led++) printLedGreenOn(led); };
inline void printLedGreenOffAll() { for (uint8_t led = 0; led < LEDS; led++) printLedGreenOff(led); };
inline void printLedGreenToggleAll() { for (uint8_t led = 0; led < LEDS; led++) printLedGreenToggle(led); };


/*
  Manipulate all LEDs of the display module at once

  DESCRIPTION:
  The particular method performs corresponding manipulation with all LEDs
  without influence digital tubes in the screen buffer.

  PARAMETERS:
  led - Driver's LED number counting from 0.
        - Data type: non-negative integer
        - Default value: 0
        - Limited range: 0 ~ 7

  RETURN: none
*/
inline void printLedAllOff() { printLedRedOffAll(); printLedGreenOffAll(); };


/*
  Manipulate grid segments

  DESCRIPTION:
  The particular method sets glyph segments (first 7 ones) of particular grid
  (digital tube) without influence on its radix segment in the screen buffer.

  PARAMETERS:
  grid - Driver's grid (digital tube) number counting from 0, which glyph
         segments should be manipulated.
         - Data type: non-negative integer
         - Default value: 0
         - Limited range: 0 ~ 7

  segmentMask - Bit mask defining what segments should be turned on. Segments
                are marked starting from A to G and relate to mask bits 0 to 6
                counting from least significant bit. The 7th bit relates to radix
                segment and therefore it is ignored.
               - Data type: non-negative integer
               - Default value: 0xFF (all glyph segments turned on)
               - Limited range: 0 ~ 127

  RETURN: none
*/
inline void printGrid(uint8_t grid = 0, uint8_t segmentMask = 0x7F) { if (grid < DIGITS) gridWrite(segmentMask, grid, grid); };
inline void printGridOn(uint8_t grid = 0) { if (grid < DIGITS) gridWrite(0x7F, grid, grid); };
inline void printGridOff(uint8_t grid = 0) { if (grid < DIGITS) gridWrite(0x00, grid, grid); };
inline void printGridOnAll() { gridWrite(0x7F); };
inline void printGridOffAll() { gridWrite(0x00); };

/*
  Print class inheritance

  DESCRIPTION:
  The library inherits the system Print class, so that all regular print
  functions can be used.
  - Actually all print functions eventually call one of listed write methods,
    so that all of them should be implemented.
  - If some character (ASCII) code is not present in the font table, i.e., it is
    unknown for the library, that character is ignored and not displayed.
  - If unknown character has ASCII code of comma, dot, or colon, the library
    turns on the radix segments of the recently displayed digit (lastly manipulated
    grid). Thus, the decimal points or colon can be present in displayed string
    at proper position and does not need to be control separately.

  PARAMETERS:
  ascii - ASCII code of a character that should be displayed at the current grid
          position. The methods is usually utilized internally by system prints.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: 0 ~ 255

  text - Pointer to a nul terminated string that should be displayed from the
         very beginnng of the display, i.e., from the first digit.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: microcontroller addressing range

  buffer - Pointer to a string, which part should be displayed from the very
           beginnng of the display, i.e., from the first digit.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: microcontroller addressing range

  size - Number of characters that should be displayed from the very beginnng of
         the display, i.e., from the first digit.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: microcontroller addressing range
*/
size_t write(uint8_t ascii);
size_t write(const char* text);
size_t write(const uint8_t* buffer, size_t size);


//------------------------------------------------------------------------------
// Public setters - they usually return result code.
//------------------------------------------------------------------------------
inline void initLastResult() { _status.lastResult = GBJ_TM1638_SUCCESS; };
inline uint8_t setLastResult(uint8_t lastResult = GBJ_TM1638_SUCCESS) { return _status.lastResult = lastResult; };
uint8_t setContrastControl(uint8_t contrast = 3);


/*
  Define font parameters for printing

  DESCRIPTION:
  The method gathers font parameters for printing characters on 7-segment displays.
  - Font definition is usually included to an application sketch from particular
    include file, while the font table resides in programmatic (flash) memory of
    a microcontroller in order to save operational memory (SRAM).
  - Each glyph of a font consists of the pair of bytes. The first byte determines
    ASCII code of a glyph and second byte determines segment mask of a glyph. It
    allows to defined only displayable glyphs on 7-segment displays and suppress
    need to waste memory for useless characters.

  PARAMETERS:
  fontTable - Pointer to a font definition table.
              - Data type: non-negative integer
              - Default value: none
              - Limited range: microcontroller addressing range

  fontTableSize - The number of bytes that should be utilized from the font
                  table.
                  - The table size in conjunction with font character pair of bytes
                    determines the number of characters used for printing.
                  - The size can be smaller than the real size of the table,
                    however, the size should be a multiple of 2.
                  - Data type: non-negative integer
                  - Default value: none
                  - Limited range: 0 ~ 255 (maximal 127 7-segments characters)

  RETURN: none
*/
void setFont(const uint8_t* fontTable, uint8_t fontTableSize);


//------------------------------------------------------------------------------
// Public getters
//------------------------------------------------------------------------------
inline uint8_t getLastResult() { return _status.lastResult; }; // Result of a recent operation
inline uint8_t getLastCommand() { return _status.lastCommand; }; // Command code of a recent operation
inline uint8_t getGrids() { return DIGITS; }; // Digital tubes for displaying
inline uint8_t getLeds() { return LEDS; }; // LEDs for displaying
inline uint8_t getPrint() { return _print.grid; }; // Current grid position
inline bool    isSuccess() { return _status.lastResult == GBJ_TM1638_SUCCESS; } // Flag about successful recent operation
inline bool    isError() { return !isSuccess(); } // Flag about erroneous recent operation


private:
//------------------------------------------------------------------------------
// Private constants
//------------------------------------------------------------------------------
enum Commands
{
  // Data command setting (0x40)
  CMD_DATA_INIT   = 0b01000000, // 0x40, Command set, ORed by proper next ones
  CMD_DATA_WRITE  = 0b00, // 0x00, Write data to display register
  CMD_DATA_READ   = 0b10, // 0x02, Read key scanning data
  CMD_DATA_AUTO   = 0b000, // 0x00, Auto increase of address
  CMD_DATA_FIXED  = 0b100, // 0x04, Fixed address setting
  CMD_DATA_NORMAL = 0b0000, // 0x00, Normal mode
  CMD_DATA_TEST   = 0b1000, // 0x08, Test mode
  // Address command setting (0xC0)
  CMD_ADDR_INIT = 0b11000000, // 0xC0, Address set, ORed by display address 0x00 ~ 0x0F in lower nibble
  // Display control (0x80)
  CMD_DISP_INIT = 0b10000000, // 0x80, Display control, ORed by proper next ones
  CMD_DISP_OFF  = 0b0000, // 0x00, Display is off
  CMD_DISP_ON   = 0b1000, // 0x08, Display is on, ORed by contrast 0x00 ~ 0x07 in lower 3 bits
};
enum Geometry // Controller TM1638
{
  DIGITS = 8, // Digital tubes
  LEDS = 8, // Two-color LEDs
};
enum Timing
{
  TIMING_RELAX = 2, // MCU relaxing delay in microseconds after pin change
};
enum Rasters
{
  FONT_WIDTH = 2,
  FONT_INDEX_ASCII = 0,
  FONT_INDEX_MASK = 1,
  FONT_MASK_WRONG = 0xFF,  // Byte value for unknown font glyph
};
enum LEDs
{
  LED_RED = 0x01, // Control bit for red led
  LED_GREEN = 0x02, // Control bit for green led
};


//------------------------------------------------------------------------------
// Private attributes
//------------------------------------------------------------------------------
struct
{
  uint8_t buffer[DIGITS + LEDS];  // Screen buffer
  // uint8_t width;  // Number of grids for printing characters
  uint8_t grid; // Current grid for next printing
} _print; // Display hardware parameters for printing
struct Bitmap
{
  const uint8_t* table; // Pointer to a font table
  uint8_t glyphs; // Number of glyphs in the font table
} _font;  // Font parameters
struct
{
  uint8_t lastResult; // Result of a recent operation
  uint8_t lastCommand;  // Command code recently sent to two-wire bus
  uint8_t pinClk; // Number of serial clock pin
  uint8_t pinDio; // Number of data input/output pin
  uint8_t pinStb; // Number of strobe pin
} _status;  // Microcontroller status features


//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------
inline void swapByte(uint8_t a, uint8_t b) { if (a > b) {uint8_t t = a; a = b; b = t;} };
inline uint8_t addrGrid(uint8_t grid) { return 2 * grid; };
inline uint8_t addrLed(uint8_t led) { return 2 * led + 1; };
inline uint8_t setLastCommand(uint8_t lastCommand) { return _status.lastCommand = lastCommand; };
void waitPulseClk();  // Delay for clock pulse duration
void beginTransmission(); // Start condition
void endTransmission(); // Stop condition
void busWrite(uint8_t data);  // Write byte to the bus
void gridWrite(uint8_t segmentMask = 0x00, uint8_t gridStart = 0, uint8_t gridStop = DIGITS); // Fill screen buffer with grid masks
uint8_t busSend(uint8_t command); // Send sole command
uint8_t busSend(uint8_t command, uint8_t data); // Send data at fixed address
uint8_t busSend(uint8_t command, uint8_t* buffer, uint8_t bufferBytes); // Send data at auto-increment addressing
uint8_t getFontMask(uint8_t ascii); // Lookup font mask in font table by ASCII code
};

#endif
