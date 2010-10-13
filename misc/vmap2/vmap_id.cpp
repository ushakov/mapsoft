#include <sstream>
#include <iomanip>
#include <cstdlib>


namespace vmap{

unsigned short id_counter=0;

// make unique id
std::string make_id(void){
  std::ostringstream s;
  s << std::hex
    << std::uppercase
    << std::setfill('0')
    << std::setw(8)
    << time(NULL) << "-"          // creation time
    << std::setw(4)
    << (++id_counter) << "-"      // counter
    << std::setw(4)
    << (rand() % 0xFFFF);         // random
  return s.str();
}

}