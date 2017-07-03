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

  assert(O1.exists("d") == true);
  assert(O1.exists("e") == false);
  assert(O1.find("d")->second == "123.1 ");

  /////////////////////////////////////////////
  // check_unknown()
  try {
    std::list<std::string> k = {"int","d","a"};
    O1.check_unknown(k);
  }
  catch (Err e) {
    std::cerr << e.get_error() << "\n";
    assert(false);
  }

  try {
    std::list<std::string> k = {"a", "b"};
    O1.check_unknown(k);
    assert(false);
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "unknown options: d, int");
  }

  try {
    std::list<std::string> k = {"a", "b", "d"};
    O1.check_unknown(k);
    assert(false);
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "unknown option: int");
  }
  /////////////////////////////////////////////

}
