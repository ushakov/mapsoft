// Получение fig-библиотеки со всеми знаками

#include <string>
#include <sstream>
#include <iomanip>
#include "vmap/zn_lists.h"
#include <sys/stat.h>

using namespace std;

void usage(){
    cerr << "usage: zn_mkfiglib <style> <out_dir>\n";
    exit(1);
}

main(int argc, char** argv){

  if (argc != 3) usage();
  const char * style   = argv[1];
  const char * out_dir = argv[2];

  if (mkdir(out_dir, 0755) != 0){
    cerr << "can't create directory: " << out_dir << "\n";
    exit(1);
  }

  zn::zn_conv z(style);
  for (map<int, zn::zn>::const_iterator i = z.znaki.begin(); i!=z.znaki.end(); i++){
    fig::fig_world F;
    list<fig::fig_object> l=z.make_pic(make_sample(i), i->first);
    F.insert(F.end(), l.begin(), l.end());
    ostringstream name;
    name << out_dir << "/0x" << setw(6) << setfill('0')
         << setbase(16) << i->first << ".fig";
    fig::write(name.str(), F);
  }
}
