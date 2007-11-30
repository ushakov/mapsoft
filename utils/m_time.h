#ifndef M_TIME_H
#define M_TIME_H

#include <ctime>
#include <iostream>
#include <boost/operators.hpp>

struct Time:
    public boost::equality_comparable<Time>,
    public boost::less_than_comparable<Time>
{
    time_t value;

    Time(){value=0;}
    Time(const Time & t){value=t.value;}
    Time(time_t v){value=v;}

    void set_current();

  bool operator<  (const Time & t) const { return value < t.value; }
  bool operator== (const Time & t) const { return value == t.value; }
};

std::ostream & operator<< (std::ostream & s, const Time & t);
std::istream & operator>> (std::istream & s, Time & t);

#endif
