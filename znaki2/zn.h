#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"

#include "../utils/m_time.h"

namespace zn{

// ���� �������
struct zn_key{
  int  mp_type;            // ��� � mp-�����
  char mp_class;           // 'p', 'l', 'a', ��� ��������, ��������, ��������� ��������

  Time time;               // ����� ���������� ���������
  
  std::string map_name;    // ������������� �����
  std::string source;      // ������������� ���������
  int id;                  // ���������� ����� �� �����
  int source_id;           // ���������� ����� � ���������
 
  bool label;
  bool empty;

  zn_key():empty(true){}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);

// ���������� �� �������� �����������, ����������� �� ����.�����
struct zn{
  std::string name, descr; // ��������, ��������� ��������
  std::string fig_mask;    // ����� ��� �������� fig-�������
  std::string txt_mask;    // ����� ��� �������� ������� (�.�. ������)
  std::string pic;         // �������� ��� ��������� ������� (�.�. ������)
};

// �����, ������� (�� ����.�����) ��� ���������� �������� ����������� 
// � ������� �� ���������������
class zn_conv{
  std::map<int, zn> zn_p, zn_l, zn_a;

  public:
  zn_conv(const std::string & conf_file);

  // ������� ����� ���� �� ���� fig-�������
  // (����������� ������ ���� mp_type � mp_class)
  zn_key make_key(const fig::fig_object & fig);


  // ������������� mp-������ � fig-������
  fig::fig_object mp2fig(const mp::mp_object & mp, g_map & ref);

  // ������������� fig-������ � mp-������
  // (���� ����� ��� - ��� ������� ������������ ���������� make_key)
  mp::mp_object fig2mp(const fig::fig_object & fig, g_map & ref);


  // ������� ���� �� ����������� � fig-�������
  zn_key get_key(const fig::fig_object & fig); 
  zn_key get_key(const mp::mp_object & mp); 

  // ��������� ���� � ����������� � fig-�������
  zn_key add_key(const fig::fig_object & fig, const zn_key & key); 
  zn_key add_key(const mp::mp_object & mp, const zn_key & key); 

  // ����������� ������ ��� ������ ������������:
  // - �������� ��������� � ������������ � ������
  // - ������� ��������, ���� ����
  // ���� ������� �� ����������� � �������.
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig);

  // - ������� ������� � �������
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig, const zn_key & key);
};


} // namespace

#endif

