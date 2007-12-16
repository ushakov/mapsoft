#include <cstdio>
//#include <unistd.h>
//#include <sys/types.h>

#include <iostream>
#include <vector>


struct ppm_reader{
  int   out_pipe[2];
  pid_t pid;

  std::vector <unsigned char *> data;
  int pos;
  int data_width;

  int w,h;

  ppm_reader(const char *depth_range, const char *infile, int _data_width){

    char a1[strlen(depth_range)+1];   strcpy(a1, depth_range);
    char a2[strlen(infile)+1];        strcpy(a2, infile);

    char  *name = "fig2dev";
    char  *args[] = {"fig2dev", "-Lppm", "-j", "-F", "-m3.75", "-g#FDFDFD", a1, a2, NULL};

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

    std::string s = gets();
    if (s!="P6") {w=0; h=0; return;}
    do {s=gets(); } while ((s.size()>0) && (s[0]=='#'));
    std::string::size_type i = s.find(' ');
    w = atoi(s.substr(0,   i).c_str());
    h = atoi(s.substr(i+1, s.size()-(i+1)).c_str());

    s=gets();
    if (s!="255") {w=0; h=0; return;}
    pos=-1;
    data_width = _data_width;
    data.resize(data_width, NULL);
    return;    
  }

  ~ppm_reader(){close(out_pipe[0]);}

  std::string gets() const {
    std::string ret;
    char c;
    while ((read(out_pipe[0], &c, 1)==1) && (c!='\n')) ret.push_back(c);
    return ret;
  }

  void data_shift(){
    if (data[data_width-1]!=NULL) delete(data[data_width-1]);
    for (int i = data_width-1; i>0; i--) data[i] = data[i-1];
    data[0]=NULL;
    pos++;
    if (pos<h){
      data[0] = new unsigned char[3*w];
      int n=3*w, m=3*w;
      while ((m>0) && (n>0)){
        m=read(out_pipe[0], data[0]+(3*w-n), n);
        n-=m;
      }
      if (m==0){
        std::cerr << "read error!\n";
        exit(0);
      }
    } 
  }


  int fd(){return out_pipe[0];}
};

void usage(){
    std::cerr << "usage: fig2pnm <file.fig> > out.pnm\n";
    exit(0);
}

bool pipe_read(int fd, unsigned char *buf, int N){
  int n=N,m=N;
  while ((m>0) && (n>0)){
    m=read(fd, buf+(N-n), n);
    n-=m;
  }
  return !(m==0);
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

main(int argc, char **argv){

  if (argc != 2) usage();
  std::string infile = argv[1];

  int r1 = 5; // поля текста
  int r2 = 5; // окрестность поиска при закрашивании темных линий

  int dw = 2*(r1+r2)+1; // ширина загружаемых данных
  int d0 = r1+r2;       // средняя линия данных

  int thr = 170;  // порог темных линий (r/3+g/3+b/3)

  int a = 128;    // прозрачность сетки

  ppm_reader lgnd("-D+30:34", infile.c_str(), dw);
  ppm_reader grid("-D+35:39", infile.c_str(), dw);
  ppm_reader text("-D+40",    infile.c_str(), dw);
  ppm_reader map("-D+41:400", infile.c_str(), dw);

  if ((lgnd.w!=grid.w)||(lgnd.w!=text.w)||(lgnd.w!=map.w)||
      (lgnd.h!=grid.h)||(lgnd.h!=text.h)||(lgnd.h!=map.h)){
    std::cerr << "different image sizes\n";
    exit(0);
  }
  int N = 3*map.w;

  unsigned char buf[N];
  std::cout << "P6\n" << map.w << " " << map.h << "\n255\n";

  for (int i = 0; i<map.h+dw; i++){
    if (map.data[d0]!=NULL){

      for (int j = 0; j < map.w; j++){
        // наложение текста
        if (is_color(text.data[d0]+3*j)) {
          // область, в которой нам надо стереть черные линии...
          for (int y = d0-r1; y<=d0+r1; y++){
            if (map.data[y]==NULL) continue;
            for (int x = j-r1; x<=j+r1; x++){
              if ((x<0)||(x>=map.w)) continue;
              if (abs(d0-y)*abs(d0-y) + abs(j-x)*abs(j-x) > r1*r1) continue;
              if (is_color(text.data[y]+3*x)) continue;
              // 
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
    if (map.data[dw-1]!=NULL){
      // сольем слои и запишем линию.      
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
