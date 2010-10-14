#include "line_utils.h"
#include "point_conv.h"

void
Conv::line_frw_p2p(dLine & l) const {
  for (dLine::iterator i=l.begin(); i!=l.end(); i++) frw(*i);
}

void
Conv::line_bck_p2p(dLine & l) const {
  for (dLine::iterator i=l.begin(); i!=l.end(); i++) bck(*i);
}

dLine
Conv::line_frw(const dLine & l, double acc, int max) const {
  dLine ret;
  if (l.size()==0) return ret;
  dPoint P1 = l[0], P1a =P1;
  frw(P1a); ret.push_back(P1a); // add first point
  dPoint P2, P2a;

  for (int i=1; i<l.size(); i++){
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

  for (int i=1; i<l.size(); i++){
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


/*************************************************/

SimpleConv &
SimpleConv::operator-= (dPoint const & s) {shift-=s; return *this;}

SimpleConv &
SimpleConv::operator+= (dPoint const & s) {shift+=s; return *this;}

SimpleConv &
SimpleConv::operator*= (double const k) {scale*=k; return *this;}

SimpleConv &
SimpleConv::operator/= (double const k) {scale/=k; return *this;}

void
SimpleConv::frw(dPoint & p){p=p*scale+shift;}

void
SimpleConv::bck(dPoint & p){p=(p-shift)/scale;}

dLine
SimpleConv::line_frw(const dLine & l, double acc, int max){
  dLine ret;
  for (dLine::const_iterator i=l.begin(); i!=l.end(); i++)
    ret.push_back((*i)*scale+shift);
  return ret;
}

dLine
SimpleConv::line_bck(const dLine & l, double acc, int max){
  dLine ret;
  for (dLine::const_iterator i=l.begin(); i!=l.end(); i++)
    ret.push_back(((*i)-shift)/scale);
  return ret;
}

dRect
SimpleConv::bb_frw(const dRect & R, double acc, int max){
  if (scale>0) return dRect(R.TLC()*scale+shift,R.BRC()*scale+shift);
  else         return dRect(R.BRC()*scale+shift,R.TLC()*scale+shift);
}
dRect
SimpleConv::bb_bck(const dRect & R, double acc, int max){
  if (scale>0) return dRect((R.TLC()-shift)*scale,(R.BRC()-shift)*scale);
  else         return dRect((R.BRC()-shift)*scale,(R.TLC()-shift)*scale);
}
