#include <iostream>
#include <sstream>
#include <options/options.h>

//unit-test for Options object
using namespace std;


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

int
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
  string options_str  = "b=\"1\" d=\"0.01\" i=\"10\" s=\"mystring\"";
  string options_str1 = "b=\"1\"\nd=\"0.01\"\ni=\"10\"\ns=\"mystring\"";
  ostringstream st1; st1 << O;
  if (st1.str() != options_str1) {
    std::cout << "operator<< test failed: \"" << st1.str() << "\" != \"" << options_str1 << "\"\n"; err++;}

  // чтение Options
  istringstream st2(st1.str());
  Options O2;
  O2.put("ttt", int(1));
  O2.put("d", string("s"));

  cerr << "str: " << st1.str() << "\n";

  cerr << "O2: " << O2 << "\n";
  st2 >> O2;
  cerr << "O2: " << O2 << "\n";

  // для собственного типа с определенными операторами << и >>
  myclass m0, m1;
  m0.value="test";
  O.put("my1", m0);
  m1=O.get("my1", m1);
  if (m0.value != m1.value) {
    std::cout << "myclass1 test failed\n"; err++;}

  if (err == 0){ cout << "\n\n -- OK\n"; return 0;}
  cout << "\n\n -- " << err << " ERRORS\n";
  return 1;
}
