#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <cstdlib>

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
    if (t.sid!=0){
      std::cerr << "Warning: sid!=0\n";
      s << t.sid << "@";
    }
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

  uint_parser<id_type, 10,1,-1> id_p;

  if (parse(str.c_str(),
      *blank_p >> 
         ch_p('0') >> 'x' >> hex_p[assign_a(newkey.type)] >> +blank_p >>
         (+graph_p >> +blank_p >> +graph_p)[assign_a(timestr)] >> +blank_p >>
         id_p[assign_a(newkey.id)] >> '@' >> (+(alnum_p|'_'|'-'))[assign_a(newkey.map)] >>
         !(+blank_p >>
           (
             (uint_p[assign_a(newkey.sid)] >> '@' >> (+(alnum_p|'_'|'-'))[assign_a(newkey.source)]) ||
             (+(alnum_p|'_'|'-'))[assign_a(newkey.source)]
           )
         ) >> *blank_p
     ).full){
    if (!timestr.empty()) newkey.time = boost::lexical_cast<Time>(timestr);
  } else {
//    std::cerr << "zn_key: can't find valid key in " << str << "\n";
    newkey = zn_key();
  }

  // backward compat. Now we don't use sid@source at all
  if (newkey.sid!=0){
    newkey.id=newkey.sid; newkey.sid=0;
    newkey.map=newkey.source; newkey.source="unknown";
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

  uint_parser<id_type, 10,1,-1> id_p;

  if (parse(str.c_str(),
      *blank_p >> ch_p('+') >> 
      id_p[assign_a(t.id)] >> '@' >> (+(alnum_p|'_'|'-'))[assign_a(t.map)] >> *blank_p
     ).full){
    return s;
  } else {
    t = zn_label_key(); return s;
  }
}

//============================================================

unsigned short key_counter=0;

// make unique id
id_type make_id(void){
  return ((id_type)time(NULL) << 32) +     // creation time
         ((id_type)(++key_counter) << 16) +  // counter
         (rand() % 0xFFFF);        // random
}

// make new key with unique id
zn_key make_key(const std::string & map, const std::string & editor){
  zn_key k;
  k.time.set_current();
  k.id     = make_id();
  k.map    = map;
  k.source = editor;
  return k;
}

// Извлечь ключ из комментария (2-я строчка) к fig-объекту
zn_key get_key(const fig::fig_object & fig){
  zn_key k;
  if (fig.comment.size() < 2) return k;
  try {k=boost::lexical_cast<zn_key>(fig.comment[1]);} 
  catch(boost::bad_lexical_cast a){}
  return k;
}

// Извлечь ключ подписи из комментария (2-я строчка) к fig-объекту
zn_label_key get_label_key(const fig::fig_object & fig){
  zn_label_key k;
  if (fig.comment.size() < 2) return k;
  try {k=boost::lexical_cast<zn_label_key>(fig.comment[1]);} 
  catch(boost::bad_lexical_cast a){}
  return k;
}

// Извлечь ключ из комментария (1-я строчка) к mp-объекту
zn_key get_key(const mp::mp_object & mp){
  zn_key k;
  if (mp.Comment.size() < 1) return k;
  try {k=boost::lexical_cast<zn_key>(mp.Comment[0]);} 
  catch(boost::bad_lexical_cast a){}
  return k;
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

// удалить любой ключ (да и всю вторую стоку комментария)
void clear_key(fig::fig_object & fig){
  if (fig.comment.size()>1) fig.comment[1]="";
}

void clear_key(mp::mp_object & mp){
  if (mp.Comment.size()>0) mp.Comment[0]="";
}

void fig_old2new(fig::fig_world & F){

  int n;

  n=0;
  fig::fig_world::iterator i;
  for (i=F.begin(); i!=F.end();i++){
    if ((i->comment.size()<2) || (i->comment[1] != "[skip]")) continue;
    i->comment.clear();
    i->opts["MapType"]="pic";
    n++;
  }
  if (n>0) std::cerr << "converting old-style pics: " << n << "\n";

  // for labels find nearest object point
  // objects with non-empty label

  n=0;
  std::map<std::string, fig::fig_object> objs;
  for (i=F.begin(); i!=F.end();i++){
    if ((i->comment.size()<1) || (i->comment[0] == "")) continue;
    zn_key k = get_key(*i);
    if (k.id==0) continue;
    objs[boost::lexical_cast<std::string>(k.id)+"@"+k.map]=*i;
    n++;
  }
  if (n>0) std::cerr << "old-style objects with text: " << n << "\n";

  n=0;
  i=F.begin();
  while (i!=F.end()){
    zn_label_key k = get_label_key(*i);
    if (k.id==0) {
      i++;
      continue;
    }
    std::string key=boost::lexical_cast<std::string>(k.id)+"@"+k.map;
    if (objs.count(key)<1){
       i=F.erase(i);
       continue;
    }
    fig::fig_object & o = objs[key];
    if ((o.size()<1) || (i->size()<1)){
      i=F.erase(i);
      continue;
    }
    iPoint pt=o[0];
    double dist=pdist(pt,(*i)[0]);
    for (fig::fig_object::iterator p=o.begin(); p!=o.end(); p++){
      if (dist > pdist((*p), (*i)[0])){
        dist=pdist((*p), (*i)[0]);
        pt=(*p);
      }
    }
/*
fig::fig_object x=fig::make_object("2 1 0 1 5 7 4 -1 -1 0.000 0 0 -1 0 0 2");
x.opts.put("key", key);
x.opts.put("txt", i->text);
x.opts.put("dist1", pdist(pt,(*i)[0]));
x.opts.put("dist2", pdist(o[0],(*i)[0]));
x.push_back(pt);
x.push_back((*i)[0]);
F.push_back(x);
*/

    i->opts["MapType"]="label";
    i->opts.put("RefPt", pt);
    i->comment.clear();
    i++;
    n++;
  }
  if (n>0) std::cerr << "converting old-style labels: " << n << "\n";

  n=0;
  for (i=F.begin(); i!=F.end();i++){
    zn_key k = get_key(*i);
    if (k.id==0) continue;
    for (int j=2; j<i->comment.size(); j++) i->comment[j-1]=i->comment[j];
    i->comment.resize(i->comment.size()-1);
    if (k.map == "westra_passes") i->opts["Source"]=k.map;
    n++;
  }
  if (n>0) std::cerr << "converting old-style objects: " << n << "\n";

}

} // namespace

