#include "pscript_global.h"
#include "pscript_token_type.h"
#include "pscript.h"
#include "pscript_chunk.h"
#include "pscript_parser.h"
#include "pscript_token_type.h"
#include "pscript_parser.ci"
#include "pscript_token.h"

#define DEBUG_PARSER 1 

const int pscript_parser__postfix_operators[] = { PSCRIPT_TOKEN_INC, PSCRIPT_TOKEN_DEC }; 
const int pscript_parser__postfix_operators__nb = ARRAY_SIZE(pscript_parser__postfix_operators); 

#ifndef PSCRIPT_PARSER_ENV_T 
#define PSCRIPT_PARSER_ENV_T 
struct pscript_parser_env_t; 
typedef struct pscript_parser_env_t pscript_parser_env_t; 
#endif 

//enum { pscript_parser_env__memory__array_size = 5 * (sizeof(pscript_chunk_env_t) + pscript_chunk__memory__array_size__default) }; 
enum { pscript_parser_env__memory__expr_tree__array_size = (1 << 14) }; 
enum { pscript_parser_env__memory__type_tree__array_size = (1 << 13) }; 
enum { pscript_parser_env__memory__inst_list__array_size = (1 << 13) }; 
enum { pscript_parser_env__memory__decl_local__array_size = (1 << 13) }; 
enum { pscript_parser_env__memory__decl_top_level__array_size = (1 << 13) }; 
enum { pscript_parser_env__memory__array_size = 
    5 * sizeof(pscript_chunk_env_t) + 
   pscript_parser_env__memory__expr_tree__array_size + 
   pscript_parser_env__memory__type_tree__array_size + 
   pscript_parser_env__memory__inst_list__array_size + 
   pscript_parser_env__memory__decl_local__array_size + 
   pscript_parser_env__memory__decl_top_level__array_size }; 

enum { PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX =  12 }; 
struct pscript_parser_env_t {
  uint16_t memory__array_nb;   // RL: Top of the array. 
  char     memory__array[pscript_parser_env__memory__array_size]; 
  
  pscript_chunk_env_t * expr_tree_env; 
  pscript_chunk_env_t * type_tree_env; 
  pscript_chunk_env_t * inst_list_env; 
  pscript_chunk_env_t * decl_local; 
  pscript_chunk_env_t * decl_toplevel; 
  
  // CUSTOM SYNTAX 
  uint8_t custom_syntax_node__nb; 
  uint8_t custom_syntax_rules__array[PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__SIZE][3 + PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX]; 
  uint8_t custom_syntax_rules__nb; 
  uint8_t custom_syntax_trees__nb; 
  uint8_t custom_syntax_trees__array[PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__SIZE][2 + PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX]; 
  uint8_t custom_syntax_root__array[PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE]; 
}; 
const        int32_t pscript_parser_env__sizeof                  = sizeof(pscript_parser_env_t); 
enum {               pscript_parser_env__sizeof__compiler_const  = sizeof(pscript_parser_env_t) }; 

static void assert_compile__dkus4uf238kjhsdfr(void) {
  ASSERT_COMPILE(pscript_parser_env__memory__array_size <= UINT16_MAX); 
  ASSERT_COMPILE(PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__TOP <= UINT8_MAX); 
}; 

extern int pscript_parser__custom_syntax__node_push(pscript_parser_env_t * this) { 
  assert(this -> custom_syntax_node__nb < PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__SIZE); 
  const int node_id = PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__BASE + this -> custom_syntax_node__nb; 
  this -> custom_syntax_node__nb ++; 
  return node_id; 
}; 

int pscript_parser__custom_syntax__node_count(const pscript_parser_env_t * this) { 
  return this -> custom_syntax_node__nb; 
}; 

int pscript_parser__custom_syntax__rule_push_array_nb(pscript_parser_env_t * this, const int node_id, const int right_len, const int right_items[]) { 
  assert(this -> custom_syntax_rules__nb < PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__SIZE); 
  const int index   = this -> custom_syntax_rules__nb; 
  const int rule_id = PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE + index; 
  this -> custom_syntax_rules__nb ++; 
  
  assert(right_len <= PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX);  

  // rule_id, left_symbol, right_length, right_symbols... 
  //this -> custom_syntax_rules__array[index] = (const int8_t[]) { rule_id, node_id, right_len }; 
  this -> custom_syntax_rules__array[index][0] = rule_id; 
  this -> custom_syntax_rules__array[index][1] = node_id; 
  this -> custom_syntax_rules__array[index][2] = right_len; 
  for (int i = 0; i < right_len; i++) { 
    this -> custom_syntax_rules__array[index][3 + i] = right_items[i]; 
  }; 
  //[3 + PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX + 1]; 
  
  return rule_id; 
}; 
 
int pscript_parser__custom_syntax__rule_count(const pscript_parser_env_t * this) { 
  return this -> custom_syntax_rules__nb; 
}; 

int pscript_parser__custom_syntax__rule_lookup(const pscript_parser_env_t * this, const int node_id, const int token_i) { 
  int epsilon_rule = -1; // RL: The rule «N → ε», if any. 
  int subexpr_rule = -1; // RL: A rule «N → S ...», if any. 
  for (int i = 0; i < this -> custom_syntax_rules__nb; i++) { 
    if (this -> custom_syntax_rules__array[i][1] != node_id) { continue; }; 
    if (this -> custom_syntax_rules__array[i][2] == 0) { epsilon_rule = this -> custom_syntax_rules__array[i][0]; continue; }; 
    if (this -> custom_syntax_rules__array[i][3] == 0) { subexpr_rule = this -> custom_syntax_rules__array[i][0]; continue; }; 
    if (this -> custom_syntax_rules__array[i][3] != token_i) { continue; }; 
    // RL: Found. 
    return this -> custom_syntax_rules__array[i][0];
  }; 
  return subexpr_rule < 0 ? epsilon_rule : subexpr_rule; 
}; 


extern void pscript_parser__custom_syntax__master_root_set(pscript_parser_env_t * this, const int token_master_type_id, const int rule_id) { 
  const int index = token_master_type_id - PSCRIPT_TOKEN__SYNTAX_MASTER__BASE; 
  this -> custom_syntax_root__array[index] = rule_id - PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE; 
}; 

extern void pscript_parser__custom_syntax__tree_set__array_nb(pscript_parser_env_t * this, const int rule_id, const int tree_type, const int items_len, const int items_indices[]) { 
  assert(rule_id >= PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE); 
  assert(rule_id <  PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__TOP ); 
  const int index = rule_id - PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE; 
  
  assert(items_len <= PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__RIGHT_LENGTH_MAX);  
  const int rule_right_len = this -> custom_syntax_rules__array[index][2]; 
  for (int i = 0; i < items_len; i++) { 
    assert(items_indices[i] < rule_right_len); 
  }; 

  // tree_type, items_len, items_indices... 
  this -> custom_syntax_trees__array[index][0] = tree_type; 
  this -> custom_syntax_trees__array[index][1] = items_len; 
  for (int i = 0; i < items_len; i++) { 
    this -> custom_syntax_trees__array[index][2 + i] = items_indices[i]; 
  }; 
  //[3 + PSCRIPT_PARSER__CUSTOM_SYNTAX_TREE__RIGHT_LENGTH_MAX + 1]; 
}; 
 
int pscript_parser__custom_syntax__tree_count(const pscript_parser_env_t * this) { 
  return this -> custom_syntax_trees__nb; 
}; 


pscript_parser_env_t * pscript_parser_env__make(void) {
  MALLOC_BZERO(pscript_parser_env_t,this); 
  
  pscript_chunk_env_t * * chunk_env[5] = { 
    &this -> expr_tree_env, 
    &this -> type_tree_env, 
    &this -> inst_list_env, 
    &this -> decl_local, 
    &this -> decl_toplevel, 
  }; 
  static const int mem_size[5] = { 
    pscript_parser_env__memory__expr_tree__array_size, 
    pscript_parser_env__memory__type_tree__array_size, 
    pscript_parser_env__memory__inst_list__array_size, 
    pscript_parser_env__memory__decl_local__array_size, 
    pscript_parser_env__memory__decl_top_level__array_size
  }; 

  int buffer_used_bytes; 
  int buffer_size = sizeof(this -> memory__array); 
  char * buffer = this -> memory__array; 
  for (int i = 0; i < 5; i++) { 
    const int ms = mem_size[i]; 
    *(chunk_env[i]) = pscript_chunk_env__make_b(/*data_memory_size*/ms, buffer_size, buffer, &buffer_used_bytes); 
    buffer += buffer_used_bytes; 
    buffer_size -= buffer_used_bytes; 
    this -> memory__array_nb += buffer_used_bytes; 
  }; 
  
  assert(0 == buffer_size); 
  
  return this; 
}; 
 
void pscript_parser_env__delete(pscript_parser_env_t * this) { 
  free(this); 
}; 


const pscript_chunk_env_t * pscript_parser__get__expr_env(const pscript_parser_env_t * this) { 
  return this -> expr_tree_env; 
}; 
 
const pscript_chunk_env_t * pscript_parser__get__type_env(const pscript_parser_env_t * this) { 
  return this -> type_tree_env; 
}; 
 
const pscript_chunk_env_t * pscript_parser__get__inst_env(const pscript_parser_env_t * this) { 
  return this -> inst_list_env; 
}; 
 
const pscript_chunk_env_t * pscript_parser__get__decl_local_env(const pscript_parser_env_t * this) { 
  return this -> decl_local; 
}; 
 
const pscript_chunk_env_t * pscript_parser__get__decl_toplevel_env(const pscript_parser_env_t * this) { 
  return this -> decl_toplevel; 
}; 
 






// *** PARSER *** 
 

enum { 
  PSCRIPT_PARSER__AUTOMATON_STATE__NULL, 
  PSCRIPT_PARSER__AUTOMATON_STATE__START, 
  declaration__top_level, 
  decl_constante, 
  decl_type__top_level, 
  decl_variable__top_level, 
  decl_procedure, 
  decl_main, 
  decl_methode, 
  expr_lambda, 
  expr, 
  type_expr, 
  record_fields, 
  decl__arg_list, 
  decl__arg_list__inner, 
  decl__arg_one, 
  decl__local_var, 
  decl__local_var__one, 
  instructions__begin, 
  instructions__one, 
  instructions__if, 
  instructions__when, 
  instructions__unless, 
  instructions__while, 
  instructions__repeat, 
  instructions__for, 
  instructions__expr, 
  PSCRIPT_PARSER__AUTOMATON_STATE__COUNT 
}; 

static const char * pscript_parser_automaton_state__cstr[] = {
  "PSCRIPT_PARSER__AUTOMATON_STATE__NULL", 
  "PSCRIPT_PARSER__AUTOMATON_STATE__START", 
  "declaration__top_level", 
  "decl_constante", 
  "decl_type__top_level", 
  "decl_variable__top_level", 
  "decl_procedure", 
  "decl_main", 
  "decl_methode", 
  "expr_lambda", 
  "expr", 
  "type_expr", 
  "record_fields", 
  "decl__arg_list", 
  "decl__arg_list__inner", 
  "decl__arg_one", 
  "decl__local_var", 
  "decl__local_var__one", 
  "instructions__begin", 
  "instructions__one", 
  "instructions__if", 
  "instructions__when", 
  "instructions__unless", 
  "instructions__while", 
  "instructions__repeat", 
  "instructions__for", 
  "instructions__expr", 
  NULL
}; 

enum { pscript_parser_automaton_state__size = ARRAY_SIZE(pscript_parser_automaton_state__cstr) }; 
static void assert_compile__dkushdsuf2387645kjhsdfr(void) {
  ASSERT_COMPILE(PSCRIPT_PARSER__AUTOMATON_STATE__COUNT <= INT8_MAX); //127); // 256 // INT8_MAX 
  ASSERT_COMPILE(pscript_parser_automaton_state__size == PSCRIPT_PARSER__AUTOMATON_STATE__COUNT + 1); 
}; 

const char * pscript_parser_automaton_state__get_cstr(const int automaton_state) { 
  return pscript_parser_automaton_state__cstr[automaton_state]; 
}; 


enum { decl__arg_ident_list__size = 15 };  






static int pscript_parser__automaton__get_next_token(const pscript_token_env_t * token_env, int * current_token_ref) { 
  *current_token_ref += 1; 
  ASSERT_DEBUG_ABORT_IF_TRUE(*current_token_ref < 0) { return PSCRIPT_TOKEN_NULL; }; 
  //if (*current_token_ref >= token_env -> nb) { return PSCRIPT_TOKEN_EOF; }; 
  if (*current_token_ref >= pscript_token__get_count(token_env)) { return PSCRIPT_TOKEN_EOF; }; 
  //return token_env -> type[*current_token_ref]; 
  return pscript_token__get_type(token_env, *current_token_ref); 
}; 


#define CASE_GOTO(id) case id: goto glue(label__state,id); break;  
#define LABEL_STATE(id) glue(label__state,id) 

//int yypscriptlex(void);
//#define yypscriptlex()         pscript_parser__automaton__get_next_token(token_env, &current_token)
#define yypscriptlex()           pscript_parser__automaton__get_next_token(token_env, current_token_ref) 
#define get_current_token_type() (*current_token_ref < 0 ? PSCRIPT_TOKEN_NULL : *current_token_ref >= pscript_token__get_count(token_env) ? PSCRIPT_TOKEN_EOF : pscript_token__get_type(token_env, *current_token_ref)) 
#if DEBUG_PARSER >= 2 
#define get_next_token_type()    (dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">", ":<get_next_token>", "\n"), pscript_parser__automaton__get_next_token(token_env, current_token_ref)) 
#else 
#define get_next_token_type()    pscript_parser__automaton__get_next_token(token_env, current_token_ref) 
#endif 

      




// *** PARSER *** 

#define EXCEPTION__RAISE__(msg)						\
  strlcpy(exception_data + 4 + sizeof(sigjmp_buf), msg, *((int16_t*)exception_data) - 4 - sizeof(sigjmp_buf)); \
  siglongjmp(*((sigjmp_buf *) (exception_data + 4)), -1); 

#define EXCEPTION__RAISE(...) {						\
  const char * msg = strconcat__stack(__VA_ARGS__);			\
  strlcpy(exception_data + 4 + sizeof(sigjmp_buf), msg, *((int16_t*)exception_data) - 4 - sizeof(sigjmp_buf)); \
  siglongjmp(*((sigjmp_buf *) (exception_data + 4)), -1);		\
  }; 
 
#define SYNTAX__EXPECTING_THIS_TOKEN__OLD(given_token_type,expected_token_type) { \
    if (given_token_type != expected_token_type) {			\
      EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting token: ", pscript_token__type_get_cstr(expected_token_type), "(", int_string__stack(expected_token_type), ")"); \
    };									\
  }; 

#if DEBUG_PARSER >= 1 
#define SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION "{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "] }: ",
#else 
#define SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION 
#endif 

#define SYNTAX__EXPECTING_THIS_TOKEN(given_token_type,expected_token_type) { \
  if (given_token_type != expected_token_type) {			\
  const int    token_i      = *current_token_ref;				\
  const int    token_type   = pscript_token__get_type(token_env, token_i);	\
  const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); \
  const char * srcval_cstr  = pscript_token__get_srcval_cstr(token_env, token_i); \
  const int    line1        = pscript_token__get_line1(token_env, token_i);	\
  const int    column0      = pscript_token__get_column0(token_env, token_i);	\
  const char * msg_array[]  = {						\
    SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION  \
    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
    "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting token: ", pscript_token__type_get_cstr(expected_token_type), "(", int_string__stack(expected_token_type), ")" }; \
  const char * msg          = strconcat__stack_nb(ARRAY_SIZE(msg_array), msg_array);			\
  EXCEPTION__RAISE__(msg);					\
  };									\
  }; 

#define SYNTAX__EXPECTING_NOT_THIS_TOKEN(unexpected_token_type) {	\
    const int    token_i      = *current_token_ref;			\
    const int    token_type   = pscript_token__get_type        (token_env, token_i); \
    const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); \
    const char * srcval_cstr  = pscript_token__get_srcval_cstr (token_env, token_i); \
    const int    line1        = pscript_token__get_line1       (token_env, token_i); \
    const int    column0      = pscript_token__get_column0     (token_env, token_i); \
    const char * msg_array[]  = {					\
      SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION \
      srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
      "Unexpected token: ", pscript_token__type_get_cstr(unexpected_token_type), "(", int_string__stack(unexpected_token_type), ") -- Was expecting another token." }; \
    const char * msg          = strconcat__stack_nb(ARRAY_SIZE(msg_array), msg_array); \
    EXCEPTION__RAISE__(msg);						\
  }; 



//       const char * msg_array[msg_array_nb] = { __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting of these tokens:" }; 


#define SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS__OLD(given_token_type, ...) {	\
    if (not(int_member_huh(given_token_type, __VA_ARGS__))) {		\
      const int expected_tokens_array[] = (const int []) { __VA_ARGS__ }; \
      const int expected_tokens_nb      = ARRAY_SIZE(((const int []) { __VA_ARGS__ })); \
      const char * prefix_msg_array[] = { __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting one of these ", int_string__stack(expected_tokens_nb), " tokens: " }; \
      const int prefix_msg_array_nb = ARRAY_SIZE(prefix_msg_array);	\
      const int msg_array_nb = prefix_msg_array_nb + 5*expected_tokens_nb; \
      const char * msg_array[msg_array_nb];				\
      bcopy(prefix_msg_array, msg_array, sizeof(prefix_msg_array));	\
      for (int i = 0; i < expected_tokens_nb; i++) {			\
	const char * * p = msg_array + prefix_msg_array_nb + 5*i;	\
	*(p + 0) = pscript_token__type_get_cstr(expected_tokens_array[i]); \
	*(p + 1) = "(";							\
	*(p + 2) = int_string__stack(expected_tokens_array[i]);		\
	*(p + 3) = ")";							\
	*(p + 4) = i == expected_tokens_nb-1 ? "" : ", ";		\
      };								\
      const char * msg = strconcat__stack_nb(msg_array_nb, msg_array);	\
      EXCEPTION__RAISE__(msg);						\
    };									\
  }; 


#define SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(given_token_type, ...) {	\
    if (not(int_member_huh(given_token_type, __VA_ARGS__))) {		\
      const int expected_tokens_array[] = (const int []) { __VA_ARGS__ }; \
      const int expected_tokens_nb      = ARRAY_SIZE(((const int []) { __VA_ARGS__ })); \
      const int token_i = *current_token_ref;				\
      const int token_type = pscript_token__get_type(token_env, token_i); \
      const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); \
      const char * srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); \
      const int line1 = pscript_token__get_line1(token_env, token_i);	\
      const int column0 = pscript_token__get_column0(token_env, token_i); \
      const char * prefix_msg_array[] = {				\
        SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION  \
	srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
	"Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting one of these ", int_string__stack(expected_tokens_nb), " tokens: " }; \
      const int prefix_msg_array_nb = ARRAY_SIZE(prefix_msg_array);	\
      const int msg_array_nb = prefix_msg_array_nb + 5*expected_tokens_nb; \
      const char * msg_array[msg_array_nb];				\
      bcopy(prefix_msg_array, msg_array, sizeof(prefix_msg_array));	\
      for (int i = 0; i < expected_tokens_nb; i++) {			\
	const char * * p = msg_array + prefix_msg_array_nb + 5*i;	\
	*(p + 0) = pscript_token__type_get_cstr(expected_tokens_array[i]); \
	*(p + 1) = "(";							\
	*(p + 2) = int_string__stack(expected_tokens_array[i]);		\
	*(p + 3) = ")";							\
	*(p + 4) = i == expected_tokens_nb-1 ? "" : ", ";		\
      };								\
      const char * msg = strconcat__stack_nb(msg_array_nb, msg_array);	\
      EXCEPTION__RAISE__(msg);						\
    };									\
  }; 



