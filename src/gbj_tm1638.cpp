#include "gbj_tm1638.h"


gbj_tm1638::gbj_tm1638(uint8_t pinClk, uint8_t pinDio, uint8_t pinStb)
{
  _status.pinClk = pinClk;
  _status.pinDio = pinDio;
  _status.pinStb = pinStb;
}


uint8_t gbj_tm1638::begin()
{
  initLastResult();
  // Check pin duplicity
  if (_status.pinClk == _status.pinDio \
  ||  _status.pinDio == _status.pinStb \
  ||  _status.pinStb == _status.pinClk) return setLastResult(GBJ_TM1638_ERR_PINS);
  // Setup pins
  pinMode(_status.pinClk, OUTPUT);
  pinMode(_status.pinDio, OUTPUT);
  pinMode(_status.pinStb, OUTPUT);
  // Initialize controller
  // setContrastControl();
  printGridOffAll();
  printRadixOffAll();
  printLedAllOff();
  return display();
}


//------------------------------------------------------------------------------
// Software manipulation - updating screen buffer
//------------------------------------------------------------------------------
// Print one character determined by a byte of ASCII code
size_t gbj_tm1638::write(uint8_t ascii)
{
  if (_print.grid >= DIGITS) return 0;
  uint8_t mask = getFontMask(ascii);
  if (mask == FONT_MASK_WRONG)
  {
    if (String(".,:").indexOf(ascii) >= 0)  // Detect radix
    {
      printRadixOn(_print.grid - 1); // Set radix to the previous grid
    }
    return 0;
  }
  else
  {
    printGrid(_print.grid, mask);
    return 1;
  }
}


// Print null terminated character array
size_t  gbj_tm1638::write(const char* text)
{
  printGridOffAll();
  uint8_t grids = 0;
  uint8_t i = 0;
  while (text[i] != '\0' && _print.grid < DIGITS)
  {
    grids += write(text[i++]);
  }
  return grids;
}


// Print byte array with length
size_t  gbj_tm1638::write(const uint8_t* buffer, size_t size)
{
  printGridOffAll();
  uint8_t grids = 0;
  _print.grid = 0;
  for (uint8_t i = 0; i < size && _print.grid < DIGITS; i++)
  {
    grids += write(buffer[i]);
  }
  return grids;
}


//------------------------------------------------------------------------------
// Hardware manipulation - communication with the controller
//------------------------------------------------------------------------------
uint8_t gbj_tm1638::display()
{
  // Automatic addressing
  if (busSend(CMD_DATA_INIT | CMD_DATA_NORMAL | CMD_DATA_WRITE | CMD_DATA_AUTO)) return getLastResult();
  if (busSend(CMD_ADDR_INIT, _print.buffer, DIGITS)) return getLastResult();
  return getLastResult();
}


//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------
uint8_t gbj_tm1638::setContrastControl(uint8_t contrast)
{
  contrast &= 0x07;
  return busSend(CMD_DISP_INIT | CMD_DISP_ON | contrast);
}


void gbj_tm1638::setFont(const uint8_t* fontTable, uint8_t fontTableSize)
{
  _font.table = fontTable;
  _font.glyphs = fontTableSize / FONT_WIDTH;
}


//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------
// Wait for delay period expiry
void gbj_tm1638::waitPulseClk()
{
  delayMicroseconds(TIMING_RELAX);
}


// Start condition - pull down STB from HIGH to LOW
void gbj_tm1638::beginTransmission()
{
  digitalWrite(_status.pinStb, HIGH); // Finish previous communication for sure
  digitalWrite(_status.pinClk, LOW); // For active rising edge of clock pulse
  waitPulseClk();
  digitalWrite(_status.pinStb, LOW); // Start communication
}


// Stop condition - pull up STB from LOW to HIGH
void gbj_tm1638::endTransmission()
{
  digitalWrite(_status.pinStb, HIGH);
  waitPulseClk();
}


void gbj_tm1638::busWrite(uint8_t data)
{
  digitalWrite(_status.pinClk, LOW); // For active rising edge of clock pulse
  shiftOut(_status.pinDio, _status.pinClk, LSBFIRST, data);
}


uint8_t gbj_tm1638::busSend(uint8_t command)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  endTransmission();
  return getLastResult();
}


uint8_t gbj_tm1638::busSend(uint8_t command, uint8_t data)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  busWrite(data);
  endTransmission();
  return getLastResult();
}


uint8_t gbj_tm1638::busSend(uint8_t command, uint8_t* buffer, uint8_t bufferItems)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  for (uint8_t bufferIndex = 0; bufferIndex < bufferItems; bufferIndex++)
  {
    busWrite(*buffer++);
  }
  endTransmission();
  return getLastResult();
}


// The method leaves grid cursor after last print grid
void gbj_tm1638::gridWrite(uint8_t segmentMask, uint8_t gridStart, uint8_t gridStop)
{
  swapByte(gridStart, gridStop);
  gridStop = min(gridStop, DIGITS - 1);
  for (_print.grid = gridStart; _print.grid <= gridStop; _print.grid++)
  {
    segmentMask &= 0x7F; // Clear radix bit in segment mask
    _print.buffer[addrGrid(_print.grid)] &= 0x80;  // Clear digit bits in screen buffer
    _print.buffer[addrGrid(_print.grid)] |= segmentMask;  // Set digit bits but leave radix bit intact
  }
}


uint8_t gbj_tm1638::getFontMask(uint8_t ascii)
{
  uint8_t mask = FONT_MASK_WRONG;
  for (uint8_t glyph = 0; glyph < _font.glyphs; glyph++)
  {
    if (ascii == pgm_read_byte(&_font.table[glyph * FONT_WIDTH + FONT_INDEX_ASCII]))
    {
      mask = pgm_read_byte(&_font.table[glyph*2 + FONT_INDEX_MASK]);
      mask &= 0x7F; // Clear radix bit not to mess with wrong mask
      break;
    }
  }
  return mask;
}
