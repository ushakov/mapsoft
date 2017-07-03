#include <cassert>
#include "err.h"

main(){
  try {
    throw Err() << "text " << 123;
  }
  catch (Err E){
    assert (E.get_message() == "text 123");
    assert (E.get_error()   == "error: text 123");
    assert (E.get_code() == -1);
    assert (E.get_domain() == "");
  }

  Err::set_domain("test");
  try {
    throw Err(3) << "text " << 123;
  }
  catch (Err E){
    assert (E.get_message() == "text 123");
    assert (E.get_error()   == "test error: text 123");
    assert (E.get_code()    == 3);
    assert (E.get_domain()  == "test");
  }
}
