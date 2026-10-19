#ifndef UTIL_H
#define UTIL_H

/*
 * parse a char into a nibble, e.g. ascii '2' would be converted into the int 0x2
 */
extern int ParseCharNibble(int nibble);

/*
 * parse two chars into a byte, e.g. ascii 'E0' would be converted into the int 0xE0
 */
extern int ParseCharByte(int hiNibble, int loNibble);

#endif