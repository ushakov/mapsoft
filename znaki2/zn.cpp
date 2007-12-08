#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "zn.h"

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
         uint_p[assign_a(newkey.id)] >> '@' >> (+(alnum_p|'_'))[assign_a(newkey.map)] >>
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
//    std::cerr << "zn_label_key: can't find valid key in " << str << "\n";
    t = zn_label_key(); return s;
  }
}

//============================================================

// в этом диапазоне глубин должны лежать 
// только картографические объекты!
bool is_map_depth(const fig::fig_object & o){
  return ((o.depth>=50) && (o.depth<400));
}

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
// Тип объекта берется из mp, а не из ключа!
// (поскольку из ключа он _никогда_ не должен браться)
zn_key get_key(const mp::mp_object & mp){
  zn_key ret;
  if (mp.Comment.size() > 0) ret = boost::lexical_cast<zn_key>(mp.Comment[0]);
  ret.type = mp.Type + ((mp.Class == "POLYLINE")?line_mask:0) + ((mp.Class == "POLYGON")?area_mask:0);

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

// поместить ключ в комментарий к mp-объекту
void add_key(mp::mp_object & mp, const zn_key & key){
  if (mp.Comment.size()<1) mp.Comment.resize(1);
  mp.Comment[0] = boost::lexical_cast<std::string>(key);
}


//============================================================

// Заполняет массив знаков 
bool zn_conv::load_znaki(YAML::Node &root) {
   using namespace YAML;
   if (root.type != ARRAY) return false;

   znaki.clear();
   NodeList::iterator it = root.value_array.begin();
   for (it;it!=root.value_array.end();it++) {
      Node child = *it;
      if (child.type != HASH)
        return false;

      zn z;

      const char *v = child.hash_str_value("name");
      if (v==NULL) return false;
      z.name = v;

      v = child.hash_str_value("mp");
      if (v==NULL) return false;
      z.mp = mp::make_object(v);

      v = child.hash_str_value("fig");
      if (v==NULL) return false;
      z.fig = fig::make_object(v);
      if (!is_map_depth(z.fig)){
        std::cerr << "reading conf.file: depth of fig object is not in valid range in "<< z.name <<"\n";
        return false;
      }

      v = child.hash_str_value("desc");
      if (v!=NULL) z.desc = v;

      v = child.hash_str_value("txt");
      if (v==NULL) z.istxt=false;
      else {
        z.istxt=true;
        z.txt = fig::make_object(v);
        if (z.txt.type!=4){ 
          std::cerr << "reading conf.file: not a text in txt object in "<< z.name <<"\n";
          return false;
        }
        if (is_map_depth(z.txt)){
          std::cerr << "reading conf.file: depth of txt object is not in valid range in "<< z.name <<"\n";
          return false;
        }

      }

      v = child.hash_str_value("pic");
      if (v!=NULL) z.pic = v;

      znaki.insert(std::pair<int, zn>(get_type(z.mp), z));   
   }
   return true;
}



// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать

zn_conv::zn_conv(const std::string & conf_file){

  // читаем конф.файл.
  FILE *file = fopen(conf_file.c_str(), "r");
  if (file == NULL) {
      cout << "Проблемы с чтением файла " << conf_file << endl;
      exit(0);
  }
  YAML::Node &root = YAML::parse(file);
  if (!YAML::error.empty()) {
      cout << "Проблемы с разбором файла " << conf_file << ":" << endl;
      cout << YAML::error;
      cout << endl;
      exit(0);
   }
   if (!load_znaki(root)) {
      cout << "Файл " << conf_file << " содержит неподходящую структуру данных" << endl;
      exit(0);
   }
  default_fig.pen_color = 4;
  default_fig.thickness = 4;
  default_fig.depth = 2;

}



// определить тип mp-объекта (почти тривиальная функция :))
int zn_conv::get_type(const mp::mp_object & o) const{
  return o.Type
     + ((o.Class == "POLYLINE")?line_mask:0)
     + ((o.Class == "POLYGON")?area_mask:0);
}

// определить тип fig-объекта по внешнему виду.
int zn_conv::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type!=3) && (o.type!=4)) return 0; // объект неинтересного вида

  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    if (((o.type==2) || (o.type==3)) && (o.size()>1)){
      // должны совпасть глубина и толщина
      if ((o.depth     != i->second.fig.depth) ||
          (o.thickness != i->second.fig.thickness)) continue;
      // для линий толщины не 0 - еще и цвет и тип линии
      if ((o.thickness  != 0 ) &&
          ((o.pen_color != i->second.fig.pen_color) ||
           (o.line_style != i->second.fig.line_style))) continue;
    
      // заливки
      int af1 = o.area_fill;
      int af2 = i->second.fig.area_fill;
      int fc1 = o.fill_color;
      int fc2 = i->second.fig.fill_color;
      // белая заливка бывает двух видов
      if ((fc1!=7)&&(af1==40)) {fc1=7; af1=20;}
      if ((fc2!=7)&&(af2==40)) {fc2=7; af2=20;}
    
      // тип заливки должен совпасть
      if (af1 != af2) continue;
      // если заливка непрозрачна, то и цвет заливки должен совпасть
      if ((af1!=-1) && (fc1 != fc2)) continue;
    
      // если заливка - штриховка, то и pen_color должен совпасть 
      // (даже для линий толщины 0)
      if ((af1>41) && (o.pen_color != i->second.fig.pen_color)) continue;
    
      // проведя все тесты, мы считаем, что наш объект соответствует
      // объекту из znaki!
      return i->first;
    }
    else if (((o.type==2) || (o.type==3)) && (o.size()==1)){ // точки
      // должны совпасть глубина, толщина, цвет, и закругление
      if ((o.depth     == i->second.fig.depth) &&
          (o.thickness == i->second.fig.thickness) &&
          (o.pen_color == i->second.fig.pen_color) &&
          (o.cap_style%2 == i->second.fig.cap_style%2))
        return i->first;
    }
    else if (o.type==4){ //текст
      // должны совпасть глубина, цвет, и шрифт
      if ((o.depth     == i->second.fig.depth) &&
          (o.pen_color == i->second.fig.pen_color) &&
          (o.font      == i->second.fig.font))
        return i->first;
    }
  }
  return 0;
}


