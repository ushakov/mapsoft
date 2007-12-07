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
        (ch_p('0') >> 'x' >> hex_p[assign_a(newkey.type)] >> +blank_p >>
         (+graph_p >> +blank_p >> +graph_p)[assign_a(timestr)] >> +blank_p >>
         uint_p[assign_a(newkey.id)] >> '@' >> (+(alnum_p|'_'))[assign_a(newkey.map)] >>
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

//============================================================

// ��������� ������ ������ 
bool zn_conv::load_znaki(YAML::Node &root, std::map<int, zn> &znaki) {
   using namespace YAML;
   if (root.type != ARRAY) return false;

   NodeList::iterator it = root.value_array.begin();
   for (it;it!=root.value_array.end();it++) {
      Node child = *it;
      if (child.type != HASH)
        return false;

      zn z;

      const char *v = child.hash_str_value("mp");
      if (v==NULL) return false;
      z.mp = mp::make_object(v);

      v = child.hash_str_value("fig");
      if (v==NULL) return false;
      z.fig = fig::make_object(v);

      v = child.hash_str_value("name");
      if (v==NULL) return false;
      z.name = v;

      v = child.hash_str_value("desc");
      if (v!=NULL) z.desc = v;

      v = child.hash_str_value("txt");
      if (v==NULL) z.istxt=false;
      else {
        z.istxt=true;
        z.txt = fig::make_object(v);
        if (z.txt.type!=4){ 
          std::cerr << "not a text in txt object!\n";
          return false;
        }
      }

      v = child.hash_str_value("pic");
      if (v!=NULL) z.pic = v;

      znaki.insert(std::pair<int, zn>(get_type(z.mp), z));   
   }
   return true;
}



// �����, ������� (�� ����.�����) ��� ���������� �������� ����������� 
// � ������� �� ���������������

zn_conv::zn_conv(const std::string & conf_file){

  // ������ ����.����.
  FILE *file = fopen(conf_file.c_str(), "r");
  if (file == NULL) {
      cout << "�������� � ������� ����� " << conf_file << endl;
      exit(0);
  }
  YAML::Node &root = YAML::parse(file);
  if (!YAML::error.empty()) {
      cout << "�������� � �������� ����� " << conf_file << ":" << endl;
      cout << YAML::error;
      cout << endl;
      exit(0);
   }
   if (!load_znaki(root, znaki)) {
      cout << "���� " << conf_file << " �������� ������������ ��������� ������" << endl;
      exit(0);
   }
  default_fig.pen_color = 4;
  default_fig.thickness = 4;
  default_fig.depth = 2;

}


// ������� ���� �� ����������� (2-� �������) � fig-�������
// ��� ������� - �� �����
zn_key zn_conv::get_key(const fig::fig_object & fig) const{
  if (fig.comment.size() < 2) return zn_key();
  return boost::lexical_cast<zn_key>(fig.comment[1]);
}

// ������� ���� �� ����������� (1-� �������) � mp-�������
// ��� ������� ������� �� mp, � �� �� �����!
// (��������� �� ����� �� _�������_ �� ������ �������)
zn_key zn_conv::get_key(const mp::mp_object & mp) const{
  zn_key ret;
  if (mp.Comment.size() > 0) ret = boost::lexical_cast<zn_key>(mp.Comment[0]);
  ret.type = get_type(mp);
  return ret;
}

// ��������� ���� � ����������� � fig-�������
void zn_conv::add_key(fig::fig_object & fig, const zn_key & key) const{
  if (fig.comment.size()<2) fig.comment.resize(2);
  fig.comment[1] = boost::lexical_cast<std::string>(key);
}

// ��������� ���� � ����������� � mp-�������
void zn_conv::add_key(mp::mp_object & mp, const zn_key & key) const{
  if (mp.Comment.size()<1) mp.Comment.resize(1);
  mp.Comment[0] = boost::lexical_cast<std::string>(key);
}



// ���������� ��� mp-������� (����� ����������� ������� :))
int zn_conv::get_type(const mp::mp_object & o) const{
  return o.Type
     + ((o.Class == "POLYLINE")?line_mask:0)
     + ((o.Class == "POLYGON")?area_mask:0);
}

// ���������� ��� fig-������� �� �������� ����.
int zn_conv::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type!=3) && (o.type!=4)) return 0; // ������ ������������� ����

  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    if (((o.type==2) || (o.type==3)) && (o.size()>1)){
      // ������ �������� ������� � �������
      if ((o.depth     != i->second.fig.depth) ||
          (o.thickness != i->second.fig.thickness)) continue;
      // ��� ����� ������� �� 0 - ��� � ���� � ��� �����
      if ((o.thickness  != 0 ) &&
          ((o.pen_color != i->second.fig.pen_color) ||
           (o.line_style != i->second.fig.line_style))) continue;
    
      // �������
      int af1 = o.area_fill;
      int af2 = i->second.fig.area_fill;
      int fc1 = o.fill_color;
      int fc2 = i->second.fig.fill_color;
      // ����� ������� ������ ���� �����
      if ((fc1!=7)&&(af1==40)) {fc1=7; af1=20;}
      if ((fc2!=7)&&(af2==40)) {fc2=7; af2=20;}
    
      // ��� ������� ������ ��������
      if (af1 != af2) continue;
      // ���� ������� �����������, �� � ���� ������� ������ ��������
      if ((af1!=-1) && (fc1 != fc2)) continue;
    
      // ���� ������� - ���������, �� � pen_color ������ �������� 
      // (���� ��� ����� ������� 0)
      if ((af1>41) && (o.pen_color != i->second.fig.pen_color)) continue;
    
      // ������� ��� �����, �� �������, ��� ��� ������ �������������
      // ������� �� znaki!
      return i->first;
    }
    else if (((o.type==2) || (o.type==3)) && (o.size()==1)){ // �����
      // ������ �������� �������, �������, ����, � �����������
      if ((o.depth     == i->second.fig.depth) &&
          (o.thickness == i->second.fig.thickness) &&
          (o.pen_color == i->second.fig.pen_color) &&
          (o.cap_style%2 == i->second.fig.cap_style%2))
        return i->first;
    }
    else if (o.type==4){ //�����
      // ������ �������� �������, ����, � �����
      if ((o.depth     == i->second.fig.depth) &&
          (o.pen_color == i->second.fig.pen_color) &&
          (o.font      == i->second.fig.font))
        return i->first;
    }
  }
  return 0;
}


