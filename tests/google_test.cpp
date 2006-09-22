#include <iostream>
#include "../utils/image.h"
#include "../utils/google.h"

main(){
  // �� ����� ����������, ��� � ��� ���� � �������� 14 
  // (� ����� �������� ��� ����� 1-� ������)
  int zoom = 14;
  // � ����� ��������� (lon1,lat1,lon2,lat2)
  Rect<double> dst_rect(37, 55.5, 38, 56);
  // ������ ��������, ������ ��������� �� � 4 ����, ����� ��� ����� ������ �������
  Image<int> i = google::load_image("/home/sla/GOOGLE/", zoom, google::latlon2xy(zoom, dst_rect), 4);
  // ��������� � jpeg
  jpeg_image::save("im_5.jpg", i, 75);
}


