#include <iostream>
#include <vector>
#include <string>

#include "menubar.h"




main(){
  MenuBar M;

  std::string addr;

  do {
    std::cout << "===============\n";
    getline(std::cin, addr, '\n');
    M.add_item(addr);
    std::cout << M.get_xml();
  } while (addr!="");
}