// преобразовать mp-объект в fig-объект
// ключ сохраняется старый, или создается неполный (только с типом)
fig::fig_object zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const{
  fig::fig_object ret = default_fig;
  int type = get_type(mp);
  if (znaki.find(type) != znaki.end()) ret = znaki.find(type)->second.fig;
  ret.comment.push_back(mp.Label);
  ret.comment.insert(ret.comment.end(), mp.Comment.begin(), mp.Comment.end());
  add_key(ret, get_key(mp));
  g_line pts = cnv.line_bck(mp);
  for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
  // замкнутая линия
  if ((mp.Class == "POLYLINE") && (ret.size()>1) && (ret[0]==ret[ret.size()-1])){
    ret.resize(ret.size()-1);
    ret.close();
  }

  return ret;
}

// преобразовать fig-объект в mp-объект
// ключ сохраняется старый, или создается неполный (только с типом)
std::list<mp::mp_object> zn_conv::fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const{
  std::list<mp::mp_object> ret;

  zn_key key = get_key(fig); 

  if (key.type == 0) return ret;

  mp::mp_object mp;
  if (znaki.find(key.type) != znaki.end()) mp = znaki.find(key.type)->second.mp;
  else mp = default_mp;

  if (fig.comment.size()>0){
    mp.Label = fig.comment[0];
    mp.Comment.insert(mp.Comment.begin(), fig.comment.begin()+1, fig.comment.end());
  }
  add_key(mp, key);

  g_line pts = cnv.line_frw(fig);
  for (int i=0; i<pts.size(); i++) mp.push_back(pts[i]);

  // если у нас замкнутая линия - добавим в mp еще одну точку:
  if ((mp.Class == "POLYLINE") && (fig.is_closed()) && (fig.size()>0))
    mp.push_back(mp[0]);

  ret.push_back(mp);
  return ret;
}

