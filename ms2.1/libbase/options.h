#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <iostream>
#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

#include "log.h"

namespace mapsoft{

/** Хранилище для объектов произвольного типа с ключом типа string 
  Удобно для хранения разных параметров типа "count=10 width=0.5
  time=10:20:00 text=aaa" Для типов данных, хранящихся в Options
  должны быть определены операторы ввода и вывода, и конструктор без
  аргументов (чтоб к ним можно было применять  boost::lexical_cast для
  преобразования в std::string и обратно)

  TODO: 
  * (?) оператор +
  * (?) сделать полноценные операторы << и >>, чтоб Options можно
  было хранить в виде строки "key1=val1 ke\\y\=2=va\ l2" :)
*/

/** Стили ввода-вывода */
typedef OPTIONS_IO_STYLE_ENUM enum{
  DEFAULT, /// = XML
  AUTO,    /// 
  XML,     /// a="aaa" b=1 c=2 
  MHEAD    /// a:\taaa\nb:\t1\nc:\t2
};

struct Options : std::map<std::string,std::string>{

    static OPTIONS_IO_STYLE_ENUM io_style;

    /** Поиск неизвестных (не перечисленных в known :)) ключей... 
    тупая, но полезная функция */
    std::set<std::string> unknown (const std::set<std::string> & known) const;

    /** Запись в Options значения val для ключа key. При неудаче с
    преобразованием val в std::string ничего не записывается, в Log
    уходит ругательство... */
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

    /** Чтение из Options значения ключа key в переменную val. Если
    такого ключа нет - val  не перезаписывается. Если происходит
    неудача с преобразованием строки  в тип val, val не
    перезаписывается, в Log уходит ругательство... */
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

    /** Проверить наличие ключа */
    bool exists (const std::string & key) {return (find(key) != end());}

};

/** Операторы вывода-вывода для Options */
std::ostream & operator<< (std::ostream & s, const Options & o);
std::ostream & operator>> (std::istream & s, Options & o);

Options get_cmdline_options(int argc, char **argv);

}//namespace
#endif
