#ifndef FIG_UTILS_H
#define FIG_UTILS_H

#include <list>
#include "fig_data.h"
#include "fig_mask.h"

/// ��������� � fig-�������
namespace fig {

  /// �������������� ����� pt �� ��������� ����� �� objects, ��������������� ����� mask,
  /// ������� ����������� (��������� ������ vec) ����� � ���� ����� � ���������� �� �����
  /// (����� ��� ��� ��� ������������ ������ �/� ��������, �������, ��������� -
  /// �������� ��������, ����������� � �����, ������� ������� ����������
  /// ��������������)
  double nearest_pt(Point<double> & vec, Point<double> & pt,
    const std::list<fig_object> & objects, const std::string & mask, double maxdist=100);

  // ������ fig-��������
  Rect<int> range(std::list<fig_object> & objects);

  // ��������� fig-������� � ��������� ������.
  void fig_make_comp(std::list<fig_object> & objects);

  // ��������� �� ���� a ������ ����� p0
  void fig_rotate(std::list<fig_object> & objects,
    const double a, const Point<int> & p0 = Point<int>(0,0));

  // ������������� ������� � ������
  void any2xspl(fig_object & o, const double x, const double y=0);

  /// ������� fig-����� �� ��������������
  void rect_crop(const Rect<int> & cutter, std::list<fig_object> & objects);

  /// ������� fig-������� �� ��������������. ������ ����� ������� 
  /// ����� �� ��������� �����, �.�. ���� ������ � �.�. fig-�!
  void rect_crop(const Rect<int> & cutter, fig_object & o);

}
#endif
