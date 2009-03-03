#ifndef POINT_CONV_H
#define POINT_CONV_H

#include <boost/operators.hpp>

// interface for point conversions

struct PointConv
  : public boost::additive<Point<double> >,
    public boost::multiplicative<PointConv, double>,
{
  // shifts and rescales
  virtual PointConv & operator-= (Point<double> const & s) = 0;
  virtual PointConv & operator+= (Point<double> const & s) = 0;
  virtual PointConv & operator/= (double k) = 0;
  virtual PointConv & operator*= (double k) = 0;

  virtual void frw(Point<double> & p) = 0;
  virtual void bck(Point<double> & p) = 0;
  virtual void frw_safe(Point<double> & p) = 0;
  virtual void bck_safe(Point<double> & p) = 0;

  // Convert a line. Each segment can be divided to provide
  // accuracy <acc> in source units. <max> is a maximum number
  // of divisions.
  virtual Line<double> line_frw(const Line<double> & l, double acc=1, int max=100){
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

  virtual Line<double> line_bck(const Line<double> & l, double acc=1, int max=100){
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
  virtual Rect<double> bb_frw(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_frw(rect2line(R), acc, max);
    return l.range();
  }
  virtual Rect<double> bb_bck(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_bck(rect2line(R),acc, max);
    return l.range();
  }
 
};

namespace convs{
struct simple: public PointConv
  Point<double> shift;
  double scale;
  simple & operator-= (Point<double> const & s) {shift-=s;}
  simple & operator+= (Point<double> const & s) {shift+=s;}
  simple & operator*= (double k) {scale*=k;}
  simple & operator/= (double k) {scale/=k;}

  void frw(Point<double> & p){p=p*scale+shift;}
  void bck(Point<double> & p){p=(p-shift)/scale;}

  void frw_safe(Point<double> & p){p=p*scale+shift;}
  void bck_safe(Point<double> & p){p=(p-shift)/scale;}

  virtual Line<double> line_frw(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    for (Line<double>::const_iterator i=l.begin(); i!=l.end(); i++)
      l.push_back((*i)*scale+shift);
    return ret;
  }

  virtual Line<double> line_bck(const Line<double> & l, double acc=1, int max=100){
    g_line ret;
    for (Line<double>::const_iterator i=l.begin(); i!=l.end(); i++)
      l.push_back(((*i)-shift)/scale);
    return ret;
  }

  // convert a rectagle and return bounding box of resulting figure
  virtual Rect<double> bb_frw(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_frw(rect2line(R), acc, max);
    return l.range();
  }
  virtual Rect<double> bb_bck(const Rect<double> & R, double acc=1, int max=100){
    g_line l = line_bck(rect2line(R),acc, max);
    return l.range();
  }


};
}//namespace

#endif /* POINT_CONV_H */
