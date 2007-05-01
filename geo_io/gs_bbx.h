#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "../utils/rect.h"
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <string>

struct gs_bbx{
  int   in_pipe[2];
  int   out_pipe[2];
  pid_t gs_pid;

  gs_bbx(){
    char  *gs_name = "gs";
    char  *gs_args[] = {"gs", "-sDEVICE=bbox", "-dNOPAUSE", "-dBATCH", "-", NULL};
    if (pipe(in_pipe)!=0){ std::cerr << "can't open pipe\n"; exit(-1); }
    if (pipe(out_pipe)!=0){ std::cerr << "can't open pipe\n"; exit(-1); }
 
    gs_pid = fork();
    if      (gs_pid==-1) { std::cerr << "can't do fork\n"; exit(-1); }
    else if (gs_pid==0)  {
      dup2(in_pipe[0], STDIN_FILENO);
      dup2(out_pipe[1], STDERR_FILENO);
      close(STDOUT_FILENO);
      close(in_pipe[1]);
      close(out_pipe[0]);
      execvp(gs_name, gs_args);
    }
    close(in_pipe[0]);
    close(out_pipe[1]);

    fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);

    char c;

    puts("1 1 scale\n");
//    puts("/Helvetica-Bold findfont 12 scalefont setfont\n");
//    puts("0 0 moveto (������!) show showpage\n");
//    while (read(out_pipe[0], &c, 1)!=1); // ����
//    while (read(out_pipe[0], &c, 1)==1);
  }

  void putc(const char c) const{
    write(in_pipe[1], &c, 1);
  }

  void puts(const char *str) const{
    write(in_pipe[1], str, strlen(str));
  }

  Rect<double> get() const {
    char c; std::string s;
    while (read(out_pipe[0], &c, 1)!=1); // ����
    do { s.push_back(c); } while (read(out_pipe[0], &c, 1)==1);
    using namespace boost::spirit;
    Rect<double> ret;
    if ( !parse(s.c_str(), 
        str_p("%%BoundingBox:") >> +space_p >> 
        int_p >> +space_p >> int_p >> +space_p >> 
        int_p >> +space_p >> int_p >> +space_p >> 
        str_p("%%HiResBoundingBox:") >> +space_p >> 
        real_p[assign_a(ret.x)] >> +space_p >> 
        real_p[assign_a(ret.y)] >> +space_p >> 
        real_p[assign_a(ret.w)] >> +space_p >> 
	real_p[assign_a(ret.h)] >> *space_p).full){
      std::cerr << "can't parse GS output: \n" << s;
      exit(-1);
    }
    ret.w-=ret.x;
    ret.h-=ret.y;
    return ret;
  }

  void set_font(int size, const char *name) const {
    std::ostringstream s;
    s << "/" << name << " findfont " << size << " scalefont setfont\n";
    puts(s.str().c_str());
  }

  Rect<double> txt2bbx(const char * text) const{
    puts("100 100 moveto (");
    for (int i = 0; i< strlen(text); i++){
      if ((text[i]==')')||(text[i]=='\\')) putc('\\');
      putc(text[i]);
    }
    puts(") show showpage\n");
    return (get()-Point<double>(100,100));
  }

 
  ~gs_bbx(){
      close(in_pipe[1]);
      close(out_pipe[0]);
  }

};
