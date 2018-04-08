#include <cassert>
#include <fstream>
#include "conv.h"

// simple conversion x->x^2, y->2y
class MyConv : public Conv {
  public:
  void frw_pt(dPoint & p) const {
    if (sc_src!=1.0) p*=sc_src;
    p.x*=p.x;
    p.y*=2;
    if (sc_dst!=1.0) p*=sc_dst;
  }
  void bck_pt(dPoint & p) const {
    if (sc_dst!=1.0) p/=sc_dst;
    p.x=sqrt(p.x);
    p.y/=2;
    if (sc_src!=1.0) p/=sc_src;
  }
};

main(){
  try{

    MyConv cnv;

    { // test frw and bck functions for Point
      dPoint p1(2,2);
      cnv.frw(p1);  assert(p1 == dPoint(4,4));
      cnv.frw(p1);  assert(p1 == dPoint(16,8));
      cnv.bck(p1);  assert(p1 == dPoint(4,4));
      cnv.bck(p1);  assert(p1 == dPoint(2,2));
    }

    { // test Line conversions
      dLine l1("[[0,0],[10,10]]");
      cnv.frw(l1);  assert(l1 == dLine("[[0,0],[100,20]]"));
      cnv.frw(l1);  assert(l1 == dLine("[[0,0],[10000,40]]"));
      cnv.bck(l1);  assert(l1 == dLine("[[0,0],[100,20]]"));
      cnv.bck(l1);  assert(l1 == dLine("[[0,0],[10,10]]"));
    }

    { // test frw_acc/bck_acc line conversions
       dLine l1("[[0,0],[10,10]]");
       cnv.rescale_dst(10);
       assert(iLine(cnv.frw_acc(l1, 4)) ==
              iLine("[[0,0],[250,100],[1000,200]]"));
       assert(iLine(cnv.frw_acc(l1, 1)) ==
              iLine("[[0,0],[62,50],[390,125],[1000,200]]"));
       assert(iLine(cnv.frw_acc(l1, 0.1)) ==
              iLine("[[0,0],[3,12],[14,24],[30,35],[77,55],[136,73],[277,105],[416,129],[676,164],[1000,200]]"));

       l1=dLine("[[0,0],[100,20]]");
       assert(iLine(10.0*cnv.bck_acc(l1, 1)) ==
              iLine("[[0,0],[31,10]]"));
       assert(iLine(10.0*cnv.bck_acc(l1, 0.2)) ==
              iLine("[[0,0],[7,0],[23,5],[31,10]]"));
       assert(iLine(10.0*cnv.bck_acc(l1, 0.05)) ==
              iLine("[[0,0],[1,0],[5,0],[9,0],[14,2],[20,4],[26,7],[31,10]]"));
       cnv.rescale_dst(0.1);
    }

    { // test frw_acc/bck_acc rect conversions
      // cnv is not so interesting here, rectangle converts to rectungle:
      assert (cnv.frw_acc(dRect(0,0,10,10),0.005) == dRect(0,0,100,20));
      assert (cnv.bck_acc(dRect(0,0,100,20),0.005) == dRect(0,0,10,10));
    }


  }
  catch (Err e) {
    std::cerr << "Error: " << e.str() << "\n";
  }
}