// заключить fig-объекты в составной объект. Комментарий
// составного объекта копируется из первого объекта (!)
void zn_conv::fig_make_comp(std::list<fig::fig_object> & objects){
  if ((objects.size()<1) || (objects.begin()->size()<1)) return;

  int minx=(*objects.begin())[0].x;
  int maxx=(*objects.begin())[0].x;
  int miny=(*objects.begin())[0].y;
  int maxy=(*objects.begin())[0].y;
  for (std::list<fig::fig_object>::const_iterator
       i = objects.begin(); i != objects.end(); i++){
     if (i->type == 1){
       int rx = i->radius_x;
       int ry = i->radius_y;
       int cx = i->center_x;
       int cy = i->center_y;
       if (minx > cx-rx) minx = cx-rx;
       if (maxx < cx+rx) maxx = cx+rx;
       if (miny > cy-ry) miny = cy-ry;
       if (maxy < cy+ry) maxy = cy+ry;
     } else {
       for (int j = 0; j < i->size(); j++){
         int x = (*i)[j].x;
         int y = (*i)[j].y;
         if (minx > x) minx = x;
         if (maxx < x) maxx = x;
         if (miny > y) miny = y;
         if (maxy < y) maxy = y;
      }
    }
  }
  fig::fig_object o = *objects.begin();
  o.type=6;
  o.clear();
  o.push_back(Point<int>(minx,miny));
  o.push_back(Point<int>(maxx,maxy));
  objects.insert(objects.begin(), o);
  o.type = -6; o.comment.clear();
  objects.insert(objects.end(), o);
}

// Подготовить объект для отдачи пользователю:
// - поменять параметры в соответствии с ключом
// - создать картинку, если надо
// Объект должен иметь полный ключ!
std::list<fig::fig_object> zn_conv::fig2user(const fig::fig_object & fig){
  std::list<fig::fig_object> ret;

  zn_key key = get_key(fig);
  if (znaki.find(key.type)==znaki.end()) {
    std::cerr << "can't find type " << key.type << " in conf.file\n";
    ret.push_back(fig);
    return ret;
  }
  // заготовка для объекта в нужном нам стиле.
  fig::fig_object o(fig);
  // из заготовки копируем разные параметры:
  o.line_style = znaki[key.type].fig.line_style;
  o.thickness  = znaki[key.type].fig.thickness;
  o.pen_color  = znaki[key.type].fig.pen_color;
  o.fill_color = znaki[key.type].fig.fill_color;
  o.depth      = znaki[key.type].fig.depth;
  o.pen_style  = znaki[key.type].fig.pen_style;
  o.area_fill  = znaki[key.type].fig.area_fill;
  o.style_val  = znaki[key.type].fig.style_val;
  o.cap_style  = znaki[key.type].fig.cap_style;
  o.join_style = znaki[key.type].fig.join_style;
  o.font       = znaki[key.type].fig.font;
  o.font_size  = znaki[key.type].fig.font_size;
  o.font_flags = znaki[key.type].fig.font_flags;
  ret.push_back(o);

  if ((znaki[key.type].pic=="") || (o.size()==0)) return ret;

  fig::fig_world PIC = fig::read(znaki[key.type].pic.c_str());
  for (fig::fig_world::iterator i = PIC.begin(); i!=PIC.end(); i++){
    (*i) += o[0];
    if (is_map_depth(*i)){
      cerr << "warning: picture in " << znaki[key.type].pic 
           << " has objects with wrong depth!\n";
    }
    i->comment.resize(2); 
    i->comment[1] = "[skip]";
    ret.push_back(*i);
  }
  fig_make_comp(ret);
  return ret;
}

