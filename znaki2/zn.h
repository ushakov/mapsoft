#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geo_convs.h"
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../yaml/yaml.h"

#include "../utils/m_time.h"

namespace zn{

const int line_mask = 0x100000; // ������� ��������� � ���� ��� �������� ��������
const int area_mask = 0x200000; // ������� ��������� � ���� ��� ��������� ��������


// ���� �������
struct zn_key{
  int  type;           // ��� � mp-����� + line_mask ��� area_mask

  Time time;          // ����� ���������� ���������
  
  int id;             // ���������� ����� �� �����
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


// � ���� ��������� ������ ������ ������
// ������ ���������������� �������!
bool is_map_depth(const fig::fig_object & o);

  // ������� �������� � ������ 

// ������� ���� �� ����������� (2-� �������) � fig-�������
zn_key get_key(const fig::fig_object & fig);

// ������� ���� ������� �� ����������� (2-� �������) � fig-�������
zn_label_key get_label_key(const fig::fig_object & fig);

// ������� ���� �� ����������� (1-� �������) � mp-�������
// ��� ������� � ����� ����������� �� mp-����, � �� �� �����!
// (��������� �� ����� mp-������� �� _�������_ �� ������ �������)
zn_key get_key(const mp::mp_object & mp);

// ��������� ���� � ����������� � fig-�������
void add_key(fig::fig_object & fig, const zn_key & key);

// ��������� ���� ������� � ����������� � fig-�������
void add_key(fig::fig_object & fig, const zn_label_key & key);

// ��������� ���� � ����������� � mp-�������
void add_key(mp::mp_object & mp, const zn_key & key);


// ���������� �� �������� �����������, ����������� �� ����.�����
struct zn{
  std::string name, desc;  // ��������, ��������� ��������
  mp::mp_object mp;        // ��������� mp-�������
  fig::fig_object fig;     // ��������� fig-�������
  fig::fig_object txt;     // ��������� fig-������� �������
  std::string pic;         // �������� ��� ��������� ������� (�.�. ������)
  bool istxt;              // ������ �� �������?
};

// �����, ������� (�� ����.�����) ��� ���������� �������� ����������� 
// � ������� �� ���������������
class zn_conv{
  std::map<int, zn> znaki;      // ����������� �� ����
  fig::fig_object default_fig;  // ��� �������� ����������� �����
  mp::mp_object default_mp;   // ��� �������� ����������� �����

  public:
  // ����������� - ������ ����������������� �����
  zn_conv(const std::string & conf_file);
  bool load_znaki(YAML::Node &root);


    // �������� � mp- � fig-���������, ��� ������� 
    // ��������� ����.����

  // ���������� ��� mp-������� (����� ����������� ������� :))
  int get_type(const mp::mp_object & o) const;

  // ���������� ��� fig-�������.
  // ���� ���� ���� - ��� ������������ �� ����, ����� -
  // �� �������� ����, � ������������ � ����.������)
  int get_type(const fig::fig_object & o) const;

  // ������������� mp-������ � fig-������
  // ���� ����������� ������, ��� ��������� �������� (������ � �����)
  fig::fig_object mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const;

  // ������������� fig-������ � mp-������
  // ���� ����������� ������, ��� ��������� �������� (������ � �����)
  // ������� �� �����������������.
  std::list<mp::mp_object> fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const;

  // ��������� fig-������� � ��������� ������. �����������
  // ���������� ������� ���������� �� ������� ������� (!)
  void fig_make_comp(std::list<fig::fig_object> & objects);

  // ����������� ������ ��� ������ ������������:
  // - �������� ��������� � ������������ � ������
  // - ������� ��������, ���� ����
  // ������ ������ ����� ������ ����!
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig);

  // ������� ������� � �������. ������ ������ ����� ������ ����!
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig);

  // ������ ���� ������ � ������� fig
  fig::fig_world make_legend(int grid = 100, int dg=30);

  // ��������� ������ ���� ������
  std::string make_text();
};


} // namespace

#endif