static int pscript_parser__automaton__aux(char * exception_data, pscript_parser_env_t * this, const pscript_token_env_t * token_env, const int state, int * current_token_ref, int *retval1_ref) { 
  int16_t expr_tree = -1; 
  int16_t type_expr_tree = -1; 
  //int current_token = -1; 

#if DEBUG_PARSER >= 2 
  dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], " - current_token: ", int_string__stack(*current_token_ref), "]\n"); 
#endif 

  switch (state) { 
    CASE_GOTO(PSCRIPT_PARSER__AUTOMATON_STATE__START); 
    CASE_GOTO(declaration__top_level); 
    CASE_GOTO(decl_constante); 
    CASE_GOTO(decl_type__top_level); 
    CASE_GOTO(decl_variable__top_level); 
    CASE_GOTO(decl_procedure); 
    CASE_GOTO(decl_methode); 
    CASE_GOTO(decl_main); 
    CASE_GOTO(expr_lambda); 
    CASE_GOTO(expr); 
    CASE_GOTO(type_expr); 
    CASE_GOTO(record_fields); 
    CASE_GOTO(decl__arg_list); 
    CASE_GOTO(decl__arg_list__inner); 
    //CASE_GOTO(decl__arg_one); 
    CASE_GOTO(decl__local_var); 
    CASE_GOTO(decl__local_var__one); 
    CASE_GOTO(instructions__begin); 
    CASE_GOTO(instructions__one); 
    CASE_GOTO(instructions__if); 
    CASE_GOTO(instructions__when); 
    CASE_GOTO(instructions__unless); 
    CASE_GOTO(instructions__while); 
    CASE_GOTO(instructions__repeat); 
    CASE_GOTO(instructions__for); 
    CASE_GOTO(instructions__expr); 
#if 0 
#endif  
  }; 
  assert(false); 
  
  
 LABEL_STATE(PSCRIPT_PARSER__AUTOMATON_STATE__START): { 
    //main: liste_declaration__top_level PSCRIPT_TOKEN_EOF ; 
    int next_token; 
    //next_token = get_current_token_type(); 
    next_token = get_next_token_type(); 
    pscript_parser__automaton__aux(exception_data, this, token_env, declaration__top_level, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_EOF); 
    return 0; 
  }; 
  
 LABEL_STATE(declaration__top_level): { 
    //declaration__top_level: liste_de_pointsvirgules_non_vide | decl_constante | decl_type__top_level | decl_variable__top_level | decl_procedure | decl_main | decl_methode ; 
    int next_token; 
    for (;;) { 
      next_token = get_current_token_type(); 
      while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
#if DEBUG_PARSER >= 10 
      dputs_array(stderr__fileno, "NEXT TOP-LEVEL DECL: current_token = ", int_string__stack(*current_token_ref), " - token_type = ", pscript_token__type_get_cstr(next_token), "(", int_string__stack(next_token), ")", "\n"); 
#endif 
      if (-1 == next_token) { next_token = get_next_token_type(); };  
      switch (next_token) { 
      case PSCRIPT_TOKEN_CONST    : pscript_parser__automaton__aux(exception_data, this, token_env, decl_constante          , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_TYPE     : pscript_parser__automaton__aux(exception_data, this, token_env, decl_type__top_level    , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_VAR      : pscript_parser__automaton__aux(exception_data, this, token_env, decl_variable__top_level, current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_PROCEDURE: pscript_parser__automaton__aux(exception_data, this, token_env, decl_procedure          , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_FUNCTION : pscript_parser__automaton__aux(exception_data, this, token_env, decl_procedure          , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_METHODE  : pscript_parser__automaton__aux(exception_data, this, token_env, decl_methode            , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_BEGIN    : pscript_parser__automaton__aux(exception_data, this, token_env, decl_main               , current_token_ref, retval1_ref); continue; 
      case PSCRIPT_TOKEN_EOF      : return PSCRIPT_TOKEN_EOF; 
      default: { 
	SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_CONST, PSCRIPT_TOKEN_TYPE, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_PROCEDURE, PSCRIPT_TOKEN_FUNCTION, PSCRIPT_TOKEN_METHODE, PSCRIPT_TOKEN_BEGIN, PSCRIPT_TOKEN_EOF); 
	return -1; 
      }; break; 
      }; 
    }; 
    assert(false); 
  }; 


 LABEL_STATE(decl_constante): { 
    //PSCRIPT_TOKEN_IDENT token_equal_or_assign expr_constante PSCRIPT_TOKEN_PTVIRG ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_CONST); 
    do { 
      next_token = get_next_token_type(); 
    } while (next_token == PSCRIPT_TOKEN_CONST); 
    for (;;) { 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
      const int ident_token = *current_token_ref; 
      next_token = get_next_token_type(); 
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_EQUAL, PSCRIPT_TOKEN_AFFECTATION, PSCRIPT_TOKEN_AFFECTATION_SIMPLE, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS); 
      next_token = get_next_token_type(); 
      expr_tree  = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE, ident_token, expr_tree); 
      next_token = get_next_token_type(); 
      if (next_token != PSCRIPT_TOKEN_IDENT) { break; }; 
    }; 
    return 0; 
  }; 



 LABEL_STATE(decl_type__top_level): { 
    //PSCRIPT_TOKEN_IDENT token_equal_or_assign type_expr PSCRIPT_TOKEN_PTVIRG ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_TYPE); 
    do { 
      next_token = get_next_token_type(); 
    } while (next_token == PSCRIPT_TOKEN_TYPE); 
    for (;;) { 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
      const int ident_token = *current_token_ref; 
      next_token = get_next_token_type(); 
      if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
	EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Defining the same underlying type with multiple names does not make much sense. If you actually want to do that, please break down that definition and that gonna be accepted."); 
      }; 
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_EQUAL, PSCRIPT_TOKEN_AFFECTATION, PSCRIPT_TOKEN_AFFECTATION_SIMPLE, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS); 
      next_token = get_next_token_type(); 
      type_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE, ident_token, type_expr_tree); 
      next_token = get_next_token_type(); 
      if (next_token != PSCRIPT_TOKEN_IDENT) { break; }; 
    }; 
    return 0; 
  }; 



 LABEL_STATE(decl_variable__top_level): { 
    //PSCRIPT_TOKEN_IDENT token_equal_or_assign type_expr PSCRIPT_TOKEN_PTVIRG ;
    int next_token; 
    int ident_list_nb = 0; 
    int ident_list_array[decl__arg_ident_list__size]; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_VAR); 
    do { 
      next_token = get_next_token_type(); 
    } while (next_token == PSCRIPT_TOKEN_VAR); 
    for (;;) { 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
      ident_list_nb = 1; 
      ident_list_array[0] = *current_token_ref; 
      next_token = get_next_token_type(); 
      if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
	for (;;) { 
	  do { next_token = get_next_token_type(); } while (next_token == PSCRIPT_TOKEN_VIRGULE); 
	  SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
	  if (ident_list_nb == decl__arg_ident_list__size) { 
	    EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Declaring too many variables at the same time - I can handle at most ", int_string__stack(decl__arg_ident_list__size), " in one declaration. Please break down that declaration (or recompile the parser with a larger stack)."); 
	  }; 
	  ident_list_array[ident_list_nb] = *current_token_ref; 
	  ident_list_nb++; 
	  next_token = get_next_token_type(); 
	  if (next_token != PSCRIPT_TOKEN_VIRGULE) { break; }; 
	}; 
      };
      
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_PTVIRG, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS); 
      if (next_token != PSCRIPT_TOKEN_PTVIRG) { 
	next_token     = get_next_token_type(); 
	type_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
	next_token     = get_current_token_type(); 
      } 
      else { 
	type_expr_tree = -1; 
      }; 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 

      int * ident_token_ptr = ident_list_array + ident_list_nb; 
      for (int i = 0; i < ident_list_nb; i++) { 
	ident_token_ptr--; 
	pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE, *ident_token_ptr, type_expr_tree); 
      }; 
      
      next_token = get_next_token_type(); 
      if (next_token != PSCRIPT_TOKEN_IDENT) { break; }; 
    }; 
    return 0; 
  }; 



 LABEL_STATE(expr): { 
    // RL: This is where we need the LALR theory as we cannot reasonably use a LL1 analysis (any LL1 grammar matching that language looks terrible and does not fit our needs). 
    // RL: Infix tokens: 
    // expr  PSCRIPT_TOKEN_OR           expr1 
    // expr  PSCRIPT_TOKEN_AND          expr1 
    // expr  PSCRIPT_TOKEN_XOR          expr1 
    // expr1 PSCRIPT_TOKEN_EQUAL        expr2 
    // expr1 PSCRIPT_TOKEN_DIFF         expr2 
    // expr1 PSCRIPT_TOKEN_INF          expr2 
    // expr1 PSCRIPT_TOKEN_SUP          expr2 
    // expr1 PSCRIPT_TOKEN_INFEQ        expr2 
    // expr1 PSCRIPT_TOKEN_SUPEQ        expr2 
    // expr2 token_plus_symbol  expr3 
    // expr2 token_minus_symbol expr3 
    // expr3 PSCRIPT_TOKEN_IMULT        expr4  
    // expr3 PSCRIPT_TOKEN_IDIV         expr4 
    // expr3 PSCRIPT_TOKEN_IMOD         expr4 
    // expr3 PSCRIPT_TOKEN_RMULT        expr4 
    // expr3 PSCRIPT_TOKEN_RDIV         expr4 
    //       PSCRIPT_TOKEN_NOT          expr4 
    //       PSCRIPT_TOKEN_IPLUS        expr4 
    //       PSCRIPT_TOKEN_IMOINS       expr4 
    //       PSCRIPT_TOKEN_RPLUS        expr4 
    //       PSCRIPT_TOKEN_RMOINS       expr4 
    // expr5 PSCRIPT_TOKEN_PTR 
    //       PSCRIPT_TOKEN_REF          expr5 
    // expr5 PSCRIPT_TOKEN_OPENBRACKET  expr  PSCRIPT_TOKEN_CLOSEBRACKET 
    // expr5 PSCRIPT_TOKEN_POINT        nom 
    // appel_funproc__expr 
    // appel_methode__expr 
    //       PSCRIPT_TOKEN_OPENPAR      expr  PSCRIPT_TOKEN_CLOSEPAR 
    //       PSCRIPT_TOKEN_TRUE 
    //       PSCRIPT_TOKEN_FALSE 
    //       PSCRIPT_TOKEN_ENTIER 
    //       PSCRIPT_TOKEN_REEL 
    //       PSCRIPT_TOKEN_CHAINE 
    //       PSCRIPT_TOKEN_IDENT 
    //       PSCRIPT_TOKEN_NIL 
    // --- 
    // RL: Premier(expr): // RL: TODO XXX FIXME: Add PSCRIPT_TOKEN_BITCOMPLEMENT, bit_shift, assign, side_effect_assigns, etc. 
    //       PSCRIPT_TOKEN_NOT 
    //       PSCRIPT_TOKEN_IPLUS 
    //       PSCRIPT_TOKEN_IMOINS 
    //       PSCRIPT_TOKEN_RPLUS 
    //       PSCRIPT_TOKEN_RMOINS 
    //       PSCRIPT_TOKEN_OPENPAR 
    //       PSCRIPT_TOKEN_TRUE 
    //       PSCRIPT_TOKEN_FALSE 
    //       PSCRIPT_TOKEN_ENTIER 
    //       PSCRIPT_TOKEN_REEL 
    //       PSCRIPT_TOKEN_CHAINE 
    //       PSCRIPT_TOKEN_IDENT 
    //       PSCRIPT_TOKEN_NIL 
    // 
    // RL: As per the LALR theory, two stacks are needed: (i) one for the outputs (trees being built); (ii) one for the current path in the automaton [used when reducing, in order to backtrack in the automaton]. 
    const int8_t path__size = 63; // RL: This represents the nestedness of an expression. If an expression is more nested than that, then we wouldn't be able to parse it. 
    int8_t path__array[path__size]; 
    int8_t path__nb = 0; 
    const int8_t outputs__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int16_t outputs__array[outputs__size]; 
    int8_t outputs__nb = 0; 
    // RL: Extra stacks, as the automaton could be simplified in adding variables instead of extending all these into states. 
    const int8_t operator__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int8_t operator__array[operator__size]; 
    int8_t operator__nb = 0; 
    const int8_t arity__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int8_t arity__array[arity__size]; 
    int8_t arity__nb = 0; 
    const int8_t master__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int8_t master__array[master__size]; 
    int8_t master__nb = 0; 
    const int8_t rule_idx__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int8_t rule_idx__array[rule_idx__size]; 
    int8_t rule_idx__nb = 0; 
    const int8_t rule_pos__size = path__size; // RL: They should be the same as it would not make any sense otherwise. 
    int8_t rule_pos__array[rule_pos__size]; 
    int8_t rule_pos__nb = 0; 
    int8_t current_lalr_state = 0; 
    int16_t current_symbol = get_current_token_type(); 
#define PUSH_OUTPUT(x)   { assert( outputs__nb <  outputs__size);  outputs__array[ outputs__nb++] = (x); }; 
#define PUSH_PATH(x)     { assert(    path__nb <     path__size);     path__array[    path__nb++] = (x); }; 
#define PUSH_OPERATOR(x) { assert(operator__nb < operator__size); operator__array[operator__nb++] = (x); }; 
#define PUSH_ARITY(x)    { assert(   arity__nb <    arity__size);    arity__array[   arity__nb++] = (x); }; 
#define PUSH_MASTER(x)   { assert(  master__nb <   master__size);   master__array[  master__nb++] = (x); }; 
#define PUSH_RULE_IDX(x) { assert(rule_idx__nb < rule_idx__size); rule_idx__array[rule_idx__nb++] = (x); }; 
#define PUSH_RULE_POS(x) { assert(rule_pos__nb < rule_pos__size); rule_pos__array[rule_pos__nb++] = (x); }; 
#define POP_OUTPUT(x)    { assert( outputs__nb > 0); (x) =  outputs__array[-- outputs__nb]; }; 
#define POP_PATH(x)      { assert(    path__nb > 0); (x) =     path__array[--    path__nb]; }; 
#define POP_OPERATOR(x)  { assert(operator__nb > 0); (x) = operator__array[--operator__nb]; }; 
#define POP_ARITY(x)     { assert(   arity__nb > 0); (x) =    arity__array[--   arity__nb]; }; 
#define POP_MASTER(x)    { assert(  master__nb > 0); (x) =   master__array[--  master__nb]; }; 
#define POP_RULE_IDX(x)  { assert(rule_idx__nb > 0); (x) = rule_idx__array[--rule_idx__nb]; }; 
#define POP_RULE_POS(x)  { assert(rule_pos__nb > 0); (x) = rule_pos__array[--rule_pos__nb]; }; 
#define DROP_OPERATOR(x) { assert(operator__nb > 0); operator__nb--; }; 
#define DROP_ARITY(x)    { assert(   arity__nb > 0);    arity__nb--; }; 
#define DROP_MASTER(x)   { assert(  master__nb > 0);   master__nb--; }; 
#define DROP_RULE_IDX(x) { assert(rule_idx__nb > 0); rule_idx__nb--; }; 
#define DROP_RULE_POS(x) { assert(rule_pos__nb > 0); rule_pos__nb--; }; 
#define EXPR_CASE_GOTO(id) case id: goto glue(label__expr_state,id); break;  
#define EXPR_LABEL_STATE(id) glue(label__expr_state,id) 
    PUSH_PATH(11); 
    for (;;) { 
#if DEBUG_PARSER >= 2
  dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", " - current_lalr_state: ", int_string__stack(current_lalr_state), " - current_token: ", int_string__stack(*current_token_ref), " - current_symbol: ", current_symbol >= 0 && current_symbol < PSCRIPT_TOKEN_COUNT ? pscript_token__type_get_cstr(current_symbol) : "_", "(", int_string__stack(current_symbol), ")", "\n"); 
#endif 
#if DEBUG_PARSER >= 3 
  dputs_array(stderr__fileno, "\t", "PATH: [ "); for (int i = 0; i < path__nb; i++) { dputs_array(stderr__fileno, int_string__stack(path__array[i]), " -> "); }; dputs_array(stderr__fileno, " ]", "\n" ); 
#endif 
#if DEBUG_PARSER >= 3 
  dputs_array(stderr__fileno, "\t", "OUTPUTS: [ "); for (int i = 0; i < outputs__nb; i++) { dputs_array(stderr__fileno, int_string__stack(outputs__array[i]), " -> "); }; dputs_array(stderr__fileno, " ]", "\n" ); 
#endif 
      switch (current_lalr_state) {
	EXPR_CASE_GOTO(0);  // RL: S → . S   _   S | ...  
	EXPR_CASE_GOTO(1);  // RL: S →   S . _   S 
	EXPR_CASE_GOTO(6);  // RL: S →       _ . S 
	EXPR_CASE_GOTO(7);  // RL: S → ( S . ) 
	EXPR_CASE_GOTO(8);  // RL: S → ident ( . ... // <funcall>  
	EXPR_CASE_GOTO(9);  // RL: arg_list: expecting ', S+' or ')' -- A 'S' was read and is on the output stack. 
	EXPR_CASE_GOTO(10); // RL: arg_list: we have read at least one comma - expecting ', S+' or ')' -- An 'ident' and a 'arg_list' were read and are on the output stack. 
	EXPR_CASE_GOTO(11); // RL: An expression was recognized! 
	EXPR_CASE_GOTO(12); // RL: S →   S   _   S . 
	EXPR_CASE_GOTO(13); // RL: S →   S [  S . ] 
	EXPR_CASE_GOTO(14); // RL: S →   S ? . S :   S 
	EXPR_CASE_GOTO(15); // RL: S →   S ?   S : . S 
	EXPR_CASE_GOTO(16); // RL: SYNTAX_MASTER - INIT 
	EXPR_CASE_GOTO(17); // RL: SYNTAX_MASTER - INSIDE 
      }; 
      assert(false); 
      
    EXPR_LABEL_STATE(0): { 
	//if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 
	if (-1 == current_symbol) { current_symbol = get_next_token_type(); }; 
	//if (int_member_huh(current_symbol, PSCRIPT_TOKEN_TRUE, PSCRIPT_TOKEN_FALSE, PSCRIPT_TOKEN_ENTIER, PSCRIPT_TOKEN_REEL__VIRG, PSCRIPT_TOKEN_REEL__DOT, PSCRIPT_TOKEN_REEL__E, PSCRIPT_TOKEN_CHAINE_C, PSCRIPT_TOKEN_CHAINE_P, PSCRIPT_TOKEN_NIL)) { 
	if (int_member_huh(current_symbol, PSCRIPT_TOKEN_TRUE, PSCRIPT_TOKEN_FALSE, PSCRIPT_TOKEN_ENTIER, PSCRIPT_TOKEN_REEL__VIRG, PSCRIPT_TOKEN_REEL__DOT, PSCRIPT_TOKEN_REEL__E, PSCRIPT_TOKEN_CHAINE_C, PSCRIPT_TOKEN_CHAINE_P, PSCRIPT_TOKEN_NIL, PSCRIPT_TOKEN_NULL_PTR, PSCRIPT_TOKEN_IDENT)) { 
	  //RL: Constant value. 
	  const int16_t new_tree = pscript_parser_chunk_env__push1(this -> expr_tree_env, current_symbol, *current_token_ref); 
	  //const int16_t new_tree = *current_token_ref; 

	  PUSH_OUTPUT(new_tree); 
	  //PUSH_PATH(current_lalr_state); 
	  current_symbol = get_next_token_type(); //-1; 
	  current_lalr_state = 1; 
	  continue;
	}; 
	if (int_member_huh(current_symbol, PSCRIPT_TOKEN_LOGICAL_NOT, PSCRIPT_TOKEN_IPLUS, PSCRIPT_TOKEN_IMOINS, PSCRIPT_TOKEN_RPLUS, PSCRIPT_TOKEN_RMOINS)) { 
	  // RL: Prefix unary operator. 
	  PUSH_OPERATOR(current_symbol); 
	  PUSH_OUTPUT(*current_token_ref); 
	  PUSH_ARITY(1); 
	  //PUSH_PATH(current_lalr_state); // RL: Where we will go after having built the unary tree. 
	  PUSH_PATH(1); // RL: Where we will go after having built the unary tree. 
	  PUSH_PATH(6); // RL: Where we will go when the expression will be reduced. 
	  current_symbol = get_next_token_type(); //-1; 
	  current_lalr_state = 0; 
	  continue; 
	}; 
	if (PSCRIPT_TOKEN_OPENPAR == current_symbol) { 
	  //PUSH_PATH(current_lalr_state); // RL: Where we will go after having built the unary tree. 
	  PUSH_PATH(1); // RL: Where we will go after having built the unary tree. 
	  PUSH_PATH(7); // RL: Where we will go when the expression will be reduced. 
	  //current_symbol = -1; 
	  current_symbol = get_next_token_type(); //-1; 
	  current_lalr_state = 0; 
	  continue;
	}; 
#if 0 
	if (PSCRIPT_TOKEN_IDENT == current_symbol) { 
	  // RL: Here, we have two possibilities: (i) an ident evaluation; or (ii) a function call. So we do a LA (look-ahead) to choose. 
	  // RL: But, first, we gonna need the ident value anyway. 
	  const int16_t new_tree = pscript_expr_tree__push0(this -> expr_tree_env, current_symbol, *current_token_ref); 
	  PUSH_OUTPUT(new_tree); 
	  current_symbol = get_next_token_type(); 
	  if (PSCRIPT_TOKEN_OPENPAR == current_symbol) { 
	    // RL: That's a function call. 
#if 0 
	    int ident_subtree; 
	    POP_OUTPUT(ident_subtree); 
	    const int16_t new_tree = pscript_expr_tree__push1(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, -1, ident_subtree); 
	    PUSH_OUTPUT(new_subtree); 
#endif 
	    PUSH_PATH(1); // RL: Where we will go back after having built the tree. 
	    //current_symbol = -1; 
	    current_lalr_state = 8; 
	    continue;
	  }; 
	  
	  // RL: That's an ident evaluation. 
	  { 
	    //PUSH_PATH(current_lalr_state); 
	    //PUSH_PATH(1); 
	    PUSH_OUTPUT(new_tree); 
	    current_symbol = get_current_token_type(); // -1; 
	    current_lalr_state = 1; 
	    continue; 
	  }; 
 
	  assert(false); 
	}; 
#endif 
	
	if (PSCRIPT_TOKEN_LAMBDA == current_symbol) { 
	  const int lambda_decl_expr = pscript_parser__automaton__aux(exception_data, this, token_env, expr_lambda, current_token_ref, retval1_ref);
	  current_symbol = get_current_token_type(); ; 
	  //const int16_t new_tree = pscript_expr_tree__push0(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__LAMBDA, lambda_decl); 
	  const int16_t new_tree = lambda_decl_expr; 
	  PUSH_OUTPUT(new_tree); 
	  current_lalr_state = 1; 
	  continue; 
	}; 

	if (PSCRIPT_TOKEN__SYNTAX_MASTER__BASE <= current_symbol && current_symbol < PSCRIPT_TOKEN__SYNTAX_MASTER__TOP) { 
	  PUSH_PATH(1); // RL: Where we will go after having reduced the custom syntax. 
	  current_lalr_state = 16; 
	  continue;
	}; 
	
	
	SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(current_symbol,PSCRIPT_TOKEN_TRUE, PSCRIPT_TOKEN_FALSE, PSCRIPT_TOKEN_ENTIER, PSCRIPT_TOKEN_REEL__VIRG, PSCRIPT_TOKEN_REEL__DOT, PSCRIPT_TOKEN_REEL__E, PSCRIPT_TOKEN_CHAINE_C, PSCRIPT_TOKEN_CHAINE_P, PSCRIPT_TOKEN_NIL, PSCRIPT_TOKEN_IDENT, PSCRIPT_TOKEN_LOGICAL_NOT, PSCRIPT_TOKEN_IPLUS, PSCRIPT_TOKEN_IMOINS, PSCRIPT_TOKEN_RPLUS, PSCRIPT_TOKEN_RMOINS, PSCRIPT_TOKEN_OPENPAR, PSCRIPT_TOKEN_LAMBDA, PSCRIPT_TOKEN_SYNTAX_MASTER); 
	
	// RL: TODO XXX FIXME: Do we need to here have an 'S' case? 
	
	dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", " - current_lalr_state: ", int_string__stack(current_lalr_state), " - current_token: ", int_string__stack(*current_token_ref), " - current_symbol: ", current_symbol >= 0 && current_symbol < PSCRIPT_TOKEN_COUNT ? pscript_token__type_get_cstr(current_symbol) : "_", "(", int_string__stack(current_symbol), ")", "\n"); 
	dputs_array(stderr__fileno, "\t", "PATH: [ "); for (int i = 0; i < path__nb; i++) { dputs_array(stderr__fileno, int_string__stack(path__array[i]), " -> "); }; dputs_array(stderr__fileno, " ]", "\n" ); 
	assert(false); 
      };  
      
      




    EXPR_LABEL_STATE(1): { 
	// RL: We have read one expression. Now we will read the operator applied to that left expression. 
	// RL: We try to here order them by priority. 
	// RL: TODO XXX FIXME: Do we need a priority stack? 
	if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 

#if 0 	// RL: That could be a quick end. 
	//if (PSCRIPT_TOKEN_EOF == current_symbol) { 
	if (PSCRIPT_TOKEN_PTVIRG == current_symbol) { 
	  // RL: Nothing to be read left. 
	  // RL: We just an 'S' expression; so that looks good. 
	  //     However, we might be nested in other expression, expecting something to be read. 
	  //assert(1 == outputs__nb); 
	  //POP_OUTPUT(expr_tree); 
	  //current_symbol = -1; 
	  //current_lalr_state = 11; 
	  POP_PATH(current_lalr_state); 
	  continue; 
	}; 
#endif 
	
	const int infix_binary_operator_huh = int_member_huh(current_symbol, PSCRIPT_TOKEN_AFFECTATION, PSCRIPT_TOKEN_AFFECTATION_SIMPLE, PSCRIPT_TOKEN_AFFECTATION_IADD, PSCRIPT_TOKEN_AFFECTATION_RADD, PSCRIPT_TOKEN_AFFECTATION_ISUB, PSCRIPT_TOKEN_AFFECTATION_RSUB, PSCRIPT_TOKEN_AFFECTATION_IMULT, PSCRIPT_TOKEN_AFFECTATION_RMULT, PSCRIPT_TOKEN_AFFECTATION_IDIV, PSCRIPT_TOKEN_AFFECTATION_RDIV, PSCRIPT_TOKEN_AFFECTATION_IMOD, PSCRIPT_TOKEN_AFFECTATION_L_AND, PSCRIPT_TOKEN_AFFECTATION_L_OR, PSCRIPT_TOKEN_AFFECTATION_L_XOR, PSCRIPT_TOKEN_AFFECTATION_B_AND, PSCRIPT_TOKEN_AFFECTATION_B_OR, PSCRIPT_TOKEN_AFFECTATION_B_XOR, PSCRIPT_TOKEN_AFFECTATION_B_RSHIFT, PSCRIPT_TOKEN_AFFECTATION_B_LSHIFT, PSCRIPT_TOKEN_LOGICAL_OR, PSCRIPT_TOKEN_LOGICAL_AND, PSCRIPT_TOKEN_LOGICAL_XOR, PSCRIPT_TOKEN_BITWISE_OR, PSCRIPT_TOKEN_BITWISE_AND, PSCRIPT_TOKEN_BITWISE_XOR, PSCRIPT_TOKEN_BITWISE_SHIFT_LEFT, PSCRIPT_TOKEN_BITWISE_SHIFT_RIGHT, PSCRIPT_TOKEN_EQUAL, PSCRIPT_TOKEN_DIFF, PSCRIPT_TOKEN_INF, PSCRIPT_TOKEN_SUP, PSCRIPT_TOKEN_INFEQ, PSCRIPT_TOKEN_SUPEQ, PSCRIPT_TOKEN_IPLUS, PSCRIPT_TOKEN_RPLUS, PSCRIPT_TOKEN_IMINUS, PSCRIPT_TOKEN_RMINUS, PSCRIPT_TOKEN_IMULT, PSCRIPT_TOKEN_RMULT, PSCRIPT_TOKEN_IDIV, PSCRIPT_TOKEN_RDIV, PSCRIPT_TOKEN_POINT, PSCRIPT_TOKEN_FLECHE); 
	
	// RL: The default behavior is to shift. 
	//     However, this is an undesirable behavior to handle priorities: 
	//        " 1 * 2 + 3 " 
	//     In this case, we should not shift, but reduce. 
	//     In order to implement that, we have just to compare priorities: 
	//       if priority(current_operator) < priority(previous_operator) then reduce else shift 
	//     
	if (infix_binary_operator_huh) { 
	  int reduce_huh = false; 
	  if (operator__nb > 0) { 
	    const int previous_operator = operator__array[operator__nb-1]; 
	    reduce_huh = (previous_operator >= current_symbol); // RL: Property of the token-types: first the lowest priority, last the highest priority. 
	  }; 
	  if (reduce_huh) { 
	    POP_PATH(current_lalr_state); 
	    continue; 
	  } 
	  else {  
	    PUSH_OPERATOR(current_symbol); 
	    PUSH_OUTPUT(*current_token_ref); 
	    PUSH_ARITY(2); 
	    //PUSH_PATH(current_lalr_state); // RL: Where we will go after having built the next 'S' tree. 
	    PUSH_PATH(12); // RL: Where we will go when the expression will be reduced. 
	    //current_symbol = -1; 
	    current_symbol = get_next_token_type(); 
	    current_lalr_state = 0; 
	    continue; 
	  }; 
	}; 
	
	if (PSCRIPT_TOKEN_OPENBRACKET == current_symbol) { 
	  PUSH_OPERATOR(current_symbol); 
	  PUSH_OUTPUT(*current_token_ref); 
	  PUSH_ARITY(2); 
	  //PUSH_PATH(current_lalr_state); // RL: Where we will go after having built the next 'S' tree. 
	  PUSH_PATH(13); // RL: Where we will go when the expression will be reduced. 
	  //current_symbol = -1; 
	  current_symbol = get_next_token_type(); 
	  current_lalr_state = 0; 
	  continue; 
	}; 
	
	if (PSCRIPT_TOKEN_OPENPAR == current_symbol) { 
	  // RL: Then it's a function call. 
	  PUSH_PATH(1); // RL: Where we will go back after having built the tree. 
	  current_lalr_state = 8; // RL: Arg parsing. 
	  continue;
	}; 
	
	if (PSCRIPT_TOKEN_HUH == current_symbol) { 
	  PUSH_OPERATOR(current_symbol); 
	  PUSH_OUTPUT(*current_token_ref); 
	  PUSH_ARITY(3); 
	  PUSH_PATH(14); // RL: Where we will go when the expression will be reduced. 
	  current_symbol = get_next_token_type(); 
	  current_lalr_state = 0; 
	  continue; 
	}; 
	
	//if (int_member_huh(current_symbol, PSCRIPT_TOKEN_INC, PSCRIPT_TOKEN_DEC)) { 
	if (int_member_array_nb(current_symbol, pscript_parser__postfix_operators__nb, pscript_parser__postfix_operators)) { 
	  // RL: Postfix unary operator. 
#if 1 
	  int subtree; 
	  POP_OUTPUT(subtree); 
	  const int operator_token = *current_token_ref; 
	  const int operator = current_symbol; 
	  const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, /*type*/operator, /*value*/operator_token, /*descendant*/subtree); 
	  PUSH_OUTPUT(new_tree); 
	  current_symbol = get_next_token_type(); 
	  current_lalr_state = 1; 
#else 
	  PUSH_OPERATOR(current_symbol); 
	  PUSH_ARITY(1); 
	  PUSH_PATH(current_lalr_state); // RL: Where we will go after having built the unary tree. 
	  PUSH_PATH(6); // RL: Where we will go when the expression will be reduced. 
	  current_symbol = -1; 
	  current_lalr_state = 0; 
#endif 
	  continue; 
	}; 

	// RL: Anything that is not expected triggers a reduction. 
	POP_PATH(current_lalr_state); 
	continue; 
	
	assert(false); 
      }; 
      
      






    EXPR_LABEL_STATE(6): { 
	// RL: PREFIX 
	// RL: A unary symbol was read, and a tree was built and is on the ouput stack. 
	// RL: So, we have to get that tree on the stack, make a new tree, and push it onto the stack. 
	// RL: After that, we should backtrack by one. 
	int subtree; 
	POP_OUTPUT(subtree); 
	int operator_token; 
	POP_OUTPUT(operator_token); 
	int operator; 
	POP_OPERATOR(operator); 
	DROP_ARITY(); 
	const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, /*type*/operator, /*value*/operator_token, /*descendant*/subtree); 
	PUSH_OUTPUT(new_tree); 
	POP_PATH(current_lalr_state); 
	continue; 
      }; 
      
    EXPR_LABEL_STATE(7): { 
	// RL: A '(' and a 'S' were read. Now we expect a ')'. 
	if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 
	SYNTAX__EXPECTING_THIS_TOKEN(current_symbol, PSCRIPT_TOKEN_CLOSEPAR); 
	// RL: So, we do not touch the tree on the stack. 
	// RL: Now, we should backtrack by one. 
	POP_PATH(current_lalr_state); 
	//current_symbol = -1; 
	current_symbol = get_next_token_type(); 
	continue; 
      }; 

    EXPR_LABEL_STATE(8): { // RL: funcall: S → ident . ( ... 
	// RL: Here, we expect a comma-separated list of expressions, that could be empty. 
	SYNTAX__EXPECTING_THIS_TOKEN(current_symbol, PSCRIPT_TOKEN_OPENPAR); 
	current_symbol = get_next_token_type(); 
	if (current_symbol == PSCRIPT_TOKEN_CLOSEPAR) { 
#if 1
	  int ident_subtree; 
	  POP_OUTPUT(ident_subtree); 
	  const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, /*fun_subtree*/ident_subtree, /*arg_list*/-1); 
	  PUSH_OUTPUT(new_tree); 
#else 
	  // RL: The funcall_subtree is already on the stack. 
#endif 
	  POP_PATH(current_lalr_state); 
	  //current_symbol = -1; 
	  current_symbol = get_next_token_type(); 
	  continue; 
	}; 
	
	// RL: Now we expect a list of arguments. 
	PUSH_OPERATOR(PSCRIPT_TOKEN_VIRGULE); 
	PUSH_ARITY(INT8_MAX); 
	PUSH_PATH(9); 
	current_lalr_state = 0; 
	//current_symbol = -1; 
	//current_symbol = get_next_token_type(); 
	continue;
      }; 
      
    EXPR_LABEL_STATE(9): { // RL: arg_list: S . , S OR ) 
	// RL: OK. We've just read one 'S'. Now we expect a ')' or a ','. Anything else is an error. 
	if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 
	if (current_symbol == PSCRIPT_TOKEN_CLOSEPAR) { 
	  DROP_OPERATOR(); 
	  DROP_ARITY(); 

	  int arg_subtree; 
	  POP_OUTPUT(arg_subtree); 
#if 1 
	  int ident_subtree; 
	  POP_OUTPUT(ident_subtree); 
	  const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, /*fun_subtree*/ident_subtree, /*arg_list*/arg_subtree); 
	  PUSH_OUTPUT(new_tree); 
#else 
	  int funcall_subtree; 
	  POP_OUTPUT(funcall_subtree); 
	  pscript_expr_tree__add_subtree(this -> expr_tree_env, funcall_subtree, arg_subtree); 
	  PUSH_OUTPUT(funcall_tree); 
#endif 
	  POP_PATH(current_lalr_state); 
	  //current_symbol = -1; 
	  current_symbol = get_next_token_type(); 
	  continue; 
	}; 
	
	if (current_symbol == PSCRIPT_TOKEN_VIRGULE) { 
	  PUSH_PATH(10); 
	  current_lalr_state = 0; 
	  current_symbol = -1; 
	  continue;
	}; 
	
	SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(current_symbol,PSCRIPT_TOKEN_VIRGULE,PSCRIPT_TOKEN_CLOSEPAR); 
      
	assert(false); 
      }; 



    EXPR_LABEL_STATE(10): { // RL: arg_list with two or more items (at least one comma read): S . , S OR ) 
	// RL: OK, we've just read one 'S'. Before that, we read a ',', and before that we read a 'S'. 
	//     So, now, we have to merge 
	if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 
	
	if (current_symbol == PSCRIPT_TOKEN_CLOSEPAR) { 
	  DROP_OPERATOR(); 
	  DROP_ARITY(); 

	  int arg_subtree; 
	  POP_OUTPUT(arg_subtree); 
	  int arglist_subtree; 
	  POP_OUTPUT(arglist_subtree); 
	  const int16_t new_arglist_subtree = pscript_parser_chunk_env__push(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, /*arglist_subtree*/arglist_subtree, /*arg_subtree*/arg_subtree); 
	  int ident_subtree; 
	  POP_OUTPUT(ident_subtree); 
	  const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, /*fun_subtree*/ident_subtree, /*arg_list*/new_arglist_subtree); 
	  PUSH_OUTPUT(new_tree); 
	  // -- 
	  POP_PATH(current_lalr_state); 
	  current_symbol = -1; 
	  continue; 
	}; 
	
	if (current_symbol == PSCRIPT_TOKEN_VIRGULE) { 
	  int arg_subtree; 
	  POP_OUTPUT(arg_subtree); 
	  int arglist_subtree; 
	  POP_OUTPUT(arglist_subtree); 
	  const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, /*arglist_subtree*/arglist_subtree, /*arg_subtree*/arg_subtree); 
	  PUSH_OUTPUT(new_tree); 
	  // -- 
	  PUSH_PATH(10); 
	  current_lalr_state = 0; 
	  current_symbol = -1; 
	  continue; 
	}; 
      
	SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(current_symbol,PSCRIPT_TOKEN_VIRGULE,PSCRIPT_TOKEN_CLOSEPAR); 
	assert(false); 
      }; 


    EXPR_LABEL_STATE(11): { // RL: S EOF . 
	break; 
      }; 

    EXPR_LABEL_STATE(12): { // RL: S →   S   _   S . 
	int right_subtree; 
	POP_OUTPUT(right_subtree); 
	int operator_token; 
	POP_OUTPUT(operator_token); 
	int left_subtree; 
	POP_OUTPUT(left_subtree); 
	int operator; 
	POP_OPERATOR(operator); 
	DROP_ARITY(); 
	const int16_t new_tree = pscript_parser_chunk_env__push3(this -> expr_tree_env, /*type*/operator, /*value*/operator_token, /*descendants1*/left_subtree, /*descendants2*/right_subtree); 
	PUSH_OUTPUT(new_tree); 
	// -- 
	//POP_PATH(current_lalr_state); 
	current_lalr_state = 1; 
	continue; 
      }; 


    EXPR_LABEL_STATE(13): { // RL: S →   S [  S . ] 
	if (-1 == current_symbol) { current_symbol = yypscriptlex(); }; 
	
	SYNTAX__EXPECTING_THIS_TOKEN(current_symbol, PSCRIPT_TOKEN_CLOSEBRACKET); 
	current_symbol = get_next_token_type(); // RL: Eating the token. 
	
	int right_subtree; 
	POP_OUTPUT(right_subtree); 
	int operator_token; 
	POP_OUTPUT(operator_token); 
	int left_subtree; 
	POP_OUTPUT(left_subtree); 
	int operator; 
	POP_OPERATOR(operator); 
	DROP_ARITY(); 
	const int16_t new_tree = pscript_parser_chunk_env__push3(this -> expr_tree_env, /*type*/operator, /*value*/operator_token, /*descendants1*/left_subtree, /*descendants2*/right_subtree); 
	PUSH_OUTPUT(new_tree); 
	// -- 
	//POP_PATH(current_lalr_state); 
	current_lalr_state = 1; 
	continue; 
      }; 


    EXPR_LABEL_STATE(14): { // RL: S →   S ?  S . : S  
	SYNTAX__EXPECTING_THIS_TOKEN(current_symbol, PSCRIPT_TOKEN_DEUXPOINTS); 
	current_symbol = get_next_token_type(); // RL: Eating the token. 
	PUSH_PATH(15); // RL: Where we will go when reducing. 
	current_lalr_state = 0; 
	continue; 
      }; 
	
    EXPR_LABEL_STATE(15): { // RL: S →   S ?  S : S . 
	int right_subtree; 
	POP_OUTPUT(right_subtree); 
	int middle_subtree; 
	POP_OUTPUT(middle_subtree); 
	int operator_token; 
	POP_OUTPUT(operator_token); 
	int left_subtree; 
	POP_OUTPUT(left_subtree); 
	int operator; 
	POP_OPERATOR(operator); 
	DROP_ARITY(); 
	const int16_t new_tree = pscript_parser_chunk_env__push4(this -> expr_tree_env, /*type*/operator, /*value*/operator_token, /*descendants1*/left_subtree, middle_subtree, /*descendants2*/right_subtree); 
	PUSH_OUTPUT(new_tree); 
	current_lalr_state = 1; 
	continue; 
      }; 

    EXPR_LABEL_STATE(16): { // RL: S →   . SYNTAX_MASTER 
#if 0 
	dputs_array(stderr__fileno, "\t", "PATH: [ "); for (int i = 0; i < path__nb; i++) { dputs_array(stderr__fileno, int_string__stack(path__array[i]), " -> "); }; dputs_array(stderr__fileno, " ]", "\n" ); 
#endif 

	const int master_token_type_id = current_symbol; 
	const int master_index         = master_token_type_id - PSCRIPT_TOKEN__SYNTAX_MASTER__BASE; 
	const int root_rule_index      = this -> custom_syntax_root__array[master_index]; 
#if 0 
	dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", 
		    " - current_lalr_state: ", int_string__stack(current_lalr_state), 
		    " - current_token: ", int_string__stack(*current_token_ref), 
		    " - current_symbol: ", current_symbol >= 0 && current_symbol < PSCRIPT_TOKEN_COUNT ? pscript_token__type_get_cstr(current_symbol) : "_", "(", int_string__stack(current_symbol), ")", 
		    " - master_index: ", int_string__stack(master_index), 
		    " - root_rule_index: ", int_string__stack(root_rule_index), 
		    "\n"); 
