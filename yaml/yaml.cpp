#include "yaml.h"

using namespace std;
namespace YAML {

   // Обработчик элемента для Syck
   SYMID node_handler( SyckParser *p, SyckNode *n ) {
      static int id = 0;
      Node node;

      if (n->kind == syck_str_kind) {
         node.type = STRING;
         node.value = n->data.str->ptr;
      } else if (n->kind == syck_seq_kind) {
         node.type = ARRAY;
         for (int i = 0; i < n->data.list->idx; i++)
            node.value_array.push_back(nodes[n->data.list->items[i]]);
      } else if (n->kind == syck_map_kind) {
         node.type = HASH;
         for (int i = 0; i < n->data.pairs->idx; i++) {
            node.key_array.push_back(nodes[n->data.pairs->keys[i]]);
            node.value_array.push_back(nodes[n->data.pairs->values[i]]);
         }
      }

      nodes.push_back(node);

      return id++;
   }

   // Обработчик ошибки для Syck
   void error_handler(SyckParser *p, char *str) {
      error = str;
   }

   // Возвращает корень
   // В случае ошибке в YAML::error оказывается сообщение об ошибке
   Node & parse(FILE *file) {

      SyckParser *parser = syck_new_parser();
      syck_parser_handler(parser, node_handler);
      syck_parser_error_handler(parser, error_handler);
      syck_parser_file(parser, file, NULL);
      error = "";
      SYMID root_id = syck_parse(parser);
      syck_free_parser(parser);

      return nodes[root_id];
   }
   
   NodeList nodes;
   string error;

};
