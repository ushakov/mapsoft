#include <iostream>
#include <options/read_conf.h>

using namespace std;

int main(int argc, char **argv){
  Options opts;

  if (!read_conf(argc, argv, opts)){
    cerr << "Error reading options\n";
  }

  cout << "  Options:\n" << opts << "\n";
}