#endif 

	//current_symbol = get_next_token_type(); // RL: Eating it. 
	PUSH_MASTER(master_index); 
	PUSH_RULE_IDX(root_rule_index); 
	//PUSH_RULE_POS(1); 
	PUSH_RULE_POS(0); 
	current_lalr_state = 17; 
	continue; 
      }; 
	
    EXPR_LABEL_STATE(17): { // RL: S →   . SYNTAX_MASTER // SHIFT 
	//const int master_token_type_id = current_symbol; 
	const int master_index =   master__array[  master__nb - 1]; 
	const int rule_index   = rule_idx__array[rule_idx__nb - 1]; 
	const int rule_len     = this -> custom_syntax_rules__array[rule_index][2]; 
	const int rule_pos     = rule_pos__array[rule_pos__nb - 1]; 
	const int expected_thg = rule_pos >= rule_len ? -1 : this -> custom_syntax_rules__array[rule_index][3 + rule_pos]; 
	//const int node_id      = this -> custom_syntax_rules__array[rule_index][1]; 
#if 0 
	dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", 
		    " - current_lalr_state: ", int_string__stack(current_lalr_state), 
		    " - current_token: ", int_string__stack(*current_token_ref), 
		    " - current_symbol: ", current_symbol >= 0 && current_symbol < PSCRIPT_TOKEN_COUNT ? pscript_token__type_get_cstr(current_symbol) : "_", "(", int_string__stack(current_symbol), ")", 
		    " - master_index: ", int_string__stack(master_index), 
		    " - rule_index: ", int_string__stack(rule_index), 
		    " - rule_len: ", int_string__stack(rule_len), 
		    " - rule_pos: ", int_string__stack(rule_pos), 
		    " - expected_thg: ", int_string__stack(expected_thg), 
		    " - TOKEN_COUNT: ", int_string__stack(PSCRIPT_TOKEN_COUNT), 
		    " - SYNTAX_MASTER__TOP: ", int_string__stack(PSCRIPT_TOKEN__SYNTAX_MASTER__TOP), 
		    " - TOKEN__SYNTAX_KEYWORD__TOP: ", int_string__stack(PSCRIPT_TOKEN__SYNTAX_KEYWORD__TOP), 
		    " - CUSTOM_SYNTAX_NODE__BASE: ", int_string__stack(PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__BASE), 
		    " - CUSTOM_SYNTAX_NODE__TOP: ", int_string__stack(PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__TOP), 
		    " - CUSTOM_SYNTAX_RULE__TOP: ", int_string__stack(PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__TOP), 
		    "\n"); 
