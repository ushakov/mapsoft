#include "rainbow.h"

int
get_rainbow(double val, const rainbow_data RD[], int rd_size){
  int r=0, g=0, b=0;
  for (int i=0; i<rd_size; i++){

    r  = (RD[i].c >> 16) & 0xFF;
    g  = (RD[i].c >> 8) & 0xFF;
    b  = RD[i].c & 0xFF;

    int ip = (i==0)? i : i-1;
    if (val <= RD[ip].v) break;

    if (val <= RD[i].v){
      int rp = (RD[ip].c >> 16) & 0xFF;
      int gp = (RD[ip].c >> 8) & 0xFF;
      int bp = RD[ip].c & 0xFF;

      r =  rp + (r - rp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);
      g =  gp + (g - gp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);
      b =  bp + (b - bp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);

      break;
    }
  }
  return (r << 16) + (g << 8) + b;
}

simple_rainbow::simple_rainbow(double min, double max, rainbow_type type){
  switch (type){
    case RAINBOW_NORMAL:
    RD[0].c = 0xff0000;
    RD[1].c = 0xffff00;
    RD[2].c = 0x00ff00;
    RD[3].c = 0x00ffff;
    RD[4].c = 0x0000ff;
    RD[5].c = 0xff00ff;
    rd_size=6;
    break;
    case RAINBOW_BURNING:
    RD[0].c = 0xffffff;
    RD[1].c = 0x00ffff;
    RD[2].c = 0x0000ff;
    RD[3].c = 0xff00ff;
    RD[4].c = 0xff0000;
    RD[5].c = 0x400000;
    rd_size=6;
    break;
  }
  set_range(min,max);
}

simple_rainbow::simple_rainbow(double min, double max, int cmin, int cmax){
  rd_size=2;
  RD[0].c = cmin; RD[0].v = min;
  RD[1].c = cmax; RD[0].v = max;
}

void
simple_rainbow::set_range(double min, double max){
  for (int i=0; i<rd_size; i++)
    RD[i].v = min + (max-min)/(rd_size-1)*i;
}

double
simple_rainbow::get_min() const{
  return RD[0].v;
}

double
simple_rainbow::get_max() const{
  return RD[rd_size-1].v;
}


int
simple_rainbow::get(double val) const{
  return get_rainbow(val, RD, rd_size);
}

int
simple_rainbow::get_bnd(double val, int low_c, int high_c) const{
  if (val<RD[0].v) return low_c;
  if (val>RD[rd_size-1].v) return high_c;
  return get_rainbow(val, RD, rd_size);
}

const rainbow_data *
simple_rainbow::get_data() const {
  return RD;
}

int
simple_rainbow::get_size() const{
  return rd_size;
}


