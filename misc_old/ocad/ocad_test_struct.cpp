#include "ocad.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

/*******************************************/

enum OCAD_FileType{
  NORMAL_MAP,
  COURSE_SETTINGS
};

struct OCAD_Symbol{
};

struct OCAD_Object{
};

struct OCAD_File{
  OCAD_FileType  type;
  std::vector<OCAD_Symbol> symbols;
  std::vector<OCAD_Object> objects;

  OCAD_File(){
    type=NORMAL_MAP;
  }
};

/*******************************************/

void
ocad_test_struct(){

  assert(sizeof(ocad_tcoord) == 8);
  assert(sizeof(ocad_tcmyk) == 4);
  assert(sizeof(ocad_tcolorinfo) == 72);
  assert(sizeof(ocad_tcolorsep) == 20);

  assert(sizeof(ocad_comm_head) == 8);
  assert(sizeof(ocad8_head) == 48);
  assert(sizeof(ocad9_head) == 48);
  assert(sizeof(ocad8_symb_head) == 19096);

  assert(sizeof(ocad8_symb_bl) == 1028);
  assert(sizeof(ocad8_base_symb) == 349);
  assert(sizeof(ocad8_sym_el) == 4112);
  // todo
}

// convert pascal-string to c-string
// maxlen includes first byte;
void str_pas2c(char * str, int maxlen){
  int l = str[0];
  if (l>=maxlen) l=maxlen-1;
  for (int i=0; i<l; i++) str[i]=str[i+1];
  str[l]='\0';
}
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

OCAD_Symbol ocad8_read_symbol(FILE *F, int pos, bool verbose=false){
  OCAD_Symbol ret;
  if (fseek(F, pos, SEEK_SET)!=0) throw "can't seek file to symbol";
  ocad8_base_symb s;
  if (fread(&s, 1, sizeof(s), F)!=sizeof(s))
    throw "can't read OCAD 6/7/8 base symbol\n";
  str_pas2c(s.descr, 32);
  if (verbose)
    std::cout << "    Symbol:\n"
              << "      size (b): " << s.size   << "\n"
              << "      id:       " << s.symb   << "\n"
              << "      otype:    " << s.otype  << "\n"
              << "      stype:    " << (int)s.stype  << "\n"
              << "      flags:    " << (int)s.flags  << "\n"
              << "      extent:   " << s.extent << "\n"
              << "      selected: " << s.selected << "\n"
              << "      status:   " << (int)s.status   << "\n"
              << "      tool:     " << s.tool     << "\n"
              << "      description: " << s.descr << "\n";
  // todo colors, icon...
  // todo real symbols
}