#endif 
	if (rule_len == 0) { 
	  const int tree_type    = this -> custom_syntax_trees__array[rule_index][0]; 
	  const int subtrees_len = this -> custom_syntax_trees__array[rule_index][1]; 
	  //current_symbol = node_id; 
	  if (0 == tree_type) { 
	    PUSH_OUTPUT(-1); 
	    DROP_RULE_POS(); 
	    DROP_RULE_IDX(); 
	    POP_PATH(current_lalr_state); 
	    continue; 
	  }; 
	  if (0 == subtrees_len) { 
	    const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, /*type*/tree_type); 
	    PUSH_OUTPUT(new_tree); 
	    DROP_RULE_POS(); 
	    DROP_RULE_IDX(); 
	    POP_PATH(current_lalr_state); 
	    current_symbol = tree_type; 
	    continue; 
	  }; 
	  //const int *   subtrees = &(this -> custom_syntax_trees__array[rule_index][2]); 
	  int subtrees[subtrees_len]; 
	  for (int i = 0; i < subtrees_len; i++) { 
	    subtrees[i] = this -> custom_syntax_trees__array[rule_index][2 + i]; 
	  }; 
	  const int16_t new_tree = pscript_parser_chunk_env__push_array(this -> expr_tree_env, /*type*/tree_type, subtrees_len, subtrees); 
	  PUSH_OUTPUT(new_tree); 
	  DROP_RULE_POS(); 
	  DROP_RULE_IDX(); 
	  POP_PATH(current_lalr_state); 
	  continue; 
	}; 
	//if (expected_thg < 0) { 
	if (rule_pos >= rule_len) { 
	  const int tree_type    = this -> custom_syntax_trees__array[rule_index][0]; 
	  const int subtrees_len = this -> custom_syntax_trees__array[rule_index][1]; 
	  if (0 == tree_type) { 
	    if (0 == subtrees_len) { 
	      PUSH_OUTPUT(-1); 
	      DROP_RULE_POS(); 
	      DROP_RULE_IDX(); 
	      POP_PATH(current_lalr_state); 
	      continue; 
	    }; 
	    assert(1 == subtrees_len); 
	    const int item_index = this -> custom_syntax_trees__array[rule_index][2]; 
	    assert(item_index >= 0); 
	    assert(item_index < rule_len); 
	    int one_subtree; 
	    int the_sought_tree = -2; 
	    for (int i = 0; i < rule_len; i++) { 
	      POP_OUTPUT(one_subtree); 
	      if (rule_len - i - 1 != item_index) continue; 
	      the_sought_tree = one_subtree; 
 	    }; 
	    PUSH_OUTPUT(the_sought_tree); 
	    DROP_RULE_POS(); 
	    DROP_RULE_IDX(); 
	    POP_PATH(current_lalr_state); 
	    continue; 
	  }; 
	  if (0 == subtrees_len) { 
	    const int16_t new_tree = pscript_parser_chunk_env__push(this -> expr_tree_env, /*type*/tree_type); 
	    PUSH_OUTPUT(new_tree); 
	    DROP_RULE_POS(); 
	    DROP_RULE_IDX(); 
	    POP_PATH(current_lalr_state); 
	    continue; 
	  }; 
	  // RL: TODO XXX FIXME: This code works only if the indices are well-ordered. 
	  int subtrees[subtrees_len]; 
	  int one_subtree; 
	  int j = subtrees_len - 1; 
	  int item_index = this -> custom_syntax_trees__array[rule_index][2 + j]; 
	  for (int i = 0; i < rule_len; i++) { 
	    POP_OUTPUT(one_subtree); 
	    if (rule_len - i - 1 != item_index) continue; 
	    subtrees[j] = one_subtree; 
	    j--; 
	    item_index = this -> custom_syntax_trees__array[rule_index][2 + j]; 
	  }; 
	  assert(j == -1); 
	  const int16_t new_tree = pscript_parser_chunk_env__push_array(this -> expr_tree_env, /*type*/tree_type, subtrees_len, subtrees); 
	  PUSH_OUTPUT(new_tree); 
	  DROP_RULE_POS(); 
	  DROP_RULE_IDX(); 
	  POP_PATH(current_lalr_state); 
	  continue; 
	}; 
	if (0 == expected_thg) { 
	  PUSH_PATH(17); 
	  //DROP_RULE_POS(); 
	  //PUSH_RULE_POS(rule_pos+1); 
	  rule_pos__array[rule_pos__nb - 1]++; 
	  current_lalr_state = 0; 
	  continue; 
	}; 
	if (current_symbol < 0) { current_symbol = get_next_token_type(); }; 
	if (expected_thg < PSCRIPT_TOKEN__SYNTAX_KEYWORD__TOP) { 
	  SYNTAX__EXPECTING_THIS_TOKEN(current_symbol, expected_thg); 
	  //DROP_RULE_POS(); 
	  //PUSH_RULE_POS(rule_pos+1); 
	  const int16_t new_tree = pscript_parser_chunk_env__push1(this -> expr_tree_env, current_symbol, *current_token_ref); 
	  //const int16_t new_tree = *current_token_ref; 
	  PUSH_OUTPUT(new_tree); 
	  rule_pos__array[rule_pos__nb - 1]++; 
	  current_symbol = get_next_token_type(); 
	  continue; 
	}; 
	if (expected_thg >= PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE) { 
	  assert(false); 
	  PUSH_PATH(17); 
	  PUSH_RULE_IDX(expected_thg); 
	  PUSH_RULE_POS(0); 
	  continue; 
	}; 
	if (expected_thg >= PSCRIPT_PARSER__CUSTOM_SYNTAX_NODE__BASE) { 
#if 0 
	  dputs_array(stderr__fileno, "HHHHHHHH---------------------------------------------------------------------------------------------------ERE\n"); 
#endif 
	  // RL: Here, we have to code the union. 
	  // RL: What would be great would be to code back-tracking. 
	  //     It would not be that awful are rules are LL(1)-like, it's just more convenient to write the same rule a second time with a minor variant 
	  //     than to factor out ourself the 'premier' set. 
	  // RL: Obviously a better approach would be to code all the alived rules; and filter them while reading. 
	  // RL: For the moment, it will be the strong-LL(1) approach, not very user-friendly, but which works and with a low complexity. 
	  const int matching_rule_id = pscript_parser__custom_syntax__rule_lookup(this, /*node_id*/expected_thg, /*token_i*/current_symbol); 
	  if (matching_rule_id < 0) { SYNTAX__EXPECTING_NOT_THIS_TOKEN(current_symbol); }; 
	  PUSH_PATH(17); 
	  PUSH_RULE_IDX(matching_rule_id - PSCRIPT_PARSER__CUSTOM_SYNTAX_RULE__BASE); 
	  rule_pos__array[rule_pos__nb - 1]++; 
	  PUSH_RULE_POS(0); 
	  continue; 
	}; 
	
	
	assert(false); 
	
	PUSH_PATH(17); // RL: Where we will go when reducing. 
	current_lalr_state = 17; 
	continue; 
      }; 
	



      assert(false); 
    }; // for (;;) 
    
    // RL: TODO XXX FIXME: Do we need that? What's the invariant? What's the convention? 
    POP_OUTPUT(expr_tree); 
    return expr_tree; 
  }; // LABEL_STATE(expr) 











 LABEL_STATE(type_expr): { 
#if 0 
  type__top_level: TOKEN_BOOLEAN | TOKEN_INTEGER | expr TOKEN_SUBRANGE expr | TOKEN_REAL | TOKEN_STRING | pointer__top_level | array__top_level | record__top_level | nom ;
  pointer__top_level: TOKEN_PTR type__top_level ;
  array__top_level: 
    TOKEN_ARRAY TOKEN_OPENBRACKET expr                     TOKEN_CLOSEBRACKET TOKEN_OF type__top_level 
      | TOKEN_ARRAY TOKEN_OPENBRACKET expr TOKEN_SUBRANGE expr TOKEN_CLOSEBRACKET TOKEN_OF type__top_level 
      ;
  record__top_level: TOKEN_RECORD liste_decl_champ__top_level TOKEN_END ; 
  liste_decl_champ__top_level: liste_decl_champ__top_level decl_champ__top_level | ;
  decl_champ__top_level: TOKEN_IDENT TOKEN_DEUXPOINTS type__top_level TOKEN_PTVIRG ;
#endif 

    int next_token; 
    for (;;) { 
      next_token = get_current_token_type(); 
      if (int_member_huh(next_token, PSCRIPT_TOKEN_BOOLEAN, PSCRIPT_TOKEN_VARIANT, PSCRIPT_TOKEN_INTEGER, PSCRIPT_TOKEN_FLOAT, PSCRIPT_TOKEN_STRING, PSCRIPT_TOKEN_IDENT)) { 
	type_expr_tree = pscript_parser_chunk_env__push1(this -> type_tree_env, /*type*/next_token, /*value*/*current_token_ref); 
	next_token = get_next_token_type(); 
	return type_expr_tree; 
      }; 
      
      if (next_token == PSCRIPT_TOKEN_ENTIER) { 
	// RL: That could be an integer range. 
	const int left_token = *current_token_ref; 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_SUBRANGE); 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_ENTIER); 
	const int right_token = *current_token_ref; 
	type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_CONST, /*value*/left_token, right_token); 
	next_token = get_next_token_type(); 
	return type_expr_tree; 
      }; 

      if (next_token == PSCRIPT_TOKEN_OPENBRACKET) { 
	// RL: This a complex range. Expressions are allowed. 
	next_token = get_next_token_type(); 
	const int left_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
	next_token = get_current_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_SUBRANGE); 
	next_token = get_next_token_type(); 
	const int right_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
	next_token = get_current_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_CLOSEBRACKET); 
	type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_EXPR, /*value*/left_expr_tree,/*descendants*/right_expr_tree); 
	next_token = get_next_token_type(); 
	return type_expr_tree; 
      }; 

      
      if (next_token == PSCRIPT_TOKEN_ARRAY) { 
	int expr1; 
	int expr2; 
	int subtype; 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_OPENBRACKET); 
	next_token = get_next_token_type(); 
	expr1 = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
	next_token = get_current_token_type(); 
	if (next_token == PSCRIPT_TOKEN_SUBRANGE) { 
	  next_token = get_next_token_type(); 
	  expr2 = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
	  next_token = get_current_token_type(); 
	} 
	else { 
	  expr2 = -1; 
	}; 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_CLOSEBRACKET); 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_OF); 
	next_token = get_next_token_type(); 
	subtype = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
	next_token = get_current_token_type(); 
	type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARRAY, /*descendants*/ expr1, expr2, subtype); 
	return type_expr_tree; 
      }; 
      
      if (next_token == PSCRIPT_TOKEN_RECORD) { 
	next_token = get_next_token_type(); 
	//const int fields = pscript_parser__automaton__aux(exception_data, this, token_env, record_fields, current_token_ref, retval1_ref); 
	const int fields_nb = pscript_parser__automaton__aux(exception_data, this, token_env, record_fields, current_token_ref, retval1_ref); 
	next_token = get_current_token_type(); 
	assert(next_token == PSCRIPT_TOKEN_END); 
	//type_expr_tree = pscript_parser_chunk_env__push1(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD, /*value*/fields); 
	type_expr_tree = pscript_parser_chunk_env__push1(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD, /*value*/fields_nb); 
	next_token = yypscriptlex(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token, PSCRIPT_TOKEN_PTVIRG); 
	return type_expr_tree; 
      }; 
      
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_ARRAY, PSCRIPT_TOKEN_RECORD, PSCRIPT_TOKEN_OPENBRACKET, PSCRIPT_TOKEN_SUBRANGE, PSCRIPT_TOKEN_BOOLEAN, PSCRIPT_TOKEN_VARIANT, PSCRIPT_TOKEN_INTEGER, PSCRIPT_TOKEN_FLOAT, PSCRIPT_TOKEN_IDENT); 
    }; 
    assert(false); 
    return 0; 

  
    return type_expr_tree; 
  }; // LABEL_STATE(type_expr) 
  
  













 LABEL_STATE(record_fields): { 
#if 0 
  record__top_level:           TOKEN_RECORD liste_decl_champ__top_level TOKEN_END ; 
  liste_decl_champ__top_level:              liste_decl_champ__top_level decl_champ__top_level | ;
  decl_champ__top_level:       TOKEN_IDENT TOKEN_DEUXPOINTS type__top_level TOKEN_PTVIRG ;
#endif 
    int next_token; 
    int ident_list_nb = 0; 
    int ident_list_array[decl__arg_ident_list__size]; 

    next_token = get_current_token_type(); 
    for (;;) { 
      if (next_token == PSCRIPT_TOKEN_END) { 
#if 1
	return 0; 
#else 
	//type_expr_tree = -1; 
	type_expr_tree = pscript_parser_chunk_env__push0(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_NIL); 
	return type_expr_tree; 
#endif 
      }; 
      
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
      ident_list_nb = 1; 
      ident_list_array[0] = *current_token_ref; 
      next_token = get_next_token_type(); 
      if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
	for (;;) { 
	  do { next_token = get_next_token_type(); } while (next_token == PSCRIPT_TOKEN_VIRGULE); 
	  SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
	  if (ident_list_nb == decl__arg_ident_list__size) { 
	    EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Declaring too many variables at the same time - I can handle at most ", int_string__stack(decl__arg_ident_list__size), " in one declaration. Please break down that declaration (or recompile the parser with a larger stack)."); 
	  }; 
	  ident_list_array[ident_list_nb] = *current_token_ref; 
	  ident_list_nb++; 
	  next_token = get_next_token_type(); 
	  if (next_token != PSCRIPT_TOKEN_VIRGULE) { break; }; 
	}; 
      }; 
      
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS); 
      next_token = get_next_token_type(); 
      const int field_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
      next_token = get_next_token_type(); 
      //const int fields_tail = pscript_parser__automaton__aux(exception_data, this, token_env, record_fields, current_token_ref, retval1_ref); 
      const int fields_tail_nb = pscript_parser__automaton__aux(exception_data, this, token_env, record_fields, current_token_ref, retval1_ref); 
      
      //int fields_list = fields_tail; 
      int fields_list_nb = fields_tail_nb; 
      int * ident_token_ptr = ident_list_array + ident_list_nb; 
      for (int i = 0; i < ident_list_nb; i++) { 
	ident_token_ptr--; 
	//fields_list = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS, /*value*/*ident_token_ptr,/*descendant1*/field_type, fields_list); 
	//fields_list = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS, /*value*/*ident_token_ptr,/*descendant1*/field_type); 
	//fields_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS, /*value*/*ident_token_ptr,/*descendant1*/field_type); 
	pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS, /*value*/*ident_token_ptr,/*descendant1*/field_type); 
	fields_list_nb ++; 
      }; 
      //return fields_list; 
      return fields_list_nb; 
    }; 
    
    assert(false); 
    return 0; 
    
    return type_expr_tree; 
  }; // LABEL_STATE(record_fields) 
  
  



 LABEL_STATE(decl_procedure): { 
    //PSCRIPT_TOKEN_IDENT OPENPAR arg_list CLOSEPAR retval_type? PTVIRG? localvar? begin ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_PROCEDURE, PSCRIPT_TOKEN_FUNCTION); 
    do { 
      next_token = get_next_token_type(); 
    } while (int_member_huh(next_token, PSCRIPT_TOKEN_PROCEDURE, PSCRIPT_TOKEN_FUNCTION)); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    const int ident_token = *current_token_ref; 
    next_token = get_next_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_OPENPAR); 
    const int arg_list = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_CLOSEPAR); 
    next_token = get_next_token_type(); 
    int retval_type; 
    if (int_member_huh(next_token, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS)) { 
      next_token = get_next_token_type(); 
      retval_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
    } 
    else { 
      retval_type = -1; 
    }; 
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
    int localvar_ref = -1; 
    int localvar_nb;
    if (next_token == PSCRIPT_TOKEN_VAR) { 
      localvar_ref = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var, current_token_ref, &localvar_nb); 
      next_token = get_current_token_type(); 
    } 
    else {
      localvar_nb = 0; 
    }; 
    
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { 
      next_token = get_next_token_type(); 
    }; 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_BEGIN); 
    int inst_nb;
    const int inst_ref = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__begin, current_token_ref, &inst_nb); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_END); 
    
    const int funproc_type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__FUNPROC, /*descendant1*/arg_list, retval_type); 
    //const int funproc_decl = pscript_decl_toplevel_list__push__funproc(this -> decl_toplevel, /*ident*/ident_token, /*typing_expr*/funproc_type_expr_tree, /*localvar_nb*/localvar_nb, /*localvar_ref*/localvar_ref, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 
    const int funproc_decl = pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC, /*ident*/ident_token, /*typing_expr*/funproc_type_expr_tree, /*localvar_nb*/localvar_nb, /*localvar_ref*/localvar_ref, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 

    
    next_token = get_next_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
    return funproc_decl; 
  }; // LABEL_STATE(decl_procedure) 




  
  
  
 LABEL_STATE(expr_lambda): { 
    //PSCRIPT_TOKEN_IDENT OPENPAR arg_list CLOSEPAR retval_type? PTVIRG? localvar? begin ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_LAMBDA); 
    do { next_token = get_next_token_type(); } while (next_token == PSCRIPT_TOKEN_LAMBDA); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_OPENPAR); 
    const int arg_list = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_CLOSEPAR); 
    next_token = get_next_token_type(); 
    
    int retval_type; 
    if (int_member_huh(next_token, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS)) { 
      do { next_token = get_next_token_type(); } while (int_member_huh(next_token, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS)); 
      //next_token = get_next_token_type(); 
      retval_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
    } 
    else { 
      retval_type = -1; 
    }; 
    
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
    int localvar_ref; 
    int localvar_nb;
    if (next_token == PSCRIPT_TOKEN_VAR) { 
      localvar_ref = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var, current_token_ref, &localvar_nb); 
      next_token = get_current_token_type(); 
    } 
    else {
      localvar_nb = 0; 
      localvar_ref = -1; 
    }; 
    
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_BEGIN); 
    int inst_nb; 
    const int inst_ref = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__begin, current_token_ref, &inst_nb); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_END); 
    
    //const int lambda_type_expr_tree = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__FUNPROC, /*descendant1*/arg_list, retval_type); 
    const int lambda_type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__FUNPROC, /*descendant1*/arg_list, retval_type); 
    // RL: TODO XXX FIXME: Is it top-level? It's definitely not in the spirit? Then... Expr? 
    //const int lambda_decl = pscript_decl_toplevel_list__push__lambda(this -> decl_toplevel, /*typing_expr*/lambda_type_expr_tree, /*localvar_nb*/localvar_nb, /*localvar_ref*/localvar_ref, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 
    //const int lambda_decl_expr = pscript_expr_tree__push3(this -> expr_tree_env, PSCRIPT_EXPR_TREE__TYPE__LAMBDA, -1, lambda_type_expr_tree, localvar_ref, inst_ref); 
    const int lambda_decl_expr = pscript_parser_chunk_env__push(this -> expr_tree_env, /*type*/PSCRIPT_EXPR_TREE__TYPE__LAMBDA, /*lambda_type_expr_tree*/lambda_type_expr_tree, /*localvar_ref*/localvar_ref, /*inst_ref*/inst_ref); 
    
    next_token = get_next_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
    return lambda_decl_expr; 
  }; // LABEL_STATE(decl_procedure) 




  
  
  
 LABEL_STATE(decl_methode): { 
    //PSCRIPT_TOKEN_IDENT of type_expr OPENPAR arg_list CLOSEPAR retval_type? PTVIRG? localvar? begin ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_METHODE); 
    do { 
      next_token = get_next_token_type(); 
    } while (next_token == PSCRIPT_TOKEN_METHODE); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    const int ident_token = *current_token_ref; 
    next_token = get_next_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_OF); 
    next_token = get_next_token_type(); 
    const int of_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_OPENPAR); 
    const int arg_list = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_CLOSEPAR); 
    next_token = get_next_token_type(); 
    int retval_type; 
    if (int_member_huh(next_token, PSCRIPT_TOKEN_DEUXPOINTS, PSCRIPT_TOKEN_AS)) { 
      next_token = get_next_token_type(); 
      retval_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
    } 
    else { 
      retval_type = -1; 
    }; 
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { 
      next_token = get_next_token_type(); 
    }; 
    int localvar_ref = -1; 
    int localvar_nb;
    if (next_token == PSCRIPT_TOKEN_VAR) { 
      localvar_ref = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var, current_token_ref, &localvar_nb); 
      next_token = get_current_token_type(); 
    } 
    else {
      localvar_nb = 0; 
    }; 
    
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { 
      next_token = get_next_token_type(); 
    }; 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_BEGIN); 
    int inst_nb;
    const int inst_ref = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__begin, current_token_ref, &inst_nb); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_END); 
    
    //const int methode_type_expr_tree = pscript_type_tree__push3(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__METHODE, /*descendant1*/of_type, arg_list, retval_type); 
    const int methode_type_expr_tree = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__METHODE, /*descendant1*/of_type, arg_list, retval_type); 
    //const int methode_decl = pscript_decl_toplevel_list__push__method(this -> decl_toplevel, /*ident*/ident_token, /*of_type*/of_type,/*typing_expr*/methode_type_expr_tree, /*localvar_nb*/localvar_nb, /*localvar_ref*/localvar_ref, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 
    const int methode_decl = pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD, /*ident*/ident_token, /*of_type*/of_type,/*typing_expr*/methode_type_expr_tree, /*localvar_nb*/localvar_nb, /*localvar_ref*/localvar_ref, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 

    
    next_token = get_next_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
    return methode_decl; 
  }; // LABEL_STATE(decl_methode) 
  
  
  
  
  
  
  
 LABEL_STATE(decl_main): { 
    //PSCRIPT_TOKEN_IDENT of type_expr OPENPAR arg_list CLOSEPAR retval_type? PTVIRG? localvar? begin ;
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_BEGIN); 
    int inst_nb; // = rand(); // = INT_MIN;
    const int inst_ref = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__begin, current_token_ref, &inst_nb); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_END); 
    
    //const int main_decl = pscript_decl_toplevel_list__push__mainproc(this -> decl_toplevel, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 
    const int main_decl = pscript_parser_chunk_env__push(this -> decl_toplevel, PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC, /*inst_nb*/inst_nb, /*inst_ref*/inst_ref); 

    
    next_token = get_next_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    //next_token = get_next_token_type(); 
      
    return main_decl; 
  }; // LABEL_STATE(decl_main) 
  




 LABEL_STATE(decl__arg_list): { 
    // OPENPAR  CLOSEPAR 
    // OPENPAR arg0 CLOSEPAR 
    // OPENPAR arg0 [; arg_i]+ CLOSEPAR 
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_OPENPAR); 
    
