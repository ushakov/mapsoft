#include <map>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <loaders/image_png.h>
#include <srtm/tracers.h>

// построение прямоугольной хребтовки

using namespace std;

int
main(){
  dPoint p0(95.4, 53.9); // ЮЗ угол
  int w=800, h=600;      // размер карты, точек
  int nmax=1000;         // максимальный размер бессточных областей
  bool down = true;
  SRTM3 S;
  size_t srtmw = S.get_width()-1;

  cerr << "Tracing rivers...\n";
  dImage ra = trace_map(S, p0*srtmw, w, h, nmax, down);
  cerr << "Tracing mountains...\n";
  dImage ma = trace_map(S, p0*srtmw, w, h, nmax, !down);
  cerr << "Done.\n";

  // fill image
  iImage img(w, h);
  simple_rainbow R(500,3000);
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      int c = R.get(S.geth(iPoint(p0*srtmw)+iPoint(x,y), true));
      if (ra.get(x,y)>0.5) c=0xFF0000;
      if (ma.get(x,y)>0.5) c=0x0;
      img.set_na(x,h-y-1,c);
    }
  }
  image_png::save(img, "trace_map.png");
  return 0;
}

