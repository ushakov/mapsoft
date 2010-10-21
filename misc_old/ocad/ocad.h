#include <stdint.h>

/**********************************/
/// OCAD6-7-8-9 data types

typedef int32_t  ocad_long;
typedef int32_t  ocad_int;
typedef int16_t  ocad_small;
typedef uint16_t ocad_word;
typedef uint16_t ocad_bool;
typedef uint8_t  ocad_byte;
typedef double   ocad_double; // 64-bit

struct ocad_tcoord{ // 8 bytes
  unsigned int x:24;
  int flags1:8;
  unsigned int y:24;
  int flags2:8;

  /// this point is the first curve point
  bool is_curve_f() const {return flags1 & 1;}

  /// this point is the second curve point
  bool is_curve_s() const {return flags1 & 2;}

  /// for double lines: there is no left line between this point and the next point
  bool is_no_left() const {return flags1 & 4;}

  /// v9: this point is a area border line gap (?)
  bool v9_is_ablg() const {return flags1 & 8;}

  /// this point is a corner point
  bool is_corner() const {return flags2 & 1;}

  /// this point is the first point of a hole in an area
  bool is_hole_f() const {return flags2 & 2;}

  /// for double lines: there is no right line between this point and the next point
  bool is_no_right() const {return flags2 & 4;}

  /// OCAD 7, OCAD 9: this point is a dash point (and 8?)
  bool v79_is_dash() const {return flags2 & 8;}

};

struct ocad_tcmyk{ // 4 bytes
  ocad_byte C,M,Y,B;   // 0-200?
};

struct ocad_tcolorinfo{ // 72 bytes
  ocad_small num;        // this number is used in the symbols when referring a color
  ocad_small r1;
  ocad_tcmyk color;      // color value
  char name[32];         // description of the color (pascal-string)
  ocad_byte sep_per[32]; // Definition how the color appears in the different 
                         // spot color separations: 0..200: 2 times the separation percentage
                         // 255: the color does not appear in the corresponding color separation
};

struct ocad_tcolorsep{ // 20 bytes
  char name[16];       // name of the color separation (pascal-string!)
  ocad_small raster_freq; // the halfton frequency
  ocad_small raster_ang;  // the halftone angle
};


/**********************************/

// Common part of header for all format versions
struct ocad_comm_head{ // 8 bytes
  ocad_small ocad_mark;   // 3245 (hex 0cad)
  ocad_byte file_type;    // v6: 0
                          // v7: 7
                          // v8: 2: normal map, 3: course setting
                          // v9: 0: normal map, 1: course setting
  ocad_byte file_status;  // not used
  ocad_small version;     // 6, 7 or 8
  ocad_small subversion;  // (0 for 9.00, 1 for 9.1 etc.)
};

struct ocad8_head : ocad_comm_head{ // 48 bytes
  ocad_long  f_symb_bl;   // file position of the first symbol index block
  ocad_long  f_index_bl;  // file position of the first index block
  ocad_long  setup_pos;   // file position of the setup record
  ocad_long  setup_size;  // size (in bytes) of the setup record
  ocad_long  info_pos;    // file position of the file information (0-term string up to 32767 characters + 0)
  ocad_long  info_size;   // size (in bytes) of the file information
  ocad_long  v8_f_str_bl; // OCAD 8 only. file position of the first string index block
  ocad_long  r2,r3,r4;    // reserved
};
struct ocad9_head : ocad_comm_head{ // 48 bytes
  ocad_long  f_symb_bl;   // file position of the first symbol index block
  ocad_long  index_bl;    // file position of the index block
  ocad_long  r0,r1,r2,r3; // not used;
  ocad_long  str_bl;      // file position of string index block
  ocad_long  fname_pos;   // file position of file name, used for temporary files
                          // and to recovery the file
  ocad_long  fname_size;  // size of the file name, used for temporary files only
  ocad_long  r4;          // reserved
};

struct ocad8_symb_head{ // 24 + 256*72 + 32*20 = 19096 bytes
  ocad_small ncolors;     // number of colors defined
  ocad_small ncolsep;     // number of color separations defined
  ocad_small cyan_freq;   // halftone frequency of the cyan color separation.
  ocad_small cyan_ang;    // halftone angle of the cyan color separation.
  ocad_small magenta_freq;
  ocad_small magenta_ang;
  ocad_small yellow_freq;
  ocad_small yellow_ang;
  ocad_small black_freq;
  ocad_small black_ang;
  ocad_small r1,r2;
  ocad_tcolorinfo cols[256];
  ocad_tcolorsep  seps[32]; /// only 24 used
};

/**********************************/

struct ocad8_symb_bl{ // 1028 bytes
  ocad_long next;  // file position of the next symbol block or 0
  ocad_long symb_pos[256]; // file positions (or 0) of symbols.
};

