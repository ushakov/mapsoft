#ifndef LINE_UTILS_H
#define LINE_UTILS_H

#include "line.h"
#include "point_utils.h"

//������������� ������������� � ����� �� 4 �����
template<typename T>
Line<T> rect2line(const Rect<T> & r){
  Line<T> ret;
  ret.push_back(r.TLC());
  ret.push_back(r.TRC());
  ret.push_back(r.BRC());
  ret.push_back(r.BLC());
  return  ret;
}

// ��������� ����� �� ���� a ������ ����� p0.
template<typename T>
void lrotate(std::list<Line<T> > & lines, const T a, const Point<T> & p0 = Point<T>(0,0)){
  double c = cos(a);
  double s = sin(a);
  for (typename std::list<Line<T> >::iterator l = lines.begin(); l!=lines.end(); l++){
    for (typename Line<T>::iterator p = l->begin(); p!=l->end(); p++){
      double x = p->x-p0.x, y = p->y-p0.y;
      p->x = (T)(x*c - y*s + p0.x);
      p->y = (T)(x*s + y*c + p0.y);
    }
  }
}

// ���������� �����, ���� �� ����� ����� e
template<typename T>
void merge (std::list<Line<T> > & lines, T e){

  //������� ������ ������� �����
  for (typename std::list<Line<T> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
    for (typename std::list<Line<T> >::iterator i2 = i1; i2!=lines.end(); i2++){
      if (i1==i2) continue;
      if ((*i1==*i2) || (i1->isinv(*i2))){
        lines.erase(i2);
        i1 = lines.erase(i1); i1--;
        i2 = i1;
      }
    }
  }
  for (typename std::list<Line<T> >::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
    for (typename std::list<Line<T> >::iterator i2 = i1; i2!=lines.end(); i2++){
      if (i1==i2) continue;
      Line<T> tmp;
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
}

// ��������� ����� �� ���������, ������ �� ����� points �����
template<typename T>
void split (std::list<Line<T> > & lines, int points){

  if (points < 2) return;
  for (typename std::list<Line<T> >::iterator i = lines.begin(); i!=lines.end(); i++){
    while (i->size() > points){
      // points ��������� �����
      Line<T> newline;
      newline.insert(newline.begin(), i->rbegin(), i->rbegin()+points);
      lines.push_back(newline);
      i->resize(i->size()-points+1);
    }
  }
}

// ������ �� ����� ��������� �����, ���, ����� �����
// �� ���������� �� ��������� ��������� ����� ��� �� e
template<typename T>
void generalize (std::list<Line<T> > & lines, double e){
  for (typename std::list<Line<T> >::iterator l = lines.begin(); l!=lines.end(); l++){

    // ����� ����� �� ����� ���������:
    std::vector<bool> skip(l->size(),false);

    while (l->size()>2){
      // ��� ������ ����� ������ ���������� �� ��� �� 
      // ������, ����������� ��� �������� (�� �����������) �����.
      // ������ ������� ���� ��������
      double min = 1e99; //�����, ��� >e 
      int mini;
      for (int i=1; i<l->size()-1; i++){
        if (skip[i]) continue;
        int ip, in;
        // ������, ��� skip[0] � skip[l->size()-1] ������ false
        for (ip=i-1; ip>=0; ip--)          if (!skip[ip]) break;
        for (in=i+1; in<l->size()-1; in++) if (!skip[in]) break;
        Point<T> p1 = (*l)[ip];
        Point<T> p2 = (*l)[i];
        Point<T> p3 = (*l)[in];
        double ll = pdist(p3-p1);
        Point<double> v = (p3-p1)/ll;
        double prj = pscal(v, p2-p1);
        double dp;
        if      (prj<=0)  dp = pdist(p2,p1);
        else if (prj>=ll) dp = pdist(p2,p3);
        else              dp = pdist(p2-p1-v*prj);
        // � ������ � ��� ���������� i==1
        if ((i==1) || (min>dp)) {min = dp; mini=i;}
      }
      // ���� ���� ������� ������ e - ������� �����
      if (min<e) skip[mini]=true;
      else break;
    }

    // ������� ����� �����
    Line<T> newl;
    for (int i = 0; i<l->size(); i++) if (!skip[i]) newl.push_back((*l)[i]);
    l->swap(newl);

    // ���� �������� ����� �� ���� ������� ����� - ������ ��.
    if ((l->size() == 2) && (pdist((*l)[0],(*l)[1]) < e)){
      l=lines.erase(l); l--;
    }
  }

}

#endif
