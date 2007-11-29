#include <iostream>
#include <vector>
#include <string>

// 
std::vector<std::string> split(const std::string & str, const std::string & delim){
  std::vector<std::string> ret;
  std::string::size_type i1 = 0, i2 = 0;
  do {
    i2 = str.find(delim, i1);
    if (i2 == std::string::npos) i2 = str.size();
    std::cout <<  i1 << " " << i2 << "\n";
    ret.push_back(str.substr(i1, i2-i1));
    i1=i2+delim.size();
  }while (i2!=str.size());
  return ret;
}

main(){
  std::vector<std::string> v = split("sss/d/s/s/s", "/");
  for (int i = 0; i<v.size(); i++ ) std::cout << v[i] << "\n";
}
