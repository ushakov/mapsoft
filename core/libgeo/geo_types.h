#ifndef GEO_TYPES_H
#define GEO_TYPES_H

// типы данных, используемые в геоданных

#include <boost/operators.hpp>
#include <iostream>
#include <string>
#include <map>

typedef std::map<std::string, int> geo_types_table;

struct Enum 
#ifndef SWIG
  : public boost::less_than_comparable<Enum>,
    public boost::equality_comparable<Enum> 
#endif  // SWIG
{
  static const int fmt_mask;
  static const int xml_fmt;
  static const int oe_fmt;
  static int output_fmt;

  int val;
  void set_from_string(const std::string & str);

  bool operator<  (const Enum & e) const { return val <  e.val;}
  bool operator== (const Enum & e) const { return val == e.val;}

  virtual geo_types_table & get_table() const = 0;
  virtual void create_table() = 0;
#ifdef SWIG
  %extend {
    swig_cmp(Enum);
    swig_str();
  }
#endif  // SWIG
};

#ifndef SWIG
std::ostream & operator<< (std::ostream & s, const Enum & e);
std::istream & operator>> (std::istream & s, Enum & e);
#endif  // SWIG

struct Datum : Enum{
  static geo_types_table names;
  Datum(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct Proj : Enum {
  static geo_types_table names;
  Proj(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct wptMapDispl : Enum {
  static geo_types_table names;
  wptMapDispl(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct wptPtDir : Enum {
  static geo_types_table names;
  wptPtDir(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct trkType : Enum {
  static geo_types_table names;
  trkType(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct trkFill : Enum {
  static geo_types_table names;
  trkFill(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

struct wptSymb : Enum {
  static geo_types_table names;
  wptSymb(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

#endif
