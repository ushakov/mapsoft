#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <zip.h>

#include "io.h"
#include "io_zip.h"

using namespace std;

namespace io_zip{ 
// функция для раскрытия zip-файлов
 
	string modfname(string s) {
		string::size_type dir,l;
		l=s.length();
		dir = s.rfind("/", l);
		if ( dir != string::npos ) {
		        s= s.substr(dir+1);
		}
		string tmpdir = getenv("TMPDIR");
		if (tmpdir=="") tmpdir = "/tmp";
		return tmpdir + "/" + s;
	}

	bool read_file(const char* filename, geo_data & world, const Options & opt) {
		struct zip *zip_file;
		struct zip_file *file_in_zip; 
		const char* fzip_name;

		int err; 
		int files_total; 
		int r,i;
		char buffer[1];
                string tmp;

		zip_file = zip_open(filename, 0, &err);
		files_total = zip_get_num_files(zip_file);
		if ((!zip_file)||(!files_total)) {
			cerr << "Error: can't open file " << filename << endl;
			return false;
		} else for (i = 0; i < files_total; i++) {

			file_in_zip = zip_fopen_index(zip_file, i, 0);
			if (!file_in_zip) {
				cerr << "Error: can't open file " << fzip_name << " in zip\n";
			} else {
				fzip_name=zip_get_name(zip_file,i,0);
				tmp = modfname(fzip_name);
				ofstream out(tmp.c_str());
				while ( (r = zip_fread(file_in_zip, buffer, sizeof(buffer))) > 0) {
					out << buffer[0];
				}
				out.close();
				zip_fclose(file_in_zip);

				io::in(tmp,world,opt);

				if (remove(tmp.c_str())) {
					cerr << "Error: can't delete temp file "<< tmp << endl;
				}
			}
		}
		zip_close(zip_file);
		return true;
	}
}
