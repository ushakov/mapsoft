#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig.h"
#include "geo_data.h"


namespace fig {

  // ��������� �������� �� fig_world
  g_map get_ref(const fig_world & w);
  // ������ �������� �� fig_world
  void rem_ref(fig_world & w);
  // �������� �������� � fig_world
  void set_ref(fig_world & w, const g_map & m, const Options & o);

  // ������� ����� � ����� � ��������� �� � geo_data
  void get_wpts(const fig_world & w, const g_map & m, geo_data & d);
  void get_trks(const fig_world & w, const g_map & m, geo_data & d);
  // ������� �������� ��������� ��������
  void get_maps(const fig_world & w, const g_map & m, geo_data & d);
  // �������� ����� � ����� � fig_world � �����. � ���������
  void put_wpts(fig_world & w, const g_map & m, const geo_data & d);
  void put_trks(fig_world & w, const g_map & m, const geo_data & d);
}
#endif
