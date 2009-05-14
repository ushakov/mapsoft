// Получение fig-библиотеки со всеми знаками

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../libzn/zn_lists.h"

using namespace std;

void usage(){
    cerr << "usage: zn_mkfiglib <conf_file> <out_dir>\n";
    exit(0);
}

main(int argc, char** argv){

  if (argc != 3) usage();
  string conf_file = argv[1];
  string out_dir = argv[2];

  zn::zn_conv z(conf_file);
  for (map<int, zn::zn>::const_iterator i = z.znaki.begin(); i!=z.znaki.end(); i++){
    fig::fig_world F;
    list<fig::fig_object> l=z.make_pic(make_sample(i), i->first);
    F.insert(F.end(), l.begin(), l.end());
    ostringstream name;
    name << out_dir << "/0x" << setw(6) << setfill('0') << setbase(16) << i->first << setbase(10) << ".fig";
    ofstream out(name.str().c_str());
    fig::write(out, F);
  }
}
