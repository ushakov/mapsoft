#include "image_ks.h"

namespace ks{

static CURL * curl_handle = 0;
static char curl_error[CURL_ERROR_SIZE];

// Загрузка картинки
int load(
    const std::string & dir, 
    int ks_scale,	
    iRect src_rect, 
    iImage & image, 
    iRect dst_rect,
    bool do_download)
{
  if (ks_scale<ks_scale_min) ks_scale=ks_scale_min;
  if (ks_scale>ks_scale_max) ks_scale=ks_scale_max;
 
  int src_width = 256* (1 << (ks_scale-1));

  iRect src_points = Rect<int>(0,0,src_width,src_width);

//  // подрежем прямоугольники
//  clip_rects_for_image_loader(
//      src_points, src_rect,
//      iRect(0,0,image.w,image.h), dst_rect);

  if (src_rect.empty() || dst_rect.empty()) return 1;


  iRect src_tiles  = tiles_on_rect(src_rect,256);

  for (int yt = src_tiles.TLC().y; yt<src_tiles.BRC().y; yt++){
    for (int xt = src_tiles.TLC().x; xt<src_tiles.BRC().x; xt++){

      int sx1 = 0;   if (xt==src_tiles.TLC().x) sx1 = src_rect.TLC().x % 256;
      int sy1 = 0;   if (yt==src_tiles.TLC().y) sy1 = src_rect.TLC().y % 256;
      int sx2 = 256; if (xt==src_tiles.BRC().x-1) sx2 = (src_rect.BRC().x-1) % 256 + 1;
      int sy2 = 256; if (yt==src_tiles.BRC().y-1) sy2 = (src_rect.BRC().y-1) % 256 + 1;
      if ((sx1==sx2) || (sy1==sy2)) continue;

      int dx1 = ((256*xt + sx1 - src_rect.x) * dst_rect.w)/src_rect.w + dst_rect.x;
      int dx2 = ((256*xt + sx2 - src_rect.x) * dst_rect.w)/src_rect.w + dst_rect.x;
      int dy1 = ((256*yt + sy1 - src_rect.y) * dst_rect.h)/src_rect.h + dst_rect.y;
      int dy2 = ((256*yt + sy2 - src_rect.y) * dst_rect.h)/src_rect.h + dst_rect.y;


      iRect src(sx1,sy1,sx2-sx1,sy2-sy1);
      iRect dst(dx1,dy1,dx2-dx1,dy2-dy1);
#ifdef DEBUG_KS
      std::cerr << "ks: loading " << addr << " " << src << " --> " << dst << "\n";
#endif
      std::ostringstream tile_file_s;
      tile_file_s << dir << "/" << ks_scale << "/" << xt << "/" << yt << ".jpg";
      std::string tile_file = tile_file_s.str();

      int ret = image_jpeg::load(tile_file.c_str(), src, image, dst);
      if (ret == 3 && do_download) {


          std::ostringstream tile_dir_s;
          tile_dir_s << dir << "/" << ks_scale << "/";
          mkdir(tile_dir_s.str().c_str(),0755);
          tile_dir_s << xt << "/";
          mkdir(tile_dir_s.str().c_str(),0755);

	  // try libcurl loading
	  if (!curl_handle) {
	      curl_handle = curl_easy_init();
	      if (!curl_handle) return 1;
	      curl_easy_setopt (curl_handle, CURLOPT_ERRORBUFFER, curl_error);
	  }
	  FILE *f = fopen (tile_file.c_str(), "w");
	  if (!f) return 1;

          std::ostringstream tile_url_s;

	  tile_url_s << "http://images.new.kosmosnimki.ru/printtilesrescaled/?t=image_&zt=n&x=" 
                     << int((2*xt + 1) * (1 << (24-ks_scale))*1.194329)
                     << "&y="
                     << int((src_width/128 - 2*yt - 1) * (1 << (24-ks_scale))*1.194329)
                     << "&z=" << ks_scale;

	  std::string tile_url = tile_url_s.str();
	  std::cerr << "fetching url " << tile_url << std::endl;
	  if (curl_easy_setopt (curl_handle, CURLOPT_URL, tile_url.c_str())) {
	      std::cerr << curl_error;
	      fclose (f); return 1;
	  }
	  if (curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, f)) {
	      std::cerr << curl_error;
	      fclose (f); return 1;
	  }
	  if (curl_easy_perform (curl_handle)) {
	      std::cerr << curl_error;
	      fclose (f); return 1;
	  }
	  long result;
	  if (curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &result)) {
	      std::cerr << curl_error;
	      fclose (f); return 1;
	  }
	  if (result != 200) {
	      std::cerr << curl_error;
	      std::cerr << "Error code: " << result << std::endl;
	      fclose (f); return 1;
	  }
	  fclose (f);
	  ret = image_jpeg::load(tile_file.c_str(), src, image, dst);
      }
    }
  }
  return 0;
}

iImage load(const std::string & dir, int ks_scale, const iRect & src_rect, int scale, bool do_download){
  int w = src_rect.w/scale;
  int h = src_rect.h/scale;
  iRect dst_rect(0,0,w,h);
  iImage ret(w,h,0xFF000000);
  load(dir, ks_scale, src_rect, ret, dst_rect, do_download);
  return ret;
}

}//namespace
