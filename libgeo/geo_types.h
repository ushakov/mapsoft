#ifndef GEO_TYPES_H
#define GEO_TYPES_H

// типы данных, используемые в геоданных

#include <boost/operators.hpp>
#include <iostream>
#include <string>
#include <map>
#include <memory>

typedef std::map<std::string, int> geo_types_table;

struct Enum :
    public boost::less_than_comparable<Enum>,
    public boost::equality_comparable<Enum> {
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
};

std::ostream & operator<< (std::ostream & s, const Enum & e);
std::istream & operator>> (std::istream & s, Enum & e);

struct Datum : Enum{
  static geo_types_table names;
  Datum(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct Proj : Enum {
  static geo_types_table names;
  Proj(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct wptMapDispl : Enum {
  static geo_types_table names;
  wptMapDispl(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct wptPtDir : Enum {
  static geo_types_table names;
  wptPtDir(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct trkType : Enum {
  static geo_types_table names;
  trkType(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct trkFill : Enum {
  static geo_types_table names;
  trkFill(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

struct wptSymb : Enum {
  static geo_types_table names;
  wptSymb(const std::string & str = "");
  geo_types_table & get_table() const { return names;}
  void create_table();
};

#endif
