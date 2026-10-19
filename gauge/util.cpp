#include <debug.h>
#include <util.h>

/*
 * parse a char into a nibble, e.g. ascii '2' would be converted into the int 0x2
 */
int ParseCharNibble(int nibble) {
  DEBUG_PORT.printf("parsing '%c'\n", nibble);
  if (nibble >= 'A' && nibble <= 'Z') {
    return nibble - 'A' + 10;
  }
  if (nibble >= 'a' && nibble <= 'z') {
    return nibble - 'a' + 10;
  }
  if (nibble >= '0' && nibble <= '9') {
    return nibble - '0';
  }
  DEBUG_PORT.printf("error parsing nibble\n");
  return 0;
}

/*
 * parse two chars into a byte, e.g. ascii 'E0' would be converted into the int 0xE0
 */
int ParseCharByte(int hiNibble, int loNibble) {
  int hi = ParseCharNibble(hiNibble);
  DEBUG_PORT.printf("hi '%c' = %d\n", hiNibble, hi);
  int lo = ParseCharNibble(loNibble);
  DEBUG_PORT.printf("lo '%c' = %d\n", loNibble, lo);
  int b = (hi<<4)+lo;
  DEBUG_PORT.printf("byte = %d\n", b); 
  return b;
}
