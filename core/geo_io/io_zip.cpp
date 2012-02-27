#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <zip.h>

#include "io.h"
#include "io_zip.h"

using namespace std;

namespace io_zip{

string
modfname(string s) {
  string::size_type dir,l;
  l=s.length();
  dir = s.rfind("/", l);
  if ( dir != string::npos ) s= s.substr(dir+1);
  const char *tmpdir = getenv("TMPDIR");
  if (tmpdir) return string(tmpdir) + "/" + s;
    return string("/tmp/") + s;
}

bool
read_file(const char* filename, geo_data & world, const Options & opt) {
  struct zip *zip_file;
  struct zip_file *file_in_zip; 
  const char* fzip_name;

  int err;
  int files_total;
  int r,i;
  char buffer[1];
  string tmp;

  zip_file = zip_open(filename, 0, &err);
  if (!zip_file){
    cerr << "Can't open " << filename << endl;
    return false;
  }

  files_total = zip_get_num_files(zip_file);
  if (!files_total) {
    cerr << "Can't read " << filename << endl;
    zip_close(zip_file);
    return false;
  }

  for (i = 0; i < files_total; i++) {
    file_in_zip = zip_fopen_index(zip_file, i, 0);
    if (!file_in_zip) {
      cerr << "Can't read file " << fzip_name
           << " from a zip archive" << endl;
      continue;
    }
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
      cerr << "Can't delete temp file "<< tmp << endl;
    }
  }
  zip_close(zip_file);
  return true;
}

bool write_file (const char* filename, const std::vector<std::string> & files){
  struct zip *Z;
  int err;
  remove(filename);
  Z = zip_open(filename, ZIP_CREATE, &err);
  if (!Z) {
    cerr << "Can't open " << filename << endl;
    return false;
  }
  std::vector<std::string>::const_iterator f;
  for (f = files.begin(); f!=files.end();  f++){
    struct zip_source *s = zip_source_file(Z, f->c_str(),0,0);
    if ((s == NULL) || (zip_add(Z, f->c_str(), s) < 0)) {
      std::cerr << "Can't add " << zip_strerror(Z) << " to zip archive" << endl;
      zip_source_free(s);
    }
  }
  if (zip_close(Z)!=0){
    std::cerr << "Can't write " << zip_strerror(Z) << endl;
    return false;
  }
  for (f = files.begin(); f!=files.end();  f++){
    if (remove(f->c_str()))
      cerr << "Can't delete "<< *f << endl;
  }
  return true;
}

}//namespace
