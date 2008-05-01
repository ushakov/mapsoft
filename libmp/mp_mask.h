#ifndef MP_MASK_H
#define MP_MASK_H

#include <string>
#include "mp_data.h"

namespace mp {
    // �����:  "POI 0x223 0 1"

    /// ��������� mp-������ �� ������ obj, ��������� ��� ��-��������� �� �����
    mp_object make_object(const mp_object & obj, const std::string & mask);
    /// ��������� mp-������ �� ������ ������� �� ���������
    mp_object make_object(const std::string & mask);
    /// ���������, ������������� �� ������ �����
    bool test_object(const mp_object & obj, const std::string & mask);
}
#endif
