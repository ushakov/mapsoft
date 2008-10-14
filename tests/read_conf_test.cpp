#include <iostream>
#include "../utils/read_conf.h"

using namespace std;

int main(int argc, char **argv){
  Options opts;
  list<string> non_opts;

  if (!read_conf(argc, argv, opts, non_opts)){
    cerr << "Error reading options\n";
  }

  cout << "Options: " << opts << "\n";
  for (list<string>::const_iterator i=non_opts.begin(); i!=non_opts.end(); i++)
    cout << "Non-options: " << *i << "\n";
}