#if 1
    next_token = get_next_token_type(); 
    if (next_token == PSCRIPT_TOKEN_CLOSEPAR) { return -1; }; 
    //next_token = get_next_token_type(); 
    *retval1_ref = 0; // args_nb 
    const int arg_list = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_CLOSEPAR); 
#else 
    int arg_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 

    next_token = get_next_token_type(); 
    for (;;) { 
      if (next_token == PSCRIPT_TOKEN_CLOSEPAR) { break; }; 
      
      //SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_IDENT, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF, PSCRIPT_TOKEN_BYVAL); 
      const int arg_one = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_one, current_token_ref, retval1_ref); 
      //arg_list = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, /*descendants*/arg_list, arg_one); 
      arg_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, /*descendants*/arg_list, arg_one); 
      next_token = get_current_token_type(); 
      if (next_token == PSCRIPT_TOKEN_CLOSEPAR) { break; }; 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      do { 
	next_token = get_next_token_type(); 
      } while (next_token == PSCRIPT_TOKEN_PTVIRG); 
    }; // for (;;)  
#endif 
      
    return arg_list; 
  }; // LABEL_STATE(decl__arg_list) 




 LABEL_STATE(decl__arg_list__inner): { 
    int next_token; 
    //const int arg_list_empty = -1; 
    //const int arg_list_empty = pscript_type_tree__push0(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL, /*value*/-1); 
    // [var|byref|byval] ident (, ident)* : <type> 
    int byref = false; 
    int ident_list_nb = 0; 
    int ident_list_array[decl__arg_ident_list__size]; 
    //SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_IDENT, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF, PSCRIPT_TOKEN_BYVAL); 
    
    next_token = get_current_token_type(); 
    if (next_token == PSCRIPT_TOKEN_CLOSEPAR) { 
      //const int arg_list_empty = -1; 
      //const int arg_list_empty = pscript_type_tree__push0(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
      //const int arg_list_empty = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
      const int arg_list_empty = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL, /*args_nb*/*retval1_ref); 
//#ifdef DEBUG_PARSER 
#if 0
      dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", " - current_token: ", int_string__stack(*current_token_ref), " - next_token: ", next_token >= 0 && next_token < PSCRIPT_TOKEN_COUNT ? pscript_token__type_get_cstr(next_token) : "_", "(", int_string__stack(next_token), ")", " - arg_list_empty: ", int_string__stack(arg_list_empty), "\n"); 
#endif 
      //assert(false); 
      return arg_list_empty; 
    }; 
    
    if (int_member_huh(next_token, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF)) { 
      byref = true; 
      do { 
	next_token = get_next_token_type(); 
      } while (int_member_huh(next_token, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF)); 
    } 
    else if (next_token == PSCRIPT_TOKEN_BYVAL) { 
      do { 
	next_token = get_next_token_type(); // RL: Skip 
      } while (next_token == PSCRIPT_TOKEN_BYVAL); 
    }; 
    
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    
    ident_list_nb = 1; 
    ident_list_array[0] = *current_token_ref; 
    next_token = get_next_token_type(); 
    if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
      for (;;) { 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
	if (ident_list_nb == decl__arg_ident_list__size) { 
	  EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Declaring too many variables at the same time - I can handle at most ", int_string__stack(decl__arg_ident_list__size), " in one declaration. Please break down that declaration (or recompile the parser with a larger stack)."); 
	}; 
	ident_list_array[ident_list_nb] = *current_token_ref; 
	ident_list_nb++; 
	next_token = get_next_token_type(); 
	if (next_token != PSCRIPT_TOKEN_VIRGULE) { break; }; 
      }; 
    }; 
    
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_DEUXPOINTS); 
    
    next_token = get_next_token_type(); 
    const int arg_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 


    
#if 1
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
    //const int arg_tail = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
    //pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
    //const int arg_head = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
    *retval1_ref += ident_list_nb; 
    const int arg_head = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
#else 
    int arg_tail; 
    if (next_token == PSCRIPT_TOKEN_PTVIRG) { 
      arg_tail = pscript_parser__automaton__aux(exception_data, this, token_env, decl__arg_list__inner, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
    }
    else { 
#if 0 
      arg_tail = -1; 
#else 
      ///const int arg_list_empty = pscript_type_tree__push0(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
      const int arg_list_empty = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
      arg_tail = arg_list_empty; 
#endif 
    }; 
#endif 



    //int arg_head; // = -1;
    //int arg_list = arg_tail; 
    //int * ident_token_ptr = ident_list_array + ident_list_nb; 
    int * ident_token_ptr = ident_list_array; // + ident_list_nb; 
    //int arg_one[ident_list_nb]; 
    for (int i = 0; i < ident_list_nb; i++) { 
      //ident_token_ptr--; 
      //arg_list = pscript_type_tree__push3(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type, arg_list); 
      //arg_list = pscript_type_tree__push3(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type); 
      //arg_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type); 
      //const int arg_one = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type); 
      //arg_one[i] = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, /*arg_type*/-1); 
      //arg_one[i] = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, /*arg_type*/arg_type); 
      pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, /*arg_type*/arg_type); 
      //if (i == 0) { arg_head = arg_one; }; 
      ident_token_ptr++; 
    }; 




#if 0 
    for (int i = 0; i < ident_list_nb; i++) { 
      pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/arg_one[i], /*field_name_cstr*/"type_expr", /*field_value*/arg_type); 
    };
#endif  
    
    //return arg_list; 
    return arg_head; 
    //return arg_one[0]; 
  }; // LABEL_STATE(decl__arg_list__inner) 
  
  
  
#if 0   
 LABEL_STATE(decl__arg_one): { 
    // [var|byref|byval] ident (, ident)* : <type> 
    int next_token; 
    int byref = false; 
    int ident_list_nb = 0; 
    int ident_list_array[decl__arg_ident_list__size]; 
    //SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_IDENT, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF, PSCRIPT_TOKEN_BYVAL); 
    
    next_token = get_current_token_type(); 
    if (int_member_huh(next_token, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF)) { 
      byref = true; 
      do { 
	next_token = get_next_token_type(); 
      } while (int_member_huh(next_token, PSCRIPT_TOKEN_VAR, PSCRIPT_TOKEN_BYREF)); 
    } 
    else if (next_token == PSCRIPT_TOKEN_BYVAL) { 
      do { 
	next_token = get_next_token_type(); // RL: Skip 
      } while (next_token == PSCRIPT_TOKEN_BYVAL); 
    }; 

    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    
    ident_list_nb = 1; 
    ident_list_array[0] = *current_token_ref; 
    next_token = get_next_token_type(); 
    if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
      for (;;) { 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
	if (ident_list_nb == decl__arg_ident_list__size) { 
	  EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Declaring too many variables at the same time - I can handle at most ", int_string__stack(decl__arg_ident_list__size), " in one declaration. Please break down that declaration (or recompile the parser with a larger stack)."); 
	}; 
	ident_list_array[ident_list_nb] = *current_token_ref; 
	ident_list_nb++; 
	next_token = get_next_token_type(); 
	if (next_token != PSCRIPT_TOKEN_VIRGULE) { break; }; 
      }; 
    };  
    
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_DEUXPOINTS); 
    
    next_token = get_next_token_type(); 
    const int arg_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 

    if (1 == ident_list_nb) { 
      //const int arg_one = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/ident_list_array[0],/*descendant1*/byref, arg_type); 
      const int arg_one = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/ident_list_array[0],/*descendant1*/byref, arg_type); 
      return arg_one; 
    }; 
    //int arg_list = pscript_type_tree__push0(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
    int arg_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
    int * ident_token_ptr = ident_list_array + ident_list_nb; 
    for (int i = 0; i < ident_list_nb; i++) { 
      ident_token_ptr--; 
      //const int arg_one = pscript_type_tree__push2(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type); 
      const int arg_one = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE, /*value*/*ident_token_ptr,/*descendant1*/byref, arg_type); 
      //arg_list = pscript_type_tree__push1(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, /*value*/arg_one,/*descendant1*/arg_list); 
      arg_list = pscript_parser_chunk_env__push(this -> type_tree_env, /*type*/PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS, /*value*/arg_one,/*descendant1*/arg_list); 
    }; 

    //next_token = get_current_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    return arg_list; 
  }; // LABEL_STATE(decl__arg_one) 
#endif 
  











 LABEL_STATE(decl__local_var__one): { 
    // ident_list : type ; 
    int next_token; 
    int ident_list_nb = 0; 
    int ident_list_array[decl__arg_ident_list__size]; 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    
    ident_list_nb = 1; 
    ident_list_array[0] = *current_token_ref; 
    next_token = get_next_token_type(); 
    if (next_token == PSCRIPT_TOKEN_VIRGULE) { 
      for (;;) { 
	next_token = get_next_token_type(); 
	SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
	if (ident_list_nb == decl__arg_ident_list__size) { 
	  EXCEPTION__RAISE(__FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Declaring too many variables at the same time - I can handle at most ", int_string__stack(decl__arg_ident_list__size), " in one declaration. Please break down that declaration (or recompile the parser with a larger stack)."); 
	}; 
	ident_list_array[ident_list_nb] = *current_token_ref; 
	ident_list_nb++; 
	next_token = get_next_token_type(); 
	if (next_token != PSCRIPT_TOKEN_VIRGULE) { break; }; 
      }; 
    }; 
    
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_DEUXPOINTS); 
    
    next_token = get_next_token_type(); 
    const int localvar_type = pscript_parser__automaton__aux(exception_data, this, token_env, type_expr, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
#if 0     
    if (1 == ident_list_nb) { 
      //const int localvar_one = pscript_decl_local_list__push2(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, /*value*/ident_list_array[0],/*descendant1*/localvar_type); 
      const int localvar_one = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, /*value*/ident_list_array[0],/*descendant1*/localvar_type); 
      return localvar_one; 
    }; 
#endif 
    //int localvar_list = pscript_decl_local_list__push0(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL); 
    //int localvar_list = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL); 
    //int * ident_token_ptr = ident_list_array + ident_list_nb; 
    int * ident_token_ptr = ident_list_array; // + ident_list_nb; 
    //int ref_to_the_first_item_on_the_stack = -1; 
    int lvar_list = *retval1_ref; // -1; 
    for (int i = 0; i < ident_list_nb; i++) { 
      //ident_token_ptr--; 
      //const int localvar_one = pscript_decl_local_list__push2(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, /*value*/*ident_token_ptr,/*descendant1*/localvar_type); 
      //const int localvar_one = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, /*value*/*ident_token_ptr,/*descendant1*/localvar_type); 
      //const int current_ref_to_the_first_item_on_the_stack = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE, /*value*/*ident_token_ptr,/*descendant1*/localvar_type); 
      lvar_list = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE, /*value*/*ident_token_ptr,/*descendant1*/localvar_type, lvar_list); 
#if 0 
      if (ref_to_the_first_item_on_the_stack < 0) { 
	ref_to_the_first_item_on_the_stack = current_ref_to_the_first_item_on_the_stack; 
      }; 
#endif 

      //localvar_list = pscript_decl_local_list__push2(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS, /*value*/localvar_one,/*descendant1*/localvar_list); 
      //localvar_list = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS, /*value*/localvar_one,/*descendant1*/localvar_list); 
      ident_token_ptr++; 
    }; 
    //return localvar_list; 
    //*retval1_ref = ident_list_nb; 
    //return ref_to_the_first_item_on_the_stack; 
    return lvar_list; 
  }; // LABEL_STATE(decl__local_var__one) 
  
  
  
 LABEL_STATE(decl__local_var): { 
    // VAR (ident_list : type ;) + 
    // RL: I need to return the address of the first item or the last item on the stack, and the number of items. 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_VAR); 
    next_token = get_next_token_type(); 
    while (next_token == PSCRIPT_TOKEN_VAR) { next_token = get_next_token_type(); }; 
    
    //int localvar_list = pscript_decl_local_list__push0(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL); 
    //int localvar_list = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL); 
    //int localvar_tail = -1; 
    //int localvar_nb = 0; 
    //int ref_to_the_first_item_on_the_stack = -1; 
    int lvar_list = -1; 
    for (;;) { 
      //const int localvar_one = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var__one, current_token_ref, retval1_ref); 
      //const int current_ref_to_the_first_item_on_the_stack = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var__one, current_token_ref, retval1_ref); 
      lvar_list = pscript_parser__automaton__aux(exception_data, this, token_env, decl__local_var__one, current_token_ref, &lvar_list); 
#if 0 
      localvar_nb += *retval1_ref; 
      if (ref_to_the_first_item_on_the_stack < 0) { 
	ref_to_the_first_item_on_the_stack = current_ref_to_the_first_item_on_the_stack; 
      }; 
#endif 
      
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
      //localvar_list = pscript_decl_local_list__push2(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS, /*value*/localvar_one,/*descendant1*/localvar_list); 
      //localvar_list = pscript_parser_chunk_env__push(this -> decl_local, /*type*/PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS, /*value*/localvar_one,/*descendant1*/localvar_list); 
      
      next_token = get_next_token_type(); 
      if (next_token != PSCRIPT_TOKEN_IDENT) { break; }; 
    }; 
    
    //return localvar_list; 
    //*retval1_ref = localvar_nb; 
    //return ref_to_the_first_item_on_the_stack; 
    return lvar_list; 
  }; // LABEL_STATE(decl__local_var) 
  
  
  
