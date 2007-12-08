#include <cstdio>
//#include <unistd.h>
//#include <sys/types.h>

#include <iostream>


struct ppm_reader{
  int   out_pipe[2];
  pid_t pid;

  int w,h;

  ppm_reader(const char *depth_range, const char *infile){

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
    return;    
  }
  ~ppm_reader(){close(out_pipe[0]);}

  std::string gets() const {
    std::string ret;
    char c;
    while ((read(out_pipe[0], &c, 1)==1) && (c!='\n')) ret.push_back(c);
    return ret;
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

main(int argc, char **argv){

  if (argc != 2) usage();
  std::string infile = argv[1];

  ppm_reader lgnd("-D+30:34", infile.c_str());
  ppm_reader grid("-D+35:39", infile.c_str());
  ppm_reader text("-D+40",    infile.c_str());
  ppm_reader map("-D+41:400", infile.c_str());
  int N = 3*map.w;
  unsigned char buf_l[N];
  unsigned char buf_g[N];
  unsigned char buf_m[N];
  unsigned char buf_t[N];
  int a = 128;

  std::cout << "P6\n" << map.w << " " << map.h << "\n255\n";
  for (int y = 0; y < map.h; y++){
    if (!pipe_read(lgnd.fd(), buf_l, N)) {std::cerr << "read error l\n"; exit(0);}
    if (!pipe_read(grid.fd(), buf_g, N)) {std::cerr << "read error g\n"; exit(0);}
    if (!pipe_read(text.fd(), buf_t, N)) {std::cerr << "read error t\n"; exit(0);}
    if (!pipe_read(map.fd(),  buf_m, N)) {std::cerr << "read error m\n"; exit(0);}
    for (int x = 0; x < map.w; x++){

      unsigned char rm = buf_m[3*x], gm = buf_m[3*x+1], bm = buf_m[3*x+2];
      unsigned char rt = buf_t[3*x], gt = buf_t[3*x+1], bt = buf_t[3*x+2];
      unsigned char rg = buf_g[3*x], gg = buf_g[3*x+1], bg = buf_g[3*x+2];
      unsigned char rl = buf_l[3*x], gl = buf_l[3*x+1], bl = buf_l[3*x+2];

// почему-то fig2dev, если его просить сделать 
// какой-то фоновый цвет, соблюдает его с точностью +-1 ?? Ошибки округления?
// поэтому просим 0xFFFFFD и смотрим, чтоб было в диапазоне +-1

      if ((rt < 0xFC) || (rt > 0xFE) ||
          (gt < 0xFC) || (gt > 0xFE) ||
          (bt < 0xFC) || (bt > 0xFE)) {rm=rt; gm=gt; bm=bt;}

      if ((rg < 0xFC) || (rg > 0xFE) ||
          (gg < 0xFC) || (gg > 0xFE) ||
          (bg < 0xFC) || (bg > 0xFE)) {
        rm = (rg*a + rm*(255-a))/255;
        gm = (gg*a + gm*(255-a))/255;
        bm = (bg*a + bm*(255-a))/255;
      }

      if ((rl < 0xFC) || (rl > 0xFE) ||
          (gl < 0xFC) || (gl > 0xFE) ||
          (bl < 0xFC) || (bl > 0xFE)) {rm=rl; gm=gl; bm=bl;}

      std::cout << rm << gm << bm;
    }
  }
}
