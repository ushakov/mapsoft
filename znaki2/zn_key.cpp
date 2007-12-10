#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "zn_key.h"

#include <boost/lexical_cast.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

namespace zn{

std::ostream & operator<< (std::ostream & s, const zn_key & t){
  s << "0x" << std::setbase(16) << std::setw(6) << std::setfill('0') 
    << t.type << std::setbase(10) << " " 
    << t.time << " "
    << t.id << "@" << t.map << " ";
  if (t.source != ""){
    if (t.sid!=0) s << t.sid << "@";
    s << t.source;
  }
  return s;
}

std::istream & operator>> (std::istream & s, zn_key & t){
  using namespace boost::spirit;
  std::string str;
  std::getline(s, str);

  zn_key newkey;
  std::string timestr;
  std::string classstr;

  if (str.empty()) {t = newkey; return s;}

  if (parse(str.c_str(),
      *blank_p >> 
         ch_p('0') >> 'x' >> hex_p[assign_a(newkey.type)] >> +blank_p >>
         (+graph_p >> +blank_p >> +graph_p)[assign_a(timestr)] >> +blank_p >>
         uint_p[assign_a(newkey.id)] >> '@' >> (*(alnum_p|'_'))[assign_a(newkey.map)] >>
         !(+blank_p >> 
           ( 
             (uint_p[assign_a(newkey.sid)] >> '@' >> (+alnum_p)[assign_a(newkey.source)]) ||
             (+alnum_p)[assign_a(newkey.source)]
           )
         ) >> *blank_p     
     ).full){
    if (!timestr.empty()) newkey.time = boost::lexical_cast<Time>(timestr);
  } else {
    std::cerr << "zn_key: can't find valid key in " << str << "\n";
    newkey = zn_key();
  }
  t = newkey;
  return s;
}

std::ostream & operator<< (std::ostream & s, const zn_label_key & t){
  s << "+" << t.id << "@" << t.map;
  return s;
}

std::istream & operator>> (std::istream & s, zn_label_key & t){
  using namespace boost::spirit;
  std::string str;
  std::getline(s, str);

  if (str.empty()) {t = zn_label_key(); return s;}

  if (parse(str.c_str(),
      *blank_p >> ch_p('+') >> 
      uint_p[assign_a(t.id)] >> '@' >> (+(alnum_p|'_'))[assign_a(t.map)] >> *blank_p
     ).full){
    return s;
  } else {
    t = zn_label_key(); return s;
  }
}

//============================================================

// Извлечь ключ из комментария (2-я строчка) к fig-объекту
zn_key get_key(const fig::fig_object & fig){
  if (fig.comment.size() < 2) return zn_key();
  return boost::lexical_cast<zn_key>(fig.comment[1]);
}

// Извлечь ключ подписи из комментария (2-я строчка) к fig-объекту
zn_label_key get_label_key(const fig::fig_object & fig){
  if (fig.comment.size() < 2) return zn_label_key();
  return boost::lexical_cast<zn_label_key>(fig.comment[1]);
}

// Извлечь ключ из комментария (1-я строчка) к mp-объекту
zn_key get_key(const mp::mp_object & mp){
  zn_key ret;
  if (mp.Comment.size() > 0) ret = boost::lexical_cast<zn_key>(mp.Comment[0]);
  return ret;
}

// поместить ключ в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_key & key){
  if (fig.comment.size()<2) fig.comment.resize(2);
  fig.comment[1] = boost::lexical_cast<std::string>(key);
}

// поместить ключ подписи в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_label_key & key){
  if (fig.comment.size()<2) fig.comment.resize(2);
  fig.comment[1] = boost::lexical_cast<std::string>(key);
}
void add_key(std::list<fig::fig_object> & l, const zn_label_key & key){
  for (std::list<fig::fig_object>::iterator i = l.begin(); i!=l.end(); i++)
    add_key(*i, key);
}

// поместить ключ в комментарий к mp-объекту
void add_key(mp::mp_object & mp, const zn_key & key){
  if (mp.Comment.size()<1) mp.Comment.resize(1);
  mp.Comment[0] = boost::lexical_cast<std::string>(key);
}

} // namespace