#if 0 
//#ifdef DEBUG_PARSER 
  dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">: ", "[", pscript_parser_automaton_state__cstr[state], "]", " - current_token: ", int_string__stack(*current_token_ref), " - next_token: ", pscript_token__type_get_cstr(next_token), "(", int_string__stack(next_token), ")", "\n"); 
#endif 

 LABEL_STATE(instructions__begin): { 
    // (instructions_one ; ) * 
    int next_token; 
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_BEGIN); 
    
#if 1
    //const int begin_ref = this -> inst_list_env -> memory__array_nb; 
    next_token = get_next_token_type();
    int inst_list_head = -1; 
    for (;;) { 
      //do { next_token = get_next_token_type(); } while (next_token == PSCRIPT_TOKEN_PTVIRG); 
      while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
      if (next_token == PSCRIPT_TOKEN_END) { 
#if 1 
	return inst_list_head; 
#else 
	*retval1_ref = this -> inst_list_env -> memory__array_nb; 
	return begin_ref; 
#endif 
      }; 
      //if (next_token == PSCRIPT_TOKEN_EOF) { break; }; 
      
      //const int instruction_one = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
      inst_list_head = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, &inst_list_head); 
      
      next_token = get_current_token_type(); 
      //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_PTVIRG,PSCRIPT_TOKEN_END,PSCRIPT_TOKEN_UNTIL); 
      
      //instructions_list = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__LIST__CONS, /*value1*/instruction_one, /*value2*/instructions_list); 
    }; 
    assert(false); 
    return -1; 
#else 
    int instructions_list = pscript_inst_list__push0(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__LIST__NIL); 
    for (;;) { 
      next_token = get_next_token_type(); 
      while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
      if (next_token == PSCRIPT_TOKEN_END) { break; }; 
      //if (next_token == PSCRIPT_TOKEN_EOF) { break; }; 
      
      const int instruction_one = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
      
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
      instructions_list = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__LIST__CONS, /*value1*/instruction_one, /*value2*/instructions_list); 
    }; 
    return instructions_list; 
#endif 
  }; // LABEL_STATE(instructions__begin) 
  
  
 LABEL_STATE(instructions__expr): { 
    // GRAMMAR: expr ; 
    int next_token; 
    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_expr = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__EXPR, /*value1:expr_tree*/-1, /*inst_list_head*/*retval1_ref); 

    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
    next_token = get_current_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_PTVIRG,PSCRIPT_TOKEN_END,PSCRIPT_TOKEN_UNTIL); 
    //const int instruction_expr = pscript_inst_list__push1(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__EXPR, /*value1*/expr_tree); 
    //const int instruction_expr = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__EXPR, /*value1*/expr_tree); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_expr, /*field_name_cstr*/"expr_tree", /*field_value*/expr_tree); 
    //next_token = get_next_token_type(); 
    return instruction_expr; 
  }; // LABEL_STATE(instructions__expr) 
  
  
 LABEL_STATE(instructions__one): { 
#if 0 
instruction:
| begin    -> direct_descendants_nb (direct descendants, not nested) 
| ifnoelse -> EXPR + if_true_direct_descendants_nb 
| ifelse   -> EXPR + if_true_direct_descendants_nb + if_false_direct_descendants_nb 
| when     -> EXPR + if_true_direct_descendants_nb 
| unless   -> EXPR + if_false_direct_descendants_nb 
| while    -> EXPR + if_true_direct_descendants_nb 
| for      -> EXPR + EXPR + if_true_direct_descendants_nb 
| repeat   -> EXPR + direct_descendants_nb 
#endif 
    int next_token; 
    
    next_token = get_current_token_type(); 
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 

    int next_state; 
    switch (next_token) { 
    case PSCRIPT_TOKEN_BEGIN : next_state = instructions__begin;  break; 
    case PSCRIPT_TOKEN_IF    : next_state = instructions__if;     break; 
    case PSCRIPT_TOKEN_WHEN  : next_state = instructions__when;   break; 
    case PSCRIPT_TOKEN_UNLESS: next_state = instructions__unless; break; 
    case PSCRIPT_TOKEN_WHILE : next_state = instructions__while;  break; 
    case PSCRIPT_TOKEN_FOR   : next_state = instructions__for;    break; 
    case PSCRIPT_TOKEN_REPEAT: next_state = instructions__repeat; break; 
    default                  : next_state = instructions__expr;   break; 
      //SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_BEGIN, PSCRIPT_TOKEN_IF, PSCRIPT_TOKEN_WHEN, PSCRIPT_TOKEN_UNLESS, PSCRIPT_TOKEN_WHILE, PSCRIPT_TOKEN_FOR, PSCRIPT_TOKEN_REPEAT); 
    }; 

    const int instruction_one = pscript_parser__automaton__aux(exception_data, this, token_env, next_state, current_token_ref, retval1_ref); 
    if (next_state == instructions__begin) { 
      // RL: Otherwise, the current token points to 'END', and for an instruction, it is expected to have everything eaten - a semi-colon is expected. 
      next_token = get_next_token_type(); 
    }; 
    
    return instruction_one; 
  }; // LABEL_STATE(instructions__one) 
  
  
  
 LABEL_STATE(instructions__if): { 
    // IF expr THEN inst 
    // IF expr THEN inst ELSE inst 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IF); 
    
    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_if = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_ELSE, /*value1*/-1, /*instruction_if_true*/-1, /*instruction_if_false*/-1, /*inst_list_head*/*retval1_ref); 

    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 

    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_if, /*field_name_cstr*/"test_expr_tree", /*field_value*/expr_tree); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_THEN); 
    
    //const int instruction_if = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_ELSE, /*value1*/expr_tree, /*instruction_if_true*/-1, /*instruction_if_false*/-1); 

    next_token = get_next_token_type(); 
    *retval1_ref = -1; 
    const int instruction_if_true = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 

    next_token = get_current_token_type(); 
    while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 

    //pscript_parser_chunk_env__set_field_value(this -> inst_list_env, /*chunk_offset*/instruction_if, /*field_i*/2, /*field_value*/instruction_if_true); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_if, /*field_name_cstr*/"inst_if_true", /*field_value*/instruction_if_true); 
    
    if (next_token != PSCRIPT_TOKEN_ELSE) { 
      //const int instruction_if = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE, /*value1*/expr_tree, /*value2*/instruction_if_true); 
      //const int instruction_if = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE, /*value1*/expr_tree, /*value2*/instruction_if_true); 
      return instruction_if; 
    }; 
    
    next_token = get_next_token_type(); 
    *retval1_ref = -1; 
    const int instruction_if_false = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    //const int instruction_if = pscript_inst_list__push3(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_ELSE, /*value1*/expr_tree, /*value2*/instruction_if_true, /*value3*/instruction_if_false); 
    //const int instruction_if = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__IF_ELSE, /*value1*/expr_tree, /*value2*/instruction_if_true, /*value3*/instruction_if_false); 
    //pscript_parser_chunk_env__set_field_value(this -> inst_list_env, /*chunk_offset*/instruction_if, /*field_i*/3, /*field_value*/instruction_if_false); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_if, /*field_name_cstr*/"inst_if_false", /*field_value*/instruction_if_false); 

    return instruction_if; 
  }; // LABEL_STATE(instructions__if) 
  
  




 LABEL_STATE(instructions__when): { 
    // WHEN expr THEN|DO inst 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_WHEN); 

    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_when = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHEN, /*value1:expr_tree*/-1, /*instruction_when_true*/-1, /*inst_list_head*/*retval1_ref); 

    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 

    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_when, /*field_name_cstr*/"test_expr_tree", /*field_value*/expr_tree); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_THEN,PSCRIPT_TOKEN_DO); 
    next_token = get_next_token_type(); 
    
    //const int instruction_when = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHEN, /*value1*/expr_tree, /*instruction_when_true*/-1); 

    *retval1_ref = -1; 
    const int instruction_when_true = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    //const int instruction_when = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHEN, /*value1*/expr_tree, /*value2*/instruction_when_true); 
    //const int instruction_when = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHEN, /*value1*/expr_tree, /*value2*/instruction_when_true); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_when, /*field_name_cstr*/"inst_if_true", /*field_value*/instruction_when_true); 

    return instruction_when; 
  }; // LABEL_STATE(instructions__when) 
  
  


 LABEL_STATE(instructions__unless): { 
    // UNLESS expr DO inst 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_UNLESS); 

    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_unless = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__UNLESS, /*value1:expr_tree*/-1, /*value2: instruction_unless_false*/-1, /*inst_list_head*/*retval1_ref); 

    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_DO); 
    
    //const int instruction_unless = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__UNLESS, /*value1*/expr_tree, /*value2: instruction_unless_false*/-1); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_unless, /*field_name_cstr*/"test_expr_tree", /*field_value*/expr_tree); 
    
    next_token = get_next_token_type(); 
    *retval1_ref = -1; 
    const int instruction_unless_false = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    //const int instruction_unless = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__UNLESS, /*value1*/expr_tree, /*value2*/instruction_unless_false); 
    //const int instruction_unless = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__UNLESS, /*value1*/expr_tree, /*value2*/instruction_unless_false); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_unless, /*field_name_cstr*/"inst_if_false", /*field_value*/instruction_unless_false); 

    return instruction_unless; 
  }; // LABEL_STATE(instructions__unless) 
  
  
  
 LABEL_STATE(instructions__while): { 
    // WHILE expr DO inst 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_WHILE); 
    
    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_DO); 

    const int instruction_while = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHILE, /*value1*/expr_tree, /*value2: instruction_while_true*/-1, /*inst_list_head*/*retval1_ref); 

    next_token = get_next_token_type(); 
    *retval1_ref = -1; 
    const int instruction_while_true = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    //const int instruction_while = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHILE, /*value1*/expr_tree, /*value2*/instruction_while_true); 
    //const int instruction_while = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__WHILE, /*value1*/expr_tree, /*value2*/instruction_while_true); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_while, /*field_name_cstr*/"inst_if_true", /*field_value*/instruction_while_true); 

    return instruction_while; 
  }; // LABEL_STATE(instructions__while) 
  
  
  
 LABEL_STATE(instructions__repeat): { 
    // REPEAT inst* UNTIL expr 
    int next_token; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_REPEAT); 
    
#if 1 
    //const int begin_ref = this -> inst_list_env -> memory__array_nb; 

    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_repeat = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__REPEAT, /*value1*/-1, /*value2*/-1, /*inst_list_head*/*retval1_ref); 

    //int dfs_inst_nb = 0; 
    int inst_list_head = -1; 
    next_token = get_next_token_type(); 
    //int end_ref; 
    for (;;) { 
      //do { next_token = get_next_token_type(); } while (next_token == PSCRIPT_TOKEN_PTVIRG); 
      while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
      if (next_token == PSCRIPT_TOKEN_UNTIL) { 
	//end_ref = this -> inst_list_env -> memory__array_nb; 
	//return begin_ref; 
	break; 
      }; 
      //const int instruction_one = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
      inst_list_head = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, &inst_list_head); 
      
      //dfs_inst_nb++; 
      
      next_token = get_current_token_type(); 
      //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_PTVIRG,PSCRIPT_TOKEN_UNTIL); 
    };

    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_UNTIL); 
    
    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    //SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_PTVIRG,PSCRIPT_TOKEN_END,PSCRIPT_TOKEN_UNTIL); 
    
    //const int instruction_repeat = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__REPEAT, /*value1*/expr_tree, /*value2*/begin_ref); 
    //const int instruction_repeat = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__REPEAT, /*value1*/expr_tree, /*value2*/begin_ref); 

    //return instruction_repeat; 

    //const int end_ref = this -> inst_list_env -> memory__array_nb; 

    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_repeat, /*field_name*/"test_expr_tree", /*field_value*/expr_tree); 
    //pscript_parser_chunk_env__set_field_value(this -> inst_list_env, /*chunk_offset*/instruction_repeat, /*field_i*/2, /*field_value*/dfs_inst_nb); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_repeat, /*field_name*/"inst", /*field_value*/inst_list_head); 
    //pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_repeat, /*field_name*/"inst_list_head", /*field_value*/inst_list_head); 

    return instruction_repeat; 
#else 
    int instructions_list = pscript_inst_list__push0(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__LIST__NIL); 
    for (;;) { 
      next_token = get_next_token_type(); 
      while (next_token == PSCRIPT_TOKEN_PTVIRG) { next_token = get_next_token_type(); }; 
      if (next_token == PSCRIPT_TOKEN_UNTIL) { break; }; 
      
      const int instruction_one = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
      
      next_token = get_current_token_type(); 
      SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
      
      instructions_list = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__LIST__CONS, /*value1*/instruction_one, /*value2*/instructions_list); 
    }; 
    
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_UNTIL); 
    
    next_token = get_next_token_type(); 
    const int expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    const int instruction_repeat = pscript_inst_list__push2(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__REPEAT, /*value1*/expr_tree, /*value2*/instructions_list); 
    return instruction_repeat; 
#endif 
  }; // LABEL_STATE(instructions__repeat) 
  



 LABEL_STATE(instructions__for): { 
    // FOR ident = expr to expr [step expr] DO inst 
    // FOR ident = expr downto expr [step expr] DO inst 
    int next_token; 
    int downto = false; 

    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_FOR); 

    next_token = get_next_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_IDENT); 
    const int ident_token = *current_token_ref; 
    
    next_token = get_next_token_type(); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token,PSCRIPT_TOKEN_AFFECTATION, PSCRIPT_TOKEN_AFFECTATION_SIMPLE); 
    
    // RL: Issue here: there can be a lambda in the expr, therefore instructions could be pushed onto the instructions stack. 
    //     Thus, we have to push the instruction first. And then we rewrite it. 
    const int instruction_for = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__FOR, /*value1*/ident_token, /*init_expr_tree*/-1, /*downto*/false, /*final_expr_tree*/-1, /*step_expr_tree*/-1, /*instruction_for_true*/-1, /*inst_list_head*/*retval1_ref); 
    
    next_token = get_next_token_type(); 
    const int init_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 

    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_for, /*field_name_cstr*/"init_expr", /*field_value*/init_expr_tree); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_TO, PSCRIPT_TOKEN_DOWNTO); 
    
    if (next_token == PSCRIPT_TOKEN_DOWNTO) { 
      downto = true; 
    }; 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_for, /*field_name_cstr*/"downto_huh", /*field_value*/downto); 
    
    next_token = get_next_token_type(); 
    const int final_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 

    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_for, /*field_name_cstr*/"final_expr", /*field_value*/final_expr_tree); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_ONE_OF_THESE_TOKENS(next_token, PSCRIPT_TOKEN_DO, PSCRIPT_TOKEN_STEP); 
    
    int step_expr_tree = -1; 
    if (next_token == PSCRIPT_TOKEN_STEP) { 
      next_token = get_next_token_type(); 
      step_expr_tree = pscript_parser__automaton__aux(exception_data, this, token_env, expr, current_token_ref, retval1_ref); 
      next_token = get_current_token_type(); 
      pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_for, /*field_name_cstr*/"step_expr", /*field_value*/step_expr_tree); 
    }; 
    
    assert(next_token == PSCRIPT_TOKEN_DO); 
    
    //const int instruction_for = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__FOR, /*value1*/ident_token, init_expr_tree, downto, final_expr_tree, step_expr_tree, /*instruction_for_true*/-1); 
    
    
    next_token = get_next_token_type(); 
    *retval1_ref = -1; 
    const int instruction_for_true = pscript_parser__automaton__aux(exception_data, this, token_env, instructions__one, current_token_ref, retval1_ref); 
    
    next_token = get_current_token_type(); 
    SYNTAX__EXPECTING_THIS_TOKEN(next_token,PSCRIPT_TOKEN_PTVIRG); 
    
    //const int instruction_for = pscript_inst_list__push6(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__FOR, /*value1*/ident_token, init_expr_tree, downto, final_expr_tree, step_expr_tree, /*value2*/instruction_for_true); 
    //const int instruction_for = pscript_parser_chunk_env__push(this -> inst_list_env, /*type*/PSCRIPT_INST_LIST__TYPE__FOR, /*value1*/ident_token, init_expr_tree, downto, final_expr_tree, step_expr_tree, /*value2*/instruction_for_true); 
    pscript_parser_chunk_env__set_field_value_by_name(this -> inst_list_env, /*chunk_offset*/instruction_for, /*field_name_cstr*/"inst_if_true", /*field_value*/instruction_for_true); 
    
    return instruction_for; 
  }; // LABEL_STATE(instructions__for) 
  
  
  
  
  
  
  
  
  assert(false); 
  
  return -1; 
}; 




#if 0 
static int pscript_parser__parenthesis[] = { 
  PSCRIPT_TOKEN_BEGIN,       PSCRIPT_TOKEN_END, 
  PSCRIPT_TOKEN_OPENPAR,     PSCRIPT_TOKEN_CLOSEPAR, 
  PSCRIPT_TOKEN_OPENBRACKET, PSCRIPT_TOKEN_CLOSEBRACKET, 
  PSCRIPT_TOKEN_REPEAT,      PSCRIPT_TOKEN_UNTIL, 
  PSCRIPT_TOKEN_RECORD,      PSCRIPT_TOKEN_END 
}; 
enum { pscript_parser__parenthesis__size = ARRAY_SIZE(pscript_parser__parenthesis) }; 
#else 
static int pscript_parser__parenthesis__opening[] = { 
  PSCRIPT_TOKEN_BEGIN,       
  PSCRIPT_TOKEN_OPENPAR, 
  PSCRIPT_TOKEN_OPENBRACKET, 
  PSCRIPT_TOKEN_REPEAT, 
  PSCRIPT_TOKEN_RECORD, 
  PSCRIPT_TOKEN_OPENBRACE 
}; 
enum { pscript_parser__parenthesis__opening__size = ARRAY_SIZE(pscript_parser__parenthesis__opening) }; 

static int pscript_parser__parenthesis__closing[] = { 
  PSCRIPT_TOKEN_END, 
  PSCRIPT_TOKEN_CLOSEPAR, 
  PSCRIPT_TOKEN_CLOSEBRACKET, 
  PSCRIPT_TOKEN_UNTIL, 
  PSCRIPT_TOKEN_END, 
  PSCRIPT_TOKEN_CLOSEBRACE 
}; 
enum { pscript_parser__parenthesis__closing__size = ARRAY_SIZE(pscript_parser__parenthesis__closing) }; 

static void assert_compile__dakjdds438sd(void) { 
  ASSERT_COMPILE(pscript_parser__parenthesis__opening__size == pscript_parser__parenthesis__closing__size); 
}; 
#endif

enum { pscript_parser__parenthesis__stack_size = 48 }; // RL: Nestedness. 

