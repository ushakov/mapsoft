#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <cmath>

#include "../srtm3.h"
#include "../line.h"
#include "../point_int.h"
#include "../../libmp/mp.h"

// �������� ������ srtm3.h, line.h, point_int.h 
// �������������� ���������� ����� ������ (�����������, ������� � �.�.)...

using namespace std;

void usage(){
    cerr << "usage: get_hor lon1 lon2 lat1 lat2 step1 step2 srtm_dir > out.mp\n";
    exit(0);
}

//���������� ���� �� ��� ������
Point<int> crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}
//����������� ��������� �� ����� ������� (��������� ������)
Point<int> dir (int k){
  return crn(k+1)-crn(k);
}


main(int argc, char** argv){
  if (argc != 8) usage();
  int lon1  = int(1200*atof(argv[1]));
  int lon2  = int(1200*atof(argv[2]));
  int lat1  = int(1200*atof(argv[3]));
  int lat2  = int(1200*atof(argv[4]));
  int step1 = atoi(argv[5]);
  int step2 = atoi(argv[6]);
  string srtm_dir = argv[7];

  if (lat2<lat1)   swap(lat2,lat1);
  if (lon2<lon1)   swap(lon2,lon1);
  if (step2<step1) swap(step2,step1);

  srtm3 s(srtm_dir, 10, interp_mode_off);
  mp::mp_world MP;

  // �������� �����������!
  cerr << "������� ������� ������������: ";
  map<short, MultyLine<double> > hors;
  int count = 0; 
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){

      Point<int> p(lon,lat);
      // ����������� ������� ������ ������ � �������������:
      // ��� ��������� �� ������� ��� ������ �� ��������,
      // ���� �� ��������� ���� ������� � ���������� ������������ � ���� �����
      multimap<short, double> pts;

      for (int k=0; k<4; k++){
        Point<int> p1 = p+crn(k);
        Point<int> p2 = p+crn(k+1);
        short h1 = s.geth(p1);
        short h2 = s.geth(p2);
        if ((h1<srtm_min) || (h2<srtm_min)) continue;
        int min = (h1<h2)? h1:h2;
        int max = (h1<h2)? h2:h1;
        min = int( floor(double(min)/step1)) * step1;
        max = int( ceil(double(max)/step1))  * step1;
        if (h2==h1) continue;
        for (int hh = min; hh<=max; hh+=step1){
          double x = double(hh-h1+0.1)/double(h2-h1);
          if ((x<0)||(x>1)) continue;
          pts.insert(pair<short, double>(hh,x+k));
        }
      } 

      // ������, ����� ����������� ���������� ������� ������,
      // �������� �� � ������ ������������ hors
      short h=srtm_undef;
      double x1,x2;

      for (multimap<short,double>::const_iterator i=pts.begin(); i!=pts.end(); i++){
        if (h!=i->first){
          h  = i->first;
          x1 = i->second;
        } else{
          x2 = i->second;
          Line<double> hor;
          hor.push_back((Point<double>(p) + Point<double>(crn(int(x1))) + Point<double>(dir(int(x1)))*double(x1-int(x1)))/1200.0);
          hor.push_back((Point<double>(p) + Point<double>(crn(int(x2))) + Point<double>(dir(int(x2)))*double(x2-int(x2)))/1200.0);
          hors[h].push_back(hor);
          h=srtm_undef;
          count+=hor.size();
        }
      }
    }
  }
  cerr << count << " ��\n";

  count = 0; 
  cerr << "  ������� ������� ������������ � �����: ";
  for(map<short, MultiLine<double> >::iterator im = hors.begin(); im!=hors.end(); im++){
    std::cerr << im->first << " ";
    merge(im->second, 1e-4);
    generalize(im->second, 1e-4); // �������� ������������� 1e-4 - �������� 1/8 �� 3 ������
    split(im->second, 100);
    
    for(MultiLine<double>::iterator iv = im->second.begin(); iv!=im->second.end(); iv++){
      if (iv->size()<3) continue;
      mp::mp_object O;
      O.Class = "POLYLINE";
      ostringstream s; s << im->first;
      O.Label = s.str();
      O.Type = 0x21;
      if (im->first%step2==0) O.Type = 0x22;
      O.insert(O.end(), iv->begin(), iv->end());
      MP.push_back(O);
      count++;
    }
  }
  cerr << count << " ��\n";
  

  // ����� ������: 
  // 1. ������ ��� ��������� ��������� (�� ������� ��� ��������� �� ������ �����!)
  // 2. �� ������� ����� ������� ��������� �����, �������� ��������� ����� �������
  // 3. ���� ������ ��������� ���������� ����� ���� �������� ����� ��� �� DH �,
  //    ��� ���� ������ ��������� ������ PS ����� - ��������� ����������, 
  //    ��������� �������� ����� ��������.
  // 4. ���� ������ ��������� ����������� ����� ������ �������� - ���������
  //    ����������
  
  int DH = 20;
  int PS = 500;
  count = 0;
  cerr << "���� �������: ";
  
  set<Point<int> > done;
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){

      Point<int> p(lon,lat);
      if (done.find(p)!=done.end()) continue;
      short h = s.geth(p);
      if (h<srtm_min) continue;

      set<Point<int> > pts; pts.insert(p);
      set<Point<int> > brd = border(pts);
      // ���� �������� �������

      do{
        short max = srtm_undef;
        Point<int> maxpt;
        for (set<Point<int> >::const_iterator i = brd.begin(); i!=brd.end(); i++){
          short h1 = s.geth(*i);
          // �������� ����� ������� ������ � ���� ������
          if ((h1<srtm_min) && (pdist(*i,p)<1.5)) {max = h1; break;}
          if (h1>max) {max = h1; maxpt=*i;}
        }
        if (max < srtm_min) break;

        // ���� �������� ���� �������� ����� - �������.
        if (max > h) { break; }

        // ���� �� ���������� �� �������� ����� ����� ��� �� DH ��� ������ ������� ����� PS
        if ((h - max > DH ) || (pts.size() > PS)) {
          mp::mp_object O;
          O.Class = "POI";
          ostringstream s; s << h;
          O.Label = s.str();
          O.Type = 0x1100;
          O.push_back(Point<double>(p)/1200.0);
          MP.push_back(O);
          count++;
          break;
        }

        add_pb(maxpt, pts, brd);
        done.insert(maxpt);

      } while (true);
    }
  }
  cerr << count << " ��\n";

  std::set<Point<int> > aset;
  MultiLine<double> aline;


