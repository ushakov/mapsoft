#include <cstdio>
//#include <unistd.h>
#include <cstring>

#include <boost/lexical_cast.hpp>

#include "../utils/pnm_shifter.h"
#include "../lib2d/point.h"
#include "../libgeo_io/geofig.h"
#include "../libgeo_io/io.h"

#include <iostream>
#include <vector>
#include <set>

#define LGND_DEPTHS "-D+1:34"
#define GRID_DEPTHS "-D+35:39"
#define TEXT_DEPTHS "-D+40"
#define BASE_DEPTHS "-D+41:999"

#define SCALE 3.75

struct fig2dev_reader{
  int fig2dev_fd;

  fig2dev_reader(const char *depth_range, const char *bg, const char *infile){
    int   out_pipe[2];
    pid_t pid;

    const char  *m = ("-m" + boost::lexical_cast<std::string>(SCALE)).c_str();
    char a1[strlen(m)+1];             strcpy(a1, m);
    char a2[strlen(depth_range)+1];   strcpy(a2, depth_range);
    char a3[strlen(bg)+1];            strcpy(a3, bg);
    char a4[strlen(infile)+1];        strcpy(a4, infile);


    const char  *name = "fig2dev";

    char *args[8] = {"fig2dev", "-Lppm", "-j", a1, a2, a3, a4, NULL};
    int p=3;
    if (strlen(m)!=0){args[p++] = a1;}
    if (strlen(depth_range)!=0){args[p++] = a2;}
    if (strlen(bg)!=0){args[p++] = a3;}
    if (strlen(infile)!=0){args[p++] = a4;}
    args[p]=NULL;

    if (pipe(out_pipe)!=0){ std::cerr << "can't open pipe\n"; exit(-1); }

    pid = fork();
    if      (pid==-1) { std::cerr << "can't do fork\n"; exit(-1); }
    else if (pid==0)  {
      dup2(out_pipe[1], STDOUT_FILENO);
      close(STDERR_FILENO);
      close(out_pipe[0]);
      execvp(name, args);
    }
    close(out_pipe[1]);
    fig2dev_fd = out_pipe[0];
  }
  ~fig2dev_reader(){close(fig2dev_fd);}
};

struct fig2dev_shifter: fig2dev_reader, public pnm_shifter{
  fig2dev_shifter(const char *depth_range, const char *bg, const char *infile, int _data_width): 
    fig2dev_reader(depth_range, bg, infile), pnm_shifter(fig2dev_fd, _data_width){}
};



struct cnt_data{
  // контур цвета с3 между цветами с1 и с2
  int c1,c2,c3;
  int dots; //расстояние между точками в пунктире
  std::set<Point<int> > pts;
  cnt_data(const int _c1, const int _c2, const int _c3, const int _dots = 0):
    c1(_c1), c2(_c2), c3(_c3), dots(_dots){}
};


void usage(){
    std::cerr << "usage: fig2pnm <options> <file.fig> > out.pnm\n"
              << "options:\n"
              << "  --help   help\n"
              << "  --no_thinrem - don't remove thin lines\n"
              << "  --no_cntrs   - don't draw countours\n"
              << "  --map <file> - make map file\n";
    exit(0);
}

// почему-то fig2dev, если его просить сделать 
// какой-то фоновый цвет, соблюдает его с точностью +-1 ?? Ошибки округления?
// поэтому просим 0xFFFFFD и смотрим, чтоб было в диапазоне +-1
bool is_color(const unsigned char r, const unsigned char g, const unsigned char b){
  return ((r < 0xFC) || (r > 0xFE) ||
          (g < 0xFC) || (g > 0xFE) ||
          (b < 0xFC) || (b > 0xFE));
}

bool is_color(const unsigned char *c){
  return ((*c < 0xFC) || (*c > 0xFE) ||
          (*(c+1) < 0xFC) || (*(c+1) > 0xFE) ||
          (*(c+2) < 0xFC) || (*(c+2) > 0xFE));
}

bool is_dark(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char thr){
  return (r/3 + g/3 + b/3 < thr);
}

bool is_dark(const unsigned char *c, const unsigned char thr){
  return ((*c)/3 + (*(c+1))/3 + (*(c+2))/3 < thr);
}

bool is_dark(int c, const unsigned char thr){
  return ((c>>16)&0xFF)/3 + ((c>>8)&0xFF)/3 + (c&0xFF)/3 < thr;
}

#define COL(x,y)  ((map.data[y][3*(x)] << 16) + (map.data[y][3*(x)+1] << 8) + map.data[y][3*(x)+2])
#define COL_SET(x,y,c)  map.data[y][3*(x)]=(c>>16)&0xFF; map.data[y][3*(x)+1] = (c>>8)&0xFF; map.data[y][3*(x)+2]=c&0xFF;
#define ADJ(x,y,i)  COL( x+ ((i==1)?1:0) - ((i==3)?1:0), y + ((i==2)?1:0) - ((i==0)?1:0))

