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

  zn_key():mp_type(0), id(0), sid(0), label(false){}
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
  fig::fig_object default_mp;   // ��� �������� ����������� �����

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
  int zn_conv::get_type(const mp::mp_object & o) const;

  // ���������� ��� fig-�������.
  // ���� ���� ���� - ��� ������������ �� ����, ����� -
  // �� �������� ����, � ������������ � ����.������)
  int zn_conv::get_type(const fig::fig_object & o) const;



  // ������������� mp-������ � fig-������
  // ���� ����������� ������, ��� ��������� �������� (������ � �����)
  fig::fig_object mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const;

  // ������������� fig-������ � mp-������
  // ���� ����������� ������, ��� ��������� �������� (������ � �����)
  mp::mp_object fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const;



  // ����������� ������ ��� ������ ������������:
  // - �������� ��������� � ������������ � ������
  // - ������� ��������, ���� ����
  // ���� ������� �� ����������� � �������.
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig){
    // ...
  }

  // ������� ������� � �������. ������ ������ ����� ������ ����!
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig){

    txt_dist = 10; // fig units

    std::list<fig::fig_object> ret;
    zn_key key = get_key(fig);
    if (fig.size() < 1) return ret;                    // �������� ������
    if ((key.id == 0) && (key.type == 0)) return ret;  // �������� ����
    if (znaki.find(key.type)==znaki.end()) return ret; // ��� ����� ��� �������� ����������
    if (!znaki[key.type].istxt) return ret; // ������� �� �����
    // ��������� ��� �������
    fig::fig_object txt = znaki[fig.type].txt;

    // ��������� ���������� � ������ �������
    Point<int> p = fig[0] + Point<int>(1,1)*txt_dist;
    if (fig.size()>=2){
      if ((key.type > line_mask) && (key.type < area_mask)){ // �����
        p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
        Point<int> v = fig[fig.size()/2-1] - fig[fig.size()/2];
        if (v.x<0) v.x=-v.x;
        txt.angle = atan2(v.x, v.y);
        txt.sub_type = 1; // centered text
        Point<int> vp(-v.y, v.x);
        p+=vp*txt_dist;
      } 
      else { // ��������� ������
        Point<int> max = p;
        for (int i = 0; i<fig.size(); i++){
          
        }
      }
    }
    txt.push_back(p);
    ret.push_back(txt)
    return(ret);
  }
};


} // namespace

#endif

