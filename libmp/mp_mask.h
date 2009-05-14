#ifndef MP_MASK_H
#define MP_MASK_H

#include <string>
#include "mp_data.h"

namespace mp {
    // Маска:  "POI 0x223 0 1"

    /// Построить mp-объект на основе obj, подставив все не-звездочки из маски
    mp_object make_object(const mp_object & obj, const std::string & mask);
    /// Построить mp-объект на основе объекта по умолчанию
    mp_object make_object(const std::string & mask);
    /// Проверить, соответствует ли объект маске
    bool test_object(const mp_object & obj, const std::string & mask);
}
#endif
