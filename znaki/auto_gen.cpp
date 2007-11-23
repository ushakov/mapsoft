#include <fstream>
#include <vector>
#include <string>
#include "legend.h" // информация обо всех условных обозначениях карты

using namespace std;

void usage(){
  std::cout << "usage: mapinfo typ|mplist [<style>]\n";
  exit(0);
}

char *styles[] = {"mmb", "hr", NULL};

main(){

  for (int i=0; styles[i]!=0; i++){
    string fname("./auto/"); fname+=styles[i];
    legend znaki(styles[i]);

    ofstream o1((fname+".typ").c_str());
    znaki.print_typ(o1);
    ofstream o2((fname+".txt").c_str());
    znaki.print_mplist(o2);

    znaki.print_figlib("./auto/");
  }

}
