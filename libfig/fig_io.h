#ifndef FIG_IO_H
#define FIG_IO_H

#include <iostream>

#include "fig_data.h"

namespace fig {

    extern const char *default_charset;

    /// ������ fig-�����.
    /// ����� ������� ������������ � world, ��������� world ��������� ����������������
    /// ��� ������ ������������ false, � world �� ��������.
    /// ����������� � ����� ������������� �� default_charset � UTF-8
    /// ������������� ����� ������������� � "�������" ����� > 0x1000000
    bool read(const char* filename, fig_world & world);

    /// ������ fig-�����
    bool write(std::ostream & out, const fig_world & world);
}
#endif
