#ifndef MP_IO_H
#define MP_IO_H

#include <iostream>
#include <string>

#include "mp_data.h"


namespace mp {

  // Обычно codepage есть в заголовке файла
  // Значение по умолчанию используется для чтения файлов с
  // не указанной кодировкой и для записи файлов
  // Название charset'a получается добавлением "CP"
  extern std::string default_codepage;

  /// Чтение mp-файла.
  /// Новые объекты дописываются в world, заголовок world полностью перезаписывается
  /// При ошибке возвращается false, а world не портится.
  /// Комментарии и текст преобразуется из default_codepage в UTF-8
  bool read(const char* filename, mp_world & world);

  /// Запись mp-файла
  bool write(std::ostream & out, const mp_world & world);

}
#endif
