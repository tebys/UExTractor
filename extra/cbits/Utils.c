#include "tree_sitter/api.h"
#include <string.h>

typedef struct {
  char **values;
  int cant;
} QueryResult;

int result_size(QueryResult *qr){
  return qr->cant;
}

char **result_values(QueryResult *qr){
  return qr->values;
}

void free_result(QueryResult *qr){
  for(int i = 0; i < qr->cant; i++)
    free(qr->values[i]);
  free(qr->values);
  free(qr);
}

QueryResult *do_query(const char *code, const char *query){
  QueryResult *out = malloc(sizeof(QueryResult));
  out->values = malloc(sizeof(char*));
  uint32_t *a = malloc(sizeof(a));
  TSQueryError *e = malloc(sizeof(e));

  const TSLanguage *lang = tree_sitter_cpp(); //FREE?
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, lang);
  TSQuery *tsQuery = ts_query_new(lang, query, strlen(query), a, e);
  TSQueryCursor *cursor = ts_query_cursor_new();

  TSTree *tree = ts_parser_parse_string(parser, NULL, code, strlen(code));
  TSNode node = ts_tree_root_node(tree);
  ts_query_cursor_exec(cursor, tsQuery, node);

  TSQueryMatch *match = malloc(sizeof(match));
  bool seguir = ts_query_cursor_next_match(cursor, match);
  int cant = 0;
  while (seguir){
    TSQueryCapture *captures = match->captures;
    for(size_t i = 0; i < match->capture_count; i++){
	  TSNode ni = captures[i].node;
      uint32_t si = ts_node_start_byte(ni);
      uint32_t ei = ts_node_end_byte(ni);
      uint32_t cl = ei - si;
      out->values = realloc(out->values, sizeof(char*)*(cant+1));
      out->values[cant] = malloc(sizeof(char)*cl);
      strncpy(out->values[cant], code+si, cl);
      cant++;
    }
    seguir = ts_query_cursor_next_match(cursor, match);
  }

  free(match);
  ts_tree_delete(tree);
  ts_query_cursor_delete(cursor);
  ts_query_delete(tsQuery);
  ts_parser_delete(parser);

  out->cant = cant;
  return out;
}
