#include "line.h"

// склеивание линий в одну, если их концы ближе e
void merge (std::list<Line<double> > & lines, double e){
  //убираем вообще двойные линии
//std::cerr << "  убираем двойные линии\n";
  for (std::list<Line<double> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
  for (std::list<Line<double> >::iterator i2 = i1; i2!=lines.end(); i2++){
    if (i1==i2) continue;
    if ((*i1==*i2) || (i1->isinv(*i2))){
      lines.erase(i2);
      i1 = lines.erase(i1); i1--;
      i2 = i1;
    }
  }
  }
//std::cerr << "  стыкуем линии\n";
  for (std::list<Line<double> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
  for (std::list<Line<double> >::iterator i2 = i1; i2!=lines.end(); i2++){
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
//std::cerr << "  ок\n";
}

// разбиение линии на несколько, каждая не более points точек
void split (std::list<Line<double> > & lines, int points){
  if (points < 2) return;
  for (std::list<Line<double> >::iterator i = lines.begin(); i!=lines.end(); i++){
    while (i->size() > points){
      // points последних точек
      Line<double> newline;
      newline.insert(newline.begin(), i->rbegin(), i->rbegin()+points);
      lines.push_back(newline);
      i->resize(i->size()-points+1);
    }
  }
}

// Убрать из линии некоторые точки, так, чтобы линия
// не сместилась от исходного положения более чем на e
void generalize (std::list<Line<double> > & lines, double e){
  for (std::list<Line<double> >::iterator l = lines.begin(); l!=lines.end(); l++){

    // какие точки мы хотим исключить:
    std::vector<bool> skip(l->size(),false);

    while (l->size()>2){
      // для каждой точки найдем расстояние от нее до 
      // прямой, соединяющей две соседние (не пропущенные) точки.
      // найдем минимум этой величины
      double min = 1e99; //важно, что >e 
      int mini;
      for (int i=1; i<l->size()-1; i++){
        if (skip[i]) continue;
        int ip, in;
        // помним, что skip[0] и skip[l->size()-1] всегда false
        for (ip=i-1; ip>=0; ip--)          if (!skip[ip]) break;
        for (in=i+1; in<l->size()-1; in++) if (!skip[in]) break;
        Point<double> p1 = (*l)[ip];
        Point<double> p2 = (*l)[i];
        Point<double> p3 = (*l)[in];
        double dp = pdist((p3-p1)/2 - (p2-p1));
        // в начале у нас обязатеьно i==1
        if ((i==1) || (min>dp)) {min = dp; mini=i;}
      }
      // если этот минимум меньше e - выкинем точку
      if (min<e) skip[mini]=true;
      else break;
    }

    // сделаем новую линию
    Line<double> newl;
    for (int i = 0; i<l->size(); i++) if (!skip[i]) newl.push_back((*l)[i]);
    l->swap(newl);

    // если осталась линия из двух близких точек - сотрем ее.
    if ((l->size() == 2) && (pdist((*l)[0],(*l)[1]) < e)){
      l=lines.erase(l); l--;
    }
  }

}

// лежит ли точка в многоугольнике poly
bool test_pt (const Point<double> & pt, const Line<double> & poly){
  double a = 0;
  Line<double>::const_iterator p1,p2;
  for (int i = 0; i<poly.size(); i++){
    Point<double> v1 = poly[i] - pt;
    Point<double> v2 = poly[(i+1)%poly.size()] - pt;
    
    double s = v1.x*v2.y - v1.y*v2.x;
    double c = acos(pscal(v1,v2)/pdist(v1)/pdist(v2));
    if (s<0) a+=c; else a-=c;
  }
  return (fabs(a)>M_PI);
}

void crop_lines(std::list<Line<double> > & lines, const Line<double> & cutter){
  for (std::list<Line<double> >::iterator l = lines.begin(); l!=lines.end(); l++){
    for (int i = 0; i<l->size()-1; i++){
      for (int j = 0; j<cutter.size(); j++){
        Point<double> pt;
        try { pt = find_cross((*l)[i], (*l)[i+1], cutter[j], cutter[(j+1)%cutter.size()]); }
        catch (int i) {continue;}
        // разбиваем линию на две, уже обработанный кусок помещаем перед l
        Line<double> l1;
        for (int k=0; k<=i; k++) l1.push_back((*l)[k]);
        l1.push_back(pt);
        lines.insert(l, l1);
        // из *l стираем все точки до i-й 
        l->erase(l->begin(), l->begin()+i);
        *(l->begin()) = pt;
        // возвращаемся в начало обоих циклов
        i=0; break;
      }
    }
  }
  // теперь удалим те линии, которые не попадают в нужный район
  for (std::list<Line<double> >::iterator l = lines.begin(); l!=lines.end(); l++){
    if (l->size()==0) {l=lines.erase(l); l--; continue;}

    // для проверки надо выбрать точку, не лежащую на линии cutter
    Point<double> testpt;
    if (l->size()==1) {testpt = (*l)[0];}
    else {testpt = ((*l)[0]+(*l)[1])/2;} 
    // (здесь может произойти фигня, если линия касается первым звеном cutter)
    if (!test_pt(testpt, cutter)) {l=lines.erase(l); l--; continue;}
  }
}
