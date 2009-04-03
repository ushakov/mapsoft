/*
    Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007  Robert Lipe, robertlipe@usa.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */
#ifndef utils_h_included
#define utils_h_included
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

// Turn on Unicode in expat?
#ifdef _UNICODE
#  define XML_UNICODE
#endif

/*
 * Amazingly, this constant is not specified in the standard...
 */
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE !FALSE
#endif

#define FEET_TO_METERS(feetsies) ((feetsies) * 0.3048)
#define METERS_TO_FEET(meetsies) ((meetsies) * 3.2808399)

#define NMILES_TO_METERS(a) ((a) * 1852.0)	/* nautical miles */
#define MILES_TO_METERS(a) ((a) * 1609.344)
#define METERS_TO_MILES(a) ((a) / 1609.344)
#define FATHOMS_TO_METERS(a) ((a) * 1.8288)

#define CELSIUS_TO_FAHRENHEIT(a) (((a) * 1.8) + 32)
#define FAHRENHEIT_TO_CELSIUS(a) (((a) - 32) / 1.8)

#define SECONDS_PER_HOUR (60L*60)
#define SECONDS_PER_DAY (24L*60*60)

/* meters/second to kilometers/hour */
#define MPS_TO_KPH(a) ((double)(a)*SECONDS_PER_HOUR/1000)

/* meters/second to miles/hour */
#define MPS_TO_MPH(a) (METERS_TO_MILES(a) * SECONDS_PER_HOUR)

/* meters/second to knots */
#define MPS_TO_KNOTS(a) (MPS_TO_KPH((a)/1.852))

/* kilometers/hour to meters/second */
#define KPH_TO_MPS(a) ((double)(a)*1000/SECONDS_PER_HOUR)

/* miles/hour to meters/second */
#define MPH_TO_MPS(a) (MILES_TO_METERS(a) / SECONDS_PER_HOUR)

/* knots to meters/second */
#define KNOTS_TO_MPS(a) (KPH_TO_MPS((a)*1.852))

/*
 * Snprintf is in SUS (so it's in most UNIX-like substance) and it's in 
 * C99 (albeit with slightly different semantics) but it isn't in C89.   
 * This tweaks allows us to use snprintf on the holdout.
 */
#if __WIN32__
#  define snprintf _snprintf
#  define vsnprintf _vsnprintf
#  ifndef fileno
#    define fileno _fileno
#  endif
#  define strdup _strdup
#endif

/* Turn off numeric conversion warning */
#if __WIN32__
#  if _MSC_VER
#    pragma warning(disable:4244)
#  endif
#  define _CRT_SECURE_NO_DEPRECATE 1
#endif

/* Pathname separator character */
#if __WIN32__
#  define GB_PATHSEP '\\'
#else
#  define GB_PATHSEP '/'
#endif

/*
 * Common definitions.   There should be no protocol or file-specific
 * data in this file.
 */
#define BASE_STRUCT(memberp, struct_type, member_name) \
   ((struct_type *)((char *)(memberp) - offsetof(struct_type, member_name)))

typedef enum {
	fix_unknown=-1,
	fix_none=0,
	fix_2d=1,	
	fix_3d,
	fix_dgps,
	fix_pps
} fix_type;

typedef enum {
	status_unknown=0,
	status_true,
	status_false
} status_type;
        
/*
 * Define globally on which kind of data gpsbabel is working.
 * Important for "file types" that are essentially a communication
 * protocol for a receiver, like the Magellan serial data.
 */
typedef enum {
	trkdata = 1 ,
	wptdata,
	rtedata,
	posndata
} gpsdata_type;

#define NOTHINGMASK		0
#define WPTDATAMASK		1
#define TRKDATAMASK		2
#define	RTEDATAMASK		4
#define	POSNDATAMASK		8

#define MILLI_TO_MICRO(t) (t * 1000)  /* Milliseconds to Microseconds */
#define MICRO_TO_MILLI(t) (t / 1000)  /* Microseconds to Milliseconds*/
#define CENTI_TO_MICRO(t) (t * 10000) /* Centiseconds to Microseconds */
#define MICRO_TO_CENTI(t) (t / 10000) /* Centiseconds to Microseconds */


