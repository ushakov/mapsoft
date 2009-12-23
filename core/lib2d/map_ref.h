#ifndef MAP_REF_H
#define MAP_REF_H

#include "line.h"
#include <vector>
#include <map>
#include <boost/operators.hpp>

/// A set of reference points with a border.
/// This object isn't used now, becouse i don't know where
/// it can be useful without geo conversions...

template <typename T1, typename T2>
struct MapRef : std::vector <std::pair<Point<T1>, Point<T2> > >
#ifndef SWIG
  , public boost::multiplicative<MapRef<T1,T2>, T1>,
    public boost::additive<MapRef<T1,T2>, Point<T1> >
#endif  // SWIG
{
    Line<T1> border;

    void add(const Point<T1> & pr, const Point<T2> & pg){
      push_back(std::pair<Point<T1>, Point<T2> >(pr, pg));
    }

    void add(const T1 xr, const T1 yr, const T2 xg, const T2 yg){
      add(Point<T1>(xr,yr), Point<T2>(xg,yg));
    }

    MapRef<T1,T2> & operator/= (const T1 k){
      for (typename MapRef<T1, T2>::iterator i=this->begin(); i!=this->end();i++){
        i->first /= k;
        i->first /= k;
      }
      border/=k;
      return *this;
    }

    MapRef & operator*= (const T1 k){
      for (typename MapRef<T1, T2>::iterator i=this->begin(); i!=this->end();i++){
        i->first *= k;
        i->first *= k;
      }
      border*=k;
      return *this;
    }

    MapRef & operator-= (const Point<T1> & p){
      for (typename MapRef<T1, T2>::iterator i=this->begin(); i!=this->end();i++){
        i->first -= p;
        i->first -= p;
      }
      border+=p;
      return *this;
    }

    MapRef & operator+= (const Point<T1> & p){
      for (typename MapRef<T1, T2>::iterator i=this->begin(); i!=this->end();i++){
        i->first += p;
        i->first += p;
      }
      border+=p;
      return *this;
    }

#ifdef SWIG
    %extend {
      MapRef operator/(T1 p) { return *$self / p; }
      MapRef operator*(T1 p) { return *$self * p; }
      MapRef operator-(const Point<T1>& p) { return *$self - p; }
      MapRef operator+(const Point<T1>& p) { return *$self + p; }
    }
#endif  // SWIG
};

typedef MapRef<double,double> dMapRef;
typedef MapRef<int,int>       iMapRef;

#endif
