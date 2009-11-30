#include <iostream>
#include "../../../core/lib2d/line.h"
#include "iface/action.h"
#include "rubber.h"


class ActionTestLine : public Action{
  public:
  ActionTestLine(Rubber * r) : rubber(r) { }

  std::string get_name() { return "TestLine"; }

  void init() {
    std::cout << "Test action for drawing polylines.\n"
              << " * left button: add points\n"
              << " * control + left button: remove last point\n"
              << " * middle button: finish line, print it to stdout\n"
              << " * right button:  abort action\n";
  }

  void reset() {
    rubber->clear();
    data.clear();
  }

  void click(const iPoint & p, const Gdk::ModifierType & state){

    rubber->clear();
    if (state&Gdk::BUTTON1_MASK){
      if (state&Gdk::CONTROL_MASK){
        if (data.size()>0) data.resize(data.size()-1);
      }
      else data.push_back(p);
      if (data.size()>0) rubber->add_diag(data[data.size()-1]);
      for (int i=0; i+1<data.size(); i++){
        rubber->add(data[i], data[i+1], RUBBFL_PLANE);
      }
    }
    else if (state&Gdk::BUTTON2_MASK){
      data.push_back(p);
      std::cout << data << "\n";
      data.clear();
    }
    else if (state&Gdk::BUTTON3_MASK){
      data.clear();
    }
  }
  Rubber * rubber;
  iLine data;
  bool clear;
};
