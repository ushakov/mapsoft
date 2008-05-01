#ifndef FIG_IO_H
#define FIG_IO_H

#include <iostream>

#include "fig_data.h"

namespace fig {

    extern const char *default_charset;

    /// Чтение fig-файла.
    /// Новые объекты дописываются в world, заголовок world полностью перезаписывается
    /// При ошибке возвращается false, а world не портится.
    /// Комментарии и текст преобразуется из default_charset в UTF-8
    /// Нестандартные цвета преобразуются в "длинные" числа > 0x1000000
    bool read(const char* filename, fig_world & world);

    /// Запись fig-файла
    bool write(std::ostream & out, const fig_world & world);
}
#endif
