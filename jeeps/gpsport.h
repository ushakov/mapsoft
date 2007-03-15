/*
 *  For portability any '32' type must be 32 bits
 *                  and '16' type must be 16 bits
 */

/* Since GPSBabel already has an integer size abstraction layer and
 * defs.h includes gbtypes.h before this file, just use that.
 */

typedef unsigned char UC;
typedef gbuint16      US;
#ifndef uint16
typedef gbuint16      uint16;
#endif
#ifndef int16
typedef gbint16       int16;
#endif
#ifndef uint32
typedef gbuint32      uint32;
#endif
#ifndef int32
typedef gbint32       int32;
#endif
