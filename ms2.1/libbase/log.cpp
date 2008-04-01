#include <iostream>
#include "log.h"

namespace mapsoft{

void LOG(const std::string & str){
  std::cerr << str << "\n";
}

}//namespace