OCAD_File ocad8_read(FILE *F, bool verbose=false){
  OCAD_File ret;

  ocad8_head h;
  rewind(F);
  if (fread(&h, 1, sizeof(h), F)!=sizeof(h))
    throw "can't read OCAD 6/7/8 header\n";

  if ((h.version!=6) &&
      (h.version!=7) &&
      (h.version!=8)) throw "use ocad8_read() for reading OCAD6/7/8 only";

  // version, subversion, file type:
  if (verbose)
    std::cout << "OCAD6/7/8 header:\n"
              << "  version:    " << h.version << "\n"
              << "  subversion: " << h.subversion << "\n"
              << "  file_type:  " << h.file_type << "\n";
  if ((h.version == 6) && (h.file_type !=0)){
     std::cerr << "warning: fixed bad section_mark: " << h.file_type << " for OCAD6\n";
     h.file_type=0;
  }
  if ((h.version == 7) && (h.file_type !=7)){
     std::cerr << "warning: fixed bad section_mark: " << h.file_type << " for OCAD7\n";
     h.file_type=7;
  }
  if (h.version == 8){
     if ((h.file_type !=2) && (h.file_type !=3)){
       std::cerr << "warning: fixed bad file type: " << h.file_type << " for OCAD8\n";
       h.file_type=2;
     }
     if (h.file_type==3) ret.type=COURSE_SETTINGS;
  }

  // data blocks:
  if (verbose)
    std::cout << "  first symbol block position:  " << h.f_symb_bl << "\n"
              << "  first index block position:   " << h.f_index_bl << "\n"
              << "  setup record position:        " << h.setup_pos << "\n"
              << "  setup size:                   " << h.setup_size << "\n"
              << "  file information position:    " << h.info_pos << "\n"
              << "  file information size:        " << h.info_size << "\n";
  if (verbose && (h.version==8))
    std::cout << "  first string index block pos: " << h.v8_f_str_bl << "\n";

  // read symbol header:
  ocad8_symb_head sh;
  if (fread(&sh, 1, sizeof(sh), F)!=sizeof(sh))
    throw "can't read OCAD 6/7/8 symbol header\n";

  if (verbose)
    std::cout << "Symbol Header:\n"
              << "  number of colors:             " << sh.ncolors << "\n"
              << "  number of color separations:  " << sh.ncolsep << "\n"
              << "  frequency/angle of cyan color separation:    "
                    << sh.cyan_freq << "/" << sh.cyan_ang << "\n"
              << "  frequency/angle of magenta color separation: "
                    << sh.magenta_freq << "/" << sh.magenta_ang << "\n"
              << "  frequency/angle of yellow color separation:  "
                    << sh.yellow_freq << "/" << sh.yellow_ang << "\n"
              << "  frequency/angle of black color separation:   "
                    << sh.black_freq << "/" << sh.black_ang << "\n"
              << "  colors:\n";

  for (int i=0; i < MIN(256,sh.ncolors); i++){
    if (sh.cols[i].num==0) continue;
    str_pas2c(sh.cols[i].name, 32);
    if (verbose)
      std::cout << "    " << sh.cols[i].num << " (" 
        << (int)sh.cols[i].color.C << ","
        << (int)sh.cols[i].color.M << ","
        << (int)sh.cols[i].color.Y << ","
        << (int)sh.cols[i].color.B << ") \"" << sh.cols[i].name << "\"";
    for (int j=0; j < MIN(24, sh.ncolsep); j++){
      if (verbose)
        std::cout << " " << sh.cols[i].sep_per; 
    }
    if (verbose)
      std::cout << "\n";
  }
  if (verbose)
    std::cout << "  color separations:\n";
  for (int i=0; i < MIN(24,sh.ncolsep); i++){
    str_pas2c(sh.seps[i].name, 16);
    if (verbose)
      std::cout << "    " << sh.seps[i].name << " "
        << sh.seps[i].raster_freq << "/"
        << sh.seps[i].raster_ang << "\n";
  }

  // read symbols
  if (verbose) std::cout << "Symbols:\n";
  int index_pos=h.f_symb_bl;
  while (index_pos!=0){
    if (verbose) std::cout << "  Index at: "<< index_pos << "\n";
    if (fseek(F, index_pos, SEEK_SET)!=0) throw "can't seek file to symbol index block";
    ocad8_symb_bl sbl;
    if (fread(&sbl, 1, sizeof(sbl), F)!=sizeof(sbl))
      throw "can't read OCAD 6/7/8 symbol index block\n";
    index_pos=sbl.next;
    for(int i=0; i<256; i++){
      int s_pos = sbl.symb_pos[i];
      if (s_pos==0) continue;
      if (verbose) std::cout << "    Symbol at: "<< s_pos << "\n";
      ret.symbols.push_back(ocad8_read_symbol(F, s_pos, verbose));
    }
  }
  // read objects
  // .. todo
  // read setup
  // .. todo
  // read info
  // .. todo
  // read strings
  // .. todo
  return ret;
}

OCAD_File ocad9_read(FILE *F, bool verbose=false){
  OCAD_File ret;

  ocad9_head h;
  rewind(F);
  if (fread(&h, 1, sizeof(h), F)!=sizeof(h))
    throw "can't read OCAD 9 header\n";

  if (h.version!=9) throw "use ocad9_read() for reading OCAD9 only";

  // version, subversion, file type:
  if (verbose)
    std::cout << "OCAD9 header:\n"
              << "  version:    " << h.version << "\n"
              << "  subversion: " << h.subversion << "\n"
              << "  file_type:  " << h.file_type << "\n";

  if ((h.file_type !=0) && (h.file_type !=1)){
    std::cerr << "warning: fixed bad file type: " << h.file_type << " for OCAD9\n";
    h.file_type=0;
  }
  if (h.file_type==1) ret.type=COURSE_SETTINGS;
  if (verbose)
    std::cout << "  file type:  "
              << ((ret.type==COURSE_SETTINGS)?"course setting":"normal map") << "\n";

  // data blocks:
  if (verbose)
    std::cout << "  first symbol block position:  " << h.f_symb_bl << "\n"
              << "  index block position:         " << h.index_bl << "\n"
              << "  string index block position:  " << h.str_bl << "\n"
              << "  file name position:           " << h.fname_pos << "\n"
              << "  file name size:               " << h.fname_size << "\n";

  return ret;
}


OCAD_File
ocad_read(const char * fname, bool verbose=false){

  // open file
  FILE * F = fopen(fname, "r");
  if (!F) throw "can't open file";

  // read common part of header and detect OCAD version
  int version=0;
  rewind(F);
  ocad_comm_head h;
  if ((fread(&h, 1, sizeof(h), F)==sizeof(h)) &&
      (h.ocad_mark==0x0CAD)) version=h.version;

  switch (version){
    case 0: throw "not if OCAD format";
    case 6:
    case 7:
    case 8:
      return ocad8_read(F, verbose);
    case 9:
      return ocad9_read(F, verbose);
    default:
      throw "unsupported OCAD version";
  }
  return OCAD_File();
}

int
main(int argc, char **argv){
  if (argc <2) {
    printf("use: %s <file>\n", argv[0]);
    exit(0);
  }
  ocad_test_struct();
  try{
    ocad_read(argv[1], true);
  }
  catch (const char * msg){
    std::cerr << argv[1] << ": " << msg << "\n";
  }
}
