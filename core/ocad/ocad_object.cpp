#include "ocad_object.h"

using namespace std;

namespace ocad{

ocad_object::ocad_object(): sym(0), type(3), status(1), viewtype(0),
  implayer(0), idx_col(-1), ang(0), col(0), width(0), flags(0),
  extent(0){}

// lib2d functions
iLine
ocad_object::line() const{
  iLine ret;
  for (int i=0; i<coords.size(); i++){
    iPoint p(coords[i].getx(),coords[i].gety());
    ret.push_back(p);
  }
  return ret;
}
iRect
ocad_object::range() const{
  iRect ret;
  for (int i=0; i<coords.size(); i++){
    iPoint p(coords[i].getx(),coords[i].gety());
    if (i==0) ret=iRect(p,p);
    else ret = rect_pump(ret, p);
  }
  return rect_pump(ret, (int)extent);
}

void
ocad_object::dump(int verb) const{
 if (verb<1) return;
  cout << "object: " << sym/1000 << "." << sym%1000
    << " type: " << (int)type
    << " col: " << idx_col << " - " << col;
  if (status!=1) cout << " status: " << (int)status;
  if (viewtype)  cout << " viewtype: " << (int)viewtype;
  if (implayer)  cout << " implayer: " << implayer;
  if (ang)       cout << " ang: " << ang/10.0;
  if (coords.size()) cout << " points: " << coords.size();
  if (text.length()) cout << " text: \""  << text << "\"";
  cout << "\n";
  if (verb<2) return;
  if (coords.size()){
    cout << "  coords: ";
    for (int i=0; i<coords.size(); i++) coords[i].dump(cout);
    cout << "\n";
  }
}



ocad_coord
ocad_object::LLC() const{
  if (coords.size() == 0) return ocad_coord();
  ocad_coord ret;
  ret.setx(coords[0].getx());
  ret.sety(coords[0].gety());
  for (int i=0; i<coords.size(); i++){
    if (coords[i].getx() < ret.getx()) ret.setx(coords[i].getx());
    if (coords[i].gety() < ret.gety()) ret.sety(coords[i].gety());
  }
  ret.setx(ret.getx()-extent);
  ret.sety(ret.gety()-extent);
  return ret;
}

ocad_coord
ocad_object::URC() const{
  if (coords.size() == 0) return ocad_coord();
  ocad_coord ret;
  ret.setx(coords[0].getx());
  ret.sety(coords[0].gety());
  for (int i=0; i<coords.size(); i++){
    if (coords[i].getx() > ret.getx()) ret.setx(coords[i].getx());
    if (coords[i].gety() > ret.gety()) ret.sety(coords[i].gety());
  }
  ret.setx(ret.getx()+extent);
  ret.sety(ret.gety()+extent);
  return ret;
}

int
ocad_object::txt_blocks(const string & txt) const{
  if (txt.length() == 0 ) return 0;
  return (txt.length()+2)/8+1;
}

void
ocad_object::write_text(const string & txt, FILE *F, int limit) const {
  int n=txt_blocks(txt);
  if (limit>=0) n=MIN(limit, n); // TODO: this can break unicode letters!
  if (n){
    char *buf = new char [n*8];
    memset(buf, 0, n*8);
    for (int i=0; i<txt.length(); i++) buf[i] = txt[i];
    if (fwrite(buf, 1, n*8, F)!=n*8)
      throw "can't write object text";
    delete [] buf;
  }
}

void
ocad_object::write_coords(FILE *F, int limit) const{
  int n = coords.size();
  if (limit>=0) n=MIN(limit, n);
  if (n){
    ocad_coord * buf = new ocad_coord[n];
    memset(buf, 0, n*8);
    for (int i=0; i<n; i++)  buf[i] = coords[i];
    if (fwrite(buf, sizeof(ocad_coord), n, F)!=n)
      throw "can't write object coordinates";
    delete [] buf;
  }
}

void
ocad_object::read_coords(FILE *F, int n){
  if (n){
    ocad_coord * buf = new ocad_coord[n];
    if (fread(buf, sizeof(ocad_coord), n, F)!=n)
      throw "can't read object coordinates\n";
    coords = vector<ocad_coord>(buf, buf+n);
    delete [] buf;
  }
}

void
ocad_object::read_text(FILE *F, int n){
  if (n){
    char *buf = new char [n*8];
    if (fread(buf, 1, n*8, F)!=n*8)
      throw "can't read object text\n";
    for (int i=0; i<n*8-1; i++){
      if ((buf[i]==0) && (buf[i+1]==0)) break; // 0x0000-terminated string
      text.push_back(buf[i]);
    }
    delete [] buf;
  }
}


} // namespace
