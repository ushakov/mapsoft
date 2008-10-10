#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <iostream>
//#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

/** Хранилище для объектов произвольного типа с ключом типа string 
  Удобно для хранения разных параметров типа "count=10 width=0.5
  time=10:20:00 text=aaa" Для типов данных, хранящихся в Options
  должны быть определены операторы ввода и вывода, и конструктор без
  аргументов (чтоб к ним можно было применять  boost::lexical_cast для
  преобразования в std::string и обратно)
*/

struct Options : std::map<std::string,std::string>{

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
	  std::cerr << e.what() << " (key: \"" << key << "\")\n";
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
	  std::cerr << e.what() << " (key: \"" << key + "\" value: \"" + it->second + "\")\n";
        }
      }
    }

    /** Поиск неизвестных (не перечисленных в known :)) ключей... 
    тупая, но полезная функция */
    //std::set<std::string> unknown (const std::set<std::string> & known) const;

    /** Поиск неизвестных (не перечисленных в known :)) ключей....
        Старая версия */
    void warn_unused (const std::string * used) const;

    /** Проверить наличие ключа */
    bool exists (const std::string & key) const;

};

/** Операторы вывода-вывода для Options */
std::ostream & operator<< (std::ostream & s, const Options & o);
std::istream & operator>> (std::istream & s, Options & o);

Options get_cmdline_options(int argc, char **argv);

#endif
