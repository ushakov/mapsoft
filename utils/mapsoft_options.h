#ifndef MAPSOFT_OPTIONS_H
#define MAPSOFT_OPTIONS_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utils/point.h>
#include <utils/generic_accessor.h>

struct Options : std::map<std::string,std::string>{

  // добавить параметр из строки вида a[=b]
  bool         put_string  (const std::string & kv);

  std::string  get_string  (const std::string & key, const std::string & dflt="" ) const;
  double       get_double  (const std::string & key, const double dflt=0        ) const;
  double       get_udouble (const std::string & key, const double dflt=0        ) const;
  int          get_int     (const std::string & key, const int dflt=0           ) const;
  unsigned int get_uint    (const std::string & key, const unsigned int dflt=0  ) const;
  char         get_char    (const std::string & key, const char dflt='?'        ) const;
  unsigned int get_hex     (const std::string & key, const unsigned int dflt=0  ) const;
  bool         get_bool    (const std::string & key) const;
  time_t       get_time    (const std::string & key, const time_t dflt=0) const;
  std::vector<Point<double> > get_poly    
	(const std::string & key, const std::vector<Point<double> > & dflt = std::vector<Point<double> >() ) const;
  void warn_unused (const std::string * used) const;
};

std::ostream & operator<< (std::ostream & s, const Options & o);

template<>
inline GenericAccessor * CreateGenericAccessor<Options> (Options * opt) {
    GenericAccessor * ga = new GenericAccessor;
    for (Options::iterator i = opt->begin();
	 i != opt->end();
	 ++i) {
	ga->add_item (i->first, &(i->second));
    }
    return ga;
}

#endif
