#include "vmap.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>


namespace vmap{

unsigned short id_counter=0;

// make unique id
id_t make_id(void){
  std::ostringstream s;
  s << std::hex << std::setw(8) << std::setfill('0')
    << time(NULL) << "-"          // creation time
    << std::setw(4)
    << (++id_counter) << "-"      // counter
    << (rand() % 0xFFFF);         // random
  return s.str();
}


}