#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <queue>

#include <2d/point_int.h>
#include "srtm3.h"
#include <zlib.h>

using namespace std;

const string def_srtm_dir = string(getenv("HOME")) + "/.srtm_data";

srtm3::srtm3(const string _srtm_dir, const unsigned cache_size) :
     srtm_cache(cache_size), srtm_dir(_srtm_dir){
}


short
srtm3::geth(const iPoint & p, const bool interp){
  iPoint key = p/(srtm_width-1);
  iPoint crd = p - key*(srtm_width-1);

  if (key.x<0) {key.x--; crd.x+=srtm_width;}
  if (key.y<0) {key.y--; crd.y+=srtm_width;}
  crd.y=srtm_width-crd.y-1;

  if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
  if (srtm_cache.get(key).empty()) return srtm_nofile;

  int h = srtm_cache.get(key).get(crd);

  if (interp){
    if (h>srtm_min_interp) return h - srtm_zer_interp;
    if (h!=srtm_undef) return h;

    // найдем всю дырку и заинтерполируем ее!
    set<iPoint> pset = plane(p);
    set<iPoint> bord = border(pset);

    set<iPoint>::iterator pi, bi;
    for (pi = pset.begin(); pi != pset.end(); pi++){
      double Srh = 0;
      double Sr  = 0;
      for (bi = bord.begin(); bi != bord.end(); bi++){
        int bh = geth(*bi);

        if (bh>srtm_min){
          double k = cos(double(pi->y)/srtm_width/180.0*M_PI);
          double r = 1.0/(pow(k*(bi->x - pi->x),2) + pow(bi->y - pi->y,2));
          Sr += r;
          Srh+= bh * r;
        }
      }
      seth(*pi, (short)Srh/Sr+srtm_zer_interp);
    }
    return geth(p,true);
  }
  else {
    if (h>srtm_min_interp) return srtm_undef;
    else return h;
  }

  return srtm_cache.get(key).get(crd);
}

short
srtm3::geth(const int x, const int y, const bool interp){
  return geth(iPoint(x,y), interp);
}

// поменять высоту точки (только в кэше!)
short
srtm3::seth(const iPoint & p, const short h){
  iPoint key = p/(srtm_width-1);
  iPoint crd = p - key*(srtm_width-1);

  if (key.x<0) {key.x--; crd.x+=srtm_width;}
  if (key.y<0) {key.y--; crd.y+=srtm_width;}
  crd.y=srtm_width-crd.y-1;

  if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
  if (srtm_cache.get(key).empty()) return srtm_nofile;

  srtm_cache.get(key).set(crd, h);
  return h;
}

short
srtm3::geth4(const dPoint & p, const bool interp){
  double x = p.x*1200;
  double y = p.y*1200;
  int x1 = floor(x);
  int x2 = x1+1;
  int y1 = floor(y);
  int y2 = y1+1;

  short h1=geth(x1,y1,interp);
  short h2=geth(x1,y2,interp);

  if ((h1<srtm_min)||(h2<srtm_min)) return srtm_undef;
  short h12 = (int)( h1+ (h2-h1)*(y-y1) );

  short h3=geth(x2,y1,interp);
  short h4=geth(x2,y2,interp);
  if ((h3<srtm_min)||(h4<srtm_min)) return srtm_undef;
  short h34 = (int)( h3 + (h4-h3)*(y-y1) );

  return (short)( h12 + (h34-h12)*(x-x1) );
}

short
srtm3::geth16(const dPoint & p, const bool interp){

  double x = p.x*1200;
  double y = p.y*1200;
  int x0 = floor(x);
  int y0 = floor(y);

  double hx[4], hy[4];

  for (int i=0; i<4; i++){
    for (int j=0; j<4; j++) hx[j]=geth(x0+j-1, y0+i-1, interp);
    int_holes(hx);
    hy[i]= cubic_interp(hx, x-x0);
  }
  int_holes(hy);
  return cubic_interp(hy, y-y0);
}

// найти множество соседних точек одной высоты (не более max точек)
set<iPoint>
srtm3::plane(const iPoint& p, int max){
  set<iPoint> ret;
  queue<iPoint> q;
  short h = geth(p);

  q.push(p);
  ret.insert(p);

  while (!q.empty()){
    iPoint p1 = q.front();
    q.pop();
    for (int i=0; i<8; i++){
      iPoint p2 = adjacent(p1, i);
      if ((geth(p2) == h)&&(ret.insert(p2).second)) q.push(p2);
    }
    if ((max!=0)&&(ret.size()>max)) break;
  }
  return ret;
}

