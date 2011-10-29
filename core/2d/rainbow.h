#ifndef RAINBOW_H
#define RAINBOW_H

struct rainbow_data{
  double v;
  int c;
};

int get_rainbow(double val, rainbow_data RD[], int rd_size){
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

#endif