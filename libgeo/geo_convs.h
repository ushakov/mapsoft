#ifndef GEO_CONVS_H
#define GEO_CONVS_H

// ������������� ��������������

#include <vector>
#include "geo_data.h"
#include "../utils/options.h"
#include "../lib2d/image.h"

// ��� ������� � ���� ��������, ����� � ������ �������� ���������� ��� ��������� 
// �������������� (��������� ��������� ��������� � �.�.), � ����� ������ ���������������...

namespace convs{

// �������������� ������������� ���������
// ����� ������������� �� ������, ����� ����� ���� �� �������
// ��������������� ���������� � ������� ������ ���� g_waypoint

// ������ ������� �������������� �� �����:
struct pt2ll{ // �������������� � ������-������� � �������
  pt2ll(const Datum & D = Datum("wgs84"), const Proj & P = Proj("lonlat"), const Options & Po = Options());
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
  ll2wgs(const Datum & D = Datum("wgs84"));
  void frw(g_point & p) const;
  void bck(g_point & p) const;

  private:
    Datum datum;
};


// � ��� ����� ��������������:
struct pt2pt{

  pt2pt(const Datum & sD, const Proj & sP, const Options & sPo, 
        const Datum & dD, const Proj & dP, const Options & dPo);

//  pt2pt(const char * sD, const char * sP, const Options & sPo, 
//        const char * dD, const char * dP, const Options & dPo);

  pt2pt();

  void frw(g_point & p);
  void bck(g_point & p);
  // �������������� �����
  // �������� acc - � ����������� �������� ��������
  g_line line_frw(const g_line & l, double acc, int max=100);
  g_line line_bck(const g_line & l, double acc, int max=100);

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
         const Datum & dD, const Proj & dP, const Options & dPo = Options());
//  map2pt(const g_map & sM,
//         const char * dD, const char * dP, const Options & dPo = Options());

  void frw(g_point & p);
  void bck(g_point & p);
  g_line line_frw(const g_line & l, int max=100);
  g_line line_bck(const g_line & l, int max=100);
  // �������������� �������������� (� ������������ ������) � ���������� 
  // ������������ ��������������, � ������� ��� �����
  Rect<double> bb_frw(const Rect<int> & R);
  Rect<int> bb_bck(const Rect<double> & R);
  private:
    pt2ll pc1, pc2;
    ll2wgs dc;
    double k_map2geo[6];
    double k_geo2map[6];
  public:
    g_line border;
    g_line border_geo;
};

// ������� �������� ������
struct border_tester{
  private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  g_line border;
  public:
  border_tester(g_line & brd);
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
  map2map(const g_map & sM, const g_map & dM, bool test_brd_ = true);
  void frw(g_point & p);
  void bck(g_point & p);

  g_line line_frw(const g_line & l, int max=100);
  g_line line_bck(const g_line & l, int max=100);
  // src_scale -- �� ������� ��� ���� ��������� ��������� �������� ��� ��������
  // cnv_rect - ������������� � ��������� _��������������� ��������_!!!
  int image_frw(Image<int> & src_img, int src_scale, Rect<int> cnv_rect,
                Image<int> & dst_img, Rect<int> dst_rect);
  int image_bck(Image<int> & src_img, int src_scale, Rect<int> cnv_rect, 
                Image<int> & dst_img, Rect<int> dst_rect);

  // ����� ������
  void image_frw(Image<int> & src_img, int src_scale, Point<int> origin, Image<int> & image);
  void image_bck(Image<int> & src_img, int src_scale, Point<int> origin, Image<int> & image);

  // �������������� �������������� (� ������������ ������) � ���������� 
  // ������������ ��������������, � ������� ��� �����
  Rect<int> bb_frw(const Rect<int> & R);
  Rect<int> bb_bck(const Rect<int> & R);

    bool test_brd;
    map2pt c1,c2;
    border_tester tst_frw, tst_bck;
    g_line border_src; // ������� sM
    g_line border_dst; // ��� ���� �� ������� sM �� dM! 
};

g_map mymap(const geo_data & world); // ������������ �������� ���������
// ��� �������� -- �� ������ �����, ��� lonlat, ���� ���� ���
// ������ �������� -- 6n+3, �������� ������� � �������� ��������� ������ � �����,
// � ���� �� ��� - � �������� ��������� ����
// ������� -- ��������������� ������������ �������� ����, ���� ��� ����,
// ��� 1/3600 ������� �� �����, ���� ���� ���
double map_mpp(const g_map &map); // �������, ������ ��� �������� (� ���.�� ��������) � �����

}//namespace
#endif
