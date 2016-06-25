#include <iomanip>
#include <string>
#include <sstream>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

#include "m_time.h"

std::ostream &
operator<< (std::ostream & s, const Time & t) {
  struct tm * ts = localtime(&t.value);
  if (ts == NULL) { time_t t = 0;  ts = localtime(&t);}
  s << std::setfill('0')
    << std::setw(4) << ts->tm_year+1900 << "-"
    << std::setw(2) << ts->tm_mon+1 << "-"
    << std::setw(2) << ts->tm_mday  << " "
    << std::setw(2) << ts->tm_hour  << ":"
    << std::setw(2) << ts->tm_min   << ":"
    << std::setw(2) << ts->tm_sec;
  return s;
}

/* time string: HH:MM:SS */
std::string
Time::time_str() const {
  struct tm * ts = localtime(&value);
  if (ts == NULL) { time_t t = 0;  ts = localtime(&t);}
  std::ostringstream s;
  s << std::setfill('0')
    << std::setw(2) << ts->tm_hour  << ":"
    << std::setw(2) << ts->tm_min   << ":"
    << std::setw(2) << ts->tm_sec;
  return s.str();
}

/* date string: yyyy-mm-dd */
std::string
Time::date_str() const {
  struct tm * ts = localtime(&value);
  if (ts == NULL) { time_t t = 0;  ts = localtime(&t);}
  std::ostringstream s;
  s << std::setfill('0')
    << std::setw(4) << ts->tm_year+1900 << "-"
    << std::setw(2) << ts->tm_mon+1 << "-"
    << std::setw(2) << ts->tm_mday;
  return s.str();
}

/* Time in GPX format: 2011-09-27T18:50:03Z */
std::string
Time::gpx_str() const {
  struct tm * ts = localtime(&value);
  if (ts == NULL) { time_t t = 0;  ts = localtime(&t);}
  std::ostringstream s;
  s << std::setfill('0')
    << std::setw(4) << ts->tm_year+1900 << '-'
    << std::setw(2) << ts->tm_mon+1 << '-'
    << std::setw(2) << ts->tm_mday  << 'T'
    << std::setw(2) << ts->tm_hour  << ':'
    << std::setw(2) << ts->tm_min   << ':'
    << std::setw(2) << ts->tm_sec << 'Z';
  return s.str();
}


void Time::set_current(){ value = time(NULL); }

Time::Time(const std::string & str){
  // date and time "yyyy-mm-dd HH:MM:SS"
  //               "yy-mm-dd HH:MM:SS"
  //               "yyyy-mm-dd HH:MM"
  //               "yyyy-mm-dd"
  //               "yyyy-mm-ddThh:mm:ssZ -- gpx,kml format
  //               "yyyy-mm-ddThh:mm:ss.sssZ -- gpx,kml format

  using namespace boost::spirit::classic;
  time_t t = time(NULL);
  struct tm * ts = localtime(&t);

  if (parse(str.c_str(),
      uint_p[assign_a(ts->tm_year)] >> '-' >>
      uint_p[assign_a(ts->tm_mon)]  >> '-' >>
      uint_p[assign_a(ts->tm_mday)] >>
      !((+blank_p | 'T') >>
        uint_p[assign_a(ts->tm_hour)] >> ':' >>
        uint_p[assign_a(ts->tm_min)]  >>
        !(':' >> uint_p[assign_a(ts->tm_sec)]) >>
        !('.' >> uint_p)
       ) >> !ch_p('Z') >> *space_p
      ).full){
    ts->tm_mon-=1;
    if (ts->tm_year>1900) ts->tm_year-=1900;
    value = mktime(ts);
  }

  // ругаемся, если строчка непуста.
  // возвращаем 0.
  else {
    if (!str.empty()) std::cerr << "Time: can't find valid date and time in " << str << "\n";
    value = 0;
  }
}

std::istream & operator>> (std::istream & s, Time & t){
  std::string str;
  std::getline(s, str);
  t.value=Time(str).value;
  return s;
}
