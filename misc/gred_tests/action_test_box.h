#include <iostream>
#include "gred/iface/action.h"
#include "gred/rubber.h"

class ActionTestBox : public Action{
  public:
  ActionTestBox(Rubber * r) : rubber(r) { }

  std::string get_name() { return "TestBox"; }

  void init() {
    clear=true;
  }

  void reset() {
    rubber->clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      rubber->add_src_sq(p, 3);
      rubber->add_dst_sq(3);
      rubber->add_rect(p);
      rubber->add_diag(p);
    } else {
      rubber->clear();
    }
    clear=!clear;
  }
  Rubber * rubber;
  bool clear;
};
