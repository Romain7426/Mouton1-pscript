#ifndef PSCRIPT_PARSER_H
#define PSCRIPT_PARSER_H


enum { 
  //PSCRIPT_EXPR_TREE__TYPE__BASE = PSCRIPT_TOKEN_COUNT - 1,//42, //PSCRIPT_TOKEN__TYPE__COUNT - 1, // RL: TODO XXX FIXME 
  PSCRIPT_EXPR_TREE__TYPE__BASE = PSCRIPT_TOKEN__SYNTAX_KEYWORD__TOP - 1, 
  //PSCRIPT_EXPR_TREE__TYPE__IOPPOSITE, 
  //PSCRIPT_EXPR_TREE__TYPE__ROPPOSITE, 
  PSCRIPT_EXPR_TREE__TYPE__FUNCALL, 
  PSCRIPT_EXPR_TREE__TYPE__LAMBDA, 
  PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 
  PSCRIPT_EXPR_TREE__TYPE__COUNT_SHIFTED 
}; 
enum { PSCRIPT_EXPR_TREE__TYPE__COUNT = PSCRIPT_EXPR_TREE__TYPE__COUNT_SHIFTED - PSCRIPT_EXPR_TREE__TYPE__BASE - 1 }; 
 
enum { 
  PSCRIPT_TYPE_TREE__TYPE__BASE = PSCRIPT_EXPR_TREE__TYPE__COUNT_SHIFTED - 1, //PSCRIPT_TOKEN_COUNT - 1,//42, //PSCRIPT_TOKEN__TYPE__COUNT - 1, // RL: TODO XXX FIXME 
  PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_CONST, 
  PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_EXPR, 
  PSCRIPT_TYPE_TREE__TYPE__ARRAY, 
  PSCRIPT_TYPE_TREE__TYPE__RECORD, 
  PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_NIL, 
  PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS, 
  PSCRIPT_TYPE_TREE__TYPE__FUNPROC, 
  PSCRIPT_TYPE_TREE__TYPE__METHODE, 
  PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL, 
  PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, 
  PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, 
  PSCRIPT_TYPE_TREE__TYPE__COUNT_SHIFTED 
}; 
enum { PSCRIPT_TYPE_TREE__TYPE__COUNT = PSCRIPT_TYPE_TREE__TYPE__COUNT_SHIFTED - PSCRIPT_TYPE_TREE__TYPE__BASE - 1 }; 

enum { 
  PSCRIPT_INST_LIST__TYPE__BASE = PSCRIPT_TYPE_TREE__TYPE__COUNT_SHIFTED - 1, //PSCRIPT_TOKEN_COUNT - 1,//42, //PSCRIPT_TOKEN__TYPE__COUNT - 1, // RL: TODO XXX FIXME 
  PSCRIPT_INST_LIST__TYPE__LIST__NIL, 
  PSCRIPT_INST_LIST__TYPE__LIST__CONS, 
  PSCRIPT_INST_LIST__TYPE__BEGIN, 
  PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE, 
  PSCRIPT_INST_LIST__TYPE__IF_ELSE, 
  PSCRIPT_INST_LIST__TYPE__WHEN, 
  PSCRIPT_INST_LIST__TYPE__UNLESS, 
  PSCRIPT_INST_LIST__TYPE__WHILE, 
  PSCRIPT_INST_LIST__TYPE__FOR, 
  PSCRIPT_INST_LIST__TYPE__REPEAT, 
  PSCRIPT_INST_LIST__TYPE__EXPR, 
  PSCRIPT_INST_LIST__TYPE__COUNT_SHIFTED 
}; 
enum { PSCRIPT_INST_LIST__TYPE__COUNT = PSCRIPT_INST_LIST__TYPE__COUNT_SHIFTED - PSCRIPT_INST_LIST__TYPE__BASE - 1 }; 

enum { 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__BASE = PSCRIPT_INST_LIST__TYPE__COUNT_SHIFTED - 1, //PSCRIPT_TOKEN_COUNT - 1,//42, //PSCRIPT_TOKEN__TYPE__COUNT - 1, // RL: TODO XXX FIXME 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE, 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL, 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS, 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, 
  PSCRIPT_DECL_LOCAL_LIST__TYPE__COUNT_SHIFTED 
}; 
enum { PSCRIPT_DECL_LOCAL_LIST__TYPE__COUNT = PSCRIPT_DECL_LOCAL_LIST__TYPE__COUNT_SHIFTED - PSCRIPT_DECL_LOCAL_LIST__TYPE__BASE - 1 }; 

enum { 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__BASE = PSCRIPT_DECL_LOCAL_LIST__TYPE__COUNT_SHIFTED - 1, //PSCRIPT_TOKEN_COUNT - 1,//42, //PSCRIPT_TOKEN__TYPE__COUNT - 1, // RL: TODO XXX FIXME 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD, 
  PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__COUNT_SHIFTED 
}; 
enum { PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__COUNT = PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__COUNT_SHIFTED - PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__BASE - 1 }; 

enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__BASE = PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__COUNT_SHIFTED - 1 }; 
enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__SIZE = 31 }; 
enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__TOP  = PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__BASE + PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__SIZE }; 

enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE = PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__TOP }; 
enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__SIZE = 33 }; //126 }; 
enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__TOP  = PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE + PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__SIZE }; 


#ifndef PSCRIPT_PARSER_ENV_T 
#define PSCRIPT_PARSER_ENV_T 
struct pscript_parser_env_t; 
typedef struct pscript_parser_env_t pscript_parser_env_t; 
#endif 
extern const int32_t pscript_parser_env__sizeof; 

extern pscript_parser_env_t * pscript_parser_env__make(void); 
extern void pscript_parser_env__delete(pscript_parser_env_t * this); 

extern int pscript_parser__parenthesis__check(const pscript_token_env_t * token_env); 
extern int pscript_parser__automaton(pscript_parser_env_t * this, const pscript_token_env_t * token_env); 

extern void pscript_parser__print_all(const int filedes, const pscript_parser_env_t * this); 
extern void pscript_parser__print_stats(const int filedes, const pscript_parser_env_t * this); 

extern void pscript_parser__print_pretty(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this); 


extern const pscript_chunk_env_t * pscript_parser__get__expr_env(const pscript_parser_env_t * this); 
extern const pscript_chunk_env_t * pscript_parser__get__type_env(const pscript_parser_env_t * this); 
extern const pscript_chunk_env_t * pscript_parser__get__inst_env(const pscript_parser_env_t * this); 
extern const pscript_chunk_env_t * pscript_parser__get__decl_local_env(const pscript_parser_env_t * this); 
extern const pscript_chunk_env_t * pscript_parser__get__decl_toplevel_env(const pscript_parser_env_t * this); 


extern const int pscript_parser__postfix_operators[]; 
extern const int pscript_parser__postfix_operators__nb;


extern int pscript_parser__custom_syntax__node_push(pscript_parser_env_t * this); 
extern int pscript_parser__custom_syntax__node_count(const pscript_parser_env_t * this); 

extern int pscript_parser__custom_syntax__rule_push_array_nb(pscript_parser_env_t * this, const int node_id, const int right_len, const int right_items[]); 
#define    pscript_parser__custom_syntax__rule_push_array(this,node_id,...) pscript_parser__custom_syntax__rule_push_array_nb(this, node_id, ARRAY_SIZE(((const int []) { __VA_ARGS__ })), ((const int []) { __VA_ARGS__ })) 
extern int pscript_parser__custom_syntax__rule_count(const pscript_parser_env_t * this); 
extern int pscript_parser__custom_syntax__rule_lookup(const pscript_parser_env_t * this, const int node_id, const int token_i); 

extern void pscript_parser__custom_syntax__tree_set__array_nb(pscript_parser_env_t * this, const int rule_id, const int tree_type, const int items_len, const int items_indices[]); 
#define     pscript_parser__custom_syntax__tree_set__array(this,rule_id,tree_type,...) pscript_parser__custom_syntax__tree_set__array_nb(this, rule_id, tree_type, ARRAY_SIZE(((const int []) { __VA_ARGS__ })), ((const int []) { __VA_ARGS__ })) 
extern int  pscript_parser__custom_syntax__tree_count(const pscript_parser_env_t * this); 


extern void pscript_parser__custom_syntax__master_root_set(pscript_parser_env_t * this, const int token_master_type_id, const int rule_id); 



#if 0 
#ifndef PSCRIPT_PARSER_ENV_T 
#define PSCRIPT_PARSER_ENV_T 
struct pscript_parser_env_t; 
typedef struct pscript_parser_env_t pscript_parser_env_t; 
#endif 
extern pscript_parser_env_t * pscript_parser_env__make(void); 
extern void pscript_parser_env__delete(pscript_parser_env_t * this); 


extern int pscript_parser__push_no_lookup(pscript_parser_env_t * this, const char * cstr); 
extern int pscript_parser__push_no_lookup__len(pscript_parser_env_t * this, const char * cstr, const int cstr_len); 
extern const char * pscript_parser__get_cstr(const pscript_parser_env_t * this, const int str_index); 
extern int pscript_parser__get_len(const pscript_parser_env_t * this, const int str_index); 
#define pscript_parser__strlen pscript_parser__get_len 
extern int pscript_parser__lookup(const pscript_parser_env_t * this, const char * cstr); 
extern int pscript_parser__lookup__len(const pscript_parser_env_t * this, const char * cstr, const int cstr_len); 
extern int pscript_parser__push_or_lookup(pscript_parser_env_t * this, const char * cstr); 
extern int pscript_parser__push_or_lookup__len(pscript_parser_env_t * this, const char * cstr, const int cstr_len); 
#define pscript_parser__push      pscript_parser__push_or_lookup
#define pscript_parser__push__len pscript_parser__push_or_lookup__len
#endif 



#endif /* PSCRIPT_PARSER_H */
