#ifndef M_TIME_H
#define M_TIME_H

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/operators.hpp>

struct Time
#ifndef SWIG
    : public boost::equality_comparable<Time>,
    public boost::less_than_comparable<Time>
#endif  // SWIG
{
    time_t value;

    Time(){value=0;}
    Time(const Time & t){value=t.value;}
    Time(time_t v){value=v;}
    Time(const std::string & s);

    void set_current();
    std::string time_str();
    std::string date_str();

#ifndef SWIG
  bool operator<  (const Time & t) const { return value < t.value; }
  bool operator== (const Time & t) const { return value == t.value; }
#endif  // SWIG

#ifdef SWIG
  %extend {
    swig_cmp(Time);
  }
#endif
};

std::ostream & operator<< (std::ostream & s, const Time & t);
std::istream & operator>> (std::istream & s, Time & t);

#endif
