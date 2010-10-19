#include "options/strvec.h"
#include <boost/lexical_cast.hpp>

main(){
  StrVec v;
  v.push_back("aaa");
  v.push_back("bbb\nccc\\ddd\\\n\\\\eee");
  std::cout << v;

  std::cout << "---\n";

  v=boost::lexical_cast<StrVec>("aaa\\\nbbb\naaa\nbb\\\\b\\\\a");

  for (int i=0; i<v.size(); i++){
    std::cout << ">>>" << v[i] << "<<<\n";
  }

  std::cout << "---\n";

}