// Создать подписи к объекту. Объект должен иметь полный ключ!
std::list<fig::fig_object> zn_conv::make_labels(const fig::fig_object & fig){


  std::list<fig::fig_object> ret;
  zn_key key = get_key(fig);
  if (fig.size() < 1) return ret;                    // странный объект
  if ((key.type == 0)||(key.id==0))  return ret;     // неполный ключ
  if (znaki.find(key.type)==znaki.end()) return ret; // про такой тип объектов неизвестно
  if (!znaki[key.type].istxt) return ret;            // подпись не нужна
  if ((fig.comment.size()==0)||
      (fig.comment[0].size()==0)) return ret;     // нечего писать!
  // заготовка для подписи
  fig::fig_object txt = znaki[key.type].txt;

  int txt_dist = 7 * (fig.thickness+2); // fig units



  // определим координаты и наклон подписи
  Point<int> p = fig[0];
  if      (txt.sub_type == 0 ) p += Point<int>(1,-1)*txt_dist;
  else if (txt.sub_type == 1 ) p += Point<int>(0,-2)*txt_dist;
  else if (txt.sub_type == 2 ) p += Point<int>(-1,-1)*txt_dist;

  if (fig.size()>=2){
    if ((key.type >= line_mask) && (key.type < area_mask)){ // линия
      // текст центруется, ставится в середину линии
      p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
      Point<double> v = pnorm(fig[fig.size()/2-1] - fig[fig.size()/2]);
      if (v.x<0) v=-1*v;
      txt.angle = atan2(-v.y, v.x);
      txt.sub_type = 1; // centered text
      p-= Point<int>(int(-v.y*txt_dist), int(v.x*txt_dist));
    }
    else { // площадной объект
      if (txt.sub_type == 0 ) { // left just.text
        // ищем точку с максимальным x-y
        p = fig[0];
        int max = p.x-p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x-fig[i].y > max) {
            max = fig[i].x-fig[i].y;
            p = fig[i];
          }
        }
        p+=Point<int>(1,-1)*txt_dist;
      } else if (txt.sub_type == 2 ) { // right just.text
        // ищем точку с минимальным x+y
        p = fig[0];
        int min = p.x+p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x+fig[i].y < min) {
            min = fig[i].x+fig[i].y;
            p = fig[i];
          }
        }
        p+=Point<int>(-1,-1)*txt_dist;
      } else if (txt.sub_type == 1 ) { // centered text
        // ищем середину объекта
        Point<int> pmin = fig[0];
        Point<int> pmax = fig[0];
        for (int i = 0; i<fig.size(); i++){
          if (pmin.x > fig[i].x) pmin.x = fig[i].x;
          if (pmin.y > fig[i].y) pmin.y = fig[i].y;
          if (pmax.x < fig[i].x) pmax.x = fig[i].x;
          if (pmax.y < fig[i].y) pmax.y = fig[i].y;
        }
        p=(pmin+pmax)/2;
      }
    }
  }
  add_key(txt, zn_label_key(key));
  txt.text = fig.comment[0];
  txt.push_back(p);
  ret.push_back(txt);
  return(ret);
}

// список всех знаков в формате fig
fig::fig_world zn_conv::make_legend(int grid){
  int count=0;
  fig::fig_world ret;

  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    fig::fig_object o = i->second.fig;
    Point<int> shift(0, count*2*grid);
    zn_key key;
    key.type = i->first;
    key.id = count+1;
    key.map = "get_legend_map";

    if (i->first >= area_mask){
      o.push_back(Point<int>(0,       0));
      o.push_back(Point<int>(grid*5,  0));
      o.push_back(Point<int>(grid*5,  grid));
      o.push_back(Point<int>(0,       grid));
      o.push_back(Point<int>(0,       0)); 
    }
    else if (i->first >= line_mask){
      o.push_back(Point<int>(0,       grid));
      o.push_back(Point<int>(grid*4,  grid));
      o.push_back(Point<int>(grid*5,  0));
    }
    else{
      o.push_back(Point<int>(grid*2,  grid));
    }
    o+=shift;
    o.comment.push_back("text");
    if (o.type==4) o.text="10";
    add_key(o, key);
    std::list<fig::fig_object> l1 = zn_conv::fig2user(o);
    std::list<fig::fig_object> l2 = zn_conv::make_labels(o);
    ret.insert(ret.end(), l1.begin(), l1.end());
    ret.insert(ret.end(), l2.begin(), l2.end());

    fig::fig_object text = fig::make_object("4 0 0 40 -1 18 8 0.0000 4");
    text.text = i->second.name;
    text.push_back(Point<int>(grid*8, grid));
    text+=shift;
    ret.push_back(text);
    
    ostringstream mp_key;
    mp_key << i->second.mp.Class << " 0x" << std::setbase(16) << i->second.mp.Type;
    text.text = mp_key.str();
    text.clear();
    text.push_back(Point<int>(-1*grid, grid));
    text+=shift;
    text.sub_type = 2;
    ret.push_back(text);

    count++;
  }
  fig::fig_object o = fig::make_object("2 2 0 0 30 30 150 -1 20 0.000 0 1 7 0 0 0");
  o.push_back(Point<int>(-15*grid,-grid));
  o.push_back(Point<int>(+40*grid,-grid));
  o.push_back(Point<int>(+40*grid,(2*count+1)*grid));
  o.push_back(Point<int>(-15*grid,(2*count+1)*grid));
  ret.push_back(o);
  return ret;
}

// текстовый список всех знаков
std::string zn_conv::make_text(){
  ostringstream out;
  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    out << std::setbase(16) << i->first << "\t" << i->second.name << " // " << i->second.desc << "\n";
  }
  return out.str();
}

} // namespace