/*
  // ����� ������ �������
  cerr << "���� ������ ������: ";
  double latdeg = 6380000/1200.0/180.0*M_PI; 
  double londeg = latdeg * cos(double(lat2+lat1)/2400.0/180.0*M_PI);

  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      Point<int> p(lon,lat);
      short h = s.geth(p);
      short hx = s.geth(p+Point<int>(1,0));
      short hy = s.geth(p+Point<int>(0,1));
      if ((h<srtm_min) || (hx<srtm_min) || (hy<srtm_min)) continue;
      Point<double> gr(double(hx-h)/londeg, double(hy-h)/latdeg);
      double a = atan(pdist(gr))*180/M_PI;
      if (a > 45) aset.insert(p);
    }
  }
  cerr << aset.size() << " �����\n";

  cerr << " ����������� ��������� ����� � ��������������: ";
  aline = pset2line(aset);
  for(MultiLine<double>::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    Line<double> l = (*iv)/1200.0;
    mp::mp_object O;
    O.Class = "POLYGON";
    O.Label = "high slope";
    O.Type = 0x15;
    O.insert(O.end(), l.begin(), l.end());
    MP.push_back(O);
  }
  cerr << aline.size() << " ��\n";
*/
  // ����� �����
  cerr << "���� ����� srtm: ";
  aset.clear();
  aline.clear();
  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){
      Point<int> p(lon,lat);
      short h = s.geth(p);
      if (h==srtm_undef) aset.insert(p);
    }
  }
  cerr << aset.size() << " �����\n";

  cerr << " ����������� ��������� ����� � ��������������: ";
  aline = pset2line(aset);
  for(MultiLine<double>::iterator iv = aline.begin(); iv!=aline.end(); iv++){
    if (iv->size()<3) continue;
    Line<double> l = (*iv)/1200.0;
    mp::mp_object O;
    O.Class = "POLYGON";
    O.Label = "no data";
    O.Type = 0x100;
    O.insert(O.end(), l.begin(), l.end());
    MP.push_back(O);
  }
  cerr << aline.size() << " ��\n";

  cerr << "���������� ��� � ����: ";
  mp::write(cout, MP);
  cerr << MP.size() << " ��������\n";


}
