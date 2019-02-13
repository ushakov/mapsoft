#include <cassert>
#include <sstream>
#include "opts.h"

int
main(){
try{
  Opts O1;
  O1.put("int", 123);
  assert ( O1.get<int>("int") == 123 );
  assert ( O1.get<std::string>("int") == "123" );

  O1.put("int", "123a");
  try {
    O1.get<int>("int");
  }
  catch (Err E){
    assert (E.get_message() == "can't parse value: 123a");
  }

  O1.put("d", "123.1 ");
  try {
    O1.get("d", 1.0);
  }
  catch (Err E){
    assert (E.get_message() == "can't parse value: 123.1 ");
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
  // dump and parse simple options:

  // put Opts inside Opts!
  O1.put("opts", O1);

  std::ostringstream os;
  os << O1;
  //std::cerr << os.str() << "\n";
  assert(os.str() == "{\"d\": \"123.1 \", \"int\": \"123a\","
                     " \"opts\": \"{\\\"d\\\": \\\"123.1 \\\", \\\"int\\\": \\\"123a\\\"}\"}");

  std::istringstream is(os.str());
  Opts O2;
  O2.put("a", 1); // these fields should disappear
  O2.put("d", 2);
  is >> O2; // read O2 from is
  os.str(std::string()); // clear the stream
  os << O2;
  //std::cerr << os.str() << "\n";
  assert(os.str() == "{\"d\": \"123.1 \", \"int\": \"123a\","
                     " \"opts\": \"{\\\"d\\\": \\\"123.1 \\\", \\\"int\\\": \\\"123a\\\"}\"}");

  os.str(std::string()); // clear the stream
  os << O2.get<Opts>("opts");
  //std::cerr << os.str() << "\n";
  assert(os.str() == "{\"d\": \"123.1 \", \"int\": \"123a\"}");

  {
    std::istringstream is("{} ");
    is >> O1;
    assert(O1.size() == 0);
  }

  // some error cases
  try {
    std::istringstream is("[1,2,3]");
    is >> O1;
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "a JSON object with string fields expected");
  }

  try {
    std::istringstream is("{");
    is >> O1;
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "string or '}' expected near end of file");
  }

  try {
    std::istringstream is("{a: 1}");
    is >> O1;
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "string or '}' expected near 'a'");
  }

  try {
    std::istringstream is("{b: \"2\"}");
    is >> O1;
  }
  catch (Err e) {
    //std::cerr << e.get_error() << "\n";
    assert(e.get_message() == "string or '}' expected near 'b'");
  }

}
catch (Err e) {
  std::cerr << e.get_error() << "\n";
  return 1;
}
return 0;
}