#define FS_GPX 0x67707800L
#define FS_AN1W 0x616e3177L
#define FS_AN1L 0x616e316cL
#define FS_AN1V 0x616e3176L
#define FS_OZI 0x6f7a6900L
#define FS_GMSD 0x474d5344L	/* GMSD = Garmin specific data */


/*
 *  Vmem flags values.
 */
#define VMFL_NOZERO (1 << 0)
typedef struct vmem {
	void *mem;		/* visible memory object */
	size_t size; 		/* allocated size of object */
} vmem_t;
vmem_t 	vmem_alloc(size_t, int flags);
void 	vmem_free(vmem_t*);
void 	vmem_realloc(vmem_t*, size_t);


#define ARGTYPE_UNKNOWN    0x00000000
#define ARGTYPE_INT        0x00000001
#define ARGTYPE_FLOAT      0x00000002
#define ARGTYPE_STRING     0x00000003
#define ARGTYPE_BOOL       0x00000004
#define ARGTYPE_FILE       0x00000005
#define ARGTYPE_OUTFILE    0x00000006

/* REQUIRED means that the option is required to be set. 
 * See also BEGIN/END_REQ */
#define ARGTYPE_REQUIRED   0x40000000

/* HIDDEN means that the option does not appear in help texts.  Useful
 * for debugging or testing options */
#define ARGTYPE_HIDDEN     0x20000000

/* BEGIN/END_EXCL mark the beginning and end of an exclusive range of
 * options. No more than one of the options in the range may be selected 
 * or set. If exactly one must be set, use with BEGIN/END_REQ
 * Both of these flags set is just like neither set, so avoid doing that. */
#define ARGTYPE_BEGIN_EXCL 0x10000000
#define ARGTYPE_END_EXCL   0x08000000

/* BEGIN/END_REQ mark the beginning and end of a required range of 
 * options.  One or more of the options in the range MUST be selected or set.
 * If exactly one must be set, use with BEGIN/END_EXCL 
 * Both of these flags set is synonymous with REQUIRED, so use that instead
 * for "groups" of exactly one option. */
#define ARGTYPE_BEGIN_REQ  0x04000000
#define ARGTYPE_END_REQ    0x02000000 

#define ARGTYPE_TYPEMASK 0x00000fff
#define ARGTYPE_FLAGMASK 0xfffff000

#define ARG_NOMINMAX NULL, NULL
#define ARG_TERMINATOR {0, 0, 0, 0, 0, ARG_NOMINMAX}


#ifndef DEBUG_MEM
void *xcalloc(size_t nmemb, size_t size);
void *xmalloc(size_t size);
void *xrealloc(void *p, size_t s);
void xfree(void *mem);
char *xstrdup(const char *s);
char *xstrndup(const char *s, size_t n);
char *xstrndupt(const char *s, size_t n);
char *xstrappend(char *src, const char *addon);
#define xxcalloc(nmemb, size, file, line) xcalloc(nmemb, size)
#define xxmalloc(size, file, line) xmalloc(size)
#define xxrealloc(p, s, file, line) xrealloc(p,s)
#define xxfree(mem, file, line) xfree(mem)
#define xxstrdup(s, file, line) xstrdup(s)
#define xxstrappend(src, addon, file, line) xstrappend(src, addon)
#else /* DEBUG_MEM */
void *XCALLOC(size_t nmemb, size_t size, DEBUG_PARAMS );
void *XMALLOC(size_t size, DEBUG_PARAMS );
void *XREALLOC(void *p, size_t s, DEBUG_PARAMS );
void XFREE(void *mem, DEBUG_PARAMS );
char *XSTRDUP(const char *s, DEBUG_PARAMS );
char *XSTRNDUP(const char *src, size_t size, DEBUG_PARAMS );
char *XSTRNDUPT(const char *src, size_t size, DEBUG_PARAMS );
char *XSTRAPPEND(char *src, const char *addon, DEBUG_PARAMS );
void debug_mem_open();
void debug_mem_output( char *format, ... );
void debug_mem_close();
#define xcalloc(nmemb, size) XCALLOC(nmemb, size, __FILE__, __LINE__)
#define xmalloc(size) XMALLOC(size, __FILE__, __LINE__)
#define xrealloc(p, s) XREALLOC(p,s,__FILE__,__LINE__)
#define xfree(mem) XFREE(mem, __FILE__, __LINE__)
#define xstrdup(s) XSTRDUP(s, __FILE__, __LINE__)
#define xstrndup(s, z) XSTRNDUP(s, z, __FILE__, __LINE__)
#define xstrndupt(s, z) XSTRNDUPT(s, z, __FILE__, __LINE__)
#define xstrappend(src,addon) XSTRAPPEND(src, addon, __FILE__, __LINE__)
#define xxcalloc XCALLOC
#define xxmalloc XMALLOC
#define xxrealloc XREALLOC
#define xxfree XFREE
#define xxstrdup XSTRDUP
#define xxstrndupt XSTRNDUPT
#define xxstrappend XSTRAPPEND
#endif /* DEBUG_MEM */