#define DIST2(x,y) ((x)*(x) + (y)*(y))



main(int argc, char **argv){

  bool thinrem=true, cntrs=true;
  std::string infile  = "";
  std::string mapfile = "";

  // разбор командной строки
  for (int i=1; i<argc; i++){

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage();

    if (strcmp(argv[i], "--no_thinrem")==0) { thinrem=false; continue; }
    if (strcmp(argv[i], "--no_cntrs")==0)   { cntrs=false; continue; }
    if (strcmp(argv[i], "--map")==0){
      if (i==argc-1) usage();
      i+=1;
      mapfile=argv[i];
      continue;
    }

    infile = argv[i];
  }
  if (infile == "") usage();



  // построение привязки
  if (mapfile!=""){

    // читаем fig
    std::cerr << "  reading fig: " << infile <<"\n";
    fig::fig_world F;
    if (!fig::read(infile.c_str(), F)) {
      std::cerr << "Bad fig file " << infile << "\n"; return 1;
    }

    // привязка fig-файла
    std::cerr << "  get ref\n";
    g_map ref = fig::get_ref(F);
    std::cerr << "  calc range\n";
    Rect<int> r = fig::range(F);
    std::cerr << r << "\n";
    std::cerr << ref.size() << "\n";
    if ((ref.size()>2) && (!r.empty())){
      std::cerr << "  making MAP-file\n";
      ref += r.TLC();
      ref *= fig::fig2cm / 2.54 * 80 * SCALE;
      geo_data w;
      w.maps.push_back(ref);
      io::out(mapfile, w, Options());
    } else {
      std::cerr << "  can't find geo-reference in fig-file!\n";
    }
  }


  // правила для построения контуров
  //-1 -- all colors
  //-2 -- lite colors
  std::vector<cnt_data> cnts;
  cnts.push_back(cnt_data(0x00ffff, 0xAAFFAA, 0x5066FF, 0));
  cnts.push_back(cnt_data(0x00ffff, 0xFFFFFF, 0x5066FF, 0));
  cnts.push_back(cnt_data(0x00ffff, 0xc06000, 0x5066FF, 0));
  cnts.push_back(cnt_data(0x87CEFF, -2, 0x5066FF, 0));
//  cnts.push_back(cnt_data(0x00ffff, 30453904, 0x5066FF, 0));
  cnts.push_back(cnt_data(0xAAFFAA, 0xFFFFFF, 0x009000, 7));


  // у нас есть три области работы:
  // 0 : 2 -- удаление тонких линий
  // 3 : 3+2*r+1 -- постановка точек, средняя линия d0a=3+r
  // 4+2*r : 4+2r + 2*(r1+r2)+1 -- работа с надписями

  int r  = 1; // радиус точки контура

  int r1 = 5; // поля текста
  int r2 = 5; // окрестность поиска при закрашивании темных линий

  int d0a = 3+r; // средняя линия для расстановки контуров

  int dw = 3 + 2*r+1 + 2*(r1+r2)+1; // ширина загружаемых данных
  int d0 = 3 + 2*r+1 + r1+r2;       // средняя линия данных

  int thr = 170;  // порог темных линий (r/3+g/3+b/3)

  int a = 128;    // прозрачность сетки


  fig2dev_shifter lgnd(LGND_DEPTHS, "-g#FDFDFD", infile.c_str(), dw);
  fig2dev_shifter grid(GRID_DEPTHS, "-g#FDFDFD", infile.c_str(), dw);
  fig2dev_shifter text(TEXT_DEPTHS, "-g#FDFDFD", infile.c_str(), dw);
  fig2dev_shifter  map(BASE_DEPTHS, "",          infile.c_str(), dw);

  if ((lgnd.w!=grid.w)||(lgnd.w!=text.w)||(lgnd.w!=map.w)||
      (lgnd.h!=grid.h)||(lgnd.h!=text.h)||(lgnd.h!=map.h)){
    std::cerr << "different image sizes\n";
    exit(0);
  }


  std::cout << "P6\n" << map.w << " " << map.h << "\n255\n";

  for (int i = 0; i<map.h+dw; i++){

//std::cerr << i << " ";

    if ((map.data[1]!=NULL) && thinrem){
      for (int j = 1; j < map.w-1; j++){
        int c  = COL(j,1);
        int c1 = COL(j-1,1);
        int c2 = COL(j+1,1);
        if ((c!=c1)&&(c!=c2)) {COL_SET(j,1,c1);}

        if ((map.data[0]==NULL) || (map.data[2]==NULL)) continue;
        c1 = COL(j,0);
        c2 = COL(j,2);
        if ((c!=c1)&&(c!=c2)) {COL_SET(j,1,c1);}
      }
    }


    if ((map.data[d0a]!=NULL) && cntrs){
      for (int j = 0; j < map.w; j++){

        for (std::vector<cnt_data>::iterator cnt=cnts.begin(); cnt!=cnts.end(); cnt++){

          if ((i+d0a-1<0)||(i+d0a+1>=map.h)) continue;
          if ((j-1<0)||(j+1>=map.w)) continue;

          // если точка имеет цвет с1 и соседа с цветом с2
          if (COL(j, d0a)==cnt->c1){
            bool make_pt = false;
            for (int k=0;k<4;k++){
              int c2 = ADJ(j,d0a,k);
              if (c2==cnt->c1) continue;
              if (c2==cnt->c3) continue;
              if ((cnt->c2 ==-1) || 
                  ((cnt->c2==-2) && (!is_dark(c2,thr)))||
                  (cnt->c2 == c2)) {make_pt = true; break;}
            }


            std::set<Point<int> >::iterator pt = cnt->pts.begin();

            while (pt!=cnt->pts.end()){
              if ((cnt->dots > 0) && (DIST2(pt->x - j, pt->y - i - d0a) < cnt->dots*cnt->dots)) {make_pt = false; break;}
              if (pt->y < i+d0a - cnt->dots) cnt->pts.erase(pt);
              pt++;
            }

            if (make_pt){
              cnt->pts.insert(Point<int>(j,i+d0a));
              for (int y = d0a-r; y<=d0a+r; y++){
                if (map.data[y]==NULL) continue;
                for (int x = j-r; x<=j+r; x++){
                  if ((x<0)||(x>=map.w)) continue;
                  COL_SET(x,y, cnt->c3);
                }
              }
            }
          }
        }
      }
    }


    // наложение текста
    if (map.data[d0]!=NULL){
      for (int j = 0; j < map.w; j++){
        if (is_color(text.data[d0]+3*j)) {
          // область, в которой нам надо стереть черные линии...
          for (int y = d0-r1; y<=d0+r1; y++){
            if (map.data[y]==NULL) continue;
            for (int x = j-r1; x<=j+r1; x++){
              if ((x<0)||(x>=map.w)) continue;
              if (abs(d0-y)*abs(d0-y) + abs(j-x)*abs(j-x) > r1*r1) continue;

              grid.data[y][3*x]   = 0xFD;
              grid.data[y][3*x+1] = 0xFD;
              grid.data[y][3*x+2] = 0xFD;

              if (!is_dark(map.data[y]+3*x, thr)) continue;
              // надо закрасить темную точку
              // найдем цвет ближайшей к ней светлой точки
              int dd = 2*r2*r2+1;
              int yym, xxm;
              for (int yy = y-r2; yy<=y+r1; yy++){
                if (map.data[yy]==NULL) continue;
                for (int xx = x-r2; xx<=x+r2; xx++){
                  if ((xx<0)||(xx>=map.w)) continue;
                  if (is_dark(map.data[yy]+3*xx, thr)) continue;
                  if (abs(y-yy)*abs(y-yy) + abs(x-xx)*abs(x-xx) < dd){
                    dd = abs(y-yy)*abs(y-yy) + abs(x-xx)*abs(x-xx);
                    yym=yy; xxm=xx;
                  }
                }
              }
              map.data[y][3*x]   = (dd>2*r2*r2)? 0xFF:map.data[yym][3*xxm];
              map.data[y][3*x+1] = (dd>2*r2*r2)? 0xFF:map.data[yym][3*xxm+1];
              map.data[y][3*x+2] = (dd>2*r2*r2)? 0xFF:map.data[yym][3*xxm+2];
            }
          }
        }
      }
    }

    // в последней строке сольем слои и запишем линию.
    if (map.data[dw-1]!=NULL){
      for (int j = 0; j < map.w; j++){
        unsigned char rm = map.data[dw-1][3*j],  gm = map.data[dw-1][3*j+1],  bm = map.data[dw-1][3*j+2];
        unsigned char rt = text.data[dw-1][3*j], gt = text.data[dw-1][3*j+1], bt = text.data[dw-1][3*j+2];
        unsigned char rg = grid.data[dw-1][3*j], gg = grid.data[dw-1][3*j+1], bg = grid.data[dw-1][3*j+2];
        unsigned char rl = lgnd.data[dw-1][3*j], gl = lgnd.data[dw-1][3*j+1], bl = lgnd.data[dw-1][3*j+2];
        if (is_color(rt,gt,bt)) { rm=rt; gm=gt; bm=bt; }
        // наложение сетки
        if (is_color(rg,gg,bg)) {
          rm = (rg*a + rm*(255-a))/255;
          gm = (gg*a + gm*(255-a))/255;
          bm = (bg*a + bm*(255-a))/255;
        }
        // наложение легенды
        if (is_color(rl,gl,bl)) {rm=rl; gm=gl; bm=bl;}
        std::cout << rm << gm << bm;
      }
    }
    map.data_shift();
    text.data_shift();
    grid.data_shift();
    lgnd.data_shift();
  }


}
