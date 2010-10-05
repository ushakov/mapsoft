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
    cerr << "usage: " << argv[0] << " <in fig|mp> <out fig|mp>\n";
    exit(1);
  }

  vmap::world V;
  if (!V.get(argv[1])) exit(1);
  if (!V.put(argv[2])) exit(1);

}


