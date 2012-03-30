#include "geo_nom.h"
#include <sstream>
#include <iostream>
#include <iomanip>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>


namespace convs{

using namespace std;

dRect
nom_to_range(const string & key, int & rscale){
    using namespace boost::spirit::classic;

    string a  = " ";
    int  b  = 0;
    string c5 = "";
    string c2 = "";
    string c1 = "";
    string d = "";
    int m=1;
    string key1 = key+" ";

    // поймем номер листа

    rule<> ndigit_p = anychar_p-digit_p;
    rule<> dash_p = ch_p('-')||'_';

    rule<> map_a_o = 
      (range_p('a','o') || range_p('A','O'))[assign_a(a)] >> !dash_p >>
      uint_p[assign_a(b)] >>
      !( dash_p >> (
         ((digit_p >> digit_p >> digit_p >> dash_p)[assign_a(c1)] >> digit_p[assign_a(d)]) ||
         (digit_p >> digit_p >> digit_p)[assign_a(c1)] ||
         (digit_p >> digit_p)[assign_a(c2)] ||
         (digit_p)[assign_a(c5)] 
       ));

    rule<> map_p_s = 
      (range_p('p','s') || range_p('P','S'))[assign_a(a)][assign_a(m,2)] >> !dash_p >>
      uint_p[assign_a(b)] >> 
      !( dash_p >> (
         ((digit_p >> digit_p >> digit_p >> dash_p)[assign_a(c1)] >> (digit_p)[assign_a(d)] >> dash_p >> digit_p) ||
         ((digit_p >> digit_p >> digit_p >> dash_p)[assign_a(c1)][assign_a(d,0)] >> digit_p >> digit_p >> digit_p) ||
         ((digit_p >> digit_p >> dash_p)[assign_a(c2)] >> digit_p >> digit_p) ||
         ((digit_p >> dash_p)[assign_a(c5)] >> digit_p) 
      ));


    if (!parse(key1.c_str(), (map_p_s || map_a_o) >> *anychar_p).full)
      return dRect(0,0,0,0);

    char ac='a';
    if (a.size()>0) ac=a[0];

    if      ((ac>='A')&&(ac <= 'T')) ac-='A';
    else if ((ac>='a')&&(ac <= 't')) ac-='a';
    else return dRect(0,0,0,0);

    double lat1,lat2,lon1,lon2;

    lat1 = ac*4; lat2=lat1+4;

    if ((b<1)||(b>=60)) return dRect(0,0,0,0);

    lon1 = b*6 - 186; lon2=lon1+6;

    int col,row;

    int c1i=0, c2i=0, c5i=0, di=0;
    parse(c1.c_str(), uint_p[assign_a(c1i)] >> !dash_p);
    parse(c2.c_str(), uint_p[assign_a(c2i)] >> !dash_p);
    parse(c5.c_str(), uint_p[assign_a(c5i)] >> !dash_p);
    parse(d.c_str(),  uint_p[assign_a(di)] >> !dash_p);

    rscale=1000000;
    if ((di != 0)&&(c1i != 0)){  // 1:50 000
      rscale=50000;
      col = ((c1i-1)%12)*2 + (di-1)%2;
      row = 23 - ((c1i-1)/12)*2 - (di-1)/2;
      lon1 += col*6.0/24; lon2=lon1+6.0/24;
      lat1 += row*4.0/24; lat2=lat1+4.0/24;
//      cerr << "1:50 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c1i != 0){  // 1:100 000
      rscale=100000;
      col = (c1i-1)%12;
      row = 11 - (c1i-1)/12;
      lon1 += col*6.0/12; lon2=lon1+6.0/12;
      lat1 += row*4.0/12; lat2=lat1+4.0/12;
//      cerr << "1:100 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c2i != 0){  // 1:200 000
      rscale=200000;
      col = (c2i-1)%6;
      row = 5 - (c2i-1)/6;
      lon1 += col*6.0/6; lon2=lon1+6.0/6;
      lat1 += row*4.0/6; lat2=lat1+4.0/6;
//      cerr << "1:200 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c5i != 0){  // 1:500 000
      rscale=500000;
      col = (c5i-1)%2;
      row = 1 - (c5i-1)/2;
      lon1 += col*6.0/2; lon2=lon1+6.0/2;
      lat1 += row*4.0/2; lat2=lat1+4.0/2;
//      cerr << "1:500 000, col: " << col << ", row: "<< row << '\n';
    }
    if (m==2) lon2+=lon2-lon1;

    return dRect(dPoint(lon1,lat1), Point<double>(lon2,lat2));
}

dRect
nom_to_range(const string & key){
  int rscale;
  return nom_to_range(key, rscale);
}

// по координатам в СК pulkovo-42 возвращает название листа
string
pt_to_nom(const dPoint & p, int sc){
    if ((p.x <-180) || (p.x>180) || (p.y<0) || (p.y>90))
      return string(); // bad coordinates

    char A = 'a' + (int)floor(p.y/4);
    int  B = 31 +  (int)floor(p.x/6);

    bool dbl = A>'o';

    int n, w;

    switch (sc){
      case 1000000: n=1;  w=0; break;
      case  500000: n=2;  w=1; break;
      case  200000: n=6;  w=2; break;
      case  100000: n=12; w=3; break;
      case   50000: n=12; w=3; break;
      default: return string(); // unknown scale
    }

    int row=n-1-(int)floor((p.y/4.0-floor(p.y/4))*n);
    int col=(int)floor((p.x/6.0-floor(p.x/6))*n);

    int C = row*n + col+1;

    int D=0;
    if (sc>=50000){
      int row1=1-(int)floor((p.y/4.0*n-floor(p.y/4*n))*2);
      int col1=(int)floor((p.x/6.0*n-floor(p.x/6*n))*2);
      D=row1*2+col1+1;
    }

    ostringstream out;

    if (dbl){
      if (sc==1000000){
        B=((B-1)/2)*2+1;
        out << A << B << '-' << B+1;
      }
      else if (sc>50000){
        C=((C-1)/2)*2+1;
        out << A << B << '-'
            << setw(w) << setfill('0') << C << '-'
            << setw(w) << setfill('0') << C+1;
      }
      else {
        D=((D-1)/2)*2+1;
        out << A << B << '-' 
            << setw(w) << setfill('0')<< C 
            << '-' << D << '-' << D+1;
      }
    }
    else {
      out << A << B;
      if (w!=0) out << '-' << setw(w) << setfill('0') << C;
      if (sc<=50000) out << '-' << D;
    }

    return out.str();
}

string
nom_shift(const std::string & name, const iPoint & shift){
  int scale;
  dRect r=nom_to_range(name, scale);
  return pt_to_nom(r.CNT() + dPoint(shift.x * r.w, shift.y * r.h), scale);
}

vector<string>
range_to_nomlist(const dRect & range, int rscale){
  vector<string> ret;

  dRect  r;
  dPoint far=range.TLC();
  dPoint cnt=range.TLC();

  while (far.y < range.BRC().y){
    far.x=cnt.x=range.x;
    bool first_column=true;
    while (far.x < range.BRC().x){
      string name=pt_to_nom(cnt, rscale);
      if (name == "") return ret;
      ret.push_back(name);
      if (first_column){ // map widths can be different in different rows
        r = nom_to_range(name);
        far=r.BRC();
        cnt=r.CNT();
        first_column=false;
      }
      else{
        far.x+=r.w;
      }
      cnt.x+=r.w;
    }
    cnt.y+=r.h;
  }
  return ret;
}


}//namespace
