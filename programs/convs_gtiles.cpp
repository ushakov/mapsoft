#include "tiles/tiles.h"
#include "opts/opts.h"
#include "geo_io/geo_refs.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <boost/geometry.hpp>

using namespace std;

void usage(){
  cerr << "\n"
       << "Google tile calculator.\n"
       << "usage: convs_tile -p <point> <z> -- tile which covers a WGS84 point\n"
       << "       convs_tile -r <range> <z> -- tiles which cover a range (return a rectangle)\n"
       << "       convs_tile -R <range> <z> -- tiles which cover a range (return a list)\n"
       << "       convs_tile -bi <track> <z> -- tiles which touches the area (return a list)\n"
       << "       convs_tile -bc <track> <z> -- tiles which fully covered by area (return a list)\n"
       << "       convs_tile -n x y z -- tile range\n"
       << "       convs_tile -c x y z -- tile center\n"
       << "       convs_tile -t x y z <range>  -- check if a tile touches a range\n"
       << "\n"
  ;
}

int main(int argc, char** argv){
  try{

    Tiles calc;

    if ((argc == 4) && (strcmp(argv[1], "-p") == 0)){
      iPoint ret = calc.pt_to_gtile(
          str_to_type<dPoint>(argv[2]),
          str_to_type<int>(argv[3]));
      cout << ret << "\n";
      return 0;
    }

    if ((argc == 4) && (strcmp(argv[1], "-r") == 0)){
      iRect ret = calc.range_to_gtiles(
          str_to_type<dRect>(argv[2]),
          str_to_type<int>(argv[3]));
      cout << ret << "\n";
      return 0;
    }

    if ((argc == 4) && (strcmp(argv[1], "-R") == 0)){
      int z = str_to_type<int>(argv[3]);
      iRect ret = calc.range_to_gtiles(
          str_to_type<dRect>(argv[2]),z);
      for (int y = ret.y; y<ret.y+ret.h; y++)
        for (int x = ret.x; x<ret.x+ret.w; x++)
          cout << x << "," << y << "," << z << "\n";
      return 0;
    }

    if ((argc == 4) && ((strcmp(argv[1], "-bi") == 0)
        || (strcmp(argv[1], "-bc") == 0))){
      using namespace boost::geometry;
      int zfin = str_to_type<int>(argv[3]);
      geo_data B;
      vector<g_track>::const_iterator bi;
      vector<g_trackpoint>::const_iterator pi;
      typedef model::d2::point_xy<double> point_t;
      std::vector<point_t> pts;
      typedef model::polygon<point_t> polygon_t;
      polygon_t poly;
      typedef struct {int x; int y; int z;} tile_s;
      vector<tile_s> oldtiles, newtiles = {{0, 0, 0}};
      vector<tile_s>::const_iterator ti;

      io::in(string(argv[2]), B);

      // Прочитаем трек в полигон
      pts.clear();
      bi=B.trks.begin();
      for (pi=bi->begin(); pi!=bi->end(); pi++){
        pts.push_back(point_t(pi->x, pi->y));
      }
      pts.push_back(point_t(bi->begin()->x, bi->begin()->y));
      append(poly, pts);

      /* Переберём все возможные тайлы
         методом последовательных приближений */
      for (int z = 1; z <= zfin; z++) {
        oldtiles=newtiles;
        newtiles.clear();
        // Переберём все тайлы с прошлого приближения
        for (ti=oldtiles.begin(); ti!=oldtiles.end(); ti++) {
          // Поделим подходящий тайл на 4 новых и проверим каждый из них
          for (int i = 0; i < 4; i++) {
            int flag;
            int x = ti->x * 2 +   (i & 1);
            int y = ti->y * 2 + !!(i & 2);
            polygon_t tile;
            dRect tr = calc.gtile_to_range(x, y, z);
            pts.clear();
            pts.push_back(point_t(tr.x,        tr.y       ));
            pts.push_back(point_t(tr.x + tr.w, tr.y       ));
            pts.push_back(point_t(tr.x + tr.w, tr.y + tr.h));
            pts.push_back(point_t(tr.x       , tr.y + tr.h));
            pts.push_back(point_t(tr.x,        tr.y       ));
            append(tile, pts);

            if ((z == zfin) && strcmp(argv[1], "-bi") != 0) {
              /* Полное покрытие */
              flag = covered_by(tile, poly);
            } else {
              /* Простое пересечение */
              flag = intersects(tile, poly);
            }
            if (flag) {
              tile_s tt = {x, y, z};
              newtiles.push_back(tt);
            }
          }
        }
      }
      for (ti=newtiles.begin(); ti!=newtiles.end(); ti++) {
        cout << ti->x << " " << ti->y << " " << ti->z << endl;
      }
      return 0;
    }

    if ((argc == 5) && (strcmp(argv[1], "-n") == 0)){
      dRect ret = calc.gtile_to_range(
        str_to_type<int>(argv[2]),
        str_to_type<int>(argv[3]),
        str_to_type<int>(argv[4]));
      cout << setprecision(9) << ret << "\n";
      return 0;
    }

    if ((argc == 5) && (strcmp(argv[1], "-c") == 0)){
      dRect ret = calc.gtile_to_range(
        str_to_type<int>(argv[2]),
        str_to_type<int>(argv[3]),
        str_to_type<int>(argv[4]));
      cout << setprecision(9) << ret.CNT() << "\n";
      return 0;
    }

    if ((argc == 6) && (strcmp(argv[1], "-t") == 0)){
      dRect r1 = calc.gtile_to_range(
        str_to_type<int>(argv[2]),
        str_to_type<int>(argv[3]),
        str_to_type<int>(argv[4]));
      dRect r2 = str_to_type<dRect>(argv[5]);
      return rect_intersect(r1,r2).empty();
    }

    usage();
  }
  catch(Err e) {
    cerr << "Error: " << e.get_error() << endl;
  }
  return 1;
}
