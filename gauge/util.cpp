#include <debug.h>
#include <util.h>

/*
 * parse a char into a nibble, e.g. ascii '2' would be converted into the int 0x2
 */
int ParseCharNibble(int nibble) {
  if (nibble >= 'A' && nibble <= 'Z') {
    return nibble - 'A' + 10;
  }
  if (nibble >= 'a' && nibble <= 'z') {
    return nibble - 'a' + 10;
  }
  if (nibble >= '0' && nibble <= '9') {
    return nibble - '0';
  }
  if (VERBOSE) {
    DEBUG_PORT.printf("error parsing nibble '%c' (0x%02x)\n");
  }
  return 0;
}

/*
 * parse two chars into a byte, e.g. ascii 'E0' would be converted into the int 0xE0
 */
int ParseCharByte(int hiNibble, int loNibble) {
  int hi = ParseCharNibble(hiNibble);
  int lo = ParseCharNibble(loNibble);
  int b = (hi<<4)+lo;
  if (VERBOSE) {
    DEBUG_PORT.printf("parsed %c%c as 0x%02x = %d\n", hiNibble, loNibble, b, b); 
  }
  return b;
}
