<a id="library"></a>
# gbjTM1638
Library for utilizing display modules with TM1638 controller. Those modules are available in various hardware equipment as follows. However each variant has 8 digital 7-segment tubes.
1. 8 seven-segment 0.56" digital tubes with decimal dots, 8 two-color LEDs red+green, keypad with 8 keys, two 10 pin connectors (usually marked as JY-LKM1368). This module can be cascaded by 10 pin cable, where output connector of a modules is tied with input connector of the next module in a chain. The entire chain is controlled through the input connector of the first module in a chain and particular modules is selected by a corresponding STB pin of that connector.
1. 8 seven-segment 0.56" digital tubes with decimal dots, two 10 pin connectors (usually unmarked). This module can be considered as a extension digital tubes only module of the previous one and can be cascaded.
1. 8 seven-segment 0.36" digital tubes with decimal dots, 8 one-color LEDs red, keypad with 8 keys, one 5 pin connector (usually marked as LED--KEY, LED&KEY).

The library controls the driver as a state machine with screen buffer in the microcontroller's operating memory, which is transmitted to the controller for displaying.
- Screen buffer is considered as an image of controller's graphical memory.
- Graphical library methods (prefixed with "**print**") performs all graphical manipulation in the screen buffer, which state reflects the desired image for display.
- Finally the dedicated method [display()](#display) transmits the content of the screen buffer to the driver and it causes to display the image on the attached display (digital tubes) and LEDs.
- The driver TM1638 can control up to 8 digital tubes each with radix (decimal dot), 8 two-color LEDs, and 24 keys of the keypad, although display modules usually have just 8 or 16 keys in the keypad implemented.
- The library controls 7-segment glyphs (digits) mutual independently from radix 8th segments of digital tubes and LEDs.
- The library implements key scan capabilities of the driver as well for all 24 keys.
- The library inherits from the system library **Print**, so that all system *print* operations are available.


<a id="dependency"></a>
## Dependency
- **Arduino.h**: Main include file for the Arduino SDK version greater or equal to 100.
- **WProgram.h**: Main include file for the Arduino SDK version less than 100.
- **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.
- **Print.h**: System library for printing.


<a id="Fonts"></a>
## Fonts
The font is an assignment of a glyph definition to particular ASCII code.
- A 7-segment display glyph is defined by a segment mask of the controller.
- Every font is defined as one-dimensional array with the same name **gbjFont7segTable**, stored in a *separate include file* with the naming convention **font7seg\_**_variant_**.h** in the subfolder `extras`. Font variants differentiate from each other by length and content of that array.
- The library contains those fonts:
	- **font7seg_basic.h**: Alphanumeric glyphs reasonably recognizable and readable on 7-segment displays.
	- **font7seg_decnums.h**: Decimal digits, space, and minus glyph.
	- **font7seg_hexnums.h**: Hexadecimal digits, space, and minus glyph.
- Despite the font array is one-dimensional one, glyphs are defined by logical group of two bytes in it.
	- The **first byte** of the glyph pair is an **ASCII code** of a glyph. Usually it is a 7-bit code, but it might be 8-bit one as well, if appropriate segment mask is provided, which can be reasonably displayed on the 7-segment display.
	- The **second byte** of the glyph pair is a **segment mask** of a glyph with least significant bit (LSB) corresponding to the segment `A`. The 8th, most significant bit (MSB) corresponding to the decimal point (DP) is ignored if set, because the library controls radix segments separately, not by fonts.
- Involving ASCII codes to the font definition enables to define just recognizable glyphs by the 7-segment displays or needed by a project and not to waste memory by definition contiguous set of ASCII codes with unused glyphs, although not starting from 0.
- After including a font include file into a sketch, the font is stored in the flash memory of a microcontroller in order to save operational SRAM.
- The library can utilize just one font at a time.


<a id="constants"></a>
## Constants
- **GBJ\_TM1638\_VERSION**: Name and semantic version of the library.
- **GBJ\_TM1638\_SUCCESS**: Result code for successful processing.
- **GBJ\_TM1638\_KEYS\_PRESENT**: Implemented keys in the keypad of a display module. Redefine it to according to your display module in order not to waist memory for not implemented but available keys. **Default is 8 keys.**
### Errors
- **GBJ\_TM1638\_ERR\_PINS**: Error code for incorrectly assigned microcontroller's pins to controller's pins, usually some o them are duplicated.
- **GBJ\_TM1638\_ERR\_ACK**: Error code for not acknowledged transmission by the driver.
### Key actions
- **GBJ\_TM1638\_KEY\_CLICK**: A key has been clicked once. The delay of key released after its pressing should be a bit longer than at the double click after the first click in order to distinguish between them.
- **GBJ\_TM1638\_KEY\_CLICK_DOUBLE**: A key has been clicked twice, i.e., double clicked with short delay between key presses.
- **GBJ\_TM1638\_KEY\_HOLD**: A key has been clicked and keep pressed a while, then released.
- **GBJ\_TM1638\_KEY\_HOLD_DOUBLE**: A key has been double clicked and keep pressed a while at the second press, then released.


<a id="interface"></a>
## Interface
The methods in bold return [result or error codes](#constants) and communicate with the controller directly. The methods for screen buffer manipulation return nothing, just update the content of the screen buffer as an image of controller registers, and must be followed by [display()](#display) method in order to display the content of the screen buffer.

It is possible to use functions from the parent library [Print](#dependency), which is extended by this library.

##### Custom data types
- [gbj_tm1638_handler](#gbj_tm1638_handler)

#### Initialization
- [gbj_tm1638()](#gbj_tm1638)
- **[begin()](#begin)**

#### Display manipulation
- **[display()](#display)**
- **[displayOn()](#displaySwitch)**
- **[displayOff()](#displaySwitch)**

#### Screen buffer manipulation
- [printRadixOn()](#printRadix)
- [printRadixOff()](#printRadix)
- [printRadixToggle()](#printRadix)
- [printRadixClear()](#printRadixFull)
- [printRadixFill()](#printRadixFull)
- [printGrid()](#printGrid)
- [printClear()](#printGridFull)
- [printFill()](#printGridFull)
- [write()](#write)

#### Setters
- [initLastResult()](#initLastResult)
- [setLastResult()](#setLastResult)
- **[setContrastControl()](#setContrastControl)**
- [setFont()](#setFont)

#### Getters
- [getLastResult()](#getLastResult)
- [getLastCommand()](#getLastCommand)
- [getPrintWidth()](#getPrintWidth)
- [isSuccess()](#isSuccess)
- [isError()](#isError)


<a id="gbj_tm1638_handler"></a>
## gbj_tm1638_handler()
#### Description
Custom data type determining the template for key action handler procedures.
- The handler method is called then particular action with any key of the keypad is detected. It allows to process and executed some code dedicated to particular key and its action.
- The handler method is registered to the library by the method [registerHandler()](#registerHandler) and should have appropriate input parameters defined.

#### Syntax
    void (*gbj_tm1638_handler)(uint8_t key, uint8_t action);

#### Parameters
- **key**: Number of a keypad's key counting from 0, for which action the handler is called.
	- **Valid values**: 0 ~ [GBJ\_TM1638\_KEYS\_PRESENT](#constants)
	- **Default value**: none


- **action**: The action with a key, which has been executed recently.
	- **Valid values**: key actions defined by [constants](#constants)
	- **Default value**: none

#### Returns
None

#### See also
[registerHandler()](#registerHandler)

[Back to interface](#interface)


<a id="gbj_tm1638"></a>
## gbj_tm1638()
#### Description
The constructor method sanitizes and stores physical features of the display to the class instance object.

#### Syntax
	gbj_tm1638(uint8_t pinClk, uint8_t pinDio, uint8_t pinStb, uint8_t digits, uint8_t leds, uint8_t keys);

#### Parameters
- **pinClk**: Microcontroller pin's number utilized as a serial clock.
	- **Valid values**: non-negative integer by microcontroller datasheet
	- **Default value**: 2


- **pinDio**: Microcontroller pin's number utilized as a data input and output.
	- **Valid values**: non-negative integer (according to a microcontroller datasheet)
	- **Default value**: 3


<a id="prm_digits"></a>
- **digits**: Number of 7-segment LED digits to be controlled.
	- **Valid values**: 0 ~ 8
	- **Default value**: 8


<a id="prm_leds"></a>
- **leds**: Number of LEDs to be controlled.
	- **Valid values**: 0 ~ 8
	- **Default value**: 8


<a id="prm_keys"></a>
- **keys**: Number of keypad's keys to be controlled. Default value is aimed for regular display modules.
	- **Valid values**: 0 ~ [GBJ\_TM1638\_KEYS\_PRESENT](#constants)
	- **Default value**: 8

#### Returns
The library instance object for a display module.

[Back to interface](#interface)


<a id="begin"></a>
## begin()
#### Description
The method sets the microcontroller's pins dedicated for the driver and performs initial sequence recommended by the data sheet for the controller.
- It clears all digital tubes including radixes and turns off all leds.
- It sets a display module to the normal operating mode.

#### Syntax
	uint8_t begin();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

[Back to interface](#interface)


<a id="display"></a>
## display()
#### Description
The method transmits current content of the screen buffer to the driver, so that its content is displayed immediately and stays unchanged until another transmission.
- The method utilizes automatic addressing mode of the driver.

#### Syntax
	uint8_t display();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

#### See also
[displayOn()](#displaySwitch)

[displayOff()](#displaySwitch)

[Back to interface](#interface)


<a id="displaySwitch"></a>
## displayOn(), displayOff()
#### Description
Particular method either turns on or off the entired display module including digital tubes and LEDs without changing current contrast level.
- Both methods are suitable for making a display module blink.

#### Syntax
	uint8_t displayOn();
	uint8_t displayOff();

#### Parameters
None

#### Returns
Some of [result or error codes](#constants).

#### See also
[display()](#display)

[Back to interface](#interface)


<a id="printRadix"></a>
## printRadixOn(), printRadixOff(), printRadixToggle()
#### Description
The particular method performs corresponding manipulation with radix segment (usually 8th one) of particular glyph without influence on its glyph segments (first 7 segments) in the screen buffer.
- Default grid is suitable for 4-digit displays aimed for digital clocks with colon instead of decimal point of second (number 1) digit.

#### Syntax
	void printRadixOn(uint8_t grid);
	void printRadixOff(uint8_t grid);
	void printRadixToggle(uint8_t grid);

#### Parameters
- **grid**: Driver's grid (LED digit) number counting from 0, which radix segment should be manipulated.
	- **Valid values**: 0 ~ [grids](#prm_grids) from constructor
	- **Default value**: 1 (second digit for usual clock LED displays)

#### Returns
None

#### See also
[printRadixClear()](#printRadixFull)

[printRadixFill()](#printRadixFull)

[Back to interface](#interface)


<a id="printRadixFull"></a>
## printRadixClear(), printRadixFill()
#### Description
The particular method performs corresponding manipulation with all radix segments at once in screen buffer without changing glyph segments.
- At clearing all glyph segments are turned off.
- At filling all glyph segments are turned on. It is suitable for testing a display itself.

#### Syntax
	void printRadixClear();
	void printRadixFill();

#### Parameters
None

#### Returns
None

#### See also
[printRadixOn()](#printRadix)

[printRadixOff()](#printRadix)

[printRadixToggle()](#printRadix)

[Back to interface](#interface)


<a id="printGrid"></a>
## printGrid()
#### Description
The method sets glyph segments (first 7 ones) of particular glyph without influence on its radix segment in the screen buffer.
- Default grid is for the very first digit of the display and grid of the controller.
- The method is useful for writing to the display without any font used.

#### Syntax
	void printGrid(uint8_t grid);

#### Parameters
- **grid**: Driver's grid (LED digit) number counting from 0, which glyph segments should be manipulated.
	- **Valid values**: 0 ~ [grids](#prm_grids) from constructor
	- **Default value**: 0


- **segmentMask**: Bit mask defining what segments should be turned on. Segments marking starting from A to G relate to mask bits 0 to 6 counting from least significant bit. The 7th bit relates to radix segment and therefore it is ignored.
	- **Valid values**: 0 ~ 127
	- **Default value**: 0x7F (all segments set)

#### Returns
None

#### See also
[printClear()](#printGridFull)

[printFill()](#printGridFull)

[Back to interface](#interface)


<a id="printGridFull"></a>
## printClear(), printFill()
#### Description
The particular method performs corresponding manipulation with all glyph segments at once of the display without changing glyph radix segments.

#### Syntax
	void printClear();
	void printFill();

#### Parameters
None

#### Returns
None

#### See also
[printGrid()](#printGrid)

[Back to interface](#interface)


<a id="write"></a>
## write()
#### Description
The library inherits the system *Print* class, so that all regular print functions can be used.
- Actually all print functions eventually call one of listed write methods, so that all of them should be implemented.
- If some character (ASCII) code is not present in the font table, i.e., it is unknown for the library, that character is ignored and not displayed.
- If unknown character has ASCII code of comma, dot, or colon, the library turns on the radix segments of the recently displayed digit (lastly manipulated grid). Thus, the decimal points or colon can be present in displayed string at proper position and does not need to be control separately.

#### Syntax
	size_t write(uint8_t ascii);
	size_t write(const char* text);
	size_t write(const uint8_t* buffer, size_t size);

#### Parameters
- **ascii**: ASCII code of a character that should be displayed at the current grid position. The methods is usually utilized internally by system prints.
	- **Valid values**: 0 ~ 255
	- **Default value**: none


- **text**: Pointer to a null terminated string that should be displayed from the very beginning of the display, i.e., from the first digit.
	- **Valid values**: microcontroller addressing range
	- **Default value**: none


- **buffer**: Pointer to a string, which part should be displayed from the very beginning of the display, i.e., from the first digit.
	- **Valid values**: microcontroller addressing range
	- **Default value**: none


- **size**: Number of characters that should be displayed from the very beginning of the display, i.e., from the first digit.
	- **Valid values**: microcontroller addressing range
	- **Default value**: none

#### Returns
None

#### See also
[printGrid()](#printGrid)

[Back to interface](#interface)


<a id="initLastResult"></a>
## initLastResult()
#### Description
The method sets internal status of recent processing of a controller code to success with value of macro [GBJ\_TM1638\_SUCCESS](#constants). It is usually called right before any operation with the controller in order to reset the internal status.

#### Syntax
    void initLastResult();

#### Parameters
None

#### Returns
None

#### See also
[getLastResult()](#getLastResult)

[setLastResult()](#setLastResult)

[Back to interface](#interface)


<a id="setLastResult"></a>
## setLastResult()
#### Description
The method sets the internal status of recent processing with controller to input value. Without input parameter it is equivalent to the method [initLastResult()](#initLastResult).

#### Syntax
    uint8_t setLastResult(uint8_t lastResult);

#### Parameters
- **lastResult**: Desired result code that should be set as a last result code.
  - *Valid values*: One of macro for [result codes](#constants).
  - *Default value*: [GBJ\_TM1367\_SUCCESS](#constants)

#### Returns
New (actual) result code of recent operation.

#### See also
[initLastResult()](#initLastResult)

[getLastResult()](#getLastResult)

[Back to interface](#interface)


<a id="setContrastControl"></a>
## setContrastControl()
#### Description
The method sets the level of the display contrast.
- The contrast is perceived as the brightness of the display.
- The brightness is technically implemented with <abbr title="Pulse Width Modulation">PWM</abbr> of segments power supply.

#### Syntax
	uint8_t setContrastControl(uint8_t contrast);

#### Parameters
- **contrast**: Level of contrast/brightness.
	- *Valid values*: 0 ~ 7
	- *Default value*: 3

#### Returns
Some of [result or error codes](#constants).

[Back to interface](#interface)


<a id="setFont"></a>
## setFont()
#### Description
The method gathers font parameters for printing characters on 7-segment displays.
- Font definition is usually included to an application sketch from particular include file, while the font table resides in programmatic (flash) memory of a microcontroller in order to save operational memory (SRAM).
- Each glyph of a font consists of the pair of bytes. The first byte determines ASCII code of a glyph and second byte determines segment mask of a glyph. It allows to defined only displayable glyphs on 7-segment displays and suppress need to waste memory for useless characters.

#### Syntax
	void setFont(const uint8_t* fontTable, uint8_t fontTableSize);

#### Parameters
- **fontTable**: Pointer to constant byte array with font characters definitions. Because the font table resides in flash memory, it has to be constant.
	- *Valid values*: microcontroller addressing range
	- *Default value*: none


- **fontTableSize**: The number of bytes that should be utilized from the font table.
	- Because the font table is referenced by a pointer and not as an array, the table size cannot be calculated internally, but has to be defined externally usually by the function ```sizeof```.
	- The table size in conjunction with font character pair of bytes determines the number of characters used for printing.
	- The size can be smaller than the real size of the table, however, the size should be a multiple of 2.
		- *Valid values*: 0 ~  255 (maximal 127 7-segments characters)
		- *Default value*: none

#### Returns
None

#### Example
Parameters of a font table are usually defined as pre-processor macro constants in including definition file, so that it is not needed to determine them manually.

``` cpp
#include "font7seg_basic.h"
gbj_tm1638 Sled = gbj_tm1638();
setup()
{
 Sled.begin();
 Sled.setFont(gbjFont7segTable, sizeof(gbjFont7segTable));
}
```

#### See also
[Fonts](#Fonts)

[Back to interface](#interface)


<a id="getLastResult"></a>
## getLastResult()
#### Description
The method returns a result code of the recent operation with controller. It is usually called for error handling in a sketch.

#### Syntax
    uint8_t getLastResult();

#### Parameters
None

#### Returns
Current result code. It is one of expected [result codes](#constants).

#### See also
[setLastResult()](#setLastResult)

[initLastResult()](#initLastResult)

[Back to interface](#interface)


<a id="getLastCommand"></a>
## getLastCommand()
#### Description
The method returns the command code used at recent communication with controller. In conjunction with returned result or error code of particular method it is possible to detect the source or reason of a communication error.

#### Syntax
	uint8_t getLastCommand();

#### Parameters
None

#### Returns
Recently used command code.

#### See also
[Error codes](#constants)

[Back to interface](#interface)


<a id="getPrintWidth"></a>
## getPrintWidth()
#### Description
The method returns print dimension of the display in horizontal direction.
- Print dimension expresses the current display capacity in horizontal direction, i.e., the number of LED digits defined in the [constructor](#prm_grids).

#### Syntax
	uint8_t getPrintWidth();

#### Parameters
None

#### Returns
Current print width counting in glyphs.

#### See also
[gbj_tm1638()](#gbj_tm1638)

[Back to interface](#interface)


<a id="isSuccess"></a>
## isSuccess()
#### Description
The method returns a flag whether the recent operation with controller was successful.

#### Syntax
    bool isSuccess();

#### Parameters
None

#### Returns
Flag about successful recent processing.

#### See also
[getLastResult()](#getLastResult)

[isError()](#isError)

[Back to interface](#interface)


<a id="isError"></a>
## isError()
#### Description
The method returns a flag whether the recent operation with controller failed. The corresponding error code can be obtained by the method [getLastResult()]((#getLastResult), which one of the error macro [constants](#constants).

#### Syntax
    bool isError();

#### Parameters
None

#### Returns
Flag about failing of the recent operation.

#### See also
[getLastResult()](#getLastResult)

[isSuccess()](#isSuccess)

[Back to interface](#interface)
