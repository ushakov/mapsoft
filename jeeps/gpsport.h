/*
 *  For portability any '32' type must be 32 bits
 *                  and '16' type must be 16 bits
 */

#if defined(_MSC_VER)

typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef long  int32;
typedef short int16;

#else

# if defined (__FreeBSD__)
#  include <inttypes.h>
# else
#  include <stdint.h>
# endif

typedef uint32_t uint32;
typedef uint16_t uint16;
typedef  int32_t int32;
typedef  int16_t int16;

#endif /* defined(_MSC_VER) */

typedef unsigned char uint8;
typedef signed char    int8;

typedef uint8         UC;
typedef uint16        US;