/**********************************/

struct ocad8_base_symb{ // 21+32+32+264= 349 b
  ocad_small size;   // size of the symbol in bytes (depends on the type and the number of subsymbols)
  ocad_small symb;   // symbol number (1010 for 101.0)
  ocad_small otype;  // object type
                     // 1: point, 2: line or line text, 3: area, 4: text, 5: rectangle
  ocad_byte stype;   // symbol type
                     // 1: line text and text, 2: other
  ocad_byte flags;   // OCAD 6/7: must be 0
                     // OCAD 8: 1: not oriented to north 2: icon is compressed
  ocad_small extent; // how much the rendered symbols can reach outside the
                     // coordinates of an object with this symbol
  ocad_bool selected;// symbol is selected in the symbol box
  ocad_byte status:8;// status of the symbol: 0: Normal, 1: Protected, 2: Hidden
  ocad_small tool;   // v7,v8 lines: Preferred drawing tool
                        //         0: off
                        //         1: Curve mode
                        //         2: Ellipse mode
                        //         3: Circle mode
                        //         4: Rectangular mode
                        //         5: Straight line mode
                        //         6: Freehand mode
  ocad_small r2;
  ocad_long pos;     // file position, not used in the file, only when loaded in memory
  ocad_byte colors[32]; // used colors (256bit). color 0 - lowest bit of the first byte
  char descr[32];       // description of the symbol (pascal-string)
  ocad_byte icon[264];  // in v8 can by uncompressed (16-bit colors) or compressed (256 color palette)

  bool is_point() const {return otype==1;}
  bool is_line()  const {return (otype==2) && (stype==2);}
  bool is_ltext() const {return (otype==2) && (stype==1);}
  bool is_area()  const {return otype==3;}
  bool is_text()  const {return (otype==4) && (stype==1);}
  bool is_rect()  const {return otype==5;}

  bool v8_is_n_oriented() const {return flags & 1;}
  bool v8_is_compr_icon() const {return flags & 2;}

  bool is_normal()    const {return status==0;}
  bool is_protected() const {return status==1;}
  bool is_hidden()    const {return status==2;}

} __attribute__((packed));

struct ocad8_point_symb : ocad8_base_symb{
  ocad_small data_size; // number of coordinates (each 8 bytes) which follow this structure,
                        // each object header counts as 2 Coordinates (16 bytes)
                        // The maximum value is 512
  ocad_small r3;
};

struct ocad8_sym_el{ // 16+512*8 = 4112 b
  ocad_small type;   // type of the object 1: line, 2: area, 3: circle, 4: dot (filled circle)
  ocad_word  flags;  // 1: line with round ends
  ocad_small color;
  ocad_small width;  // line width for lines and circles (unit 0.01 mm)
  ocad_small diam;   // diameter for circles and dots.
                     // (line width is included in this dimension for circles. Unit 0.01 mm)
  ocad_small  n;     // number of coordinates
  ocad_small r1,r2;
  ocad_tcoord coord[512]; // coordinates of the object //// !!!
};
// If there are several objects, they just follow each other (only
// the coordinates used are stored). To determine the number of
// objects the DataSize variable must be used.

struct ocad8_line_symb : ocad8_base_symb{
  ocad_small line_color;
  ocad_small line_width;
  ocad_word  line_ends; // true if Round line ends is checked
  ocad_small dist_from_st; // distance from start
  ocad_small dist_to_end;  // distance to the end
  ocad_small main_len;     // main length a
  ocad_small end_len;      // end length b
  ocad_small main_gap;     // main gap C
  ocad_small sec_gap;      // gap D
  ocad_small end_gap;      // gap E
  ocad_small main_sym;     // -1: at least 0 gaps/symbols
                           // 0: at least 1 gap/symbol
                           // 1: at least 2 gaps/symbols etc.
                           // for OCAD 6 only the values 0 and 1 are allowed
  ocad_small n_sym;        // n of symbols
  ocad_small prim_sym_d;   // distance
  ocad_word  dbl_mode;     // Mode (Double line page)
  ocad_word  dbl_flags;    // low order bit is set if Fill is checked
  ocad_small dbl_fill_c;   // fill color
  ocad_small dbl_left_c;   // left line color
  ocad_small dbl_right_c;  // right line color
  ocad_small dbl_width;    // width
  ocad_small dbl_left_w;   // left line width
  ocad_small dbl_right_w;  // right line width
  ocad_small dbl_length;   // Dashed/Distance a
  ocad_small dbl_gap;      // Dashed/Gap
  ocad_small dbl_res[3];   // reserved
  ocad_word  dec_mode;     // Decrease mode: 0: off, 1: decreasing towards the end,
                           //                2: decreasing towards both ends
  ocad_small dec_last;     // Last symbol
  ocad_small dec_res;      // reserved
  ocad_small fr_color;     // OCAD 6: reserved, OCAD 7/8: color of the framing line
  ocad_small fr_width;     // OCAD 6: reserved, OCAD 7/8: width of the framing line
  ocad_small fr_style;     // OCAD 6: reserved, OCAD 7/8: style of the framing line
                           // 0: flat cap/bevel join, 1: round cap/round join,
                           // 4: flat cap/miter join
  ocad_small prim_dsize;   // number or coordinates (8 bytes) for the Main symbol A
                           // which follow this structure
                           // Each symbol header counts as 2 coordinates (16 bytes)
                           // The maximum value is 512
  ocad_small sec_dsize;    // number or coordinates (8 bytes) for the Secondary symbol
                           // which follow the Main symbol A
                           // Each symbol header counts as 2 coordinates (16 bytes)
                           // The maximum value is 512
  ocad_small corn_dsize;   // number or coordinates (8 bytes) for the Corner symbol
                           // which follow the Secondary symbol
                           // Each symbol header counts as 2 coordinates (16 bytes)
                           // The maximum value is 512
  ocad_small start_dsize;  // number or coordinates (8 bytes) for the Start symbol C
                           // which follow the Corner symbol
                           // Each symbol header counts as 2 coordinates (16 bytes)
                           // The maximum value is 512
  ocad_small end_dsize;    // number or coordinates (8 bytes) for the End symbol D
                           // which follow the Start symbol
                           // Each symbol header counts as 2 coordinates (16 bytes)
                           // The maximum value is 512
  ocad_small r3;
};

