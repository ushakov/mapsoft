#ifndef MAPSOFT_CONVS_H
#define MAPSOFT_CONVS_H

// ������������� ��������������

// ��� ������� � ���� ��������, ����� � ������ �������� ���������� ��� ��������� 
// �������������� (��������� ��������� ��������� � �.�.), � ����� ������ ���������������...

#include <vector>
#include "../geo_io/geo_data.h"
#include "mapsoft_options.h"
#include "mapsoft_geo.h"
#include "image.h"

namespace convs{

// �������������� ������������� ���������
// ����� ������������� �� ������, ����� ����� ���� �� �������
// ��������������� ���������� � ������� ������ ���� g_waypoint

// ������ ������� �������������� �� �����:
struct pt2ll{ // �������������� � ������-������� � �������
  pt2ll(const Datum & D, const Proj & P, const Options & Po);
  void frw(g_point & p) const;
  void bck(g_point & p); // ����� �������� lon0!

  private:
    double lat0,lon0,E0,N0,k;
    int zone;
    char zc;
    double a,f;
  public:
    Datum datum;
    Proj  proj;
};

struct ll2wgs{ // �������������� ������-������� � wgs84 � �������
  ll2wgs(const Datum & D);
  void frw(g_point & p) const;
  void bck(g_point & p) const;

  private:
    Datum datum;
};


// � ��� ����� ��������������:
struct pt2pt{
  pt2pt(const Datum & sD, const Proj & sP, const Options & sPo, 
        const Datum & dD, const Proj & dP, const Options & dPo);
  void frw(g_point & p);
  void bck(g_point & p);

  private:
    pt2ll pc1, pc2;
    ll2wgs dc1, dc2;
    bool triv1, triv2;
};

// �������������� �� ����� ����� � ������������� �����
// ����� �� - ��������� ������ ���������� ����� (������ �����������, ������� ������/�����)
// ���� �� - �������������� �����!
struct map2pt{
  map2pt(const g_map & sM, 
         const Datum & dD, const Proj & dP, const Options & dPo);
  void frw(g_point & p);
  void bck(g_point & p);
  std::vector<g_point> line_frw(const std::vector<g_point> & l);
  std::vector<g_point> line_bck(const std::vector<g_point> & l);
  private:
    pt2ll pc1, pc2;
    ll2wgs dc;
    double k_map2geo[6];
    double k_geo2map[6];
  public:
    std::vector<g_point> border;
    std::vector<g_point> border_geo;
};

// ������� �������� ������
struct border_tester{
  private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  std::vector<g_point> border;
  public:
  border_tester(std::vector<g_point> & brd);
  // �������� �� ����� � ������� �������
  bool test(const int x, const int y) const;
  // ���������� �� ��������� ������� ������
  int nearest_border (const int x, const int y) const;
  // "��������" �� ����� ������ �����
  bool test_range(Rect<int> range) const;
};


// �������������� �� ����� � �����
// ����� ����� ���� ������� ��������� ����� �� ����� � ������������� ��������� ����������������...
// ����� �� - �������������� �����
// ����� �� - �������������� �������� (� ����������� ��� � image loader'a)

struct map2map{
  map2map(const g_map & sM, const g_map & dM, const Options & O);
  void frw(g_point & p);
  void bck(g_point & p);

  std::vector<g_point> line_frw(const std::vector<g_point> & l);
  std::vector<g_point> line_bck(const std::vector<g_point> & l);
  // src_scale -- �� ������� ��� ���� ��������� ��������� �������� ��� ��������
  // cnv_rect - ������������� � ��������� _��������������� ��������_!!!
  int image_frw(Image<int> & src_img, int src_scale, Rect<int> cnv_rect,
                Image<int> & dst_img, Rect<int> dst_rect);
  int image_bck(Image<int> & src_img, int src_scale, Rect<int> cnv_rect, 
                Image<int> & dst_img, Rect<int> dst_rect);
    map2pt c1,c2;
    border_tester tst_frw, tst_bck;
    std::vector<g_point> border_src; // ������� sM
    std::vector<g_point> border_dst; // ��� ���� �� ������� sM �� dM! 
};



}//namespace
#endif
