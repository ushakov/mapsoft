#ifndef MP_IO_H
#define MP_IO_H

#include <iostream>
#include <string>

#include "mp_data.h"


namespace mp {

  // ������ codepage ���� � ��������� �����
  // �������� �� ��������� ������������ ��� ������ ������ �
  // �� ��������� ���������� � ��� ������ ������
  // �������� charset'a ���������� ����������� "CP"
  extern std::string default_codepage;

  /// ������ mp-�����.
  /// ����� ������� ������������ � world, ��������� world ��������� ����������������
  /// ��� ������ ������������ false, � world �� ��������.
  /// ����������� � ����� ������������� �� default_codepage � UTF-8
  bool read(const char* filename, mp_world & world);

  /// ������ mp-�����
  bool write(std::ostream & out, const mp_world & world);

}
#endif
