#ifndef YAML_H
#define YAML_H

#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <syck.h>

using namespace std;


// �������� �� ��� ��� � ����� YAML::Parser...
// ����� �� ����������.

namespace YAML {

   // ���� ���������
   typedef enum { STRING, ARRAY, HASH } Type;

   // ��� "������ ���������"
   struct Node;
   typedef vector<Node> NodeList;

   // �������
   struct Node {
      // ������������ ���� ��� -- ������
      string value;
      // ������������ ���� ��� -- ������ ��� ���
      NodeList value_array;
      // ������������ ��� �������� ������ ����. ��������������� ��������
      // ����� � ��� �� �������� � value_array
      NodeList key_array;
      // ��� ��������
      Type type;
      // ���������� ������ �� 0 �� key_array.size()-1 ���������������
      // ����� �� ������. ���� �� ������� ���������� -1.
      int key_index (const char *str) {
         for (NodeList::size_type i=0; i<key_array.size(); i++) {
            if (key_array[i].type == STRING && key_array[i].value == str)
               return i;
         }
         return -1;
      }
      // ���������� ������-�������� �� ������-�����. ��� ������
      // ���������� NULL.
      const char *hash_str_value(const char *str) {
         int k = key_index(str);
         if (k == -1) return NULL;
         else {
            Node v = value_array[k];
            if (v.type != STRING) return NULL;
            else return v.value.c_str();
         }
      }
   };

   // ���������� �������� ��� Syck
   SYMID node_handler(SyckParser *p, SyckNode *n );
   // ���������� ������ ��� Syck
   void error_handler(SyckParser *p, char *str);

   Node  &parse(FILE *file);

   // ������ ���������
   extern NodeList nodes;
   // ��������� ������� �� ������
   extern string error;

};
#endif