/*
// interpolate function between 4 points
// for use in get16()
short
srtm3::int4(int x1, int x2, int x3, int x4,
            int f1, int f2, int f3, int f4, double x){

  x-=x1; x2-=x1; x3-=x1; x4-=x1; x1=0;

  double k2=double(f2-f1)/double(x2-x1);
  double k3=double(f3-f2)/double(x3-x2);
  double k4=double(f4-f3)/double(x4-x3);

  double m3=(k3-k2)/double(x3-x1);
  double m4=(k4-k3)/double(x4-x2);

  double a = (m4-m3)/double(x4-x1);
  double b = m3 - a*(x1+x2+x3);
  double c = k2 - (x1*x1+x2*x2+x1*x2)*a - (x1+x2)*b;
  double d = f1 - a*x1*x1*x1 - b*x1*x1 - c*x1;

  return short(a*x*x*x + b*x*x + c*x + d);
}

// the same with fixed distance between points
short
srtm3::int4(int x1, int f1, int f2, int f3, int f4, double x){
  x-=x1;
  x1=0;
  int x2=1;
  int x3=2;
  int x4=3;

  double k2=f2-f1;
  double k3=f3-f2;
  double k4=f4-f3;

  double m3=(k3-k2)/2.0;
  double m4=(k4-k3)/2.0;

  double a = (m4-m3)/3.0;
  double b = m3 - a*(x1+x2+x3);
  double c = k2 - (x1*x1+x2*x2+x1*x2)*a - (x1+x2)*b;
  double d = f1 - a*x1*x1*x1 - b*x1*x1 - c*x1;

  return short(a*x*x*x + b*x*x + c*x + d);
}
*/

// the same
// see http://www.paulinternet.nl/?page=bicubic
short
srtm3::cubic_interp(const double h[4], const double x) const{
  return h[1] + 0.5 * x*(h[2] - h[0] + x*(2.0*h[0] - 5.0*h[1] + 4.0*h[2] -
              h[3] + x*(3.0*(h[1] - h[2]) + h[3] - h[0])));
}

void
srtm3::int_holes(double h[4]) const{
  // interpolate 1-point or 2-points holes
  // maybe this can be written smarter...
  if ((h[0]>srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min) && (h[3]>srtm_min)) return;
  for (int cnt=0; cnt<2; cnt++){
    if      ((h[0]<=srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min)) h[0]=2*h[1]-h[2];
    else if ((h[1]<=srtm_min) && (h[0]>srtm_min) && (h[2]>srtm_min)) h[1]=(h[0]+h[2])/2;
    else if ((h[1]<=srtm_min) && (h[2]>srtm_min) && (h[3]>srtm_min)) h[1]=2*h[2]-h[3];
    else if ((h[2]<=srtm_min) && (h[1]>srtm_min) && (h[3]>srtm_min)) h[2]=(h[1]+h[3])/2;
    else if ((h[2]<=srtm_min) && (h[0]>srtm_min) && (h[1]>srtm_min)) h[2]=2*h[1]-h[0];
    else if ((h[3]<=srtm_min) && (h[1]>srtm_min) && (h[2]>srtm_min)) h[3]=2*h[2]-h[1];
    else break;
  }
  if ((h[1]<=srtm_min) && (h[2]<=srtm_min) && (h[0]>srtm_min) && (h[3]>srtm_min)){
    h[1]=(2*h[0] + h[3])/3;
    h[2]=(h[0] + 2*h[3])/3;
  }
}





bool
srtm3::load(iPoint key){

  while (key.x < -max_lon) key.x+=2*max_lon;
  while (key.x >= max_lon) key.x-=2*max_lon;
  if ((key.y<-max_lat)||(key.y>=max_lat)) return false;

  char NS='N';
  char EW='E';
  if (key.y<0) {NS='S';}
  if (key.x<0) {EW='W';}
  // название файла
  ostringstream file;
  file << srtm_dir << "/" 
       << NS << setfill('0') << setw(2) << abs(key.y)
       << EW << setw(3) << abs(key.x) << ".hgt";

  // try to read file
  gzFile F = gzopen(file.str().c_str(), "rb");
  if (!F){
    file << ".gz";
    F = gzopen(file.str().c_str(), "rb");
  }
  if (!F){
    cerr << "can't find file " << file.str() << '\n';
    srtm_cache.add(key, Image<short>(0,0));
    return false;
  }

  Image<short> im(srtm_width,srtm_width);
  int length = srtm_width*srtm_width*sizeof(short);

  if (length != gzread(F, im.data, length)){
    cerr << "error while reading from file " << file.str() << '\n';
    srtm_cache.add(key, Image<short>(0,0));
    return false;
  }

  for (int i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  srtm_cache.add(key, im);
  return true;
}
