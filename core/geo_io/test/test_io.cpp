#include "../io.h"
main(){
  geo_data world;
  Options  o;
  o["gps_switch_off"]="";
  gps::fetch_all("usb:", world, o);
  gu::write_file("test2.gu", world, o);
//  gu::read_file("test1.gu", world, o);
  xml::write_file("test2.xml", world, o);

}
