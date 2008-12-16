#ifndef ZN_KEY_H
#define ZN_KEY_H

#include <string>
#include <list>
#include <map>
#include "../libgeo/geo_convs.h"
#include "../libgeo_io/geofig.h"
#include "../libmp/mp.h"

#include "../utils/m_time.h"

namespace zn{

typedef unsigned long long  id_type;
// ���� �������
struct zn_key{
  int  type;           // ��� � mp-����� + line_mask ��� area_mask

  Time time;          // ����� ���������� ���������

  id_type id;         // ���������� ����� �� �����
  std::string map;    // ������������� �����
  int sid;            // ���������� ����� � ���������
  std::string source; // ������������� ���������
 
  zn_key(): type(0), id(0), sid(0) {}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);

// ���� ������������ �������
struct zn_label_key{
  int id;             // ����� �������, � �������� ���������������
  std::string map;    // ������������� �����
  zn_label_key(const int i=0, const std::string & s=""): id(i), map(s) {}
  zn_label_key(const zn_key & k): id(k.id), map(k.map) {}
  zn_label_key operator= (const zn_key & k){id=k.id; map=k.map;}
  bool operator== (const zn_label_key & k){return (id==k.id)&&(map==k.map);}
  bool operator== (const zn_key & k){return (id==k.id)&&(map==k.map);}
};

std::ostream & operator<< (std::ostream & s, const zn_label_key & t);
std::istream & operator>> (std::istream & s, zn_label_key & t);


  // ������� �������� � ������

// make unique id
id_type make_id(void);

// make new key with unique id
zn_key make_key(const std::string & map, const std::string & editor);

// ������� ���� �� ����������� (2-� �������) � fig-�������
zn_key get_key(const fig::fig_object & fig);

// ������� ���� ������� �� ����������� (2-� �������) � fig-�������
zn_label_key get_label_key(const fig::fig_object & fig);

// ������� ���� �� ����������� (1-� �������) � mp-�������
zn_key get_key(const mp::mp_object & mp);

// ��������� ���� � ����������� � fig-�������
void add_key(fig::fig_object & fig, const zn_key & key);

// ��������� ���� ������� � ����������� � fig-�������
void add_key(fig::fig_object & fig, const zn_label_key & key);
void add_key(std::list<fig::fig_object> & l, const zn_label_key & key);

// ��������� ���� � ����������� � mp-�������
void add_key(mp::mp_object & mp, const zn_key & key);

// ������� ����� ���� (�� � ��� ������ ����� �����������)
void clear_key(fig::fig_object & fig);
void clear_key(mp::mp_object & mp);


} // namespace

#endif

