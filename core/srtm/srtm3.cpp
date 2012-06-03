#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <queue>

#include <cstdio>

#include <2d/point_int.h>
#include "srtm3.h"
#include <zlib.h>

using namespace std;

srtm3::srtm3(const string & _srtm_dir, const unsigned cache_size) :
     srtm_cache(cache_size),
     srtm_dir(_srtm_dir),
     size0(6380e3 * M_PI/srtm_width/180),
     area0(size0*size0){
  if (srtm_dir == "") srtm_dir =
    string(getenv("HOME")? getenv("HOME"):"") + "/.srtm_data";
}

void
srtm3::set_dir(const string & str){
  srtm_dir = str;
}

const string &
srtm3::get_dir(void) const{
  return srtm_dir;
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
          double r = 1.0/(pow(k*(bi->x - pi->x),2) + pow(double(bi->y - pi->y),2));
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

double
srtm3::slope(const iPoint &p, const bool interp){
  short h  = geth(p, interp);
  short hx = geth(p.x, p.y+1, interp);
  short hy = geth(p.x+1, p.y, interp);
  double a=0;
  if ((h > srtm_min) && (hx > srtm_min) && (hy > srtm_min)){
    // sqares of cell dimensions (km/sec)^2:
    const double dx2 = area0 * pow(cos(M_PI*p.y/180/srtm_width),2);
    const double dy2 = area0;
    const double  U = sqrt(pow(double(h-hx) ,2)/dx2 + pow(double(h-hy) ,2)/dy2);
    a = atan(U)*180.0/M_PI;
  }
  return a;
}

short
srtm3::geth(const int x, const int y, const bool interp){
  return geth(iPoint(x,y), interp);
}

double
srtm3::slope(const int x, const int y, const bool interp){
  return slope(iPoint(x,y), interp);
}

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
  int x1 = floor(x), x2 = x1+1;
  int y1 = floor(y), y2 = y1+1;

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

double
srtm3::slope4(const dPoint & p, const bool interp){
  double x = p.x*1200 - 0.5;
  double y = p.y*1200 - 0.5;
  int x1 = floor(x), x2 = x1+1;
  int y1 = floor(y), y2 = y1+1;

  double h1=slope(x1,y1, interp);
  double h2=slope(x1,y2, interp);
  double h3=slope(x2,y1, interp);
  double h4=slope(x2,y2, interp);

  double h12 = h1+ (h2-h1)*(y-y1);
  double h34 = h3 + (h4-h3)*(y-y1);
  return h12 + (h34-h12)*(x-x1);
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

void
srtm3::move_to_extr(iPoint & p0, bool max){
  iPoint p1 = p0;
  do {
    short h = geth(p0, true);
    for (int i=0; i<8; i++){
      iPoint p=adjacent(p0,i);
      if ((max && (geth(p,true) > geth(p1,true))) ||
         (!max && (geth(p,true) < geth(p1,true)))) p1=p;
    }
    if (p1==p0) break;
    p0=p1;
  } while (1);
}

void
srtm3::move_to_min(iPoint & p0){
  move_to_extr(p0, false);
}
void
srtm3::move_to_max(iPoint & p0){
  move_to_extr(p0, true);
}
double
srtm3::area(const iPoint &p) const{
  return area0 * cos((double)p.y *M_PI/180.0/srtm_width);
}


/**********************************************************/

Image<short>
read_zfile(const string & file){
  gzFile F = gzopen(file.c_str(), "rb");
  if (!F) return Image<short>(0,0);

  Image<short> im(srtm_width,srtm_width);
  int length = srtm_width*srtm_width*sizeof(short);

  if (length != gzread(F, im.data, length)){
    cerr << "bad file: " << file << '\n';
    return Image<short>(0,0);
  }
  for (int i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  gzclose(F);
  return im;
}

Image<short>
read_file(const string & file){
  FILE *F = fopen(file.c_str(), "rb");
  if (!F) return Image<short>(0,0);

  Image<short> im(srtm_width,srtm_width);
  int length = srtm_width*srtm_width*sizeof(short);

  if (length != fread(im.data, 1, length, F)){
    cerr << "bad file: " << file << '\n';
    return Image<short>(0,0);
  }
  for (int i=0; i<length/2; i++){ // swap bytes
    int tmp=(unsigned short)im.data[i];
    im.data[i] = (tmp >> 8) + (tmp << 8);
  }
  fclose(F);
  return im;
}

bool
srtm3::load(const iPoint & key){

  if ((key.x < -max_lon) ||
      (key.x >= max_lon) ||
      (key.y < -max_lat) ||
      (key.y >= max_lat)) return false;

  char NS='N';
  char EW='E';
  if (key.y<0) {NS='S';}
  if (key.x<0) {EW='W';}

  ostringstream file;
  file << NS << setfill('0') << setw(2) << abs(key.y)
       << EW << setw(3) << abs(key.x) << ".hgt";

  // try f2.gz, f2, f1.gz, f1
  Image<short> im;

  im = read_zfile(srtm_dir + "/fixed/" + file.str() + ".gz");
  if (!im.empty()) goto read_ok;
  im = read_file(srtm_dir + "/fixed/" + file.str());
  if (!im.empty()) goto read_ok;

  im = read_zfile(srtm_dir + "/" + file.str() + ".gz");
  if (!im.empty()) goto read_ok;
  im = read_file(srtm_dir + "/" + file.str());
  if (!im.empty()) goto read_ok;

  cerr << "can't find file " << file.str() << '\n';
  read_ok:

  srtm_cache.add(key, im);
  return !im.empty();
}



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
