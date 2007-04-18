#include <vector>
#include "fig.h"

#include <sstream>

#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
/*
#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
*/

// найти прямоугольники, накрывающие весь текст

namespace fig{

using namespace std;

vector<Rect<int> > text_bbxs(const fig_world & w){


  // запускаем gs, настраиваем pipe к нему
  int   gs_pipe[2];
  pid_t gs_pid;
  
  char  *gs_name = "gs";

/*  char *ofile = tmpnam(NULL);
  string farg = "-sOutputFile=" + ofile;*/
  char  *gs_args[] = {"gs", "-sDEVICE=bbox", "-dNOPAUSE", "-dBATCH", "-", NULL};

  if (pipe(gs_pipe)!=0){ cerr << "can't open pipe\n"; exit(-1); }

  gs_pid = fork();
  if      (gs_pid==-1) { cerr << "can't do fork\n"; exit(-1); }
  else if (gs_pid==0)  {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(gs_pipe[0], STDIN_FILENO);
    dup2(gs_pipe[1], STDOUT_FILENO);
    close(gs_pipe[0]);
    close(gs_pipe[1]);
    execvp(gs_name, gs_args);
  }
  /* Нам надо дать в gs что-то такое:
  1 1 scale
  /Helvetica-Bold findfont 12 scalefont setfont
  1000 1000 moveto 15.0 rotate (П) show showpage
  1000 1000 moveto (р) show showpage
  1000 1000 moveto (и) show showpage
  1000 1000 moveto (в) show showpage
  1000 1000 moveto (е) show showpage
  1000 1000 moveto (т) show showpage
  */

  ostringstream out_str;
  fig_psfonts fontnames;
  
  out_str << "1 1 scale\n";
  
  ::write(gs_pipe[1], out_str.str().c_str(), out_str.str().size()); 
  out_str.str("");

  for (fig_world::const_iterator i  = w.begin(); i != w.end(); i++){
    if (i->type!=4) continue;
    out_str << "/" << fontnames[i->font] << " findfont " 
         << i->font_size << " scalefont setfont\n";
    ::write(gs_pipe[1], out_str.str().c_str(), out_str.str().size()); 
      cout << "in:   " << out_str.str() << "\n";
    out_str.str("");
    
    for (string::const_iterator l = i->text.begin(); l!=i->text.end(); l++){
      out_str << "1000 1000 moveto ";
      if (i->angle!=0) out_str << i->angle << " rotate ";
      out_str << "(" << *l << ")" << " show showpage\n";
      ::write(gs_pipe[1], out_str.str().c_str(), out_str.str().size()); 
      cout << "in:   " << out_str.str() << "\n";
      out_str.str("");
      char c; string s;
      while ((::read(gs_pipe[0], &c, 1)>1)&&(c!='\n')) s.push_back(c);
      cout << "str1: " << s << "\n"; s.clear();
      while ((::read(gs_pipe[0], &c, 1)>1)&&(c!='\n')) s.push_back(c);
      cout << "str2: " << s << "\n";
    }

  }
  close(gs_pipe[1]);
/*
  using namespace boost::spirit;
  typedef char                    char_t;
  typedef file_iterator <char_t>  iterator_t;
  typedef scanner<iterator_t>     scanner_t;
  typedef rule <scanner_t>        rule_t;
 
  // iterators for parsing
  iterator_t first(ofile);
  if (!first) { cerr << "can't find file " << ofile << '\n'; return world;}
  iterator_t last = first.make_end();

  parse
*/

}
}//namespace
