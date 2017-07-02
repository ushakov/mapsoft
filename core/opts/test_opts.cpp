#include <cassert>
#include "opts.h"

main(){
  Opts O1;
  O1.put("int", 123);
  assert ( O1.get<int>("int") == 123 );
  assert ( O1.get<std::string>("int") == "123" );

  O1.put("int", "123a");
  try {
    O1.get<int>("int");
  }
  catch (Err E){
    assert (E.get_message() == "can't parse value of int: 123a");
  }

  O1.put("d", "123.1 ");
  try {
    O1.get("d", 1.0);
  }
  catch (Err E){
    assert (E.get_message() == "can't parse value of d: 123.1 ");
  }

  assert(O1.find("d")->second == "123.1 ");

}
