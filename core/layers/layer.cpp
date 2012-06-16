#include "layer.h"

ConvTriv Layer::trivial_cnv;

iImage
Layer::get_image (iRect src){
  iImage ret(src.w, src.h, 0);
  draw(src.TLC(), ret);
  return ret;
}

Conv *
Layer::get_cnv() const{
  return cnv;
}

void
Layer::set_cnv(Conv * c, int hint){
  cnv = c;
  cnv_hint=hint;
}

void
Layer::refresh() {}

Layer &
Layer::operator/= (double k){
  cnv->rescale_src(1/k);
  return *this;
}

Layer &
Layer::operator*= (double k){
  cnv->rescale_src(k);
  return *this;
}

Layer::Layer():
  cnv(&trivial_cnv),cnv_hint(-1) {}

Layer::Layer (Layer const & other):
  cnv(&trivial_cnv),cnv_hint(-1) {}

void
Layer::swap (Layer & other) { }
