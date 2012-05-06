#include <iostream>
#include <iomanip>
#include <queue>

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "utils/err.h"
#include "options/m_getopt.h"

using namespace std;

static struct ext_option options[] = {
  {"lon0",   1,'l', 1, "set lon0 (default: auto)"},
  {"fmt",    1,'f', 1, "format string (default: \"%x %y %z %T %D %S\")"},
  {"window", 1,'w', 1, "window for speed calculation, sec (default: 120)"},
  {"break",  1,'b', 1, "place to break calculation and put empty line (none|day|track, default: none)"},
  {0,0,0,0}
};

void usage(bool pod=false){
  const char * fname = "mapsoft_toxyz";
  cerr << fname << " -- write track data in text form\n"
       << "Usage: " << fname << " <options> <input files>\n";
  cerr << "Options:\n";
  print_options(options, 1, cerr);
  cerr << "Format:\n"
       << "  %% -- % sign\n"
       << "  %l -- wgs lon\n"
       << "  %m -- wgs lat\n"
       << "  %x -- wgs tmerc x\n"
       << "  %y -- wgs tmerc y\n"
       << "  %z -- altitude, m\n"

       << "  %t -- time (seconds since 1970-01-01 00:00:00 UTC)\n"
       << "  %T -- date and time (yyyy-mm-dd HH:MM:SS)\n"
       << "  %u -- time from the previous point, s\n"

       << "  %d -- % distance from the part beginning, km\n"
       << "  %D -- % distance from the data beginning, km\n"
       << "  %e -- % distance from the previous point, m\n"

       << "  %S -- % speed, km/h\n"

       << "  %n -- %  point number starting from current part\n"
       << "  %N -- %  point number staarting from the beginning\n"
       << "  %p -- %  part number\n"
  ;
  exit(1);
}

int main (int argc, char **argv) {

  if (argc==1) usage();
  vector<string> infiles;
  Options O = parse_options_all(&argc, &argv, options, 1, infiles);
  if (O.exists("help")) usage();

  geo_data world;
  for (vector<string>::const_iterator i = infiles.begin(); i!=infiles.end(); i++){
    try { io::in(*i, world, O);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }


  double lon0   = O.get<double>("lon0", convs::lon2lon0(world.range().CNT().x));
  string fmt    = O.get<string>("fmt", "%x %y %z %T %D %S");
  double window = O.get<double>("window", 120.0);
  string br     = O.get<string>("break", "none");

  Options geo_opts;
  geo_opts.put("lon0", lon0);
  convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), geo_opts,
                    Datum("wgs84"), Proj("lonlat"), Options());

  double speed=0, dist = 0, Dist = 0;
  queue<pair<double, double> > timedist;

  int nn = 0;
  int NN = 0;
  int pn = 0;
  g_trackpoint tp, pp;
  for (int i = 0; i < world.trks.size(); ++i) {
    for (int p = 0; p < world.trks[i].size(); ++p) {

      tp = world.trks[i][p];
      pc.bck(tp); // convert lonlat -> tmerc

      if ( (br == "track" && (tp.start || p==0)) ||
           (br == "day"   && (nn != 0) &&
             (tp.t.value/86400 != pp.t.value/86400)) ){
        nn=0;
        pn++;
        cout << endl;
      }

      double dd=0;
      int    dt=0;
      if (nn != 0) {
        // update values:
        dd = hypot(tp.x - pp.x, tp.y - pp.y);
        dt = tp.t.value - pp.t.value;
        dist +=dd;
        Dist +=dd;
        timedist.push(make_pair(tp.t.value, Dist));
        while (timedist.front().first < tp.t.value - window &&
               timedist.size() > 2) timedist.pop();

        double traveled = timedist.back().second - timedist.front().second;
        double time = timedist.back().first - timedist.front().first;
        speed = traveled/time * 3.6;
      } else {
        dist = 0;
        speed = 0;
      }
      pp = tp;
      ++nn;
      ++NN;

      // print values according to fmt
      bool esc=false;
      for (string::const_iterator c=fmt.begin(); c!=fmt.end(); c++){
        if (!esc){
          if (*c=='%') esc=true;
          else cout << *c;
          continue;
        }
        switch(*c){
          case '%': cout << *c; break;
          case 'l': cout << fixed << setprecision(7) << world.trks[i][p].x; break;
          case 'm': cout << fixed << setprecision(7) << world.trks[i][p].y; break;
          case 'x': cout << (int)tp.x; break;
          case 'y': cout << (int)tp.y; break;
          case 'z': cout << fixed << setprecision(1) << tp.z; break;

          case 't': cout << tp.t.value; break;
          case 'T': cout << tp.t; break;
          case 'u': cout << dt;         break;
 
          case 'd': cout << fixed << setprecision(2) << dist/1000.0; break;
          case 'D': cout << fixed << setprecision(3) << Dist/1000.0; break;
          case 'e': cout << fixed << setprecision(2) << dd; break;

          case 'S': cout << fixed << setprecision(2) << speed;    break;

          case 'n': cout << nn+1;    break;
          case 'N': cout << NN+1;    break;
          case 'p': cout << pn+1;    break;
          default:
            cout << '%' << *c;
        }
        esc=false;
      }
      cout << endl;

    }
  }
}
