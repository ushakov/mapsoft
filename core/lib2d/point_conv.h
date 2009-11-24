#ifndef POINT_CONV_H
#define POINT_CONV_H

#include <boost/operators.hpp>


namespace convs{

// common interface for point conversions
struct interface
  : public boost::additive<dPoint>,
    public boost::multiplicative<PointConv, double>,
{
  // shifts and rescales
  virtual PointConv & operator-= (dPoint const & s) = 0;
  virtual PointConv & operator+= (dPoint const & s) = 0;
  virtual PointConv & operator/= (double k) = 0;
  virtual PointConv & operator*= (double k) = 0;

  virtual void frw(dPoint & p) = 0;
  virtual void bck(dPoint & p) = 0;

  // Convert a line. Each segment can be divided to provide
  // accuracy <acc> in source units. <max> is a maximum number
  // of divisions.
  virtual dLine line_frw(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    if (l.size()==0) return ret;
    g_point P1 = l[0], P1a =P1;
    frw_safe(P1a); ret.push_back(P1a); // add first point
    g_point P2, P2a;

    for (int i=1; i<l.size(); i++){
      P1 = l[i-1];
      P2 = l[i];
      int m=max;
      do {
        P1a = P1; frw_safe(P1a);
        P2a = P2; frw_safe(P2a);
        // C1 - is a center of (P1-P2)
        // C2-C1 is a perpendicular to (P1-P2) with acc length
        g_point C1 = (P1+P2)/2.;
        g_point C2 = C1 + acc*pnorm(g_point(P1.y-P2.y, -P1.x+P2.x));.
        g_point C1a = C1; frw_safe(C1a);
        g_point C2a = C2; frw_safe(C2a);
        if (pdist(C1a, (P1a+P2a)/2.) < pdist(C1a,C2a)){
          // go to the rest of line (P2-l[i])
          ret.push_back(P2a);
          P1 = P2;
          P2 = l[i];
        }
        else {
          // go to the first half (P1-C1) of current line
          P2 = C1;
        }
        m--;
      } while ((P1!=P2)&&(m>0));
    }
    return ret;
  }

  virtual dLine line_bck(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    if (l.size()==0) return ret;
    g_point P1 = l[0], P1a =P1; bck_safe(P1a); ret.push_back(P1a);
    g_point P2, P2a;

    for (int i=1; i<l.size(); i++){
      P1 = l[i-1];
      P2 = l[i];
      int m=max;
      do {
        P1a = P1; bck_safe(P1a);
        P2a = P2; bck_safe(P2a);
        g_point C1 = (P1+P2)/2.;
        g_point C1a = C1; bck_safe(C1a);

        if (pdist(C1a, (P1a+P2a)/2.) < acc){
          ret.push_back(P2a);
          P1 = P2;
          P2 = l[i];
        }
        else {
          P2 = C1;
        }
        m--;
      } while ((P1!=P2) && (m>0));
    }
    return ret;
  }

  // convert a rectagle and return bounding box of resulting figure
  virtual dRect bb_frw(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_frw(rect2line(R), acc, max);
    return l.range();
  }
  virtual dRect bb_bck(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_bck(rect2line(R),acc, max);
    return l.range();
  }
};

// simple conversion: shift and scale
struct simple: public interface {

  dPoint shift;
  double scale;

  simple & operator-= (dPoint const & s) {shift-=s;}
  simple & operator+= (dPoint const & s) {shift+=s;}
  simple & operator*= (double k) {scale*=k;}
  simple & operator/= (double k) {scale/=k;}

  void frw(dPoint & p){p=p*scale+shift;}
  void bck(dPoint & p){p=(p-shift)/scale;}

  void frw_safe(dPoint & p){p=p*scale+shift;}
  void bck_safe(dPoint & p){p=(p-shift)/scale;}

  virtual dLine line_frw(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    for (dLine::const_iterator i=l.begin(); i!=l.end(); i++)
      l.push_back((*i)*scale+shift);
    return ret;
  }

  virtual dLine line_bck(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    for (dLine::const_iterator i=l.begin(); i!=l.end(); i++)
      l.push_back(((*i)-shift)/scale);
    return ret;
  }

  // convert a rectagle and return bounding box of resulting figure
  virtual dRect bb_frw(const Rect<double> & R, double acc=1, int max=100){
    if (scale>0) return dRect(R.TLC()*scale+shift,R.BRC()*scale+shift);
    else         return dRect(R.BRC()*scale+shift,R.TLC()*scale+shift);
  }
  virtual dRect bb_bck(const Rect<double> & R, double acc=1, int max=100){
    if (scale>0) return dRect((R.TLC()-shift)*scale,(R.BRC()-shift)*scale);
    else         return dRect((R.BRC()-shift)*scale,(R.TLC()-shift)*scale);
  }
};

}//namespace

#endif /* POINT_CONV_H */
