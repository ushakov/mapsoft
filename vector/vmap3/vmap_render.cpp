#include "vmap_renderer.h"
#include "options/m_getopt.h"
#include "options/m_time.h"

using namespace std;

void usage(){
  const char * prog = "vmap_render";

  cerr
     << prog << " -- convert vector maps to raster.\n"
     << "  usage: " << prog << " [<options>] <in_file> <out_file>\n"
     << "\n"
     << "  options:\n"
     << "    -m, --map <map file>    -- write OziExplorer map file\n"
     << "    -d, --dpi <dpi>         -- set map dpi (default 300)\n"
     << "    -g  --grid <step>       -- draw step x step cm grid\n"
     << "    -l  --grid_labels       -- draw grid labels\n"
     << "    -N  --draw_name         -- draw map name\n"
     << "    -D  --draw_date         -- draw date stamp\n"
     << "    -T  --draw_text <text>  -- draw text\n"
  ;
  exit(1);
}


static struct option options[] = {
  {"map",           1, 0, 'm'},
  {"dpi",           1, 0, 'd'},
  {"grid",          1, 0, 'g'},
  {"grid_labels",   0, 0, 'l'},
  {"draw_name",     0, 0, 'N'},
  {"draw_date",     0, 0, 'D'},
  {"draw_text",     1, 0, 'T'},
  {0,0,0,0}
};

