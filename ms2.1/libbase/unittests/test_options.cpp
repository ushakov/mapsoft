#include <iostream>
#include <sstream>
#include "../options.h"

//unit-test for Options object
using namespace std;
using namespace mapsoft;


class myclass{
  public:
  string value;
  myclass(){}
  myclass(const string & s){ value = s;}
};

std::istream & operator>> (std::istream & s, myclass & t){
  s >> t.value;
  return s;
}
std::ostream & operator<< (std::ostream & s, const myclass & t){
  s << t.value;
  return s;
}

main(){
  std::cout << "Test for Options class: \n";

  Options O;
  int err = 0;

  string s0="mystring", s1="";
  int    i0 = 5,    i1=0;
  double d0 = 0.01, d1=0.1;
  bool   b0 = true, b1=false;


  // запись/чтение новых ключей
  O.put("s", s0);
  O.put("i", i0);
  O.put("d", d0);
  O.put("b", b0);

  s1=O.get("s", s1);
  i1=O.get("i", i1);
  d1=O.get("d", d1);
  b1=O.get("b", b1);

  if (s1!= s0) {
    std::cout << "put/get test for string failed: "<< s0 << " != " << s1 << "\n"; err++;}
  if (i1!= i0) {
    std::cout << "put/get test for int failed: "<< i0 << " != " << i1 << "\n"; err++;}
  if (d1!= d0) {
    std::cout << "put/get test for double failed: "<< d0 << " != " << d1 << "\n"; err++;}
  if (b1!= b0) {
    std::cout << "put/get test for bool failed: "<< b0 << " != " << b1 << "\n"; err++;}

  // перезапись ключа
  i0=10;
  O.put("i", i0);
  i1=O.get("i", i1);
  if (i1!= i0) {
    std::cout << "overwriting test for int failed: "<< i0 << " != " << i1 << "\n"; err++;}

  // проверка наличия ключа
  if (O.exists("unex") || (!O.exists("s")) ){
    std::cout << "exists test failed\n"; err++;}

  // при чтении несуществующего ключа сохраняется значение по умолчанию
  s1=O.get("unex", s1);
  if (s1!=s0){
    std::cout << "getting unexisting key breaks value: "<< s0 << " != " << s1 << "\n"; err++;}

  // в случае ошибки при разборе, значение переменной не меняется
  i1=O.get("s", i1);
  if (i1!=i0){
    std::cout << "broken value after bad_lexical_cast in get: "<< i0 << " != " << i1 << "\n"; err++;}

  // запись всего объекта Options
  string options_str = "b=1 d=0.01 i=10 s=mystring";
  ostringstream s; s << O;
  if (s.str() != options_str) {
    std::cout << "operator<< test failed: \"" << s.str() << "\" != \"" << options_str << "\"\n"; err++;}

  // для собственного типа с определенными операторами << и >>
  myclass m0, m1;
  m0.value="test";
  O.put("my1", m0);
  m1=O.get("my1", m1);
  if (m0.value != m1.value) {
    std::cout << "myclass1 test failed\n"; err++;}

  if (err == 0){ cout << " -- OK\n"; exit(0);}
  else {cout << " -- " << err << " ERRORS\n"; exit(1);}


}
