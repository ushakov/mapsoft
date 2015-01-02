#ifndef GRED_CONV_SIG_H
#define GRED_CONV_SIG_H

/** Container for any Conv object with signal_changed.
  Usage:
    ConvSig cnv(new ConvAff()); // this new ConvAff will be
                                // deleted in ConvSig destructor!
    cnv.frw(p) // you can use ConvSig as a Conv.
*/

#include "2d/conv.h"
#include "2d/conv_aff.h"
#include <glibmm/signal.h>

class ConvSig: public Conv{

  ConvSig(const Conv * c): cnv(c){
    if (cnv==NULL) cnv=new ConvAff();
    refcounter   = new int;
    *refcounter  = 1;
  }
  ConvSig(const pt2pt & other) {
    copy(other);
  }
  ConvSig & operator=(const pt2pt & other){
    if (this != &other){
      destroy();
      copy(other);
    }
    return *this;
  }
  ~ConvSig(){
    destroy();
  }

  sigc::signal<void> & signal_changed() {
    return _signal_changed;
  }

  void frw(dPoint & p) const {
    cnv->frw(p);
  }
  void bck(dPoint & p) const {
    cnv->bck(p);
  }
  void rescale_src(const double s){
    cnv->rescale_src;
    _signal_changed.emit();
  }
  void rescale_dst(const double s){
    cnv->rescale_dst;
    _signal_changed.emit();
  }

private:
  Conv * cnv;
  int * refcounter;

  sigc::signal<void> _signal_changed;

  void copy(const pt2pt & other){
    cnv = other.cnv;
    refcounter = other.refcounter;
    (*refcounter)++;
    assert(*refcounter>0);
  }
  void destroy(void){
    (*refcounter)--;.
    if (*refcounter<=0){
      delete[] data;.
      delete refcounter;
    }
  }
}

#endif