main(int argc, char* argv[]){

  if (argc==1) usage();

  Options O = parse_options(argc, argv, options);
  argc-=optind;
  argv+=optind;
  optind=0;

  if (argc<2) usage();
  const char * ifile = argv[0];
  const char * ofile = argv[1];

  int dpi=O.get<int>("dpi", 300);

  int tm=0, bm=0, lm=0, rm=0;
  if (O.get<int>("draw_name", 0) ||
      O.get<int>("draw_date", 0) ||
      (O.get<string>("draw_text") != "")) {
    tm=dpi/3;
    bm=lm=rm=dpi/6;
  }

  int grid_labels = O.get<int>("grid_labels", 0);
  if (grid_labels){
    bm+=dpi/6;
    tm+=dpi/6;
    rm+=dpi/6;
    lm+=dpi/6;
  }

  VMAPRenderer R(ifile, dpi, lm, tm, rm, bm);
  bool hr = (R.W.style == "hr");

  if (O.get<int>("draw_name", 0))
    R.render_text(R.W.name.c_str(), dPoint(dpi/5,dpi/15), 0, 0, 18, 14, 0, 2);

  if (O.get<int>("draw_date", 0)){
    Time t; t.set_current();
    R.render_text(t.date_str().c_str(), dPoint(dpi/30,dpi), -M_PI/2, 0, 18, 10, 2, 2);
  }

  if (O.get<string>("draw_text") != ""){
    R.render_text(O.get<string>("draw_text").c_str(), dPoint(dpi/5,-dpi/30), 0, 0, 18, 10, 0, 0);
  }

  //*******************************

  R.render_polygons(0x16, 0xAAFFAA); // лес
  R.render_polygons(0x52, 0xFFFFFF); // поле
  R.render_polygons(0x15, 0xAAFFAA); // остров леса

  list<iPoint> cnt = R.make_cnt(0xAAFFAA, 2);     // контуры леса

  R.render_polygons(0x4f, "vyr_n.png");
  R.render_polygons(0x50, "vyr_o.png");
  R.render_polygons(0x14, "redk.png");

  R.render_polygons(0x15, 0xAAFFAA); // остров леса поверх вырубок

  R.filter_cnt(cnt, 0xAAFFAA); // убираем контуры, оказавшеся поверх вырубок
  R.draw_cnt(cnt, 0x009000, 1); // рисуем контуры

  R.set_cap_round(); R.set_join_round(); R.set_dash(0, 2);
  R.render_line(0x23, 0x009000, 1, 0); // контуры, нарисованные вручную
  R.unset_dash();

  R.render_polygons(0x4d, 0xC3E6FF,0,0,20); // ледник

  //*******************************

  R.render_polygons(0x4,  0xB0B0B0, 0x000000, 0.7); // закрытые территории
  R.render_polygons(0xE,  0xFF8080, 0x000000, 0.7); // деревни
  R.render_polygons(0x1,  0xB05959, 0x000000, 0.7); // города
  R.render_polygons(0x4E, 0x557F55, 0x000000, 0.7); // дачи
  R.render_polygons(0x1A, 0x557F55, 0x000000, 0.7); // кладбища

  //*******************************

  int hor_col = 0xC06000;
  if (hr) hor_col = 0xD0B090;

  R.set_dash(8, 3);
  R.render_line(0x20, hor_col, 1, 20); // пунктирные горизонтали
  R.set_dash(2, 2);
  R.render_line(0x2B, 0xC06000, 1, 0); // сухая канава
  R.unset_dash();
  R.render_line(0x21, hor_col, 1, 20); // горизонтали
  R.render_line(0x22, hor_col, 1.6, 20); // жирные горизонтали

  R.render_line(0x25, 0xA04000, 2, 20); // овраг

  int hreb_col = 0x803000;
  if (hr) hreb_col = 0xC06000;
  R.render_line(0xC,  hreb_col, 2, 20); // хребет

  //*******************************

  R.set_cap_butt();
  R.render_line(0x32, 0x00B400, 3, 10); // плохой путь
  R.set_dash(1, 1);
  R.render_line(0x33, 0x00B400, 3, 10); // удовлетворительный путь
  R.render_line(0x34, 0xFFD800, 3, 10); // хороший путь
  R.unset_dash();
  R.render_line(0x35, 0xFFD800, 3, 10); // отличный путь

  //*******************************

  R.render_polygons(0x51, "bol_l.png"); // болота
  R.render_polygons(0x4C, "bol_h.png"); // болота труднопроходимые
  R.render_line(0x24, 0x5066FF, 1, 0); // старые болота

  //*******************************

  int water_col = 0x00FFFF;
  if (hr) water_col = 0x87CEFF;

  R.set_cap_round();
  R.set_dash(4, 3);
  R.render_line(0x26, 0x5066FF, 1, 10); // пересыхающая река
  R.unset_dash();
  R.render_line(0x15, 0x5066FF, 1, 10); // река-1
  R.render_line(0x18, 0x5066FF, 2, 10); // река-2
  R.render_line(0x1F, 0x5066FF, 3, 10); // река-3

  R.render_polygons(0x29, water_col, 0x5066FF, 1, 20); // водоемы
  R.render_polygons(0x3B, water_col, 0x5066FF, 1, 20); // большие водоемы
  R.render_polygons(0x53, 0xFFFFFF, 0x5066FF, 1, 20); // острова

  R.render_line(0x1F, water_col, 1, 10); // середина реки-3

  //*******************************

  R.set_cap_butt(); R.set_join_miter();
  R.render_line_el(0x1A, 0x888888, 2); // маленькая ЛЭП
  R.render_line_el(0x29, 0x888888, 3); // большая ЛЭП
  R.render_line_gaz(0x28, 0x888888, 3); // газопровод

  //*******************************

  R.render_line(0x5, 0, 3, 0); // дома (перенести выше?)

  R.set_join_round();
  R.set_cap_round();
  R.set_dash(0, 2.5);
  R.render_line(0x2C, hor_col, 3, 10); // вал
  R.unset_dash();

  //*******************************
  R.set_cap_butt();
  R.render_line(0x7, 0xFFFFFF, 3, 10); // непроезжий грейдер - белая подложка
  R.set_dash(5, 4); R.render_line(0x16, 0x0, 0.7, 0); // просека
  R.set_dash(8, 5); R.render_line(0x1C, 0x0, 1.4, 0); // просека широкая
  R.set_dash(6, 2); R.render_line(0xA,  0x0, 1, 10); // непроезжая грунтовка
  R.set_dash(2, 2); R.render_line(0x2A, 0x0, 1, 10); // тропа
  R.set_dash(6, 1); R.render_line(0x7,  0x0, 3, 10); // непроезжий грейдер - пунктир
  R.unset_dash();
  R.render_line(0x6,  0x0, 1, 10); // прозжая грунтовка
  R.render_line(0x4,  0x0, 3, 10); // проезжий грейдер
  R.render_line(0x2,  0x0, 4, 10); // асфальт
  R.render_line(0xB,  0x0, 5, 10); // большой асфальт
  R.render_line(0x1,  0x0, 7, 10); // автомагистраль
  R.render_line(0x4,  0xFFFFFF, 1, 10); // проезжий грейдер - белая середина
  R.render_line(0x7,  0xFFFFFF, 1, 10); // непроезжий грейдер - белая середина
  R.render_line(0x2,  0xFF8080, 2, 10); // асфальт - середина
  R.render_line(0xB,  0xFF8080, 3, 10); // большой асфальт - середина
  R.render_line(0x1,  0xFF8080, 5, 10); // автомагистраль - середина
  R.render_line(0x1,  0x0,      1, 10); // автомагистраль - черная середина
  R.render_line(0xD,  0x0, 3, 10); // маленькая Ж/Д
  R.render_line(0x27, 0x0, 4, 10); // Ж/Д
  R.set_cap_round();
  R.set_dash(4, 2, 0, 2);   R.render_line(0x1D, 0x900000, 1, 0); // граница

  R.set_dash(2, 2); R.render_line(0x1E, 0x900000, 1, 0); // нижний край обрыва
  R.unset_dash();   R.render_line_obr(0x03, 0x900000, 1); // верхний край обрыва
  R.render_line_zab(0x19, 0x900000, 1); // забор

  R.render_bridge(0x1B, 0, 1, 2); // туннель
  R.render_bridge(0x08, 1, 1, 2); // мост-1
  R.render_bridge(0x09, 3, 1, 2); // мост-2
  R.render_bridge(0x0E, 6, 1, 2); // мост-5

  int pt_col = 0;
  if (hr) pt_col = 0x803000;

// точечные объекты
  R.set_cap_round();
  R.render_points(0x1100, pt_col, 4); // отметка высоты
  R.render_points(0xD00,  pt_col, 3); // маленькая отметка высоты
  R.render_points(0x6414, 0x5066FF, 4); // родник

  R.render_im_in_points(0x6402, "dom.png"); // дом
  R.render_im_in_points(0x1000, "ur_vod.png"); // отметка уреза воды
  R.render_im_in_points(0x6508, "por.png"); // порог
  R.render_im_in_points(0x650E, "vdp.png"); // водопад
  if (hr) R.render_im_in_points(0x0F00, "trig_hr.png");
  else R.render_im_in_points(0x0F00, "trig.png");
  R.render_im_in_points(0x2C04, "pam.png");
  R.render_im_in_points(0x2C0B, "cerkov.png");
  R.render_im_in_points(0x2F08, "avt.png");
  R.render_im_in_points(0x5905, "zd.png");
  R.render_im_in_points(0x6406, "per.png");
  R.render_im_in_points(0x6620, "pernk.png");
  R.render_im_in_points(0x6621, "per1a.png");
  R.render_im_in_points(0x6622, "per1b.png");
  R.render_im_in_points(0x6623, "per2a.png");
  R.render_im_in_points(0x6624, "per2b.png");
  R.render_im_in_points(0x6625, "per3a.png");
  R.render_im_in_points(0x6626, "per3b.png");
  R.render_im_in_points(0x660B, "kan.png");
  R.render_im_in_points(0x650A, "ldp.png");
  R.render_im_in_points(0x6403, "kladb.png");
  R.render_im_in_points(0x6411, "bash.png");
  R.render_im_in_points(0x6415, "razv.png");
  R.render_im_in_points(0x640C, "shaht.png");
  R.render_im_in_points(0x6603, "yama.png");
  R.render_im_in_points(0x6606, "ohotn.png");
  R.render_im_in_points(0x6613, "pupyr.png");
  R.render_im_in_points(0x6616, "skala.png");
  R.render_im_in_polygons(0x1A, "cross.png"); // крестики на кладбищах

  //*******************************

  double grid_step = O.get<double>("grid", 0);
  if (grid_step>0) R.render_pulk_grid(grid_step, grid_step,grid_labels);

  R.render_labels();

  //*******************************
  string map = O.get<string>("map");
  R.save_image(ofile, (map=="") ? NULL:map.c_str());


  //*******************************
}
