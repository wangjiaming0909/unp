/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -DG -K headerName -N isValidHttpHeader -L C++ -Z HttpCommomHeaderInternal -t HttpCommomHeadersHash.h.gperf  */
/* Computed positions: -k'1,8,22,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "HttpCommomHeadersHash.h.gperf"

#include "HttpCommomHeadersHash.h"

namespace http{

#line 7 "HttpCommomHeadersHash.h.gperf"

inline unsigned int
HttpCommomHeaderInternal::hash (const char *str, size_t len)
{
  static unsigned char asso_values[] =
    {
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149,   0, 149, 149, 149, 149,
      149, 149, 149,  45, 149, 149, 149, 149,  55, 149,
      149, 149, 149, 149, 149,   0, 149,   0,  45,   5,
       60, 149,   0,   5, 149,   5,  40, 120, 149,  90,
        0, 149,   5,  20,  45, 100,  35,  70,  20, 149,
      149, 149, 149, 149, 149, 149, 149,  35, 149,   0,
       35,  10,  75,  35,  25,   0, 149,  10,  30,  30,
        5,  45,   0, 149,   5,   0,   0,  25, 149,   0,
      149,  35,  50, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149, 149, 149, 149, 149,
      149, 149, 149, 149, 149, 149
    };
  unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[static_cast<unsigned char>(str[21])];
      /*FALLTHROUGH*/
      case 21:
      case 20:
      case 19:
      case 18:
      case 17:
      case 16:
      case 15:
      case 14:
      case 13:
      case 12:
      case 11:
      case 10:
      case 9:
      case 8:
        hval += asso_values[static_cast<unsigned char>(str[7])];
      /*FALLTHROUGH*/
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[static_cast<unsigned char>(str[0])];
        break;
    }
  return hval + asso_values[static_cast<unsigned char>(str[len - 1])];
}

static signed char lookup[] =
  {
    -1, -1, -1,  0,  1,  2,  3,  4, -1, -1, -1,  5,  6,  7,
     8,  9, 10, 11, 12, -1, 13, 14, 15, 16, 17, 18, 19, -1,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, -1, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
    47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, -1, 62, 63, 64, 65, -1, 66, -1, -1, 67, -1, 68, -1,
    69, 70, -1, 71, 72, -1, -1, -1, 73, 74, 75, -1, 76, 77,
    -1, -1, 78, 79, 80, -1, -1, -1, -1, 81, -1, -1, -1, -1,
    82, -1, -1, -1, -1, 83, -1, -1, 84, 85, 86, 87, -1, -1,
    -1, 88, -1, -1, 89, -1, 90, -1, -1, -1, -1, -1, -1, 91,
    -1, -1, -1, -1, -1, -1, -1, -1, 92
  };

struct HttpHeader *
HttpCommomHeaderInternal::isValidHttpHeader (const char *str, size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          int index = lookup[key];

          if (index >= 0)
            {
              const char *s = wordlist[index].headerName;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
#line 106 "HttpCommomHeadersHash.h.gperf"

} //namespace
