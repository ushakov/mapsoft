#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <zip.h>

#include "io.h"
#include "io_zip.h"
#include "err/err.h"


namespace io_zip{
using namespace std;

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

void
read_file(const char* filename, geo_data & world, const Options & opt) {
  struct zip *zip_file;
  struct zip_file *file_in_zip; 
  const char* fzip_name;

  int err;
  int files_total;
  int r,i;
  char buffer[1];
  string tmp;

  if (opt.exists("verbose")) cerr <<
    "Reading ZIP file " << filename << endl;

  zip_file = zip_open(filename, 0, &err);
  if (!zip_file)
    throw Err() << "Can't open ZIP file " << filename << ": " << zip_strerror(zip_file);

  files_total = zip_get_num_files(zip_file);
  if (!files_total) {
    zip_close(zip_file);
    throw Err() << "Can't read ZIP file " << filename << ": " << zip_strerror(zip_file);
  }

  for (i = 0; i < files_total; i++) {

    fzip_name=zip_get_name(zip_file,i,0);
    if (!fzip_name){
      zip_close(zip_file);
      throw Err() << "Can't read ZIP file "
        << filename << ": " << zip_strerror(zip_file);
    }

    file_in_zip = zip_fopen_index(zip_file, i, 0);
    if (!file_in_zip){
      zip_close(zip_file);
      throw Err() << "Can't read file " << fzip_name << " from ZIP file "
        << filename << ": " << zip_strerror(zip_file);
    }

    tmp = modfname(fzip_name);
    ofstream out(tmp.c_str());
    while ( (r = zip_fread(file_in_zip, buffer, sizeof(buffer))) > 0) {
      out << buffer[0];
    }
    out.close();
    zip_fclose(file_in_zip);
    io::in(tmp,world,opt);

    if (remove(tmp.c_str())) {
      zip_close(zip_file);
      throw Err() << "Can't delete temp file "<< tmp;
    }
  }
  if (zip_close(zip_file)!=0)
    throw Err() << "Can't write data to ZIP file: " << zip_strerror(zip_file);
}

void write_file (const char* filename, const std::vector<std::string> & files, const Options & opt){
  struct zip *Z;
  int err;

  if (opt.exists("verbose")) cerr <<
    "Writing ZIP file " << filename << endl;

  remove(filename);
  Z = zip_open(filename, ZIP_CREATE, &err);
  if (!Z) throw Err() << "Can't open ZIP file " << filename << " for writing";

  std::vector<std::string>::const_iterator f;
  for (f = files.begin(); f!=files.end();  f++){
    struct zip_source *s = zip_source_file(Z, f->c_str(),0,0);
    if ((s == NULL) || (zip_add(Z, f->c_str(), s) < 0)) {
      zip_source_free(s);
      zip_close(Z);
      throw Err() << "Can't write data to ZIP file: " << zip_strerror(Z);
    }
  }
  if (zip_close(Z)!=0)
    throw Err() << "Can't write data to ZIP file: " << zip_strerror(Z);

  for (f = files.begin(); f!=files.end();  f++){
    if (remove(f->c_str())){
      throw Err() << "Can't delete temp file " << *f;
    }
  }
}

}//namespace