struct ocad8_ltext_symb : ocad8_base_symb{
 // todo
};
struct ocad8_area_symb : ocad8_base_symb{
 // todo
};
struct ocad8_text_symb : ocad8_base_symb{
 // todo
};
struct ocad8_rectangle_symb : ocad8_base_symb{
 // todo
};

/**********************************/

// Objects structure

// Each Index block contains the file position of 256 objects.
// In addition it contains the symbol number and the extent (where on
// the map the object is located) of each object.

struct ocad8_ind{
  ocad_tcoord lower_left; // lower left corner of a rectangle covering the
                          // entire object. All flag bits are set to 0
  ocad_tcoord upper_right;// upper right corner of a rectangle covering the
                          // entire object. All flag bits are set to 0
  ocad_long pos;          // file position of the object
  ocad_word len;          // OCAD 6 and 7: size of the object in the file in bytes
                          // OCAD 8: number of coordinate pairs, the size of the object
                          // in the file is then calculated by 32 + 8*Len
                          // Note: this is reserved space in the file, the effective length
                          // of the object may be shorter
  ocad_small sym;         // symbol number. Deleted objects are marked with Sym=0
};

struct ocad8_ind_bl{
  ocad_long next; // file position of the next block or 0
  ocad8_ind index[256];
};


struct ocad9_ind{
// todo
};

struct ocad9_ind_bl{ // 10296 bytes
  ocad_long next;  // file position of the next symbol block or 0
  ocad9_ind index[256]; // file positions (or 0) of symbols.
};

// For calculating LowerLeft and UpperRight the value Extent from
// the corresponding symbol is added/subtracted to the outermost
// coordinates of the object. The rectangle covers everything
// which appears from the object.

/**********************************/


struct ocad78_element{
  ocad_small sym;         // symbol number
  ocad_byte otype;        // object type
  ocad_byte unicode;      // OCAD 6/7: must be 0
                          // OCAD 8: 1 if the text is Unicode
  ocad_small n;           // number of coordinates in the Poly array
  ocad_small ntext;       // number of coordinates in the Poly array used for storing text
                          // > 0 only for line text objects and formatted text objects
  ocad_small angle;       // Angle, unit is 0.1 degrees
                          // used for: point object, area objects with structure,
                          // unformatted and formatted text objects, rectangle objects
  ocad_small r1;          // reserved
  ocad_long  res_height;  // reserved for future use to store a height information
  char res_id[16];        // reserved
};

struct ocad7_element : ocad78_element{
  ocad_tcoord crd[2000]; // coordinates of the object followed by a zero-terminated
                         // string if nText > 0
};

struct ocad8_element : ocad78_element{
  ocad_tcoord crd[32768]; // coordinates of the object followed by a zero-terminated
                          // string if nText > 0
};

// Limits in OCAD 6 and 7:
// n + ntext <= 2000, ntext <= 1024 (8191 characters + terminating zero)
// The number of bytes of an object (as used in the TIndex structure) can be calculated
// as Index.Len := 32 + 8 * nItem + 8 * nText;

// Limits in OCAD 8:
// n + ntext <= 32768, ntext <= 1024 (8191 characters + terminating zero)
// Index.Len := nItem+nText

/**********************************/

// Setup structure

// todo

// can be shorter, then padded by 0s
