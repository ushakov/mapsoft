#include <stdint.h>

/**********************************/
typedef int32_t  ocad_long;
typedef int32_t  ocad_int;
typedef int32_t  ocad_uint;
typedef int16_t  ocad_small;
typedef uint16_t ocad_word;
typedef uint16_t ocad_bool;
typedef uint8_t  ocad_byte;
typedef double   ocad_double;

/**********************************/
struct ocad_head{ // 38 bytes
  ocad_small ocad_mark;   // 3245
  ocad_byte  file_type;   // 0: normal map, 1: course setting project, 2: OCAD8 MAP 3: OCAD 8 course setting project
  ocad_byte  file_status; // not used
  ocad_small version;     // 9
  ocad_small subversion;  // (0 for 9.00, 1 for 9.1 etc.)
  ocad_int   first_symb_index_bl; // file position of the first symbol index block
  ocad_int   obj_index_bl;        // file position of object index block -> TObjectIndexBlock    // max 65536 * 256 objects
  ocad_int   res0,res1,res2,res3; // not used
  ocad_long  info_size;      // not used
  ocad_long  str_index_bl;   // file position of string index block -> TStringIndexBlock    // max 256 strings
  ocad_int   file_name_pos;  // file position of file name, used for temporary files and to recovery the file // unsupported in OCAD8?
  ocad_int   file_name_size; // size of the file name, used for temporary files only // unsupported in OCAD8?
  ocad_int   res4;           // not used
};

/**********************************/
struct ocad9_tobj_index{ // 40 bytes
  ocad_int x1,y1,x2,y2; // bounding box (lower left and upper right). all flag bits are set to 0
  ocad_int pos;  // file position of the object -> TElement
  ocad_int len;  // number of coordinate pairs, the size of the object in the file is then calculated by: 32 + 8*Len
                 // Note: this is reserved space in the file, the effective length of the object may be shorter
  ocad_int sym;  // > 0 = symbol number
                 // -3 = image object eg AI object
                 // -2 = graphic object
                 // -1 = imported, no symbol assigned or symbol number
  ocad_byte obj_type;  // 1 = Point object
                       // 2 = Line object
                       // 3 = Area object
                       // 4 = Unformatted text
                       // 5 = Formatted text
                       // 6 = Line text
                       // 7 = Rectangle object
  ocad_byte res0;      // not used
  ocad_byte status;    // 0 = deleted (not undo) (eg from symbol editor or cs)
                       // 1 = normal
                       // 2 = hidden
                       // 3 = deleted for undo
  ocad_byte view_type; // 0 = normal object
                       // 1 = course setting object
                       // 2 = modified preview object
                       // 3 = unmodified preview object
                       // 4 = temporary object (symbol editor or control description)
                       // 10 = DXF import, GPS import
  ocad_small color;    // symbolized objects: color number
                       // graphic object: color number
                       // image object: CYMK color of object
  ocad_small res1;     // not used
  ocad_small imp_layer; // Layer number of imported objects. 0 means no layer number
  ocad_small res2;     // not used
};

struct ocad8_tobj_index{ // 24 bytes
  ocad_int x1,y1,x2,y2; // bounding box (lower left and upper right). all flag bits are set to 0
  ocad_int pos;  // file position of the object -> TElement
  ocad_small points;
  ocad_small symb; // symbol
};

struct ocad9_tobj_index_bl{ //40*256 + 4 = 10244b
  ocad_int next;
  struct ocad9_tobj_index table[256];
};

struct ocad8_tobj_index_bl{ //24*256 + 4 = 6148b
  ocad_int next;
  struct ocad8_tobj_index table[256];
};

/**********************************/
struct ocad9_base_symb{
  ocad_int  size;
  ocad_int  num;
  ocad_byte otp;
  ocad_byte flags;

  ocad_bool selected;
  ocad_byte status;
  ocad_byte drawing_tool;
  ocad_byte cs_mode;
  ocad_byte cs_obj_type;
  ocad_byte cs_cd_flags;
  ocad_int  extent;
  ocad_int  file_pos; //undef
  ocad_small group;
  ocad_small ncolors;
  ocad_small colors[14];
  char       descr[31];
  ocad_byte  icon[483];
};

struct ocad8_base_symb{ // base = 52+32+264 = 348b
  ocad_small size;
  ocad_small num;
  ocad_byte  otp;
  ocad_byte  flags;

  ocad_byte   i[46];
  ocad_byte  descr_size;
  char       descr[31];
  ocad_byte  icon[264];
};

/**********************************/

struct ocad8_tobj{ // 32 byte + points
  ocad_small symb;
  ocad_byte  otp;
  ocad_byte  flags;
  ocad_small points;
  ocad_small s1;
  ocad_int   i3,i4,i5,i6,i7,i8; // not used?
};

