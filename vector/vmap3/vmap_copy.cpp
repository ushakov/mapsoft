#include <string>
#include <cstring>
#include "vmap.h"
#include "libfig/fig.h"

using namespace std;

bool testext(const string & nstr, const char *ext){
    int pos = nstr.rfind(ext);
    return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

main(int argc, char **argv){

  if (argc!=3){
    cerr << "usage: " << argv[0] << " <in.fig> <out.fig>\n";
    exit(1);
  }

  string ifile = argv[1];
  string ofile = argv[2];

  fig::fig_world F;
  mp::mp_world   M;
  vmap::world V;

  if (testext(ifile, ".fig")){
    if (!fig::read(ifile.c_str(), F)) {
      cerr << "ERR: bad fig file " << ifile << "\n"; exit(1);
    }
    V.get(F);
  }
  else if (testext(ifile, ".mp")){
    if (!mp::read(ifile.c_str(), M)) {
      cerr << "ERR: bad mp file " << ifile << "\n"; exit(1);
    }
    V.get(M);
  }
  else { cerr << "ERR: input file is not .fig or .mp\n"; exit(1);}

//

  fig::fig_world F1;
  mp::mp_world   M1;

  // читаем выходной файл, дописываем туда новые объекты (фильтруя их), записываем
  if (testext(ofile, ".fig")){
    if (!fig::read(ofile.c_str(), F1)) {
      cerr << "ERR: bad fig file " << ofile << "\n"; return 1;
    }
    V.put(F1, true, true);
    fig::write(ofile, F1);
  }
  else if (testext(ofile, ".mp")){
    mp::read(ofile.c_str(), M1);
    V.put(M1);
    mp::write(ofile, M1);
  }
  else { cerr << "ERR: output file is not .fig or .mp\n"; return 1; }

}