extern int pscript_parser__parenthesis__check(const pscript_token_env_t * token_env) { 
  int paren_stack[pscript_parser__parenthesis__stack_size]; 
  int paren_nb = 0; 
  const int token_nb = pscript_token__get_count(token_env); 
  for (int token_i = 0; token_i < token_nb; token_i++) { 
    const int token_type = pscript_token__get_type(token_env, token_i); 
    if (int_member_array(token_type, pscript_parser__parenthesis__opening)) { 
      if (paren_nb >= pscript_parser__parenthesis__stack_size) { 
	const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); 
	const char * srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); 
	const int line1 = pscript_token__get_line1(token_env, token_i); 
	const int column0 = pscript_token__get_column0(token_env, token_i); 
	dputs_array(stderr__fileno, 
#ifdef DEBUG_PARSER 
		    "{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:token ", pscript_token__type_get_cstr(token_type), "(", int_string__stack(token_type),")", "}:", 
#endif 
		    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", "Nestedness is too large for the stack parser. Please un-nest your program or recompile the parser with a larger stack (opening token '", srcval_cstr, "')", ".", "\n"); 
	return (INT_MIN | token_i); 
      }; 
      paren_stack[paren_nb] = token_i; 
      paren_nb++; 
      continue; 
    }; 
    if (int_member_array(token_type, pscript_parser__parenthesis__closing)) { 
      int closing_paren_i = 0; while (token_type != pscript_parser__parenthesis__closing[closing_paren_i]) { closing_paren_i++; }; 
      const int expected_opening_type = pscript_parser__parenthesis__opening[closing_paren_i]; 
      if (paren_nb <= 0) { 
	const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); 
	const char * srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); 
	const int line1 = pscript_token__get_line1(token_env, token_i); 
	const int column0 = pscript_token__get_column0(token_env, token_i); 
	dputs_array(stderr__fileno, 
#ifdef DEBUG_PARSER 
"{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:token ", pscript_token__type_get_cstr(token_type), "(", int_string__stack(token_type),")", "}:", 
#endif 
		    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", "Token '", srcval_cstr, "' is a closing token and does not have any matching opening token; there should be a ", pscript_token__type_get_cstr(expected_opening_type), " token before this one.", "\n"); 
	return (INT_MIN | token_i); 
      }; 
      paren_nb--; 
      const int opening_token_i = paren_stack[paren_nb]; 
      const int actual_opening_type = pscript_token__get_type(token_env, opening_token_i); 
      int opening_paren_i = 0; while (actual_opening_type != pscript_parser__parenthesis__opening[opening_paren_i]) { opening_paren_i++; }; 
//#ifdef DEBUG_PARSER 
#if 0 
      dputs_array(stderr__fileno, "{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:token ", int_string__stack(opening_token_i), ": ", pscript_token__type_get_cstr(actual_opening_type), "(", int_string__stack(actual_opening_type),")", "}:", "'opening_paren_i' = ", int_string__stack(opening_paren_i), "\n");
#endif 
      const int expected_closing_type = pscript_parser__parenthesis__closing[opening_paren_i]; 
      //if (actual_opening_type != expected_opening_type) { 
      if (token_type != expected_closing_type) { 
	const char * opening_srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, opening_token_i); 
	const char * opening_srcval_cstr = pscript_token__get_srcval_cstr(token_env, opening_token_i); 
	const int opening_line1 = pscript_token__get_line1(token_env, opening_token_i); 
	const int opening_column0 = pscript_token__get_column0(token_env, opening_token_i); 
	const char * closing_srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); 
	const char * closing_srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); 
	const int closing_line1 = pscript_token__get_line1(token_env, token_i); 
	const int closing_column0 = pscript_token__get_column0(token_env, token_i); 
	dputs_array(stderr__fileno, 
#ifdef DEBUG_PARSER 
"{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:token ", pscript_token__type_get_cstr(token_type), "(", int_string__stack(token_type),")", "}:", 
#endif 
		    opening_srcfile_cstr, ":", int_string__stack(opening_line1), ":", int_string__stack(opening_column0), ": ", "Opening token '", opening_srcval_cstr, "' and closing token '", closing_srcval_cstr, "' (", (opening_srcfile_cstr == closing_srcfile_cstr ? "" : closing_srcfile_cstr), ":", int_string__stack(closing_line1), ":", int_string__stack(closing_column0), ") do not match. The closing token should have been a ", pscript_token__type_get_cstr(expected_closing_type), ".", "\n"); 
	return (INT_MIN | token_i); 
      }; 
      
      continue; 
    }; 
  }; 
  
  if (paren_nb == 0) { return 0; }; 
  
  
   dputs_array(stderr__fileno, 
#ifdef DEBUG_PARSER 
	      "{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">", "}:", 
#endif
	      "There are ", int_string__stack(paren_nb), " opening tokens without any matching closing tokens:", "\n"); 
  for (int i = 0; i < paren_nb; i++) { 
    const int token_i = paren_stack[i]; 
    const int token_type = pscript_token__get_type(token_env, token_i); 
    int opening_paren_i = 0; while (token_type != pscript_parser__parenthesis__opening[opening_paren_i]) { opening_paren_i++; }; 
    const int expected_closing_type = pscript_parser__parenthesis__closing[opening_paren_i]; 
    const char * opening_srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); 
    const char * opening_srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); 
    const int opening_line1 = pscript_token__get_line1(token_env, token_i); 
    const int opening_column0 = pscript_token__get_column0(token_env, token_i); 
    dputs_array(stderr__fileno, "\t", opening_srcfile_cstr, ":", int_string__stack(opening_line1), ":", int_string__stack(opening_column0), ": ", "Opening token '", opening_srcval_cstr, "' needs a ", pscript_token__type_get_cstr(expected_closing_type), " closing token.", "\n"); 
  }; 
  return (INT_MIN | *paren_stack); 
}; 
 
 

extern int pscript_parser__automaton(pscript_parser_env_t * this, const pscript_token_env_t * token_env); 


enum { pscript_parser__automaton__exception_data_size = sizeof(sigjmp_buf) + (1 << 9) }; 
int pscript_parser__automaton(pscript_parser_env_t * this, const pscript_token_env_t * token_env) { 
  int current_token = -1; 
  int retval1; 
  char exception_data[pscript_parser__automaton__exception_data_size]; 
  *((int16_t*) exception_data + 0) = sizeof(exception_data); 
  *((int16_t*) exception_data + 2) = sizeof(sigjmp_buf); 
  sigjmp_buf * setjmp_env_ptr = (sigjmp_buf *) (exception_data + 4); 
  const int setjmp_value = sigsetjmp(*setjmp_env_ptr, 0); 
  if (0 == setjmp_value) { 
    const int last_topdecl_chunk_offset = pscript_parser__automaton__aux(exception_data, this, token_env, PSCRIPT_PARSER__AUTOMATON_STATE__START, /*current_token_ref*/&current_token, /*retval1_ref*/&retval1); 
    return last_topdecl_chunk_offset; 
  } 
  else { 
    // RL: An exception was raised. 
    // RL: Basically, syntax error. 
    //dputs_array(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">:", "[", pscript_parser_automaton_state__cstr[state], "]: ", "Unexpected token ", int_string__stack(next_token), " - I was expecting a name to identify the currently being defined constant value.", "\n"); return -1; 
    const char * msg = (const char *) (exception_data + 4 + sizeof(sigjmp_buf)); 
    dputs(stderr__fileno, msg); 
    dput_eol(stderr__fileno); 
    return setjmp_value; 
  }; 
  return -1; 
}; 














// *** DATA PRINT *** 

// --- chunk -- 

int pscript_parser__chunk_env__print_one(const int filedes, const pscript_chunk_env_t * this, const int chunk_env_i, const int chunk_offset) {
  const int chunk_size  = pscript_chunk_env__get__chunk_size(this, chunk_offset); 
  const int type        = pscript_chunk_env__get__type(this, chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(this, chunk_offset) - field_shift; 
  const char * chunk_env_i__str                = int_string__stack(chunk_env_i); 
  const int    chunk_env_i__str__spaces_nb     = MAX(0, 6 - strlen(chunk_env_i__str)); 
  const int    chunk_env_chunk_offset                  = chunk_offset; 
  const char * chunk_env_chunk_offset_str              = int_string__stack(chunk_env_chunk_offset); 
  const int    chunk_env_chunk_offset_str__spaces_nb   = MAX(0, 6 - strlen(chunk_env_chunk_offset_str)); 
  const int    chunk_env_chunk_size                  = chunk_size; 
  const char * chunk_env_chunk_size_str              = int_string__stack(chunk_env_chunk_size); 
  const int    chunk_env_chunk_size_str__spaces_nb   = MAX(0, 3 - strlen(chunk_env_chunk_size_str)); 
  const int    chunk_env_type                  = type; 
  const char * chunk_env_type_str              = pscript_chunk_env__type__get_cstr(chunk_env_type); 
  const int    chunk_env_type_str__spaces_nb   = MAX(0, 18 - strlen(chunk_env_type_str)); 
  const int    chunk_env_fields_nb                  = fields_nb; 
  const char * chunk_env_fields_nb_str              = int_string__stack(chunk_env_fields_nb); 
  const int    chunk_env_fields_nb_str__spaces_nb   = MAX(0, 2 - strlen(chunk_env_fields_nb_str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, chunk_env_i__str__spaces_nb); 
  dputs(filedes, chunk_env_i__str); 
  dputs(filedes, ": CHKOFF "); 
  dput_spaces(filedes, chunk_env_chunk_offset_str__spaces_nb); 
  dputs(filedes, chunk_env_chunk_offset_str); 
  dputs(filedes, ": CHKSZ "); 
  dput_spaces(filedes, chunk_env_chunk_size_str__spaces_nb); 
  dputs(filedes, chunk_env_chunk_size_str); 
  dputs(filedes, ": TYPE "); 
  dput_spaces(filedes, chunk_env_type_str__spaces_nb); 
  dputs(filedes, chunk_env_type_str); 
  dputs(filedes, ": FIELDS_NB "); 
  dput_spaces(filedes, chunk_env_fields_nb_str__spaces_nb); 
  dputs(filedes, chunk_env_fields_nb_str); 
  dputs(filedes, " { "); 
  for (int field_i = 0; field_i < fields_nb; field_i++) { // RL: field_0 is the 'type' 
    const char * field_i__str                = int_string__stack(field_i); 
    const int    field_i__str__spaces_nb     = MAX(0, 2 - strlen(field_i__str)); 
    const char * field_name                  = pscript_chunk_env__get__field_name(this, chunk_offset, field_i + field_shift); 
    const int    field_name__spaces_nb       = MAX(0, 16 - strlen(field_name)); 
    const int    field_value                 = pscript_chunk_env__get__field_value(this, chunk_offset, field_i + field_shift); 
    const char * field_value__str            = int_string__stack(field_value); 
    const int    field_value__str__spaces_nb = MAX(0, 7 - strlen(field_value__str)); 
    if (field_i > 0) 
      dputs(filedes, " ; "); 
    dputs(filedes, "FIELD "); 
    dput_spaces(filedes, field_i__str__spaces_nb); 
    dputs(filedes, field_i__str); 
    dputs(filedes, " : "); 
    dputs(filedes, field_name); 
    dput_spaces(filedes, field_name__spaces_nb); 
    dputs(filedes, " = "); 
    dput_spaces(filedes, field_value__str__spaces_nb); 
    dputs(filedes, field_value__str); 
  }; 
  dputs(filedes, " } "); 
  
  return chunk_size; 
}; 

static void pscript_parser__chunk_env__print_stats__base(const int filedes, const char * header, const pscript_chunk_env_t * this, const int base) {
  //const int filedes = fileno(yypscriptout); 
  //dputs(filedes, "CHUNK_ENV_STACK("); 
  dputs_array(filedes, header == NULL ? "CHUNK_ENV_STACK" : header, "("); 
  dputn(filedes, this -> memory__array_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> memory__array_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void pscript_parser__chunk_env__print_stats__nb_base(const int filedes, const char * header, const int nb, const int base) {
  //const int filedes = fileno(yypscriptout); 
  //dputs(filedes, "CHUNK_ENV_STACK("); 
  dputs_array(filedes, header == NULL ? "CHUNK_ENV_STACK" : header, "("); 
  dputn(filedes, nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

void pscript_parser__chunk_env__print_stats(const int filedes, const char * header, const pscript_chunk_env_t * this) {
  pscript_parser__chunk_env__print_stats__nb_base(filedes, header, this -> memory__array_nb, this -> memory__array_size); 
}; 

void pscript_parser__chunk_env__print_nb(const int filedes, const char * header, const pscript_chunk_env_t * this, const int chunk_env_nb) {
  //const int filedes = fileno(yypscriptout); 
  //dputs(filedes, "CHUNK_ENV_STACK("); 
  dputs_array(filedes, header == NULL ? "CHUNK_ENV_STACK" : header, "("); 
  dputn(filedes, this -> memory__array_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, this -> memory__array_size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> memory__array_nb)) / ((double) this -> memory__array_size))); 
  dputs(filedes, "% ) = { \n"); 
  int chunk_offset = 0; 
  for (int i = 0; i < chunk_env_nb; i++) { 
    if (chunk_offset >= this -> memory__array_nb) { break; }; 
    dputs(filedes, "  "); 
    const int chunk_size = pscript_parser__chunk_env__print_one(filedes, this, i, chunk_offset);
    dputs(filedes, "\n"); 
    chunk_offset += chunk_size; 
    assert(chunk_size > 0); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void pscript_parser__chunk_env__print_all(const int filedes, const char * header, const pscript_chunk_env_t * this) { 
  pscript_parser__chunk_env__print_nb(filedes, header, this, INT_MAX); 
}; 


// --- parser -- 

void pscript_parser__print_all(const int filedes, const pscript_parser_env_t * this) { 
  pscript_parser__chunk_env__print_all(filedes, "PARSER::TOP-LEVEL_DECL", this -> decl_toplevel); 
  pscript_parser__chunk_env__print_all(filedes,     "PARSER::LOCAL_DECL", this -> decl_local); 
  pscript_parser__chunk_env__print_all(filedes,      "PARSER::INST_LIST", this -> inst_list_env); 
  pscript_parser__chunk_env__print_all(filedes,      "PARSER::TYPE_TREE", this -> type_tree_env); 
  pscript_parser__chunk_env__print_all(filedes,      "PARSER::EXPR_TREE", this -> expr_tree_env); 
#if 0 
TOTAL READ CHARS: 10681
TOTAL COMPUTED STRINGS: 193
TOTAL COMPUTED TOKENS: 2370
STRING_STACK(193 / 1024 = 18% ) 
STRING_MEM(1661 / 32767 = 5% ) 
TOKEN_STACK(2370 / 32767 = 7% ) 
DECL_TOPLEVEL_STACK(853 / 8192 = 10% ) 
DECL_LOCAL_STACK(214 / 8192 = 2% ) 
INST_LIST_STACK(2276 / 8192 = 27% ) 
TYPE_TREE_STACK(944 / 8192 = 11% ) 
EXPR_TREE_STACK(1141 / 4096 = 27% ) 
//----
INST_LIST_STACK(942 / 8192 = 11% ) 
//----
INST_LIST_STACK(914 / 8192 = 11% ) 
//----
TOTAL READ CHARS: 10681
TOTAL COMPUTED STRINGS: 193
TOTAL COMPUTED TOKENS: 2370
STRING_STACK(193 / 1024 = 18% ) 
STRING_MEM(1661 / 32767 = 5% ) 
TOKEN_STACK(2370 / 32767 = 7% ) 
PARSER::TOP-LEVEL_DECL(784 / 8192 = 9% ) 
PARSER::LOCAL_DECL(169 / 8192 = 2% ) 
PARSER::INST_LIST(703 / 8192 = 8% ) 
PARSER::TYPE_TREE(837 / 8192 = 10% ) 
PARSER::EXPR_TREE(5672 / 8192 = 69% ) 
#endif 
}; 

void pscript_parser__print_stats(const int filedes, const pscript_parser_env_t * this) { 
  //pscript_parser__chunk_env__print_stats__nb_base(filedes, "PARSER_TOTAL", this -> memory__array_nb, /*this -> memory__array_size*/pscript_parser_env__memory__array_size); 
  pscript_parser__chunk_env__print_stats(filedes, "PARSER -> TOP-LEVEL_DECL", this -> decl_toplevel); 
  pscript_parser__chunk_env__print_stats(filedes,     "PARSER -> LOCAL_DECL", this -> decl_local); 
  pscript_parser__chunk_env__print_stats(filedes,      "PARSER -> INST_LIST", this -> inst_list_env); 
  pscript_parser__chunk_env__print_stats(filedes,      "PARSER -> TYPE_TREE", this -> type_tree_env); 
  pscript_parser__chunk_env__print_stats(filedes,      "PARSER -> EXPR_TREE", this -> expr_tree_env); 
  int nb = 0; 
  nb += this -> decl_toplevel -> memory__array_nb; 
  nb += this -> decl_local -> memory__array_nb; 
  nb += this -> inst_list_env -> memory__array_nb; 
  nb += this -> type_tree_env -> memory__array_nb; 
  nb += this -> expr_tree_env -> memory__array_nb; 
  pscript_parser__chunk_env__print_stats__nb_base(filedes, "PARSER_TOTAL", nb, /*this -> memory__array_size*/pscript_parser_env__memory__array_size); 
}; 




// *** PRETTY PRINT *** 

#define CASE_GOTO_PRETTY_PRINT(id) case id: goto glue(label__pretty_print__,id); break;  
#define LABEL_PRETTY_PRINT(id) glue(label__pretty_print__,id) 

static int  pscript_parser__print_pretty__expr(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level); 
static int  pscript_parser__print_pretty__type(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level); 
static int  pscript_parser__print_pretty__inst(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level); 
static int  pscript_parser__print_pretty__decl_local(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level); //, const int localvar_nb); 

 
static int pscript_parser__print_pretty__expr(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level) { 
  assert(chunk_offset < this -> expr_tree_env -> memory__array_nb); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(this -> expr_tree_env, chunk_offset); 
  assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_type  = pscript_chunk_env__get__type(this -> expr_tree_env, chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(this -> expr_tree_env, chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(this -> expr_tree_env, chunk_offset, i + field_shift); 
  }; 

  int used_inst_bytes = 0; 
  
  switch (chunk_type) { 
  default: { 
#if 0 
    dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__TOKEN", "\n"); 
#endif 
    //assert(chunk_type < PSCRIPT_TOKEN_COUNT); // RL: TODO XXX FIXME: Throw an exception please. 
    assert(chunk_type < TOKEN_MAX); // RL: TODO XXX FIXME: Throw an exception please. 
    assert(0 < descendants_nb); // RL: TODO XXX FIXME: Throw an exception please. 
    if (1 == descendants_nb) {
      //dput_spaces(filedes, indent_level); 
      dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0])); 
      break;
    }; 
    if (3 == descendants_nb) { 
      if (chunk_type == PSCRIPT_TOKEN_OPENBRACKET) { 
	//dputs_array(filedes, "("); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level);
	//dputs_array(filedes, ")"); 
	dputs_array(filedes, "["); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[2], indent_level);
	dputs_array(filedes, "]"); 
      }
      else { 
	dputs_array(filedes, "("); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level);
	dputs_array(filedes, " ", pscript_token__get_srcval_cstr(token_env, fields_values[0]), " "); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[2], indent_level);
	dputs_array(filedes, ")"); 
      }; 
      break; 
    }; 
    if (2 == descendants_nb) { 
      if (int_member_array_nb(chunk_type, pscript_parser__postfix_operators__nb, pscript_parser__postfix_operators)) { 
	dputs_array(filedes, "("); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level + 2);
	dputs_array(filedes, " "); 
	dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0])); 
	dputs_array(filedes, ")"); 
      } 
      else { 
	dputs_array(filedes, "("); 
	dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0])); 
	dputs_array(filedes, " "); 
	used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level + 2);
	dputs_array(filedes, ")"); 
      }; 
      break; 
    }; 
    dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
    dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
    assert(false); // RL: TODO XXX FIXME: Throw an exception please. 

  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__FUNCALL: { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL"); 
    const int fun_subtree = fields_values[0];
    const int arg_list = fields_values[1];
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL[fun_subtree = ", int_string__stack(fun_subtree)," ; arg_list = ", int_string__stack(arg_list), "]"); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fun_subtree, indent_level);
    dputs_array(filedes, "("); 
    if (arg_list >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, arg_list, indent_level);
    }; 
    dputs_array(filedes, ")"); 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__LAMBDA:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA"); 
    const int lambda_type_expr_tree = fields_values[0]; 
    const int localvar_ref          = fields_values[1]; 
    const int inst_ref              = fields_values[2]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA[lambda_type_expr_tree = ", int_string__stack(lambda_type_expr_tree)," ; localvar_ref = ", int_string__stack(localvar_ref)," ; inst_ref = ", int_string__stack(inst_ref), "]"); 
    dputs_array(filedes, "("); 
    dputs_array(filedes, "lambda"); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, lambda_type_expr_tree, indent_level);
    if (localvar_ref >= 0) { 
      dputs_array(filedes, " var "); 
      used_inst_bytes += pscript_parser__print_pretty__decl_local(filedes, string_env, token_env, this, localvar_ref, indent_level + 4); 
    }; 
    dput_eol(filedes); dput_spaces(filedes, indent_level+2); dputs_array(filedes, "begin", "\n"); 
    used_inst_bytes = pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 4); 
