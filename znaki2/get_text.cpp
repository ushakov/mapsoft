// ��������� fig-�������� �� ����� �������

#include <string>
#include <fstream>
#include "zn_lists.h"

using namespace std;

// ������������ ��������� �������� � ���������� ������

void usage(){
    cerr << "usage: get_text <conf_file> > out.txt\n";
    exit(0);
}


main(int argc, char** argv){

  if (argc != 2) usage();
  string conf_file = argv[1];

  zn::zn_conv zconverter(conf_file);
  cout << zconverter.make_text();
}