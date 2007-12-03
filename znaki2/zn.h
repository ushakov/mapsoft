#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geo_convs.h"
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"

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
 
  bool label;

  zn_key(): type(0), id(0), sid(0), label(false){}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);



// ���������� �� �������� �����������, ����������� �� ����.�����
struct zn{
  std::string name, descr; // ��������, ��������� ��������
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


    // ������� �������� � ������ 

  // ������� ���� �� ����������� (2-� �������) � fig-�������
  zn_key get_key(const fig::fig_object & fig) const;

  // ������� ���� �� ����������� (1-� �������) � mp-�������
  // ��� ������� � ����� ����������� �� mp-����, � �� �� �����!
  // (��������� �� ����� mp-������� �� _�������_ �� ������ �������)
  zn_key get_key(const mp::mp_object & mp) const;

  // ��������� ���� � ����������� � fig-�������
  void add_key(fig::fig_object & fig, const zn_key & key) const;

  // ��������� ���� � ����������� � mp-�������
  void add_key(mp::mp_object & mp, const zn_key & key) const;



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
  mp::mp_object fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const;

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

};


} // namespace

#endif

