#include "conv.h"
#include "line_utils.h"

Conv::Conv(){sc_dst=1; sc_src=1;}

void
Conv::line_frw_p2p(dLine & l) const {
  for (dLine::iterator i=l.begin(); i!=l.end(); i++) frw(*i);
}

void
Conv::line_frw_p2p(dMultiLine & l) const {
  for (dMultiLine::iterator i=l.begin(); i!=l.end(); i++)
    for (dLine::iterator j=i->begin(); j!=i->end(); j++) frw(*j);
}

void
Conv::line_bck_p2p(dLine & l) const {
  for (dLine::iterator i=l.begin(); i!=l.end(); i++) bck(*i);
}

void
Conv::line_bck_p2p(dMultiLine & l) const {
  for (dMultiLine::iterator i=l.begin(); i!=l.end(); i++)
    for (dLine::iterator j=i->begin(); j!=i->end(); j++) bck(*j);
}

dLine
Conv::line_frw(const dLine & l, double acc, int max) const {
  dLine ret;
  if (l.size()==0) return ret;
  dPoint P1 = l[0], P1a =P1;
  frw(P1a); ret.push_back(P1a); // add first point
  dPoint P2, P2a;

  for (size_t i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    double d = pdist(P1-P2)/(max+1)*1.5;
    do {
      P1a = P1; frw(P1a);
      P2a = P2; frw(P2a);
      // C1 - is a center of (P1-P2)
      // C2-C1 is a perpendicular to (P1-P2) with acc length
      dPoint C1 = (P1+P2)/2.;
      dPoint C2 = C1 + acc*pnorm(dPoint(P1.y-P2.y, -P1.x+P2.x));
      dPoint C1a = C1; frw(C1a);
      dPoint C2a = C2; frw(C2a);
      if ((pdist(C1a, (P1a+P2a)/2.) < pdist(C1a,C2a)) ||
          (pdist(P1-P2) < d)){
        // go to the rest of line (P2-l[i])
        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        // go to the first half (P1-C1) of current line
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}

dLine
Conv::line_bck(const dLine & l, double acc, int max) const {
  dLine ret;
  if (l.size()==0) return ret;
  dPoint P1 = l[0], P1a =P1; bck(P1a); ret.push_back(P1a);
  dPoint P2, P2a;

  for (size_t i=1; i<l.size(); i++){
    P1 = l[i-1];
    P2 = l[i];
    double d = pdist(P1-P2)/(max+1)*1.5;
    do {
      P1a = P1; bck(P1a);
      P2a = P2; bck(P2a);
      dPoint C1 = (P1+P2)/2.;
      dPoint C1a = C1; bck(C1a);

      if ((pdist(C1a, (P1a+P2a)/2.) < acc) ||
          (pdist(P1-P2) < d)){

        ret.push_back(P2a);
        P1 = P2;
        P2 = l[i];
      }
      else {
        P2 = C1;
      }
    } while (P1!=P2);
  }
  return ret;
}

// convert a rectagle and return bounding box of resulting figure
dRect
Conv::bb_frw(const dRect & R, double acc, int max) const {
  dLine l = line_frw(rect2line(R), acc, max);
  return l.range();
}
dRect
Conv::bb_bck(const dRect & R, double acc, int max) const {
  dLine l = line_bck(rect2line(R),acc, max);
  return l.range();
}

double
Conv::ang_frw(dPoint p, double a, double dx) const{
  dPoint p1 = p + dPoint(dx*cos(a), dx*sin(a));
  dPoint p2 = p - dPoint(dx*cos(a), dx*sin(a));
 frw(p1); frw(p2);
  p1-=p2;
  return atan2(p1.y, p1.x);
}

double
Conv::ang_bck(dPoint p, double a, double dx) const{
  dPoint p1 = p + dPoint(dx*cos(a), dx*sin(a));
  dPoint p2 = p - dPoint(dx*cos(a), dx*sin(a));
  bck(p1); bck(p2);
  p1-=p2;
  return atan2(p1.y, p1.x);
}

double
Conv::angd_frw(dPoint p, double a, double dx) const{
  return 180.0/M_PI * ang_frw(p, M_PI/180.0*a, dx);
}

double
Conv::angd_bck(dPoint p, double a, double dx) const{
  return 180.0/M_PI * ang_bck(p, M_PI/180.0*a, dx);
}

dPoint
Conv::units_frw(dPoint p) const{
  dPoint p1 = p + dPoint(1,0);
  dPoint p2 = p + dPoint(0,1);
  frw(p), frw(p1), frw(p2);
  return dPoint(pdist(p1-p), pdist(p2-p));
}

dPoint
Conv::units_bck(dPoint p) const{
  dPoint p1 = p + dPoint(1,0);
  dPoint p2 = p + dPoint(0,1);
  bck(p), bck(p1), bck(p2);
  return dPoint(pdist(p1-p), pdist(p2-p));
}

void
Conv::image_frw(const iImage & src_img, iImage & dst_img,
                const iPoint & shift, const double scale) const{
  for (int y=0; y<dst_img.h; y++){
    for (int x=0; x<dst_img.w; x++){
      dPoint p(x,y); p+=shift; bck(p); p*=scale;
      int c = src_img.safe_get(int(p.x),int(p.y));
      dst_img.set_a(x,y,c);
    }
  }
}

void
Conv::image_bck(const iImage & src_img, iImage & dst_img,
                const iPoint & shift, const double scale) const{
  for (int y=0; y<dst_img.h; y++){
    for (int x=0; x<dst_img.w; x++){
      dPoint p(x,y); p+=shift; frw(p); p*=scale;
      int c = src_img.safe_get(int(p.x),int(p.y));
      dst_img.set_a(x,y,c);
    }
  }
}

void
Conv::rescale_src(const double s){ sc_src*=s; }
void
Conv::rescale_dst(const double s){ sc_dst*=s; }
