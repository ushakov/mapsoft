#ifndef YAML_H
#define YAML_H

#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <syck.h>

using namespace std;


// Запихать бы это все в класс YAML::Parser...
// Сходу не получилось.

namespace YAML {

   // Типы элементов
   typedef enum { STRING, ARRAY, HASH } Type;

   // Тип "Массив элементов"
   struct Node;
   typedef vector<Node> NodeList;

   // Элемент
   struct Node {
      // Используется если тип -- строка
      string value;
      // Используется если тип -- массив или хэш
      NodeList value_array;
      // Используется для хранения ключей хэша. Соответствующие значения
      // лежат с тем же индексом в value_array
      NodeList key_array;
      // Тип элемента
      Type type;
      // Возвращает индекс от 0 до key_array.size()-1 соответствующий
      // ключу по строке. Если не находит возвращает -1.
      int key_index (const char *str) {
         for (NodeList::size_type i=0; i<key_array.size(); i++) {
            if (key_array[i].type == STRING && key_array[i].value == str)
               return i;
         }
         return -1;
      }
      // Возвращает строку-значение по строке-ключу. При ошибке
      // возвращает NULL.
      const char *hash_str_value(const char *str) {
         int k = key_index(str);
         if (k == -1) return NULL;
         else {
            Node v = value_array[k];
            if (v.type != STRING) return NULL;
            else return v.value.c_str();
         }
      }
   };

   // Обработчик элемента для Syck
   SYMID node_handler(SyckParser *p, SyckNode *n );
   // Обработчик ошибки для Syck
   void error_handler(SyckParser *p, char *str);

   Node  &parse(FILE *file);

   // Массив элементов
   extern NodeList nodes;
   // Сообщение парсера об ошибке
   extern string error;

};
#endif