// ������������� mp-������ � fig-������
// ���� ����������� ������, ��� ��������� �������� (������ � �����)
fig::fig_object zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const{
  fig::fig_object ret = default_fig;
  int type = get_type(mp);
  if (znaki.find(type) != znaki.end()) ret = znaki.find(type)->second.fig;
  ret.comment.push_back(mp.Label);
  ret.comment.insert(ret.comment.end(), mp.Comment.begin(), mp.Comment.end());
  add_key(ret, get_key(mp));
  g_line pts = cnv.line_bck(mp);
  for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
  return ret;
}

// ������������� fig-������ � mp-������
// ���� ����������� ������, ��� ��������� �������� (������ � �����)
std::list<mp::mp_object> zn_conv::fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const{
  std::list<mp::mp_object> ret;

  zn_key key = get_key(fig); 

  if (key.type == 0) return ret;
  if (key.label) return ret;

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
  ret.push_back(mp);
  return ret;
}

// ��������� fig-������� � ��������� ������. �����������
// ���������� ������� ���������� �� ������� ������� (!)
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
cerr << "making compound with " << objects.size() << "objects\n";
  fig::fig_object o = *objects.begin();
  o.type=6;
  o.clear();
  o.push_back(Point<int>(minx,miny));
  o.push_back(Point<int>(maxx,maxy));
  objects.insert(objects.begin(), o);
  o.type = -6; o.comment.clear();
  objects.insert(objects.end(), o);
}

// ����������� ������ ��� ������ ������������:
// - �������� ��������� � ������������ � ������
// - ������� ��������, ���� ����
// ������ ������ ����� ������ ����!
std::list<fig::fig_object> zn_conv::fig2user(const fig::fig_object & fig){
  std::list<fig::fig_object> ret;

  zn_key key = get_key(fig);
  if (znaki.find(key.type)==znaki.end()) {
    std::cerr << "can't find type " << key.type << " in conf.file\n";
    ret.push_back(fig);
    return ret;
  }
  fig::fig_object fig1 = znaki[key.type].fig;
//fig1.clear();
  fig1.insert(fig1.begin(), fig.begin(), fig.end());
  fig1.comment.insert(fig1.comment.begin(), fig.comment.begin(), fig.comment.end());
  fig1.text=fig.text;
  ret.push_back(fig1);


  if ((znaki[key.type].pic=="") || (fig1.size()==0)) return ret;

  fig::fig_world PIC = fig::read(znaki[key.type].pic.c_str());
  PIC+=fig1[0];

  ret.insert(ret.end(), PIC.begin(), PIC.end());
  fig_make_comp(ret);
  return ret;
}

