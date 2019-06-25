#ifndef _UNP_MACROS_H_
#define _UNP_MACROS_H_

#define SET_BIT(flags, bit) ((flags) |= (bit))
#define CLR_BIT(flags, bit) ((flags) &= ~(bit))
#define BIT_ENABLED(flags, bit) (flags & bit)
#define BIT_DISABLED(flags, bit) (!BIT_ENABLED((flags), (bit)))

#define INVALID_HANDLE -1

#endif //_UNP_MACROS_H_ 
