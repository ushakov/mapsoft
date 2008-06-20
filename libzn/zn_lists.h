#ifndef ZN_LISTS_H
#define ZN_LISTS_H

#include <string>
#include <list>
#include <map>
#include "../libfig/fig.h"

#include "zn.h"

// ����������� ������ ������� ������

namespace zn{

  // ������� ������ �������
  fig::fig_object make_sample(const std::map<int, zn>::const_iterator &i, int grid=100, int dg=30);

  // ������ ���� ������ � ������� fig
  fig::fig_world make_legend(zn_conv & z, int grid = 100, int dg=30);
  // ��������� ������ ���� ������
  std::string make_text(zn_conv & z);

} // namespace

#endif