// ������� ������� � �������. ������ ������ ����� ������ ����!
std::list<fig::fig_object> zn_conv::make_labels(const fig::fig_object & fig){


  std::list<fig::fig_object> ret;
  zn_key key = get_key(fig);
  if (fig.size() < 1) return ret;                    // �������� ������
  if ((key.type == 0)||(key.id==0))  return ret;     // �������� ����
  if (znaki.find(key.type)==znaki.end()) return ret; // ��� ����� ��� �������� ����������
  if (!znaki[key.type].istxt) return ret;            // ������� �� �����
  if ((fig.comment.size()==0)||
      (fig.comment[0].size()==0)) return ret;     // ������ ������!
  // ��������� ��� �������
  fig::fig_object txt = znaki[key.type].txt;

  int txt_dist = 7 * (fig.thickness+2); // fig units



  // ��������� ���������� � ������ �������
  Point<int> p = fig[0];
  if      (txt.sub_type == 0 ) p += Point<int>(1,-1)*txt_dist;
  else if (txt.sub_type == 1 ) p += Point<int>(0,-2)*txt_dist;
  else if (txt.sub_type == 2 ) p += Point<int>(-1,-1)*txt_dist;

  if (fig.size()>=2){
    if ((key.type >= line_mask) && (key.type < area_mask)){ // �����
      // ����� ����������, �������� � �������� �����
      p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
      Point<double> v = pnorm(fig[fig.size()/2-1] - fig[fig.size()/2]);
      if (v.x<0) v=-1*v;
      txt.angle = atan2(-v.y, v.x);
      txt.sub_type = 1; // centered text
      p-= Point<int>(int(-v.y*txt_dist), int(v.x*txt_dist));
    }
    else { // ��������� ������
      if (txt.sub_type == 0 ) { // left just.text
        // ���� ����� � ������������ x-y
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
        // ���� ����� � ����������� x+y
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
        // ���� �������� �������
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
  key.label=true;
  add_key(txt, key);
  txt.text = fig.comment[0];
  txt.push_back(p);
  ret.push_back(txt);
  return(ret);
}

// ������ ���� ������ � ������� fig
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

    ostringstream mp_key;
    if (i->first >= area_mask){
      o.push_back(Point<int>(0,       0));
      o.push_back(Point<int>(grid*5,  0));
      o.push_back(Point<int>(grid*5,  grid));
      o.push_back(Point<int>(0,       grid));
      o.push_back(Point<int>(0,       0)); 
      mp_key << "POLYGON 0x" << std::setbase(16) << i->first - area_mask;
    }
    else if (i->first >= line_mask){
      o.push_back(Point<int>(0,       grid));
      o.push_back(Point<int>(grid*5,  grid));
      mp_key << "POLYLINE 0x" << std::setbase(16) << i->first - line_mask;
    }
    else{
      o.push_back(Point<int>(grid*2,  grid));
      mp_key << "POI 0x" << std::setbase(16) << i->first;
    }
    o+=shift;
    o.comment.push_back("text");
    if (o.type==4) o.text="10";
    add_key(o, key);
    std::list<fig::fig_object> l1 = zn_conv::fig2user(o);
    std::list<fig::fig_object> l2 = zn_conv::make_labels(o);
    ret.insert(ret.end(), l1.begin(), l1.end());
    ret.insert(ret.end(), l2.begin(), l2.end());

    fig::fig_object text = fig::make_object("4 0 0 55 -1 18 8 0.0000 4");
    text.text = i->second.name;
    text.push_back(Point<int>(grid*8, grid));
    text+=shift;
    ret.push_back(text);
    
    text.text = mp_key.str();
    text.clear();
    text.push_back(Point<int>(-1*grid, grid));
    text+=shift;
    text.sub_type = 2;
    ret.push_back(text);

    count++;
  }
  return ret;
}


} // namespace

