#include <iostream>
#include <fstream>
#include "../libfig/fig.h"
#include "../utils/m_time.h"
#include <cmath>
#include "zn.h"
#include "zn_key.h"
#include "line_dist.h"

using namespace std;
using namespace fig;

/*
������������ ���������� ������� �������
��� ������� ���.������-�������� ��������� ����� 0x700 ��� 0x900,
���� �������������� �� ���, ��� ��� ��� �������� �������.
*/

main(int argc, char **argv){

  if (argc!=4){
    std::cerr << "usage: " << argv[0] << " <conf_file> <in.fig> <out.fig>\n";
    exit(0);
  }
  string conf_file = argv[1];
  string infile    = argv[2];
  string outfile   = argv[3];

  zn::zn_conv zconverter(conf_file);

  fig_world W;
  if (!read(infile.c_str(), W)){cerr << "Bad fig file " << infile << "\n"; exit(0);}
  fig_world NW;

  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){

    int type = zconverter.get_type(*i);

    // �����, �������
    if ((type == 0x200001) || (type == 0x20000E)){
      // ���� �������� �������
      Point<int> p=i->center();
      // ������� �����
      fig_object o = make_object("2 1 0 3 18 7 50 -1 -1 0.000 1 1 7 0 0 1");
      o.push_back(p);
      o.thickness = (type == 0x200001)? 5:3;
      o.comment.swap(i->comment);
      NW.push_back(o);
    }
    // ������ ������� - ��� ���������
    NW.push_back(*i);
  }
  ofstream out(outfile.c_str());
  write(out, NW);
}
