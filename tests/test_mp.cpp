#include <iostream>
#include <mp/mp.h>

using namespace std;

int main(int argc, char** argv){
    if (argc < 3){
      cerr << "usage: test_mp <in> <out>\n";
      exit(1);
    }

    mp::mp_world M;
    if (!mp::read(argv[1], M)) {
      cerr << "ERR: bad mp file\n"; exit(1);
    }

    mp::write(argv[2], M);
}