#if 0 
    dputs_array(stderr__fileno, "LAMBDA: used_inst_bytes = ", int_string__stack(used_inst_bytes), "\n"); 
#endif 
    //dput_eol(filedes); dput_spaces(filedes, indent_level+2); dputs_array(filedes, "end", ")"); 
    dput_spaces(filedes, indent_level+2); dputs_array(filedes, "end", ")"); 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST"); 
    const int arglist_subtree = fields_values[0]; 
    const int arg_subtree = fields_values[1]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST[arglist_subtree = ", int_string__stack(arglist_subtree)," ; arg_subtree = ", int_string__stack(arg_subtree), "]"); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, arglist_subtree, indent_level); 
    if (arg_subtree >= 0) { 
      dputs_array(filedes, ", "); 
      used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, arg_subtree, indent_level);
    }; 
  }; break; 
  }; 
  return used_inst_bytes; 
}; 


static int pscript_parser__print_pretty__type(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level) { 
#if 0 
  dputs_array(filedes, __func__, ": ", "chunk_offset: ", int_string__stack(chunk_offset), "\n");
  { 
    static int u = 0; 
    assert(u == 0); 
    if (chunk_offset == 459) u++; 
  }; 
#endif 
  assert(chunk_offset < this -> type_tree_env -> memory__array_nb); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(this -> type_tree_env, chunk_offset); 
  assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_type  = pscript_chunk_env__get__type(this -> type_tree_env, chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(this -> type_tree_env, chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 

  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(this -> type_tree_env, chunk_offset, i + field_shift); 
  }; 
  
  int used_inst_bytes = 0; 

  switch (chunk_type) { 
  default: { 
    assert(chunk_type < PSCRIPT_TOKEN_COUNT); // RL: TODO XXX FIXME: Throw an exception please. 
    assert(3 == fields_nb); // RL: TODO XXX FIXME: Throw an exception please. 
    assert(1 == descendants_nb); // RL: TODO XXX FIXME: Throw an exception please. 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0]));     
  }; break; // RL: TODO XXX FIXME: Throw an exception please. 
  case PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_CONST: { 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0])); 
    dputs_array(filedes, ".."); 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[1])); 
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_EXPR: { 
    dputs_array(filedes, "["); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[0], indent_level);
    dputs_array(filedes, ".."); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level);
    dputs_array(filedes, "]"); 
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__ARRAY: { 
    dputs_array(filedes, "array["); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[0], indent_level);
    if (fields_values[1] != -1) { 
      dputs_array(filedes, ".."); 
      used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level);
    };
    dputs_array(filedes, "] of "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[2], indent_level);
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__RECORD: { 
    dputs_array(filedes, "Record", "\n"); 
    //used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[0], indent_level + 2);
    const int record_fields_nb = fields_values[0]; 
    const int record_fields_cons_chunk_size = GET_CHUNK_SIZE__TYPE_TREE(PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS,-1); 
    int record_offset = chunk_offset; 
    for (int i = 0; i < record_fields_nb; i++) { 
      record_offset -= record_fields_cons_chunk_size; 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, record_offset, indent_level + 2);
    }; 
    //dput_eol(filedes); 
    dput_spaces(filedes, indent_level); 
    dputs_array(filedes, "end"); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_NIL: { 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS: { 
    //dputs_array(filedes, "PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS", "\n"); 
    dput_spaces(filedes, indent_level); 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, fields_values[0])); 
    dputs_array(filedes, ": "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[1], indent_level);
    dputs_array(filedes, "; \n"); 
    //pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[2], indent_level);
    //pscript_parser__print_pretty__type(filedes, string_env, token_env, this, chunk_offset - chunk_size, indent_level);
    //dput_eol(filedes); 
    //dputs_array(filedes, "end"); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__FUNPROC: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__FUNPROC"); 
    const int arg_list = fields_values[0]; 
    const int retval_type_expr = fields_values[1]; 
    dputs(filedes, "("); 
    if (arg_list >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, arg_list, indent_level);
    }; 
    dputs(filedes, ")"); 
    if (retval_type_expr >= 0) { 
      dputs(filedes, " : "); 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, retval_type_expr, indent_level);
    }; 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__METHODE: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__METHODE"); 
    const int of_type = fields_values[0]; 
    const int arg_list = fields_values[1]; 
    const int retval_type_expr = fields_values[2]; 
#if 0 // RL: Seems to be redundant. 
    dputs(filedes, " of "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, of_type, indent_level);
#endif 
    dputs(filedes, " ("); 
    if (arg_list >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, arg_list, indent_level);
    }; 
    dputs(filedes, ")"); 
    if (retval_type_expr >= 0) { 
      dputs(filedes, " : "); 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, retval_type_expr, indent_level);
    }; 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL"); 
    const int args_nb = fields_values[0]; 
    //dputs_array(stderr__fileno, "----------------> ARGS_NB: ", int_string__stack(args_nb), "\n"); 
    //int arg_chunk_offset = chunk_offset + chunk_size; 
    const int arg_chunk_size = GET_CHUNK_SIZE__TYPE_TREE(PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE,-1); 
    int arg_chunk_offset = chunk_offset + chunk_size + args_nb * arg_chunk_size; 
    for (int i = 0; i < args_nb; i++) { 
      arg_chunk_offset -= arg_chunk_size; 
      used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, arg_chunk_offset, indent_level); 
      //arg_chunk_offset += arg_chunk_size; 
      if (i < args_nb - 1) { 
	dputs_array(filedes, "; "); 
      }; 
    }; 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS: { 
    dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS"); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE"); 
    const int ident_token = fields_values[0]; 
    const int byref_huh = fields_values[1]; 
    const int type_expr = fields_values[2]; 
    if (byref_huh) { 
      dputs(filedes, "var "); 
    }; 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, type_expr, indent_level);
    //dputs_array(filedes, ";"); 
    //pscript_parser__print_pretty__type(filedes, string_env, token_env, this, chunk_offset + chunk_size, indent_level);
  }; break; 
    
  }; 
  return used_inst_bytes; 
}; 




static int pscript_parser__print_pretty__inst(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level) { 
  assert(chunk_offset < this -> inst_list_env -> memory__array_nb); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(this -> inst_list_env, chunk_offset); 
  assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_type  = pscript_chunk_env__get__type(this -> inst_list_env, chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(this -> inst_list_env, chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  int used_inst_bytes = chunk_size; 

  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(this -> inst_list_env, chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: assert(false); break; // RL: TODO XXX FIXME: Throw an exception please. 


  case PSCRIPT_INST_LIST__TYPE__LIST__NIL: {  
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__LIST__NIL", "\n");  
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__LIST__CONS: {  
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__LIST__CONS", "\n");  
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__BEGIN: {  
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__BEGIN", "\n");  
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE: {  
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE", "\n");  
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__IF_ELSE: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__IF_ELSE", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_if_false    = fields_values[2]; 
    const int inst_list_head   = fields_values[3]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs(filedes, "if "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, test_expr_tree, indent_level);
    dputs(filedes, " then \n"); 
    used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_true, indent_level + 2); 
    if (inst_if_false >= 0) { 
      dput_spaces(filedes, indent_level); dputs(filedes, "else \n"); 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_false, indent_level + 2);
    }; 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__WHEN: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__WHEN", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs(filedes, "when "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, test_expr_tree, indent_level);
    dputs(filedes, " do \n"); 
    used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_true, indent_level + 2);
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__UNLESS: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__UNLESS", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_false    = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs(filedes, "unless "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, test_expr_tree, indent_level);
    dputs(filedes, " do \n"); 
    used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_false, indent_level + 2);
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__WHILE: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__WHILE", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs(filedes, "while "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, test_expr_tree, indent_level);
    dputs(filedes, " do \n"); 
    used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_true, indent_level + 2);
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__FOR: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__FOR", "\n");  
    const int ident_token   = fields_values[0]; 
    const int init_expr        = fields_values[1]; 
    const int downto_huh       = fields_values[2]; 
    const int final_expr       = fields_values[3]; 
    const int step_expr        = fields_values[4]; 
    const int inst_if_true     = fields_values[5]; 
    const int inst_list_head   = fields_values[6]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs_array(filedes, "for ", pscript_token__get_srcval_cstr(token_env, ident_token), " = "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, init_expr, indent_level); 
    dputs(filedes, downto_huh ? " downto " : " to "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, final_expr, indent_level); 
    if (step_expr >= 0) { 
      dputs(filedes, " step "); 
      used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, step_expr, indent_level); 
    }; 
    dputs(filedes, " do \n"); 
    used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_if_true, indent_level + 2);
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__REPEAT: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__REPEAT", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    //const int inst_nb          = fields_values[1]; 
    const int inst_ref         = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    dputs(filedes, "repeat \n"); 
#if 1 
    if (inst_ref >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
    }; 
#else 
    for (int i = 0; i < inst_nb; i++) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, chunk_offset + used_inst_bytes, indent_level + 2); 
    }; 
#endif 
    dput_spaces(filedes, indent_level); dputs(filedes, "until "); 
    used_inst_bytes += pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, test_expr_tree, indent_level);
    dputs(filedes, " ; \n"); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__EXPR: {  
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__EXPR", "\n");  
    const int expr_tree       = fields_values[0]; 
    const int inst_list_head  = fields_values[1]; 
    if (inst_list_head >= 0) { 
      used_inst_bytes += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_list_head, indent_level); 
    }; 
    dput_spaces(filedes, indent_level); 
    const int expr_used_inst_bytes = pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, expr_tree, indent_level); 
#if 0 
    if (expr_used_inst_bytes > 0) {
      dputs_array(stderr__fileno, "EXPR: expr_used_inst_bytes = ", int_string__stack(expr_used_inst_bytes), "\n"); 
    }; 
#endif 
    used_inst_bytes += expr_used_inst_bytes; 
    dputs(filedes, ";\n"); 
  }; break;  


  }; 

  return used_inst_bytes; 
}; 



static int pscript_parser__print_pretty__decl_local(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this, const int chunk_offset, const int indent_level) { 
  assert(chunk_offset < this -> decl_local -> memory__array_nb); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(this -> decl_local, chunk_offset); 
  assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
  const int chunk_type  = pscript_chunk_env__get__type(this -> decl_local, chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(this -> decl_local, chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  //int used_bytes = chunk_size; 
  int used_inst_bytes = 0; 

  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(this -> decl_local, chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: assert(false); break; // RL: TODO XXX FIXME: Throw an exception please. 

  case PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE: { 
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE", "\n"); 
    const int ident_token = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    const int tail = fields_values[2]; 
    if (tail >= 0) { 
      pscript_parser__print_pretty__decl_local(filedes, string_env, token_env, this, tail, indent_level); 
    }; 
    dputs_array(filedes, " ", pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, typing_expr_tree, indent_level);
    dputs_array(filedes, ";"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL: { 
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL", "\n"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS: { 
    dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS", "\n"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE: { 
    //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE", "\n"); 
    const int ident_token = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    dputs_array(filedes, " ", pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    used_inst_bytes += pscript_parser__print_pretty__type(filedes, string_env, token_env, this, typing_expr_tree, indent_level);
    dputs_array(filedes, ";"); 
  }; break;  

  }; 

  return used_inst_bytes; 
}; 



#if 1 


void pscript_parser__print_pretty(const int filedes, const pscript_string_env_t * string_env, const pscript_token_env_t * token_env, const pscript_parser_env_t * this) { 
  //dputs_array(filedes, __func__, "\n"); 
  int indent_level = 0; 
  { 
    int chunk_offset = 0; 
    for (;;) { 
      //dputs_array(filedes, "LOOP", "\n"); 
      if (chunk_offset >= this -> decl_toplevel -> memory__array_nb) { break; }; 
      const int chunk_size  = pscript_chunk_env__get__chunk_size(this -> decl_toplevel, chunk_offset); 
      const int chunk_type  = pscript_chunk_env__get__type(this -> decl_toplevel, chunk_offset); 
      const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
      const int fields_nb   = pscript_chunk_env__get__fields_nb(this -> decl_toplevel, chunk_offset); 
      assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
      int fields_values[fields_nb - field_shift]; 
      for (int i = 0; i < fields_nb - field_shift; i++) { 
	fields_values[i] = pscript_chunk_env__get__field_value(this -> decl_toplevel, chunk_offset, i + field_shift); 
      }; 

      switch (chunk_type) { 
      default: assert(false); break; // RL: TODO XXX FIXME: Throw an exception please. 
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE: { 
	  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
	  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "const ", pscript_token__get_srcval_cstr(token_env, fields_values[0]), " = "); 
	  //dputs_array(filedes, "<<EXPR[", int_string__stack(fields_values[1]), "]>>", ";", "\n"); 
	  pscript_parser__print_pretty__expr(filedes, string_env, token_env, this, fields_values[1], indent_level + 2);
	  dputs_array(filedes, ";", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE: { 
	  //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE", "\n"); 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "type ", pscript_token__get_srcval_cstr(token_env, fields_values[0]), " = "); 
	  //dputs_array(filedes, "<<TYPE_EXPR[", int_string__stack(fields_values[1]), "]>>", ";", "\n"); 
	  pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[1], indent_level + 2);
	  dputs_array(filedes, ";", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE: { 
	  //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE", "\n"); 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "var ", pscript_token__get_srcval_cstr(token_env, fields_values[0]), " : "); 
	  pscript_parser__print_pretty__type(filedes, string_env, token_env, this, fields_values[1], indent_level + 2);
	  dputs_array(filedes, ";", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC: { 
	  //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC", "\n"); 
	  const int end_ref = fields_values[0]; 
	  const int inst_ref0 = fields_values[1]; 
	  int inst_ref = inst_ref0; 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "begin", "\n"); 
#if 1 
	  if (inst_ref >= 0) { 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#else 
	  for (;;) { 
	    if (inst_ref >= end_ref) break; 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#endif 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "end;", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC: { 
	  //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC", "\n"); 
	  const int ident_token      = fields_values[0]; 
	  const int typing_expr_tree = fields_values[1]; 
	  const int localvar_nb      = fields_values[2]; 
	  const int localvar_ref     = fields_values[3]; 
	  const int end_ref          = fields_values[4]; 
	  const int inst_ref0         = fields_values[5]; 
	  int inst_ref = inst_ref0; 
	  dputs_array(filedes, "function ", pscript_token__get_srcval_cstr(token_env, ident_token)); 
	  pscript_parser__print_pretty__type(filedes, string_env, token_env, this, typing_expr_tree, indent_level + 2);
	  if (localvar_ref >= 0) { 
	    dputs(filedes, "\n"); 
	    dput_spaces(filedes, indent_level + 2); 
	    dputs_array(filedes, "var "); 
	    pscript_parser__print_pretty__decl_local(filedes, string_env, token_env, this, localvar_ref, indent_level + 4); 
	  }; 
	  dput_eol(filedes); 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "begin", "\n"); 
#if 1 
	  if (inst_ref >= 0) { 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#else 
	  for (;;) { 
	    if (inst_ref >= end_ref) break; 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#endif 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "end;", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD: { 
	  //dput_spaces(filedes, indent_level); dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD", "\n"); 
	  const int ident_token      = fields_values[0]; 
	  const int of_type          = fields_values[1]; 
	  const int typing_expr_tree = fields_values[2]; 
	  const int localvar_nb      = fields_values[3]; 
	  const int localvar_ref     = fields_values[4]; 
	  const int end_ref          = fields_values[5]; 
	  const int inst_ref0        = fields_values[6]; 
	  int inst_ref = inst_ref0; 
	  dputs_array(filedes, "method ", pscript_token__get_srcval_cstr(token_env, ident_token)); 
	  dputs_array(filedes, " of "); 
	  pscript_parser__print_pretty__type(filedes, string_env, token_env, this, of_type, indent_level + 2);
	  pscript_parser__print_pretty__type(filedes, string_env, token_env, this, typing_expr_tree, indent_level + 2);
	  if (localvar_ref >= 0) { 
	    dputs(filedes, "\n"); 
	    dput_spaces(filedes, indent_level + 2); 
	    dputs_array(filedes, "var "); 
	    pscript_parser__print_pretty__decl_local(filedes, string_env, token_env, this, localvar_ref, indent_level + 4); 
	  }; 
	  dput_eol(filedes); 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "begin", "\n"); 
#if 1 
	  if (inst_ref >= 0) { 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#else 
	  for (;;) { 
	    if (inst_ref >= end_ref) break; 
	    inst_ref += pscript_parser__print_pretty__inst(filedes, string_env, token_env, this, inst_ref, indent_level + 2); 
	  }; 
#endif 
	  dput_spaces(filedes, indent_level); 
	  dputs_array(filedes, "end;", "\n"); 
	}; break;  
      }; 
      chunk_offset += chunk_size; 
    }; 
    return; 
  }; 
}; 
#else 
void pscript_parser__print_pretty(const int filedes, const string_env_t * string_env, const token_env_t * token_env, const pscript_parser_env_t * this) { 
  int indent = 0; 
  { 
    int chunk_offset = 0; 
    for (;;) { 
      if (chunk_offset >= this -> memory__array_nb) { break; }; 
      const int chunk_size  = pscript_parser__chunk_env__print_one(filedes, this, i, chunk_offset); 
      const int chunk_type  = pscript_chunk_env__get__type(this, chunk_offset); 
      const int field_shift = pscript_parser_chunk_env__type__get_field_shift(type); 
      const int fields_nb   = pscript_chunk_env__get__fields_nb(this, chunk_offset) - field_shift; 
      assert(chunk_size > 0); // RL: TODO XXX FIXME: Throw an exception please. 
      int fields_values[fields_nb - field_shift]; 
      for (int i = 0; i < fields_nb - field_shift; i++) { 
	fields_values[i] = pscript_chunk_env__get__field_value(this, chunk_offset, i + field_shift); 
      }; 

      switch (chunk_type) { 
      default: assert(false); break; // RL: TODO XXX FIXME: Throw an exception please. 
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE: { 
	  dput_spaces(filedes, field_value__str__spaces_nb); 
	  dputs_array(filedes, "const ", pscript_token__get_srcval_cstr(token_env, fields_values[0]), " = ", "<<EXPR[", int_string__stack(fields_values[1]), "]>>", ";", "\n"); 
	}; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE: { }; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE: { }; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC: { }; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC: { }; break;  
	case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD: { }; break;  
      }; 

      switch (chunk_type) { 
      default: assert(false); break; // RL: TODO XXX FIXME: Throw an exception please. 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE); 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE); 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE); 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC); 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC); 
	CASE_GOTO_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD); 
      }; 
    }; 
    return; 
  }; 


 
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE): { 
  (type) == PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE ? ((field_i) == 0 ? "type" : (field_i) == 1 ? "ident_token" : (field_i) == 2 ? "expr_tree"        : NULL) : \

  }; 
  
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE): { 
  }; 
  
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE): {
  }; 
  
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC): { 
  }; 
  
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC): { 
  }; 
  
 LABEL_PRETTY_PRINT(PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD): { 
  }; 
  
  
    

    
    
    
    
    
    
    chunk_offset += chunk_size; 
  }; 
  dputs(filedes, "} \n"); 
}; 
#endif 






