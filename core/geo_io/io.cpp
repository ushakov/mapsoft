#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <dirent.h>
#include <string>
#include <vector>
#include <list>

#include <sys/stat.h>
#include <math.h>
#include <zip.h>

#include "io.h"
#include "geofig.h"

namespace io {
	
	using namespace std;

	bool testext(const string & nstr, const char *ext){
		int pos = nstr.rfind(ext);
		return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
	}

        // -1 -- can't access file; 0 - regular, 1 - character device
	int check_file(const string & name){
	  struct stat st_buf;
	  if (stat(name.c_str(), &st_buf) != 0) return -1;
	  if (S_ISREG(st_buf.st_mode)) return 0;
	  if (S_ISCHR(st_buf.st_mode)) return 1;
        }

        std::string gps_detect(){
          DIR *D = opendir("/sys/module/garmin_gps/drivers/usb-serial:garmin_gps");
          if (!D) return "";
          dirent * de;
          while (de = readdir(D)){
            string fname = string("/dev/") + string(de->d_name);
            if (check_file(fname) == 1) return fname;
          }
          return "";
        }

	bool in(const string & in_name, geo_data & world, const Options & opt){

                string name(in_name);

/*
		if (name == "usb:"){
			cerr << "Reading data from GPS via libusb\n";
			if (!gps::get_all ("usb:", world, opt)) {
				cerr << "Error.\n";
				return false;
			}
			return true;
		}
*/

		if (name == "gps:"){
                   name = gps_detect();
                   if (name == ""){
                     cerr << "Can't detect gps device\n";
                     return false;
                   }
                }

                int c = check_file(name);
                if (c < 0){
			cerr << "Can't access file " << name << "\n";
			return false;
		}

		if (c == 1){
		  cerr << "Reading data from GPS via serial port "
		    << name << "\n";
		  if (!gps::get_all(name.c_str(), world, opt)){
		    cerr << "Error.\n";
		    return false;
		  }
		  return true;
		}

		if (testext(name, ".xml")){
			cerr << "Reading data from XML file " << name << "\n";
			if (!xml::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		}
		if (testext(name, ".gpx")){
			cerr << "Reading data from GPX file " << name << "\n";
			if (!gpx::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		}
		if (testext(name, ".gu")){
			cerr << "Reading data from Garmin-utils file " << name << "\n";
			if (!gu::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		}
		if ((testext(name, ".plt")) || (testext(name, ".wpt")) || (testext(name, ".map"))){
			cerr << "Reading data from Ozi file " << name << "\n";
			if (!oe::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		}
		if (testext(name, ".fig")){
			cerr << "Reading data from Fig file " << name << "\n";
                        fig::fig_world F;
                        fig::read(name.c_str(), F);
                        g_map m=fig::get_ref(F);
                        fig::get_wpts(F, m, world);
                        fig::get_trks(F, m, world);
                        fig::get_maps(F, m, world);
			return true;
                }
		if (testext(name, ".zip")) {
			cerr << "Reading data from zip archive " << name << "\n";
			if(!io_zip::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
                }
		cerr << "Unknown format in file " << name << "\n";
		return false;
	}

	void in(list<string> const & names, geo_data & world, const Options & opt)
	{
		for (list<string>::const_iterator it = names.begin(); 
			 it != names.end();
			 it++){
			in(*it, world, opt);
		}
	}


// запись в файл

	void out(const string & out_name, geo_data const & world, const Options & opt){

                string name(out_name);

/*
		if (name == "usb:"){
			cerr << "Sending data to GPS via libusb\n";
			gps::put_all (name.c_str(), world, opt);
                        return;
		}
*/


		if (name == "gps:"){
                   name = gps_detect();
                   if (name == ""){
                     cerr << "Can't detect gps device\n";
                     return;
                   }
                }

		if (check_file(name) == 1){
		  cerr << "Sending data to GPS via serial port " << name << "\n";
		  if (!gps::put_all (name.c_str(), world, opt)){
		    cerr << "Error.\n";
		    return;
		  }
		  return;
		}

// Исследование расширения
//  file.gu file.xml -- вывод в соответствующий файл
//  file.oe file.wpt file.plt file.map -- вывод в Ozi-файлы 
//    file1.wpt, file2.wpt, file1.plt, file2.plt и т.д.

  
		// Запись XML-файла
		if (testext(name, ".xml")){
		
			cerr << "Writing to XML file " << name << "\n";
			xml::write_file (name.c_str(), world, opt);
			return;
		}

		// Запись GPX-файла
		if (testext(name, ".gpx")){
			cerr << "Writing to GPX file " << name << "\n";
			gpx::write_file (name.c_str(), world, opt);
			return;
		}
		
		// Запись KML-файла
		if (testext(name, ".kml") || testext(name, ".kmz")){
			string base(name.begin(), name.begin()+name.rfind('.'));
			string kml=base+".kml";
			cerr << "Writing to Google KML file " << kml << "\n";
			kml::write_file (kml.c_str(), world, opt);

			if (testext (name, ".kmz")){
				cerr << "Zipping "<< kml << "\n";
				string zipcmd = "zip " + base + ".kmz " + kml;
				string rmcmd =  "rm " + kml;
				if (system (zipcmd.c_str())==-1) cerr << "Error: can't do zip\n";
				if (system (rmcmd.c_str())==-1)  cerr << "Error: can't do rm\n";
			}
			return;
		}

		// Запись растровой картинки
		if ((testext(name, ".tiff")) ||
		    (testext(name, ".tif")) ||
		    (testext(name, ".png")) ||
		    (testext(name, ".jpeg")) ||
		    (testext(name, ".jpg")) ){
		
			cerr << "Writing image " << name << "\n";
			img::write_file(name.c_str(), world, opt);
			return;
		}

		// Запись плиток
		if (testext(name, ".tiles")) {
			cerr << "Writing tiles to " << name << "\n";
			tiles::write_file(name.c_str(), world, opt);
			return;
		}

		// Запись файла Garmin-Utils
		if (testext(name, ".gu")){
			cerr << "Writing to Garmin-utils file " << name << "\n";
			gu::write_file (name.c_str(), world, opt);
			return;
		}
//		// Запись файла FIG
//		if (testext(name, ".fig")){
//			cerr << "Writing to FIG file " << name << "\n";
//			fig::write (name, world, opt);
//			return;
//		}
		// Запись файла HTML
//		if (testext(name, ".html") || testext(name, ".htm")){
//			cerr << "Writing to HTML file " << name << "\n";
//			std::ofstream f(name.c_str());
//			html::write (f, world, opt);
//			return;
//		}

		// Запись файла OziExplorer
		if ((testext(name, ".wpt"))||
			(testext(name, ".plt"))||
			(testext(name, ".map"))||
			(testext(name, ".zip"))||
			(testext(name, ".oe"))){
			string base(name.begin(), name.begin()+name.rfind('.'));
			cerr << "Writing to OziExplorer files: \n";
			vector<string> files;
			// подсчитаем, сколько треков и сколько точек нам надо записать
			int wn = world.wpts.size();
			int tn = world.trks.size();
			int mn = world.maps.size();

			int ww=0, tw=0, mw=0; 
			if (wn > 1) ww = (int) floor (log (1.0 * wn) / log (10.0)) + 1;
			if (tn > 1) tw = (int) floor (log (1.0 * tn) / log (10.0)) + 1;
			if (mn > 1) mw = (int) floor (log (1.0 * mn) / log (10.0)) + 1;

			for (size_t n = 0; n != world.wpts.size(); ++n)
			{
				ostringstream oef;
				if (ww>0)
					oef << base << setw(ww) << setfill('0') << n+1 << ".wpt";
				else 
					oef << base << ".wpt";
				ofstream f(oef.str().c_str());
				if (!oe::write_wpt_file (f, world.wpts[n], opt))
				{
					cerr << "! " << oef.str() << " FAILURE\n";
				}
				else
				{
					cerr << "  " << oef.str() << " -- " << world.wpts[n].size() << " waypoints\n";
					files.push_back(oef.str());
				}
			}
			for (size_t n = 0; n != world.trks.size(); ++n)
			{
				ostringstream oef;
				if (tw > 0)
					oef << base << setw(tw) << setfill('0') << n+1 << ".plt";
				else 
					oef << base << ".plt";

				ofstream f(oef.str().c_str());
				if (!oe::write_plt_file (f, world.trks[n], opt))
				{
					cerr << "! " << oef.str() << " FAILURE\n";
				}
				else
				{
					cerr << "  " << oef.str() << " -- " << world.trks[n].size() << " points\n";
					files.push_back(oef.str());
				}
			}

			for (size_t nn = 0; nn != world.maps.size(); ++nn) {

			  int mmn = world.maps[nn].size(); // map count in this maplist
			  int mmw=0;
			  if (mmn > 1) mmw = (int) floor (log (1.0 * mmn) / log (10.0)) + 1;

			  for (size_t n = 0; n != world.maps[nn].size(); ++n) {
				ostringstream oef;
                                oef << base;
				if (mmw > 0)  oef << setw(mmw) << setfill('0') << nn+1;
				if ((mmw > 0) && (mw > 0))  oef << "_";
				if (mw > 0)  oef << setw(mw) << setfill('0') << n+1;
				oef << ".map";

				ofstream f(oef.str().c_str());
				if (!oe::write_map_file (f, world.maps[nn][n], opt)){
					cerr << "! " << oef.str() << " FAILURE\n";
				}
				else {
					cerr << "  " << oef.str() << " -- " << world.maps[n].size() << " reference points\n";
					files.push_back(oef.str());
				}
			  }
			}

			if (testext (name, ".zip")){
			  cerr << "Zipping files to " << name << "\n";
                          io_zip::write_file(name.c_str(), files);
			}

			return;
		}

		cerr << "Can't determine output format. Please use filename with \n"
			 << " extensions (.fig), .xml, .gu, (.bmp, .png, .jpg), .wpt, .plt, .oe or .zip\n"
			 << " or name of serial port device (for example /dev/ttyS0),\n"
			 << " or \"usb:\" for using libusb\n";
	}

}  // namespace io


