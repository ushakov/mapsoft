#ifndef RAINBOW_H
#define RAINBOW_H

struct rainbow_data{
  double v;
  int c;
};

int get_rainbow(double val, const rainbow_data RD[], int rd_size){
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

class simple_rainbow{
  static const int rd_size=6;
  rainbow_data RD[rd_size];

public:
  simple_rainbow(double min, double max){
    RD[0].c = 0x0000ff;
    RD[1].c = 0x00ffff;
    RD[2].c = 0x00ff00;
    RD[3].c = 0xffff00;
    RD[4].c = 0xff0000;
    RD[5].c = 0xff00ff;

    for (int i=0; i<rd_size; i++)
      RD[i].v = min + (max-min)/(rd_size-1)*i;
  }

  int get(double val) const{
    return get_rainbow(val, RD, rd_size);
  }
};

#endif