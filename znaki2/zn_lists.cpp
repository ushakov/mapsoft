#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "zn.h"
#include "zn_key.h"
#include "zn_lists.h"

namespace zn{

// список всех знаков в формате fig
fig::fig_world make_legend(const zn_conv & z, int grid, int dg){
  int count=0;
  fig::fig_world ret;

  for (std::map<int, zn>::const_iterator i = z.znaki.begin(); i!=z.znaki.end(); i++){
    fig::fig_object o = i->second.fig;
    Point<int> shift(0, count*2*grid);
    zn_key key;
    key.type = i->first;
    key.id = count+1;
    key.map = "get_legend_map";

    if (i->first >= area_mask){
      o.push_back(Point<int>(dg,      -dg));
      o.push_back(Point<int>(grid*5-dg,  0));
      o.push_back(Point<int>(grid*5,  grid));
      o.push_back(Point<int>(0,       grid));
    }
    else if (i->first >= line_mask){
      o.push_back(Point<int>(0,       grid-dg));
      o.push_back(Point<int>(grid*4+dg,  grid));
      o.push_back(Point<int>(grid*5,  0));
    }
    else{
      o.push_back(Point<int>(grid*2,  grid));
    }
    o+=shift;
    o.comment.push_back("text");
    if (o.type==4) o.text="10";
    add_key(o, key);
    
    std::list<fig::fig_object> l1 = z.make_pic(o);
    std::list<fig::fig_object> l2 = z.make_labels(o);
    add_key(l2, zn_label_key(key));

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
std::string make_text(const zn_conv & z){
  ostringstream out;
  for (std::map<int, zn>::const_iterator i = z.znaki.begin(); i!=z.znaki.end(); i++){
    out << std::setbase(16) << i->first << "\t" << i->second.name << " // " << i->second.desc << "\n";
  }
  return out.str();
}

} // namespace

