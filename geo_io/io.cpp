#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "io.h"
#include "fig.h"
#include "geofig.h"

namespace io {
	
	using namespace std;

	bool testext(const string & nstr, const char *ext){
		int pos = nstr.rfind(ext);
		return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
	}

	bool in(const string & name, geo_data & world, const Options & opt){
		if (name == "usb:"){
			cerr << "Reading data from GPS via libusb\n";
			if (!gps::get_all ("usb:", world, opt)) {
				cerr << "Error.\n";
				return false;
			}
			return true;
		}

		struct stat st_buf;
		if (stat(name.c_str(), &st_buf) != 0)
		{
			cerr << "Can't access file " << name << "\n";
			return false;
		}
		if (S_ISCHR(st_buf.st_mode))
		{
			cerr << "Reading data from GPS via serial port "
				 << name << "\n";
			if (!gps::get_all(name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		}

		ifstream in(name.c_str());
		char c;
		// определяем формат файла. Очень просто - берем
		// первый не-пробел и смотрим
		do {in >> c;} while ((c==' ')||(c=='\t')||
							 (c=='\r')||(c=='\n'));
		switch (c){
		case '<':
			cerr << "Reading data from XML file " << name << "\n";
			if (!xml::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		case '[':
			cerr << "Reading data from Garmin-utils file " << name << "\n";
			if (!gu::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		case 'O':
			cerr << "Reading data from OziExplorer file " << name << "\n";
			if (!oe::read_file (name.c_str(), world, opt)){
				cerr << "Error.\n";
				return false;
			}
			return true;
		case '#': {
			cerr << "Reading data from Fig file " << name << "\n";
                        fig::fig_world F;
                        fig::read(name.c_str(), F);
                        g_map m=fig::get_ref(F);
                        fig::get_wpts(F, m, world);
                        fig::get_trks(F, m, world);
                        fig::get_maps(F, m, world);
			return true; }
		default:
			cerr << "Unknown format in file " << name << "\n";
			return false;
		}
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

	void out(const string & outfile, geo_data const & world, const Options & opt){

		if (outfile == "usb:"){
			cerr << "Sending data to GPS via libusb\n";
			gps::put_all (outfile.c_str(), world, opt);
                        return;
		}
	
		struct stat st_buf;
		if (stat(outfile.c_str(), &st_buf) == 0) {
			if (S_ISCHR(st_buf.st_mode)){
				cerr << "Sending data to GPS via serial port "
				     << outfile << "\n";	
				gps::put_all (outfile.c_str(), world, opt);
				return;
			}
		}

// Исследование расширения
//  file.gu file.xml -- вывод в соответствующий файл
//  file.oe file.wpt file.plt file.map -- вывод в Ozi-файлы 
//    file1.wpt, file2.wpt, file1.plt, file2.plt и т.д.

  
		// Запись XML-файла
		if (testext(outfile, ".xml")){
		
			cerr << "Writing to XML file " << outfile << "\n";
			xml::write_file (outfile.c_str(), world, opt);
			return;
		}
		
		// Запись KML-файла
		if (testext(outfile, ".kml")){
		
			cerr << "Writing to Google KML file " << outfile << "\n";
			kml::write_file (outfile.c_str(), world, opt);
			return;
		}
/*
		// Запись BMP-файла
		if ((testext(outfile, ".bmp")) ||
			(testext(outfile, ".png")) ||
			(testext(outfile, ".jpg")) ){
		
			cerr << "Writing image " << outfile << "\n";
			bmp::write_image(outfile.c_str(), world, opt);
			return;
		}
*/
		// Запись файла Garmin-Utils
		if (testext(outfile, ".gu")){
			cerr << "Writing to Garmin-utils file " << outfile << "\n";
			gu::write_file (outfile.c_str(), world, opt);
			return;
		}
//		// Запись файла FIG
//		if (testext(outfile, ".fig")){
//			cerr << "Writing to FIG file " << outfile << "\n";
//			std::ofstream f(outfile.c_str());
//			fig::write (f, world, opt);
//			return;
//		}
		// Запись файла HTML
		if (testext(outfile, ".html") || testext(outfile, ".htm")){
			cerr << "Writing to HTML file " << outfile << "\n";
			std::ofstream f(outfile.c_str());
			html::write (f, world, opt);
			return;
		}

		// Запись файла OziExplorer
		if ((testext(outfile, ".wpt"))||
			(testext(outfile, ".plt"))||
			(testext(outfile, ".map"))||
			(testext(outfile, ".zip"))||
			(testext(outfile, ".oe"))){
			string base(outfile.begin(), outfile.begin()+outfile.rfind('.'));
			cerr << "Writing to OziExplorer files: \n";
			string files;
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
					files += " " + oef.str();
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
					files += " " + oef.str();
				}
			}
		
			for (size_t n = 0; n != world.maps.size(); ++n)
			{
				ostringstream oef;
				if (mw > 0)
					oef << base << setw(mw) << setfill('0') << n+1 << ".map";
				else 
					oef << base << ".map";
			
				ofstream f(oef.str().c_str());
				if (!oe::write_map_file (f, world.maps[n], opt))
				{
					cerr << "! " << oef.str() << " FAILURE\n";
				}
				else
				{
					cerr << "  " << oef.str() << " -- " << world.maps[n].size() << " reference points\n";
					files += " " + oef.str();
				}
			}
    
			if (testext (outfile, ".zip")){
				cerr << "Zipping "<< files << "\n";
				string zipcmd = "zip " + base + ".zip " + files;
				string rmcmd =  "rm " + files;
				system (zipcmd.c_str());
				system (rmcmd.c_str());
			}

			return;
		}

		cerr << "Can't determine output format. Please use filename with \n"
			 << " extensions (.fig), .xml, .gu, (.bmp, .png, .jpg), .wpt, .plt, .oe or .zip\n"
			 << " or name of serial port device (for example /dev/ttyS0),\n"
			 << " or \"usb:\" for using libusb\n";
	}
	
}
