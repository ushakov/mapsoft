#include <iostream>
#include "read_conf.h"

int main(int argc, char **argv){
  Options opts;
  std::vector<std::string> non_opts;

  if (!read_conf(argc, argv, opts, non_opts)){
    std::cerr << "Error reading options\n";
  }

  std::cout << "Options: " << opts << "\n";
  for (std::vector<std::string>::const_iterator i=non_opts.begin(); i!=non_opts.end(); i++)
    std::cout << "Non-options: " << *i << "\n";
}
