#include "line.h"

// ���������� ����� � ����, ���� �� ����� ����� e
std::list<Line<double> > merge (std::list<Line<double> > lines, double e){
  //������� ������ ������� �����
//std::cerr << "  ������� ������� �����\n";
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
//std::cerr << "  ������� �����\n";
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
//std::cerr << "  ��\n";
  return lines;
}

// ��������� ����� �� ���������, ������ �� ����� points �����
std::list<Line<double> > split (std::list<Line<double> > lines, int points){
  if (points < 2) return lines;
  for (std::list<Line<double> >::iterator i = lines.begin(); i!=lines.end(); i++){
    while (i->size() > points){
      // points ��������� �����
      Line<double> newline;
      newline.insert(newline.begin(), i->rbegin(), i->rbegin()+points);
      lines.push_back(newline);
      i->resize(i->size()-points+1);
    }
  }
  return lines;
}

// ������ �� ����� ��������� �����, ���, ����� �����
// �� ���������� �� ��������� ��������� ����� ��� �� e
std::list<Line<double> > generalize (std::list<Line<double> >, double e){

}

