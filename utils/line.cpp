#include "line.h"

// склеивание линий в одну, если их концы ближе e
std::vector<Line<double> > merge (std::vector<Line<double> > lines, double e){
  //убираем вообще двойные линии
  for (std::vector<Line<double> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
  for (std::vector<Line<double> >::iterator i2 = i1; i2!=lines.end(); i2++){
    if (i1==i2) continue;
    if ((*i1==*i2) || (i1->isinv(*i2))){
      lines.erase(i2);
      i1 = lines.erase(i1); i1--;
      i2 = i1;
    }
  }
  }

  for (std::vector<Line<double> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
  for (std::vector<Line<double> >::iterator i2 = i1; i2!=lines.end(); i2++){
    if (i1==i2) continue;
    Line<double> tmp;
    if      (pdist(*(i1->begin()),*(i2->begin()))<e)   {tmp.insert(tmp.end(), i1->rbegin(), i1->rend()); tmp.insert(tmp.end(), i2->begin()+1, i2->end());}
    else if (pdist(*(i1->begin()),*(i2->rbegin()))<e)  {tmp.insert(tmp.end(), i1->rbegin(), i1->rend()); tmp.insert(tmp.end(), i2->rbegin()+1, i2->rend());}
    else if (pdist(*(i1->rbegin()),*(i2->begin()))<e)  {tmp.insert(tmp.end(), i1->begin(), i1->end()); tmp.insert(tmp.end(), i2->begin()+1, i2->end());}
    else if (pdist(*(i1->rbegin()),*(i2->rbegin()))<e) {tmp.insert(tmp.end(), i1->begin(), i1->end()); tmp.insert(tmp.end(), i2->rbegin()+1, i2->rend());}
    else continue;
    i1->swap(tmp);
    lines.erase(i2); 
    i2=i1;
  }
  }
  return lines;
}
