#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <iostream>
#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

#include "log.h"

namespace mapsoft{

/** ��������� ��� �������� ������������� ���� � ������ ���� string 
  ������ ��� �������� ������ ���������� ���� "count=10 width=0.5
  time=10:20:00 text=aaa" ��� ����� ������, ���������� � Options
  ������ ���� ���������� ��������� ����� � ������, � ����������� ���
  ���������� (���� � ��� ����� ���� ���������  boost::lexical_cast ���
  �������������� � std::string � �������)

  TODO: 
  * (?) �������� +
  * (?) ������� ����������� ��������� << � >>, ���� Options �����
  ���� ������� � ���� ������ "key1=val1 ke\\y\=2=va\ l2" :)
*/

/** ����� �����-������ */
typedef OPTIONS_IO_STYLE_ENUM enum{
  DEFAULT, /// = XML
  AUTO,    /// 
  XML,     /// a="aaa" b=1 c=2 
  MHEAD    /// a:\taaa\nb:\t1\nc:\t2
};

struct Options : std::map<std::string,std::string>{

    static OPTIONS_IO_STYLE_ENUM io_style;

    /** ����� ����������� (�� ������������� � known :)) ������... 
    �����, �� �������� ������� */
    std::set<std::string> unknown (const std::set<std::string> & known) const;

    /** ������ � Options �������� val ��� ����� key. ��� ������� �
    ��������������� val � std::string ������ �� ������������, � Log
    ������ ������������... */
    template<typename T>
    void put (const std::string & key, T val) {
        std::string str_val;
	try {
	  (*this)[key] = boost::lexical_cast<std::string>(val);
	}
	catch (boost::bad_lexical_cast & e) {
	  LOG(std::string(e.what()) + " for key: \"" + key + "\"");
	}
    }

    /** ������ �� Options �������� ����� key � ���������� val. ����
    ������ ����� ��� - val  �� ����������������. ���� ����������
    ������� � ��������������� ������  � ��� val, val ��
    ����������������, � Log ������ ������������... */
    template<typename T>
    void get (const std::string & key, T & val) const {
      std::map<std::string, std::string>::const_iterator it = find(key);
      if (it != end()) {
        try {
          val = boost::lexical_cast<T>(it->second);
        }
        catch (boost::bad_lexical_cast & e) {
	  LOG(std::string(e.what()) + " for key: \"" + key + "\" value: \"" + it->second + "\"");
        }
      }
    }

    /** ��������� ������� ����� */
    bool exists (const std::string & key) {return (find(key) != end());}

};

/** ��������� ������-������ ��� Options */
std::ostream & operator<< (std::ostream & s, const Options & o);
std::ostream & operator>> (std::istream & s, Options & o);

Options get_cmdline_options(int argc, char **argv);

}//namespace
#endif
