#include <iostream>
#include <map>
#include "map.h"

map::map(char *dir, int Lat1,int Lon1, int Lat2, int Lon2){
  lat1 = (Lat1<Lat2)? Lat1:Lat2;
  lat2 = (Lat1<Lat2)? Lat2:Lat1;
  lon1 = (Lon1<Lon2)? Lon1:Lon2;
  lon2 = (Lon1<Lon2)? Lon2:Lon1;
  w = lon2-lon1;
  h = lat2-lat1;
  // ���������� � hgt-�������, ������ ����, ������������
  srtm s(dir, w/1200+2, mode_interp);
  // w/1200+2, ��� ��� �������, ����� �� ����� ����������� �������� ��� �������
  // ������ ������. �������� ����� ���������� ��� ������������

  for (int lat=lat1; lat<lat2; lat++){
    for (int lon=lon1; lon<lon2; lon++){
      map_pt p; 
      p.alt = s.geth(lat, lon);
      if (p.alt >srtm_min_interp) p.alt -=srtm_zer_interp;
      data.push_back(p);
    }
  }
}

map_pt* map::pt(int lat, int lon){
  if ((lat<lat1)||(lat>=lat2)||(lon<lon1)||(lon>=lon2)) return &p0;
  return &data[(lon-lon1)+(lat-lat1)*w];
}

short map::geth(int lat, int lon){
  if ((lat<lat1)||(lat>=lat2)||(lon<lon1)||(lon>=lon2)) return srtm_undef;
  return data[(lon-lon1)+(lat-lat1)*w].alt;
}


/*
��������� �������� ���� �� ���� (��� ����� �� ������) �� �����:
1. ������ ������ �����, ������� � ��������
   h0 - ������ �������� �����
2. � ������� ������ ���� ����� ������ �����
3. ���� ��� �� ���� h0 -- ��������� �� � ������ � -> �.2.
   ���� ������ ������ > max_list -- �������� �������� ����� ��� ����������
�� ����� �����, ����� ������, ��� ��������. 
   ������ h0 - ������ ���� �����.
��������� ���, ���� �� ������ �� ���� ����� ��� �� ��� ������������
����� (� ������� �������� �����������). ��� ����� �������� �����.

������ ���� �� ��� �  �������� �����: �� ������ ���� ����� �������
������ ����� ���� �����  ������ ����� �� ������ � ��������� � ���. �
������ �������� �����������.

��� ���������� ������� ��������� ���� ������ � ����
�������� � ������� ����������! (��. one_river.cpp)
*/

void map::rtrace(point p0, int rmax){

  int h0=geth(p0); // h0 - ������ �� ��������� ����
                   // (��� ��������� ������ ����� ���� ������ ����)

  if (h0 < srtm_min) return; // �� ��� �����
  if (pt(p0)->rdir != -1) return; // �� �� ��� ����������� ����������

  std::list<point> L;            // ������ ������������� �����
  L.push_back(p0);
  std::set<point> B = border(L); // ��� �������

  int n=0; // ������� ���������� ����������

  point p;     // ������� �����
  point pe=p0; // ����� ���������� ����

  int min;
  do {
    // ������ ������� �� ������� � ������� ��� � ������
    min = -srtm_min;
    for (std::set<point>::iterator b = B.begin(); b!=B.end(); b++){
      int h = geth(*b);
      if (min > h){ min=h; p=*b;}
    }
    add_pb(p, L,B);

    // ���� �� ����� ����� ������ �����, ��� ������� - ������ ���:
    if ((min> srtm_min)&&(min<h0)) {pe=p; h0=min; n=0;}

    n++;
    // ���� �� ��� ���� ������, � ��� �� �������, �� ����� ���������� 
    if (n>rmax) {pt(pe)->rdir = 8; p=pe; break;} // ���� - ����������

  } while ((min>srtm_min)&&(pt(p)->rdir==-1)); // ���� �� ����� ���� (�.�. ���� �����)

  //������ p - ����� �����
  //������ �������� ������:
  while (p!=p0){
    for (std::list<point>::iterator b = L.begin(); b!=L.end(); b++){
      int dir = b->isadjacent(p);
      if (dir != -1) { p=*b; pt(p)->rdir=dir; break;}
    }
  }
  return;
}


void map::mtrace(point p0, int mmax){

  int h0=geth(p0); // h0 - ������ �� ��������� ����
                   // (��� ��������� ������ ����� ���� ������ ����)

  if (h0 < srtm_min) return; // �� ��� �����
  if (pt(p0)->mdir != -1) return; // �� �� ��� ����������� ����������

  std::list<point> L;            // ������ ������������� �����
  L.push_back(p0);
  std::set<point> B = border(L); // ��� �������

  int n=0; // ������� ���������� ����������

  point p;     // ������� �����
  point pe=p0; // ����� ���������� ����

  int max;
  do {
    // ������ �������� �� ������� � ������� ��� � ������
    max = srtm_min;
    for (std::set<point>::iterator b = B.begin(); b!=B.end(); b++){
      int h = geth(*b);
      if (max < h){ max=h; p=*b;}
      if (h < srtm_min){ max=-srtm_min; p=*b;} // ��������� ��������� ������
    }
    add_pb(p, L,B);

    // ���� �� ����� ����� ������� �����, ��� ������� - ������ ���:
    if ((max< -srtm_min)&&(max>h0)) {pe=p; h0=max; n=0;}

    n++;
    // ���� �� ��� ���� ������, � ��� �� �������, �� ����� ���������� 
    if (n>mmax) {pt(pe)->mdir = 8; p=pe; break;} // ���� - ����������

  } while ((max < -srtm_min)&&(pt(p)->mdir==-1)); // ���� �� ����� ���� (�.�. ���� �����)

  //������ p - ����� �����
  //������ �������� ������:
  while (p!=p0){
    for (std::list<point>::iterator b = L.begin(); b!=L.end(); b++){
      int dir = b->isadjacent(p);
      if (dir != -1) { p=*b; pt(p)->mdir=dir; break;}
    }
  }
  return;
}


// ����������� ����������� ��� ���� ����� �����

void map::set_dirs(int rmax, int mmax){

  for (int lat=lat1; lat<lat2; lat+=1){
    std::cerr << lat-lat1  << "\n";
    for (int lon=lon1; lon<lon2; lon+=1){
      rtrace(point(lon,lat), rmax);
      mtrace(point(lon,lat), mmax);
    }
  }
}

// ����������� �������� ��� ���� ����� �����
void map::set_areas(void){
  for (int lat=lat1; lat<lat2; lat++){
    double area = pow(6380.0 * M_PI/srtm_width/180, 2)*
      cos((1.0*lat)/srtm_width * M_PI/180.0);
    std::cerr << lat-lat1  << "\n";

    for (int lon=lon1; lon<lon2; lon++){
      point p  = point(lon, lat);

      pt(p)->marea+=area;
      int dir = pt(p)->mdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = p.adjacent(dir);
        pt(p)->marea+=area;
        dir = pt(p)->mdir;
      }
      p  = point(lon, lat);
      pt(p)->rarea+=area;
      dir = pt(p)->rdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = p.adjacent(dir);
        pt(p)->rarea+=area;
        dir = pt(p)->rdir;
      }
    }
  }
}
