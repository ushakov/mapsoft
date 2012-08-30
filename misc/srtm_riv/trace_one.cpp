#include <vector>
#include <set>
#include <2d/point_int.h>
#include <2d/rainbow.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>

// алгоритм нахождения русла реки
dPoint p0(95.786934, 54.057950);
bool down=true;
int nmax=1000; // максимальный размер бессточных областей
int hmin=600;  // минимальная высота - критерий остановки

using namespace std;
set<iPoint> trace(srtm3 & S, const iPoint & p0, bool down=true){
  // проход вниз
  int h0=S.geth(p0); // h0 - высота на последнем шаге
                   // (шаг происхоит строго вверх либо строго вниз)

  if (h0 < srtm_min) return set<iPoint>(); // мы вне карты

  vector<iPoint> L; // упорядоченный список просмотренных точек
  set<iPoint> P; // то же самое, но в set, чтоб быстро строить границу
  L.push_back(p0);
  P.insert(p0);
  set<iPoint> B = border(P); // граница

  int n=0;      // счетчик бессточных территорий
  iPoint p;     // текущая точка
  iPoint pe=p0; // точка последнего шага

  do {
    // найдем минимум на границе и добавим его в список
    int hm = down?-srtm_min:srtm_min; // min for rivers, max for mountains
    for (set<iPoint>::iterator b = B.begin(); b!=B.end(); b++){
      int h = S.geth(*b);
      if (h < srtm_min) continue;
      if ((!down && hm < h) || ( down && hm > h)) { hm=h; p=*b;}
    }
    add_pb(p, P,B);
    L.push_back(p);
    n++;

    // если мы нашли более низкую точку, чем прошлая - делаем шаг,
    // обнуляем счетчик бессточных территорий
    if (!down && (hm > h0) || (down && hm < h0)) { h0=hm; n=0; pe=p; }

    // если мы уже ушли далеко, а шаг не сделали, то точка
    // последнего шага - бессточная
    if (n>nmax) { p=pe; break; }

    // критерий выхода: до высоты hmin, или край данных
  } while (h0>hmin);

  set<iPoint> ret;
  // обратный проход от p до p0
  while (p!=p0){
    for (vector<iPoint>::const_iterator b = L.begin(); b!=L.end(); b++){
      if (isadjacent(*b, p)<0) continue;
      p=*b;
      ret.insert(p);
      break;
    }
  }
  return ret;
}

main(){
  srtm3 S;
  set<iPoint> R = trace(S, p0*1200, down);
  if (!R.size()) exit(1);

  // find data range
  iRect r(*R.begin(),*R.begin());
  for (set<iPoint>::const_iterator b = R.begin(); b!=R.end(); b++){
    r = rect_pump(r,*b);
  }
  r = rect_pump(r,5);

  //
  iImage img(r.w, r.h);
  simple_rainbow rb(500,3000);

  for (int y=0; y<r.h; y++){
    for (int x=0; x<r.w; x++){
      iPoint p(x+r.x, y+r.y);
      short h  = S.geth(p, true);
      int c = rb.get(h);
      if (R.count(p)) c = 0x0;
      img.set(x,r.h-y-1,c);
    }
  }
  image_png::save(img, "trace_one.png");
}