int case_ignore_strcmp(const char *s1, const char *s2);
int case_ignore_strncmp(const char *s1, const char *s2, int n);
int str_match(const char *str, const char *match);
int case_ignore_str_match(const char *str, const char *match);
char * strenquote(const char *str, const char quot_char);

char *strsub(const char *s, const char *search, const char *replace);
char *gstrsub(const char *s, const char *search, const char *replace);
char *xstrrstr(const char *s1, const char *s2);
void rtrim(char *s);
char * lrtrim(char *s);
int xasprintf(char **strp, const char *fmt, ...);
int xvasprintf(char **strp, const char *fmt, va_list ap);
char *strupper(char *src);
char *strlower(char *src);
signed int get_tz_offset(void);
time_t mklocaltime(struct tm *t);
time_t mkgmtime(struct tm *t);
time_t current_time(void);
signed int month_lookup(const char *m);
char * pretty_deg_format(double lat, double lon, char fmt, const char *sep, int html);   /* decimal ->  dd.dddd or dd mm.mmm or dd mm ss */

char * get_filename(const char *fname);				/* extract the filename portion */

/* 
 * Character encoding transformations.
 */

#define CET_NOT_CONVERTABLE_DEFAULT '$'
#define CET_CHARSET_ASCII	"US-ASCII"
#define CET_CHARSET_UTF8	"UTF-8"
#define CET_CHARSET_MS_ANSI	"MS-ANSI"
#define CET_CHARSET_LATIN1	"ISO-8859-1"

#define str_utf8_to_cp1252(str) cet_str_utf8_to_cp1252((str)) 
#define str_cp1252_to_utf8(str) cet_str_cp1252_to_utf8((str))

#define str_utf8_to_iso8859_1(str) cet_str_utf8_to_iso8859_1((str)) 
#define str_iso8859_1_to_utf8(str) cet_str_iso8859_1_to_utf8((str))

/*
 * Protypes for Endianness helpers.
 */

signed int be_read16(const void *p);
signed int be_read32(const void *p);
signed int le_read16(const void *p);
unsigned int le_readu16(const void *p);
signed int le_read32(const void *p);
unsigned int le_readu32(const void *p);
void le_read64(void *dest, const void *src);
void be_write16(void *pp, const unsigned i);
void be_write32(void *pp, const unsigned i);
void le_write16(void *pp, const unsigned i);
void le_write32(void *pp, const unsigned i);

double endian_read_double(void* ptr, int read_le);
float  endian_read_float(void* ptr, int read_le);
void   endian_write_double(void* ptr, double d, int write_le);
void   endian_write_float(void* ptr, float f, int write_le);

float  be_read_float(void *p);
double be_read_double(void *p);
void   be_write_float(void *pp, float d);
void   be_write_double(void *pp, double d);

float  le_read_float(void *p);
double le_read_double(void *p);
void   le_write_float(void *ptr, float f);
void   le_write_double(void *p, double d);

#define pdb_write_float be_write_float
#define pdb_read_float be_read_float
#define pdb_write_double be_write_double
#define pdb_read_double be_read_double

/*
 * Prototypes for generic conversion routines (util.c).
 */

double ddmm2degrees(double ddmm_val);
double degrees2ddmm(double deg_val);

typedef enum {
	grid_unknown = -1,
	grid_lat_lon_ddd = 0,
	grid_lat_lon_dmm = 1,
	grid_lat_lon_dms = 2,
	grid_bng = 3,
	grid_utm = 4,
	grid_swiss = 5
} grid_type;

#define GRID_INDEX_MIN	grid_lat_lon_ddd
#define GRID_INDEX_MAX	grid_swiss

#define DATUM_OSGB36	86
#define DATUM_WGS84	118

#endif /* utils_h_included */
