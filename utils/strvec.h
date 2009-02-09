#ifndef STRVEC_H
#define STRVEC_H

#include <iostream>
#include <vector>
#include <string>

/*
<< and >> operators for vector<string>
'\n' is protected by '\\'
*/
// �� ���� ������, �� ��� typedef-� lexical_cast �� ������� ����������
struct StrVec : std::vector<std::string> {};

std::ostream & operator<< (std::ostream & s, const StrVec & v);
std::istream & operator>> (std::istream & s, StrVec & v);

#endif /* STRVEC_H */
