#include <fstream>
#include <iomanip>
#include "zn.h"

#include <boost/lexical_cast.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

namespace zn{


std::ostream & operator<< (std::ostream & s, const zn_key & t){
  if ((t.id==0)&&(t.type==0)&&(t.map=="")) 
    s << "-";
  else if (t.label) 
    s << "+" << t.id << "@" << t.map;
  else  {
    s << "0x" << std::setbase(16) << std::setw(6) << std::setfill('0') 
      << t.type << std::setbase(10) << " " 
      << t.time << " "
      << t.id << "@" << t.map << " ";
    if (t.source != ""){
      if (t.sid!=0) s << t.sid << "@";
      s << t.source;
    }
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

  newkey.label=false;

  if (parse(str.c_str(),
      *blank_p >> (
        ch_p('-') | 
        (ch_p('+') >> uint_p[assign_a(newkey.id)] >> '@' >> (+alnum_p)[assign_a(newkey.map)] 
        )[assign_a(newkey.label, true)] |
        ('0' >> 'x' >> hex_p[assign_a(newkey.type)] >> +blank_p >>
         (+graph_p >> +blank_p >> +graph_p)[assign_a(timestr)] >> +blank_p >>
         uint_p[assign_a(newkey.id)] >> '@' >> (+alnum_p)[assign_a(newkey.map)] >>
         !(+blank_p >> 
           ( 
             (uint_p[assign_a(newkey.sid)] >> '@' >> (+alnum_p)[assign_a(newkey.source)]) ||
             (+alnum_p)[assign_a(newkey.source)]
           )
         )
        )
      ) >> *blank_p     
     ).full){
    if (timestr!="") newkey.time = boost::lexical_cast<Time>(timestr);
  } else {
    if (!str.empty()) std::cerr << "zn_key: can't find valid key in " << str << "\n";
    newkey = zn_key();
  }
  t = newkey;
  return s;
}






// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать

zn_conv::zn_conv(const std::string & conf_file){

  // читаем конф.файл.
  std::ifstream conf(conf_file.c_str());
  if (!conf) {
    std::cerr << "Can't open " << conf_file << "\n";
    exit(0);
  }

  // простой конф.файл: 
  // name
  // descr
  // mp_mask
  // fig_mask
  // fig_mask for text
  // fig-picture
  // fig-picture
  // <blank line>

  zn               z;
  mp::mp_object    m;
  std::string      str;
  while (!conf.eof()){

   do { getline(conf, str); } while (str == "");
   z.name = str;
   getline(conf, z.descr);
   getline(conf, str); 
   z.mp = mp::make_object(str);

   std::string fig_mask; 
   getline(conf, fig_mask); 
   z.fig = fig::make_object(fig_mask);

   std::string txt_mask; 
   getline(conf, txt_mask);
   z.istxt=true;
   if (txt_mask!="") z.txt = fig::make_object(txt_mask);
   else z.istxt = false;
   znaki.insert(std::pair<int, zn>(get_type(z.mp), z));
  }

  default_fig.pen_color = 4;
  default_fig.thickness = 4;
  default_fig.depth = 2;
}


// Извлечь ключ из комментария (2-я строчка) к fig-объекту
// Тип объекта - из ключа
zn_key zn_conv::get_key(const fig::fig_object & fig) const{
  if (fig.comment.size() < 2) return zn_key();
  return boost::lexical_cast<zn_key>(fig.comment[1]);
}

// Извлечь ключ из комментария (1-я строчка) к mp-объекту
// Тип объекта берется из mp, а не из ключа!
// (поскольку из ключа он _никогда_ не должен браться)
zn_key zn_conv::get_key(const mp::mp_object & mp) const{
  zn_key ret;
  if (mp.Comment.size() > 0) ret = boost::lexical_cast<zn_key>(mp.Comment[0]);
  ret.type = get_type(mp);
  return ret;
}

// поместить ключ в комментарий к fig-объекту
void zn_conv::add_key(fig::fig_object & fig, const zn_key & key) const{
  if (fig.comment.size()<2) fig.comment.resize(2);
  fig.comment[1] = boost::lexical_cast<std::string>(key);
}

// поместить ключ в комментарий к mp-объекту
void zn_conv::add_key(mp::mp_object & mp, const zn_key & key) const{
  if (mp.Comment.size()<1) mp.Comment.resize(1);
  mp.Comment[0] = boost::lexical_cast<std::string>(key);
}



// определить тип mp-объекта (почти тривиальная функция :))
int zn_conv::get_type(const mp::mp_object & o) const{
  return o.Type
     + (o.Class == "POLYLINE")?line_mask:0
     + (o.Class == "POLYGON")?area_mask:0;
}

// определить тип fig-объекта по внешнему виду.
int zn_conv::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type==3)) return 0; // объект неинтересного вида

  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    // по умолчанию должны совпасть глубина, толщина,
    if ((o.depth     != i->second.fig.depth) ||
        (o.thickness != i->second.fig.thickness)) continue;
    // для линий толщины не 0 - еще и цвет и тип линии
    if ((o.thickness  != 0 ) &&
        ((o.pen_color != i->second.fig.pen_color) ||
         (o.line_style != i->second.fig.line_style))) continue;
    // для многоугольников - еще и цвет и вид заливки
    bool closed1 = false, closed2 = false;
    if ((o.type==2)&&(o.sub_type>1))       closed1 = true;
    if ((o.type==3)&&(o.sub_type %2 == 1)) closed1 = true;
    if ((i->second.fig.type==2)&&(i->second.fig.sub_type>1))       closed2 = true;
    if ((i->second.fig.type==3)&&(i->second.fig.sub_type %2 == 1)) closed2 = true;
    if (closed1!=closed2) continue;
    int af1 = o.area_fill;
    int af2 = i->second.fig.area_fill;
    int fc1 = o.fill_color;
    int fc2 = i->second.fig.fill_color;
    // белая заливка бывает двух видов
    if ((fc1!=7)&&(af1==40)) {fc1=7; af1=20;}
    if ((fc2!=7)&&(af2==40)) {fc2=7; af2=20;}
    if (closed1 && ((fc1 != fc2) || (af1 != af2))) continue;
    // проведя все тесты, мы считаем, что наш объект соответствует
    // объекту из znaki!
    return i->first;
  }
  return 0;
}


// преобразовать mp-объект в fig-объект
// ключ сохраняется старый, или создается неполный (только с типом)
fig::fig_object zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const{
  fig::fig_object ret = default_fig;
  int type = get_type(mp);
  if (znaki.find(type) != znaki.end()) ret = znaki[type].fig;
  fig.comment.push_back(mp.Label);
  fig.comment.insert(fig.comment.end(), mp.Comment.begin(), mp.Comment.end());
  add_key(fig, get_key(mp));
  g_line pts = cnv.line_bck(mp);
  for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
  return ret;
}

// преобразовать fig-объект в mp-объект
// ключ сохраняется старый, или создается неполный (только с типом)
mp::mp_object zn_conv::fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const{
  mp::mp_object ret = default_mp;
  zn_key key = get_key(fig); 
  if (key.type == 0) key.type = get_type(fig);
  if (znaki.find(type) != znaki.end()) ret = znaki[type].mp;

  if (fig.comment.size()>0){
    ret.Label.push_back(fig.comment[0]);
    ret.Comment.insert(ret.Comment.begin(), fig.comment.begin()+1, fig.comment.end());
  }
  add_key(ret, key);

  g_line pts = cnv.line_frw(fig);
  for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
  return ret;
}




} // namespace

