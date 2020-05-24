#include "pscript_global.h"
#include "pscript.h"
#include "pscript_chunk.h"
#include "pscript_token_type.h"
#include "pscript_parser.h"
#include "pscript_code_parser.h"
#include "pscript_token.h"
#include "pscript_exception.h"
#include "pvm_bytecode__type.ci" 

#include "pscript_parser.ci"

#define PSCRIPT_CODE_PARSER__DEBUG 1
#define PSCRIPT_CODE_PARSER__ABORT_INSTEAD_OF_EXCEPTION 1 

static pscript_exception_env_t * pscript_code_parser__exception_env = NULL; 



#define SYNTAX__EXPECTING_THIS_TOKEN(given_token_type,expected_token_type) { \
  if (given_token_type != expected_token_type) {			\
  const int token_i = *current_token_ref;				\
  const int token_type = pscript_token__get_type(token_env, token_i);	\
  const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); \
  const char * srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); \
  const int line1 = pscript_token__get_line1(token_env, token_i);	\
  const int column0 = pscript_token__get_column0(token_env, token_i);	\
  const char * msg_array[] = {						\
    SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION  \
    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
    "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting token: ", pscript_token__type_get_cstr(expected_token_type), "(", int_string__stack(expected_token_type), ")" }; \
  const char * msg = strconcat__stack_nb(ARRAY_SIZE(msg_array), msg_array);			\
  EXCEPTION__RAISE__(msg);					\
  };									\
  }; 

#define TOKEN_LOCATION__INLINE(token_i) token_i

#define SYNTAX__EXPECTING_THIS_TOKEN(given_token_type,expected_token_type) { \
  if (given_token_type != expected_token_type) {			\
  const int token_i = *current_token_ref;				\
  const int token_type = pscript_token__get_type(token_env, token_i);	\
  const char * srcfile_cstr = pscript_token__get_srcfile_cstr(token_env, token_i); \
  const char * srcval_cstr = pscript_token__get_srcval_cstr(token_env, token_i); \
  const int line1 = pscript_token__get_line1(token_env, token_i);	\
  const int column0 = pscript_token__get_column0(token_env, token_i);	\
  const char * msg_array[] = {						\
    SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION  \
    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
    "Unexpected token: ", pscript_token__type_get_cstr(given_token_type), "(", int_string__stack(given_token_type), ") -- Was expecting token: ", pscript_token__type_get_cstr(expected_token_type), "(", int_string__stack(expected_token_type), ")" }; \
  const char * msg = strconcat__stack_nb(ARRAY_SIZE(msg_array), msg_array); \
  EXCEPTION__RAISE__(msg);						\
  };									\
  }; 




#define EXCEPTION_CODE PSCRIPT_EXCEPTION_TYPE__CODE_PARSER
#if PSCRIPT_CODE_PARSER__ABORT_INSTEAD_OF_EXCEPTION >= 1
#define EXCEPTION_RAISE__ (code,msg,data_size,data)			\
  dputs(stderr__fileno, (msg)); dput_eol(stderr__fileno); abort(); 
#else
#define EXCEPTION_RAISE__ (code,msg,data_size,data)			\
  pscript_exception__raise(pscript_code_parser__exception_env, (code), (msg), (data_size), (data)) 
#endif

#if PSCRIPT_CODE_PARSER__DEBUG <= 0 
#define EXCEPTION_RAISE(...)						\
  pscript_exception__raise(pscript_code_parser__exception_env, EXCEPTION_CODE, strconcat__stack(__VA_ARGS__), 0, NULL) 
#else 
#define EXCEPTION_RAISE(...)						\
  pscript_exception__raise(pscript_code_parser__exception_env, EXCEPTION_CODE, strconcat__stack(__FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", __VA_ARGS__), 0, NULL) 
#endif 

#if PSCRIPT_CODE_PARSER__DEBUG >= 1 
#define SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION "{", __FILE__ ":" STRINGIFY(__LINE__) ":<",__func__, ">}: ",
#else 
#define SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION 
#endif 

#if PSCRIPT_CODE_PARSER__ABORT_INSTEAD_OF_EXCEPTION >= 1
#define RAISE_OR_ABORT dputs(stderr__fileno, msg); dput_eol(stderr__fileno); abort(); 
#else 
#define RAISE_OR_ABORT pscript_exception__raise(pscript_code_parser__exception_env, /*code*/EXCEPTION_CODE, msg, /*data_size*/0, /*data*/NULL); 
#endif 

#define EXCEPTION_RAISE_AT_TOKEN(given_token_i,...) {			\
  const int token_i_        = (given_token_i);				\
  const int token_type      = pscript_token__get_type        (this -> token_env, token_i_); \
  const char * srcfile_cstr = pscript_token__get_srcfile_cstr(this -> token_env, token_i_); \
  const char * srcval_cstr  = pscript_token__get_srcval_cstr (this -> token_env, token_i_); \
  const int line1           = pscript_token__get_line1       (this -> token_env, token_i_); \
  const int column0         = pscript_token__get_column0     (this -> token_env, token_i_); \
  const char * msg_array[]  = {						\
    SYNTAX__EXPECTING_THIS_TOKEN__SRC_LOCATION  \
    srcfile_cstr, ":", int_string__stack(line1), ":", int_string__stack(column0), ": ", \
    "Code generation: ",__VA_ARGS__  };					\
  const char * msg          = strconcat__stack_nb(ARRAY_SIZE(msg_array), msg_array); \
  RAISE_OR_ABORT;							\
  }; 

#if PSCRIPT_CODE_PARSER__ABORT_INSTEAD_OF_EXCEPTION >= 1
#define EXCEPTION_ASSERT(cond)						\
  if (!(cond)) {							\
    dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "assertion failed: \"", #cond, "\"", "\n"); \
    abort();								\
  }; 
#else 
#define EXCEPTION_ASSERT(cond)						\
  if (!(cond)) {							\
    EXCEPTION_RAISE(__FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "assertion failed: \"", #cond, "\""); \
  };
#endif 


 
// In order to simulate inherited attributes. 
//static int byref_huh__global = false; 
//static int env_type__global = -1; 
static int    funproc_i_being_called__global = -1; 
static int    ffiproc_i_being_called__global = -1; 
static int    libproc_i_being_called__global = -1; 
static int lambdaproc_i_being_called__global = -1; 
static int arg_i__global = -1; 
static int    funproc_i_being_compiled__global = -1; 
//static int lambdaproc_i_being_compiled__global = -1; 
static int decl_type__def_or_alias__global = -1; 
static int menu_i__global = -1; 



static int chaine_p__pscript__analyze_and_convert(const int src_len, const char src[], const int dest_size, char * dest) {
  assert(dest_size >= src_len + 1); 
  const char * p_src = src; 
  char * p_dest = dest; 
  p_src++; 
  for (;;) {
    if (*p_src == '\0') break; 
    *p_dest = *p_src; 
    p_src++; 
    p_dest++; 
  }; 
  p_dest--; 
  *p_dest = '\0'; 
  return 0; 
}; 

static int chaine_c__analyze_and_convert(const int src_len, const char src[], const int dest_size, char * dest) {
  assert(dest_size >= src_len + 1); 
  return chaine_p__pscript__analyze_and_convert(src_len, src, dest_size, dest); 
};

static int chaine_pstr__pscript__analyze_and_convert(pscript_string_env_t * string_env, const int pstr) { 
  const char * src_cstr = pscript_string__get_cstr(string_env, pstr); 
  const int src_len  = pscript_string__get_len(string_env, pstr); 
  const int dest_size = src_len + 1; 
  char dest[dest_size]; 
  chaine_p__pscript__analyze_and_convert(src_len, src_cstr, dest_size, dest);
  const int dest_pstr = pscript_string__push(string_env, dest); 
  return dest_pstr; 
}; 










static int pscript_code_parser__expr_token(pscript_t * this, const pscript_parser_env_t * syntax_env, const int byref_huh__global, const int token_i) { 
  const int    token_type        = pscript_token__get_type(this -> token_env, token_i); 
  const char * token_srcval_cstr = pscript_token__get_srcval_cstr(this -> token_env, token_i); 
  const int    token_srcval_pstr = pscript_token__get_srcval_pstr(this -> token_env, token_i); 
  switch (token_type) { 
    case PSCRIPT_TOKEN_TRUE: { 
      if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int type_i = pscript_type__get__bool(this -> type_env); 
      pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
      pscript_bytecode__push_bool(this -> bytecode_env, true); 
    }; break; 
  case PSCRIPT_TOKEN_FALSE: { 
      if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int type_i = pscript_type__get__bool(this -> type_env); 
      pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
      pscript_bytecode__push_bool(this -> bytecode_env, false); 
  }; break; 
  case PSCRIPT_TOKEN_ENTIER: { 
      if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int type_i = pscript_type__get__integer(this -> type_env); 
      const int value = atoi(token_srcval_cstr); 
      pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
      pscript_bytecode__push_integer(this -> bytecode_env, value); 
   }; break; 
  case PSCRIPT_TOKEN_REEL__VIRG: 
  case PSCRIPT_TOKEN_REEL__DOT: 
  case PSCRIPT_TOKEN_REEL__E: 
    {
      if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int type_i = pscript_type__get__float(this -> type_env); 
      const float value = atof(token_srcval_cstr); 
      pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
      pscript_bytecode__push_float(this -> bytecode_env, value); 
   }; break; 
  case PSCRIPT_TOKEN_CHAINE_C: 
  case PSCRIPT_TOKEN_CHAINE_P: 
    { 
      if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
      const int pstr = token_srcval_pstr; 
      const int filtered_pstr = chaine_pstr__pscript__analyze_and_convert(this -> string_env, pstr); 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int type_i = pscript_type__get__string(this -> type_env); 
      pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
      pscript_bytecode__push_string(this -> bytecode_env, filtered_pstr); 
    }; break; 
    
label_case_PSCRIPT_TOKEN_IDENT: 
  case PSCRIPT_TOKEN_IDENT: { 
    const int ident_i = pscript_token__get_srcval_pstr(this -> token_env, token_i); 
    const int   env_i = pscript_env__lookup(this -> env, ident_i); 
    if (env_i < 0) { EXCEPTION_RAISE_AT_TOKEN(token_i, "The identifier '", pscript_string__get_cstr(this -> string_env, ident_i), "' has not been defined yet."); }; 
    const int env_type   = pscript_env__get_type  (this -> env, env_i); 
    const int env_value1 = pscript_env__get_value (this -> env, env_i); 
    const int env_value2 = pscript_env__get_value2(this -> env, env_i); 
    if (byref_huh__global) { 
      do { 
	if (env_type == PSCRIPT_ENV__VAR) {
	  const int var_i          = env_value1; 
	  const int subtype_i      = pscript_var__get_type(this -> var_env, var_i); 
	  const int subtype_i_core = pscript_type__get_core_type(this -> type_env, subtype_i); 
	  const int var_offset = pscript_var__get_mem_offset(this -> var_env, var_i); 
	  if ((subtype_i_core == PSCRIPT_TYPE__VARIANT) && (subtype_i != pscript_type__get__variant(this -> type_env))) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD); 
	    pscript_bytecode__push_int8       (this -> bytecode_env,              subtype_i); 
	    pscript_bytecode__push_mem_offset (this -> bytecode_env,             var_offset); 
	    break; 
	  }; 
	  //const int type_i   = pscript_type__make__pointer(this -> type_env, subtype_i); 
	  const int type_i     = pscript_type__make__proxy(this -> type_env, subtype_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, type_i); 
	  pscript_bytecode__push_mem_offset (this -> bytecode_env, var_offset); 
	  break; 
	};
      
	if (env_type == PSCRIPT_ENV__RETVAL) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  //const int retval_i = env_value;
	  const int retval_type   = pscript_funproc__get_retval_type  (this -> funproc_env, funproc_i); //, retval_i);
	  const int retval_offset = pscript_funproc__get_retval_offset(this -> funproc_env, funproc_i); //, retval_i);
	  const int retval_sizeof = pscript_funproc__get_retval_sizeof(this -> funproc_env, funproc_i); //, retval_i);
	  const int subtype = retval_type; 
	  //const int lea_type_i = pscript_type__make__pointer(this -> type_env, subtype); 
	  const int lea_type_i = pscript_type__make__proxy(this -> type_env, subtype); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__LEA_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, lea_type_i); 
	  //pscript_bytecode__push_integer    (this -> bytecode_env, retval_offset); 
	  pscript_bytecode__push_mem_offset (this -> bytecode_env, retval_offset); 
	  break; 
	};
	
	if (env_type == PSCRIPT_ENV__METHOD_THIS) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  //const int retval_i = pscript_env__get_value(this -> env, env_i); 
	  const int method_this__offset = pscript_funproc__get_method_this__offset(this -> funproc_env, funproc_i); // methodof_i);
	  const int method_this__sizeof = pscript_funproc__get_method_this__sizeof(this -> funproc_env, funproc_i); // methodof_i);
	  const int method_this__type   = pscript_funproc__get_method_this__type  (this -> funproc_env, funproc_i); // methodof_i); 
#if 1 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, method_this__type); 
	  pscript_bytecode__push_mem_offset    (this -> bytecode_env, method_this__offset); 
#else 
	  const int subtype             = method_this__type; 
	  const int lea_type_i          = pscript_type__make__proxy(this -> type_env, subtype); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__LEA_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, method_this__type); 
	  pscript_bytecode__push_integer    (this -> bytecode_env, method_this__offset); 
#endif 
	  break; 
	}; 
	
	if (env_type == PSCRIPT_ENV__ARG) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  const int arg_i      = env_value2;
	  const int arg_type   = pscript_funproc__get_arg_type  (this -> funproc_env, funproc_i, arg_i); 
	  const int arg_offset = pscript_funproc__get_arg_offset(this -> funproc_env, funproc_i, arg_i); 
	  const int arg_sizeof = pscript_funproc__get_arg_sizeof(this -> funproc_env, funproc_i, arg_i); 
	  const int arg_byref  = pscript_funproc__get_arg_byref (this -> funproc_env, funproc_i, arg_i); 
	  const int subtype = arg_type; 
	  //const int lea_type_i = pscript_type__make__pointer(this -> type_env, subtype); 
	  const int lea_type_i = pscript_type__make__proxy(this -> type_env, subtype); 
	  if (arg_byref) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	  } 
	  else {
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__LEA_LOCAL); 
	  };
	  pscript_bytecode__push_int8         (this -> bytecode_env, lea_type_i); 
	  //pscript_bytecode__push_integer      (this -> bytecode_env, arg_offset); 
	  pscript_bytecode__push_mem_offset (this -> bytecode_env, arg_offset); 
	  break; 
	};
      
	if (env_type == PSCRIPT_ENV__VAR_LOCAL) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  const int localvar_i = env_value2;
	  const int localvar_type   = pscript_funproc__get_localvar_type(this -> funproc_env, funproc_i, localvar_i);
	  const int localvar_offset = pscript_funproc__get_localvar_offset(this -> funproc_env, funproc_i, localvar_i);
	  const int localvar_sizeof = pscript_funproc__get_localvar_sizeof(this -> funproc_env, funproc_i, localvar_i);
	  const int subtype = localvar_type; 
	  //const int lea_type_i = pscript_type__make__pointer(this -> type_env, subtype); 
	  const int lea_type_i = pscript_type__make__proxy(this -> type_env, subtype); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__LEA_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, lea_type_i); 
	  //pscript_bytecode__push_integer    (this -> bytecode_env, localvar_offset); 
	  pscript_bytecode__push_mem_offset (this -> bytecode_env, localvar_offset); 
	  break; 
	};
      
	EXCEPTION_RAISE("The identifier '", pscript_string__get_cstr(this -> string_env, ident_i),"' should be a variable and is actually a '", pscript_env__type__get_cstr(this -> env, env_type), "'."); 
      } while (false); 
    } 
    else { // NOT(byref_huh__global) 
      do { 
	if (env_type == PSCRIPT_ENV__CONST) { 
	  //pmesslog("Loading a const value onto the stack." "\n"); 
	  const int const_i = env_value1; 
	  //pmesslog("env_i = %d"   "\n", env_i); 
	  //pmesslog("const_i = %d" "\n", const_i); 
	  const int const_type = pscript_const__get_type(this -> const_env, const_i); 
	  //pmesslog("const_type = %d" "\n", const_type); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, const_type); 
	  pscript_bytecode__push_const      (this -> bytecode_env, this, const_i); 
	  break; 
	};
	
	if (env_type == PSCRIPT_ENV__VAR_LOCAL) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  const int localvar_i = env_value2; //pscript_env__get_value(this -> env, env_i);
	  const int localvar_offset = pscript_funproc__get_localvar_offset(this -> funproc_env, funproc_i, localvar_i);
	  const int localvar_sizeof = pscript_funproc__get_localvar_sizeof(this -> funproc_env, funproc_i, localvar_i);
	  const int localvar_type = pscript_funproc__get_localvar_type(this -> funproc_env, funproc_i, localvar_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, localvar_type); 
	  pscript_bytecode__push_mem_offset    (this -> bytecode_env, localvar_offset); 
	  break; 
	}; 
	
	if (env_type == PSCRIPT_ENV__RETVAL) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i              = funproc_i_being_compiled__global; 
	  //const int retval_i = pscript_env__get_value(this -> env, env_i); 
	  const int retval_offset          = pscript_funproc__get_retval_offset(this -> funproc_env, funproc_i); // retval_i);
	  const int retval_sizeof          = pscript_funproc__get_retval_sizeof(this -> funproc_env, funproc_i); // retval_i);
	  const int retval_type            = pscript_funproc__get_retval_type  (this -> funproc_env, funproc_i); // retval_i); 
	  const int retval_type_scalar_huh = pscript_type__scalar_huh(this -> type_env, retval_type); 
	  if (retval_type_scalar_huh) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	    pscript_bytecode__push_int8       (this -> bytecode_env,                  retval_type); 
	    //pscript_bytecode__push_integer    (this -> bytecode_env,                retval_offset); 
	    pscript_bytecode__push_mem_offset (this -> bytecode_env, retval_offset); 

	  } 
	  else { 
	    const int subtype    = retval_type; 
	    const int lea_type_i = pscript_type__make__proxy(this -> type_env, subtype); 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__LEA_LOCAL); 
	    pscript_bytecode__push_int8       (this -> bytecode_env,              lea_type_i); 
	    //pscript_bytecode__push_integer    (this -> bytecode_env,           retval_offset); 
	    pscript_bytecode__push_mem_offset (this -> bytecode_env, retval_offset); 
	  }; 
	  break; 
	}; 
	
	if (env_type == PSCRIPT_ENV__METHOD_THIS) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i = funproc_i_being_compiled__global; 
	  //const int retval_i = pscript_env__get_value(this -> env, env_i); 
#if 1 
	  const int method_this__offset = pscript_funproc__get_method_this__offset(this -> funproc_env, funproc_i); // methodof_i);
	  const int method_this__sizeof = pscript_funproc__get_method_this__sizeof(this -> funproc_env, funproc_i); // methodof_i);
	  const int method_this__type   = pscript_funproc__get_method_this__type  (this -> funproc_env, funproc_i); // methodof_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, method_this__type); 
	  pscript_bytecode__push_mem_offset    (this -> bytecode_env, method_this__offset); 
#else 
	  const int methodof_offset = pscript_funproc__get_method_of__offset(this -> funproc_env, funproc_i); // methodof_i);
	  const int methodof_sizeof = pscript_funproc__get_method_of__sizeof(this -> funproc_env, funproc_i); // methodof_i);
	  const int methodof_type   = pscript_funproc__get_method_of__type  (this -> funproc_env, funproc_i); // methodof_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, methodof_type); 
	  pscript_bytecode__push_integer    (this -> bytecode_env, methodof_offset); 
#endif 
	  break; 
	}; 

	if (env_type == PSCRIPT_ENV__ARG) {
	  //const int funproc_i = pscript_env__get_current_funproc(this -> env); 
	  const int funproc_i  = funproc_i_being_compiled__global; 
	  const int arg_i      = env_value2; 
	  const int arg_offset = pscript_funproc__get_arg_offset(this -> funproc_env, funproc_i, arg_i);
	  const int arg_sizeof = pscript_funproc__get_arg_sizeof(this -> funproc_env, funproc_i, arg_i);
	  const int arg_byref  = pscript_funproc__get_arg_byref (this -> funproc_env, funproc_i, arg_i); 
	  const int arg_type   = pscript_funproc__get_arg_type(this -> funproc_env, funproc_i, arg_i); 
	  if (arg_byref) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	    //const int lea_type_i = pscript_type__make__pointer(this -> type_env, arg_type); 
	    const int lea_type_i = pscript_type__make__proxy(this -> type_env, arg_type); 
	    pscript_bytecode__push_int8(this -> bytecode_env, lea_type_i); 
	    //pscript_bytecode__push_integer(this -> bytecode_env, arg_offset); 
	    pscript_bytecode__push_mem_offset (this -> bytecode_env, arg_offset); 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD__STACK); 
	  } 
	  else {
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
	    pscript_bytecode__push_int8(this -> bytecode_env, arg_type); 
	    //pscript_bytecode__push_integer(this -> bytecode_env, arg_offset); 
	    pscript_bytecode__push_mem_offset (this -> bytecode_env, arg_offset); 
	  };
	  break; 
	}; 
    
	if (env_type == PSCRIPT_ENV__VAR) {
	  const int var_i                = env_value1; 
	  const int var_offset           = pscript_var__get_mem_offset(this -> var_env, var_i); 
	  const int var_type             = pscript_var__get_type(this -> var_env, var_i); 
	  const int var_type__scalar_huh = pscript_type__scalar_huh(this -> type_env, var_type); 
	  if (var_type__scalar_huh) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD); 
	    pscript_bytecode__push_int8       (this -> bytecode_env, var_type); 
	    pscript_bytecode__push_mem_offset    (this -> bytecode_env, var_offset); 
	  } 
	  else { 
	    const int proxy_type = pscript_type__make__proxy(this -> type_env, var_type); 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	    pscript_bytecode__push_int8       (this -> bytecode_env, proxy_type); 
	    pscript_bytecode__push_mem_offset    (this -> bytecode_env, var_offset); 
	  }; 
	  break; 
	}; 

	if (env_type == PSCRIPT_ENV__FUNPROC) {
	  //const int type_i       = pscript_type__get__integer(this -> type_env); 
	  const int funproc_i      = pscript_env__get_value(this -> env, env_i); 
	  const int funproc_offset = pscript_funproc__get_offset(this -> funproc_env, funproc_i); 
	  //dputs_array(stderr__fileno, "funproc_offset: ", int_string__stack(funproc_offset), "\n"); 
	  const int funproc_type   = pscript_funproc__get_type(this -> funproc_env, funproc_i); 
	  const int proxy_type_i   = pscript_type__make__proxy_bytecode(this -> type_env, funproc_type); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	  pscript_bytecode__push_int8       (this -> bytecode_env, proxy_type_i); 
	  pscript_bytecode__push_bytecode_offset(this -> bytecode_env, funproc_offset); 
	  break; 
	}; 

	if (env_type == PSCRIPT_ENV__FFIPROC) {
	  //const int type_i = pscript_type__get__integer(this -> type_env); 
	  const int ffiproc_i      = pscript_env__get_value(this -> env, env_i); 
	  //const int ffiproc_offset = pscript_ffiproc__get_offset(this -> ffiproc_env, ffiproc_i); 
	  const int ffiproc_type_i = pscript_ffiproc__get_type(this -> ffiproc_env, ffiproc_i); 
	  //const int proxy_type_i   = pscript_type__make__proxy(this -> type_env, ffiproc_type_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	  //pscript_bytecode__push_int8(this -> bytecode_env, proxy_type_i); 
	  pscript_bytecode__push_int8(this -> bytecode_env, ffiproc_type_i); 
	  //pscript_bytecode__push_integer(this -> bytecode_env, ffiproc_offset); 
	  pscript_bytecode__push_int16(this -> bytecode_env, ffiproc_i); 
	  break; 
	}; 

	if (env_type == PSCRIPT_ENV__LIBPROC) {
	  //const int type_i = pscript_type__get__integer(this -> type_env); 
	  const int libproc_i      = pscript_env__get_value(this -> env, env_i); 
	  //const int libproc_offset = pscript_libproc__get_offset(this -> libproc_env, libproc_i); 
	  const int libproc_type_i = pscript_libproc__get_type(this -> libproc_env, this -> type_env, libproc_i); 
	  //const int proxy_type_i   = pscript_type__make__proxy(this -> type_env, libproc_type_i); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	  //pscript_bytecode__push_int8(this -> bytecode_env, proxy_type_i); 
	  pscript_bytecode__push_int8(this -> bytecode_env, libproc_type_i); 
	  //pscript_bytecode__push_integer(this -> bytecode_env, libproc_offset); 
	  pscript_bytecode__push_int16(this -> bytecode_env, libproc_i); 
	  break; 
	}; 

	EXCEPTION_RAISE_AT_TOKEN(token_i, "Expression code generation: The identifier '", pscript_string__get_cstr(this -> string_env, ident_i), "' is a '", pscript_env__type__get_cstr(this -> env, env_type), "'."); 
    
      } while (false); 
    }; 
  }; break; 
    
  case PSCRIPT_TOKEN_NIL: { 
    if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
    const int type_i = pscript_type__get__nil_list(this -> type_env); 
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    //pscript_bytecode__push_int8(this -> bytecode_env, 0); 
    //pscript_bytecode__push_pointer_nil(this -> bytecode_env); 
  }; break; 
    
  case PSCRIPT_TOKEN_NULL_PTR: { 
    if (byref_huh__global) { EXCEPTION_RAISE("I cannot compute the address of a constant value."); }; 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
    const int type_i = pscript_type__get__nil_ptr(this -> type_env); 
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    pscript_bytecode__push_mem_offset(this -> bytecode_env, 0); 
    //pscript_bytecode__push_pointer_nil(this -> bytecode_env); 
  }; break; 
    
  default: { 
    if (PSCRIPT_TOKEN__SYNTAX_MASTER__BASE <= token_type && token_type < PSCRIPT_TOKEN__SYNTAX_KEYWORD__TOP) { goto label_case_PSCRIPT_TOKEN_IDENT; }; 
    EXCEPTION_ASSERT(false); 
  }; break; 
  };
  
  return 0; 
}; 


static int pscript_code_parser__expr__get_ident_i(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_TOKEN_IDENT); 
  const int token_i     = fields_values[0]; 
  const int ident_i     = pscript_token__get_srcval_pstr(this -> token_env, token_i); 
  return ident_i; 
}; 


// RL: It returns how many expressions it has pushed onto the stack. 
static int pscript_code_parser__expr(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int byref_huh__global); 
static int pscript_code_parser__expr_const(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int please_try_but_do_not_fail); 
static int pscript_code_parser__inst(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset); 
static int pscript_code_parser__type(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset); 
// RL: Needed to compile lambda expressions. 
static void pscript_code_parser__decl_toplevel__funproc_decl(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int of_type_i, const int typing_expr_tree, const int localvar_ref); 
static void pscript_code_parser__decl_toplevel__funproc_def(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int inst_ref0, const int lambda_huh); 



static int pscript_code_parser__expr__arrow_rhs(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_EXPR_TREE__TYPE__FUNCALL); 
  const int fun_subtree = fields_values[0]; // RL: We're a method - therefore it must be an ident. 
  const int arg_list    = fields_values[1]; 
  const int ident_i     = pscript_code_parser__expr__get_ident_i(this, syntax_env, fun_subtree); 

  int pushed_arg_nb = 0; 
  if (arg_list >= 0) { 
    // byref? 
    pushed_arg_nb = pscript_code_parser__expr(this, syntax_env, arg_list, false); 
  }; 
  
  const int    method_ident_pstr = ident_i; 
  const char * method_ident_cstr = pscript_string__get_cstr(this -> string_env, method_ident_pstr); 
  const int arg_nb = pushed_arg_nb;  
  //pmessage("CALLING METHOD '%s' with %d arguments. " "\n", method_ident_cstr, arg_nb); 
  //dputs_array(stderr__fileno,  __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "CALLING METHOD '", method_ident_cstr, "' with ", int_string__stack(arg_nb), " arguments.", "\n"); 
  const int on_stack_arg_nb = 1 + pushed_arg_nb; 
  // CALLING CONVENTION: POSTFIX 
  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CALL_METHOD); 
  pscript_bytecode__push_int8       (this -> bytecode_env,           on_stack_arg_nb); 
  pscript_bytecode__push_int16      (this -> bytecode_env,         method_ident_pstr); 
  
  // RL: The type of object is run-time computed. Therefore, we can't do the look up now. 
  //     And we can't know whether any data will be returned on the stack. 
  
  return 0; 
}; 



static int pscript_code_parser__expr__arg_list__get_nb(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  if (chunk_offset < 0) { return 0; }; 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: return 1; break; 
    
  case PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST"); 
    const int arglist_subtree = fields_values[0]; 
    const int arg_subtree     = fields_values[1]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST[arglist_subtree = ", int_string__stack(arglist_subtree)," ; arg_subtree = ", int_string__stack(arg_subtree), "]"); 
    int values_nb = 0; 
    values_nb += pscript_code_parser__expr__arg_list__get_nb(this, syntax_env, arglist_subtree); 
    values_nb += pscript_code_parser__expr__arg_list__get_nb(this, syntax_env, arg_subtree); 
    return values_nb; 
  }; 
  }; 
    
  return -1; 
}; 

static int pscript_code_parser__expr__arg_i__aux(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, int * arg_i_ref, const int byref_huh) { 
  //dputs_array(stderr__fileno, "---------------------------------------------", __func__, ": arg_i: ", int_string__stack(*arg_i_ref), "\n");
  if (*arg_i_ref <  0) { return 0; }; 
  //if (*arg_i_ref == 0) { *arg_i_ref -= 1; return pscript_code_parser__expr(this, syntax_env, chunk_offset, byref_huh); }; 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: 
    if (*arg_i_ref == 0) { *arg_i_ref -= 1; return pscript_code_parser__expr(this, syntax_env, chunk_offset, byref_huh); }; 
    *arg_i_ref -= 1; return 0; break; 
    
  case PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST"); 
    const int arglist_subtree = fields_values[0]; 
    const int arg_subtree     = fields_values[1]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST[arglist_subtree = ", int_string__stack(arglist_subtree)," ; arg_subtree = ", int_string__stack(arg_subtree), "]"); 
    int stacked_nb = 0; 
    stacked_nb += pscript_code_parser__expr__arg_i__aux(this, syntax_env, arglist_subtree, arg_i_ref, byref_huh); 
    stacked_nb += pscript_code_parser__expr__arg_i__aux(this, syntax_env,     arg_subtree, arg_i_ref, byref_huh); 
    return stacked_nb; 
  }; 
  }; 
  
  return -1; 
}; 

static int pscript_code_parser__expr__arg_i(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int arg_i, const int byref_huh) { 
  //dputs_array(stderr__fileno, "---------------------------------------------", __func__, ": arg_i: ", int_string__stack(arg_i), "\n");
  int arg_i_temp = arg_i; 
  return pscript_code_parser__expr__arg_i__aux(this, syntax_env, chunk_offset, &arg_i_temp, byref_huh); 
}; 

static int pscript_code_parser__expr__funcall(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int byref_huh__global, const int funproc_type, const int funproc_value_sizeof, const char * funproc_value, const int arg_list) { 
  
  const int core_type = pscript_type__get_core_type(this -> type_env, funproc_type); 
  
  const int on_stack_arg_nb = pscript_code_parser__expr__arg_list__get_nb(this, syntax_env, arg_list); 
  
  switch (core_type) { 
  case PSCRIPT_TYPE__LAMBDA: 
  default: EXCEPTION_ASSERT(false); break; 

  case PSCRIPT_TYPE__LIBPROC: do { 
      const int    libproc_i              = *(const int16_t*) funproc_value; 
      const int    libproc_arg_nb         = pscript_libproc__get_arg_nb     (this -> libproc_env, libproc_i); 
      const int    libproc_atleast_arg_nb = pscript_libproc__get_arg_atleast(this -> libproc_env, libproc_i); 
      const int    libproc_name_pstr      = pscript_libproc__get_name_pstr  (this -> libproc_env, libproc_i); 
      const char * libproc_name_cstr      = pscript_libproc__get_name_cstr  (this -> libproc_env, this -> string_env, libproc_i); 
      
      if (libproc_atleast_arg_nb >= 0) { 
	if (on_stack_arg_nb < libproc_atleast_arg_nb) { EXCEPTION_RAISE("Lors d'un appel de la fonction LIB '", libproc_name_cstr, "', au moins ", int_string__stack(libproc_atleast_arg_nb), " arguments sont attendus, et seulement ", int_string__stack(on_stack_arg_nb), " ont été fournis." "\n"); }; 
      }; 
      
      if (libproc_arg_nb >= 0) { 				  
	if (on_stack_arg_nb != libproc_arg_nb) { EXCEPTION_RAISE("Lors d'un appel de la fonction LIB '", libproc_name_cstr, "', ", int_string__stack(libproc_atleast_arg_nb), " arguments sont attendus, alors que  ", int_string__stack(on_stack_arg_nb), " ont été fournis." "\n"); }; 
      }; 

      
      // RL: CALLING CONVENTION: POSTFIX 
      
      for (int arg_i = 0; arg_i < on_stack_arg_nb; arg_i++) { 
 	const int byref_huh = pscript_libproc__get_arg_byref(this -> libproc_env, libproc_i, arg_i); 
	//dputs_array(stderr__fileno, "--------------------------------------------- BYREF = ", bool_string(byref_huh), "\n"); 
	pscript_code_parser__expr__arg_i(this, syntax_env, arg_list, arg_i, byref_huh); 
      }; 
      
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CALL_LIB); 
      pscript_bytecode__push_int8       (this -> bytecode_env,        on_stack_arg_nb); 
      pscript_bytecode__push_int16      (this -> bytecode_env,              libproc_i); 
      
#if 1 
      // RL: The type of the function on which 'map' is applied is not known at compile-time. 
      //     Therefore we can't foreknow whether it will return any object. 
      //     As such, we always suppose that it does. 
      return 1; 
#else 
      const int retval_type   = pscript_libproc__get_retval_type(this -> libproc_env, libproc_i); 
      const int retval_sizeof = pscript_type__sizeof(this -> type_env, retval_type); 
      return 0 == retval_sizeof ? 0 : 1; 
#endif 
    } while (false); break; 

    
  case PSCRIPT_TYPE__FFIPROC: do { 
      const int ffiproc_i = *(const int16_t*) funproc_value; 
      //const int ffiproc_arg_nb         = pscript_ffiproc__get_arg_nb(this -> ffiproc_env, ffiproc_i); 
      const int ffiproc_atleast_arg_nb = pscript_ffiproc__get_atleast_arg_nb(this -> ffiproc_env, ffiproc_i); 
      
      if (on_stack_arg_nb < ffiproc_atleast_arg_nb) { EXCEPTION_RAISE("Lors d'un appel de la fonction FFI 'TODO XXX FIXME', au moins ", int_string__stack(ffiproc_atleast_arg_nb), " arguments sont attendus, et ", int_string__stack(on_stack_arg_nb), " ont été fournis." "\n"); }; 
      
      // RL: CALLING CONVENTION: POSTFIX 
      
      for (int arg_i = 0; arg_i < on_stack_arg_nb; arg_i++) { 
 	const int byref_huh = pscript_ffiproc__get_arg_byref(this -> ffiproc_env, ffiproc_i, arg_i); 
	//dputs_array(stderr__fileno, "--------------------------------------------- BYREF = ", bool_string(byref_huh), "\n"); 
	pscript_code_parser__expr__arg_i(this, syntax_env, arg_list, arg_i, byref_huh); 
      }; 
      
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CALL_FFI); 
      pscript_bytecode__push_int8 (this -> bytecode_env, on_stack_arg_nb); 
      pscript_bytecode__push_int16(this -> bytecode_env, ffiproc_i); 
      
      const int retval_type   = pscript_ffiproc__get_retval_type(this -> ffiproc_env, ffiproc_i); 
      const int retval_sizeof = pscript_type__sizeof(this -> type_env, retval_type); 
      return 0 == retval_sizeof ? 0 : 1; 
    } while (false); break; 

  case PSCRIPT_TYPE__FUNPROC: do { 
      //dputs_array(stderr__fileno, "funproc_type = ", pscript_type__get_cstr(this -> type_env, funproc_type), "\n"); 
      const int funproc_offset  = *(const int*) funproc_value; 
      const int funproc_arg_nb  = pscript_type__funproc__get_arg_nb(this -> type_env, funproc_type); 
      if (funproc_arg_nb != on_stack_arg_nb) { EXCEPTION_RAISE("Lors d'un appel de la fonction '", "TODO XXX FIXME", "', ", int_string__stack(funproc_arg_nb), " arguments sont attendus, et ", int_string__stack(on_stack_arg_nb), " ont été fournis." "\n"); }; 
      for (int arg_i = 0; arg_i < funproc_arg_nb; arg_i++) { 
 	const int byref_huh = pscript_type__funproc__get_arg_byref_huh(this -> type_env, funproc_type, arg_i); 
	//dputs_array(stderr__fileno, "--------------------------------------------- BYREF = ", bool_string(byref_huh), "\n"); 
	pscript_code_parser__expr__arg_i(this, syntax_env, arg_list, arg_i, byref_huh); 
      }; 
      // RL: CALLING CONVENTION: POSTFIX 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CALL); 
      pscript_bytecode__push_int8(this -> bytecode_env, on_stack_arg_nb); 
      pscript_bytecode__push_int (this -> bytecode_env, funproc_offset); 
      const int retval_type   = pscript_type__funproc__get_retval_type(this -> type_env, funproc_type); 
      const int retval_sizeof = pscript_type__sizeof(this -> type_env, retval_type); 
      return 0 == retval_sizeof ? 0 : 1; 
    } while (false); break; 
    
  }; 
  
  EXCEPTION_ASSERT(false); 
  return INT_MIN; 
}; 


// RL: It returns how many expressions it has pushed onto the stack. 
static int pscript_code_parser__expr(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int byref_huh__global) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: { 
    //EXCEPTION_ASSERT(chunk_type < PSCRIPT_TOKEN_COUNT); 
    EXCEPTION_ASSERT(chunk_type < TOKEN_MAX); 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__TOKEN", "\n"); 
    EXCEPTION_ASSERT(0 < descendants_nb); 
    if (1 == descendants_nb) { 
      const int token_i = fields_values[0]; 
      pscript_code_parser__expr_token(this, syntax_env, byref_huh__global, token_i); 
      break; 
    }; 
    if (3 == descendants_nb) { 
      if (chunk_type == PSCRIPT_TOKEN_OPENBRACKET) { 
	const int array_expr = fields_values[1]; 
	const int index_expr = fields_values[2]; 
	pscript_code_parser__expr(this, syntax_env, array_expr, true); 
	pscript_code_parser__expr(this, syntax_env, index_expr, false); 
	if (byref_huh__global) { 
	  //EXCEPTION_RAISE("To be implemented."); }; 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__ARRAY_GET_ELT_I); 
	  // RL: Weird that's the same code... 
	}
	else {
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__ARRAY_GET_ELT_I); 
	}; 
      } 
      else { 
	const int    token_i = fields_values[0]; 
	const int  left_expr = fields_values[1]; 
	const int right_expr = fields_values[2]; 
	int bytecode = -1; 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_LOGICAL_OR : bytecode = PVM_BYTECODE__BOOL_OR; break; 
	case PSCRIPT_TOKEN_LOGICAL_AND: bytecode = PVM_BYTECODE__BOOL_AND; break; 
	case PSCRIPT_TOKEN_EQUAL      : bytecode = PVM_BYTECODE__EQUAL_HUH; break; 
	case PSCRIPT_TOKEN_INF        : bytecode = PVM_BYTECODE__INF_HUH; break; 
	case PSCRIPT_TOKEN_SUP        : bytecode = PVM_BYTECODE__SUP_HUH; break; 
	case PSCRIPT_TOKEN_INFEQ      : bytecode = PVM_BYTECODE__INFEQ_HUH; break; 
	case PSCRIPT_TOKEN_SUPEQ      : bytecode = PVM_BYTECODE__SUPEQ_HUH; break; 
	case PSCRIPT_TOKEN_IPLUS      : bytecode = PVM_BYTECODE__ADD; break; 
	case PSCRIPT_TOKEN_IMOINS     : bytecode = PVM_BYTECODE__SUB; break; 
	case PSCRIPT_TOKEN_RPLUS      : bytecode = PVM_BYTECODE__ADD; break; 
	case PSCRIPT_TOKEN_RMOINS     : bytecode = PVM_BYTECODE__SUB; break; 
	case PSCRIPT_TOKEN_IMULT      : bytecode = PVM_BYTECODE__MULT; break; 
	case PSCRIPT_TOKEN_IDIV       : bytecode = PVM_BYTECODE__IDIV; break; 
	case PSCRIPT_TOKEN_IMOD       : bytecode = PVM_BYTECODE__IMOD; break; 
	case PSCRIPT_TOKEN_RMULT      : bytecode = PVM_BYTECODE__MULT; break; 
	case PSCRIPT_TOKEN_RDIV       : bytecode = PVM_BYTECODE__RDIV; break; 
	}; 
	if (bytecode >= 0) { 
	  if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
	  pscript_code_parser__expr(this, syntax_env, left_expr, false);
	  pscript_code_parser__expr(this, syntax_env, right_expr, false); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, bytecode); 
	  break; 
	}; 
	if (PSCRIPT_TOKEN_DIFF == chunk_type) { 
	  if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
	  pscript_code_parser__expr(this, syntax_env, left_expr, false);
	  pscript_code_parser__expr(this, syntax_env, right_expr, false); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__EQUAL_HUH); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__BOOL_NOT); 
	  break; 
	}; 
	if (PSCRIPT_TOKEN_POINT == chunk_type) { 
	  //if (byref_huh__global) { EXCEPTION_RAISE_AT_TOKEN(token_i, "Needs to be implemented."); }; 
	  pscript_code_parser__expr(this, syntax_env, left_expr, false); 
	  //pscript_code_parser__expr(this, syntax_env, left_expr, /*byref?*/true); 
	  const int ident_i = pscript_code_parser__expr__get_ident_i(this, syntax_env, right_expr); 
	  if (byref_huh__global) { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__RECORD_GET_FIELD_ADDRESS_BY_NAME); 
	  } 
	  else { 
	    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__RECORD_GET_FIELD_VALUE_BY_NAME); 
	  }; 
	  pscript_bytecode__push_int16(this -> bytecode_env, ident_i); 
	  break; 
	}; 
	if (PSCRIPT_TOKEN_FLECHE == chunk_type) { 
	  if (byref_huh__global) { EXCEPTION_RAISE_AT_TOKEN(token_i, "Not sure of the meaning of taking an address of a method call."); }; 
	  // RL: As of the current way the tree is built, the whole right side is a function call. 
	  pscript_code_parser__expr(this, syntax_env, left_expr, false); 
	  //const int ident_i = pscript_code_parser__expr__arrow_rhs(this, syntax_env, right_expr); 
	  pscript_code_parser__expr__arrow_rhs(this, syntax_env, /*chunk_offset*/right_expr); 
	  break; 
	}; 
	if (PSCRIPT_TOKEN_AFFECTATION == chunk_type) { 
	  pscript_code_parser__expr(this, syntax_env, left_expr, true); 
	  pscript_code_parser__expr(this, syntax_env, right_expr, false); 
	  pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_STORE__STACK); 
	  return 0; 
	  break; 
	}; 

	const char * chunk_type_cstr = pscript_token__type_get_cstr(chunk_type); 
	
	EXCEPTION_RAISE_AT_TOKEN(token_i,"Do not know what to do with that token: ", chunk_type_cstr); 
	break; 
      }; 
      break; 
    }; 
    
    if (2 == descendants_nb) { 
      if (int_member_array_nb(chunk_type, pscript_parser__postfix_operators__nb, pscript_parser__postfix_operators)) { 
	const int  left_expr = fields_values[1]; 
	pscript_code_parser__expr(this, syntax_env, left_expr, true); 
	EXCEPTION_ASSERT(false); 
	if (byref_huh__global) { EXCEPTION_RAISE("To be implemented."); }; 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_INC: 
	case PSCRIPT_TOKEN_DEC: 
	default: break; 
	}; 
	break; 
      } 
      else { 
	if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
	const int right_expr = fields_values[1]; 
	pscript_code_parser__expr(this, syntax_env, right_expr, false); 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_LOGICAL_NOT: pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__BOOL_NOT); break; 
	case PSCRIPT_TOKEN_IPLUS : break; 
	case PSCRIPT_TOKEN_IMOINS: pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__NEG); break; 
	case PSCRIPT_TOKEN_RPLUS : break; 
	case PSCRIPT_TOKEN_RMOINS: pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__NEG); break; 
	default: EXCEPTION_ASSERT(false); break; 
	}; 
      }; 
      break; 
    }; 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__FUNCALL: { 
    //dputs_array(stderr__fileno, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL", "\n"); 
    // RL: Most of the things are dynamic here. 
    //     Only one thing is statically needed: if args are byref or byval. 
    //     ... 
    //     In Scheme-like languages, all values are byval, and there are specific values which are boxes. 
    //     ... 
    //     In the generic case, if the type could not be determined at compile-time, 
    //     then all args to the CALL_STACK_OBJECT instruction shall be byref (unless not possible), 
    //     and then the CALL_STACK_OBJECT instruction converts those values to match the type of the function. 
    //     ... 
    //     First, we should try to statically compute the function. 
    if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
    const int fun_subtree = fields_values[0]; 
    const int arg_list    = fields_values[1]; 
    //dputs_array(stderr__fileno, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL[fun_subtree = ", int_string__stack(fun_subtree)," ; arg_list = ", int_string__stack(arg_list), "]", "\n"); 

    // RL: So, the first thing to do is try to statically compute the function. 
    const int values_const_nb = pscript_code_parser__expr_const(this, syntax_env, fun_subtree, /*please_try_but_do_not_fail*/true); 
    
    if (values_const_nb >= 0) { 
      // RL: Constant computation succeeded. 
      const int item_type        = pscript_const__stack__top_get_type(this -> const_env); 
      const int item_type_sizeof = pscript_type__sizeof(this -> type_env, item_type); 
      char item_value[item_type_sizeof]; 
      //pscript_const__stack__top_get_value(this -> const_env, 
      const int item_type_sizeof_actual = pscript_const__stack__top_get_value2(this -> const_env, sizeof(item_value), item_value); 
      pscript_const__stack__drop(this -> const_env); 
      return pscript_code_parser__expr__funcall(this, syntax_env, chunk_offset, byref_huh__global, item_type, item_type_sizeof_actual, item_value, arg_list); 
    } 
    else { 
      // RL: Constant computation failed.  
      int arg_nb = 0; 
      if (arg_list >= 0) { 
	arg_nb += pscript_code_parser__expr(this, syntax_env, arg_list, false); 
      }; 
      pscript_code_parser__expr(this, syntax_env, fun_subtree, false); 
      //dputs_array(stderr__fileno, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL", "\n"); 
      //pscript_parser__print_all(stderr__fileno, syntax_env); 
      //bytecode_stack__print_all(this -> bytecode_env, this -> type_env); 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CALL_STACK_OBJECT); 
      pscript_bytecode__push_int8(this -> bytecode_env, arg_nb); 
      //bytecode_stack__print_all(this -> bytecode_env, this -> type_env); 
      //dputs_array(stderr__fileno, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL", "\n"); 
    }; 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__LAMBDA:  { 
    if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA"); 
    const int lambda_type_expr_tree = fields_values[0]; 
    const int localvar_ref          = fields_values[1]; 
    const int inst_ref              = fields_values[2]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA[lambda_type_expr_tree = ", int_string__stack(lambda_type_expr_tree)," ; localvar_ref = ", int_string__stack(localvar_ref)," ; inst_ref = ", int_string__stack(inst_ref), "]"); 
    //EXCEPTION_RAISE("Need to implement lambda expression."); 

    const int nesting_funproc_i = funproc_i_being_compiled__global; 
    const int         funproc_i = pscript_funproc__push_empty(this -> funproc_env); 
    pscript_funproc__lambda__nesting_funproc_i__set(this -> funproc_env, funproc_i, nesting_funproc_i); 
    funproc_i_being_compiled__global = funproc_i; 
    
    const char * ident_cstr = "<lambda>"; 
    const int    ident_pstr = pscript_string__push(this -> string_env, ident_cstr); 
    pscript_funproc__set_name(this -> funproc_env, funproc_i, ident_pstr); 
    
    pscript_code_parser__decl_toplevel__funproc_decl(this, syntax_env, funproc_i, /*of_type*/-1, lambda_type_expr_tree, localvar_ref); 
    
    pscript_code_parser__decl_toplevel__funproc_def(this, syntax_env, funproc_i, inst_ref, /*lambda_huh*/true); 
    
    funproc_i_being_compiled__global = nesting_funproc_i; 
    
    // RL: Alright. Now, we're an expression. So we have to push the object newly created. 
    { 
      //      - type 
      //      - offset 
      //      - lexical env 
      const int   nesting_level   = pscript_funproc__lambda__nesting_level__get(this -> funproc_env, funproc_i); 
      const int   arg_nb          = pscript_funproc__get_arg_nb                (this -> funproc_env, funproc_i); 
      const int * arg_name        = pscript_funproc__get_arg_name__array       (this -> funproc_env, funproc_i); 
      const int * arg_byref_huh   = pscript_funproc__get_arg_byref__array      (this -> funproc_env, funproc_i); 
      const int * arg_type        = pscript_funproc__get_arg_type__array       (this -> funproc_env, funproc_i); 
      const int   retval_type_i   = pscript_funproc__get_retval_type           (this -> funproc_env, funproc_i); 
      const int   lambda_type_i   = pscript_type__make__lambda(this -> type_env, /* lexical_env_nb := */nesting_level, arg_nb, arg_name, arg_byref_huh, arg_type, retval_type_i); 
      // RL: - 
      int nesting_funproc__to_lex_env_stack__offset; 
      if (nesting_funproc_i < 0) { // RL: Could be the case in mainproc, meaning that there is no parent memory frame other than the top-level one. 
	nesting_funproc__to_lex_env_stack__offset = 0; 
      } 
      else { 
	const int nesting_funproc__retval_type              = pscript_funproc__get_retval_type        (this -> funproc_env, nesting_funproc_i); 
	const int nesting_funproc__retval_sizeof            = pscript_type__sizeof                    (this -> type_env,    nesting_funproc__retval_type); 
	const int nesting_funproc__arg_all_sizeof           = pscript_funproc__get_arg_all_sizeof     (this -> funproc_env, nesting_funproc_i); 
	const int nesting_funproc__localvar_all_sizeof      = pscript_funproc__get_localvar_all_sizeof(this -> funproc_env, nesting_funproc_i); 
	nesting_funproc__to_lex_env_stack__offset = nesting_funproc__retval_sizeof + nesting_funproc__arg_all_sizeof + nesting_funproc__localvar_all_sizeof; 
      }; 
      //pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEMORY_FRAME__INC_AND_LOAD); 
      //pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
      const int begin_funproc_bytecode_offset = pscript_funproc__get_bytecode_offset(this -> funproc_env, funproc_i); 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MAKE_LAMBDA); 
      pscript_bytecode__push_int        (this -> bytecode_env, nesting_funproc__to_lex_env_stack__offset); 
      pscript_bytecode__push_int8       (this -> bytecode_env, lambda_type_i); 
      pscript_bytecode__push_int8       (this -> bytecode_env, nesting_level); 
      pscript_bytecode__push_int        (this -> bytecode_env, begin_funproc_bytecode_offset); 
    }; 

    







  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST:  { 
    if (byref_huh__global) { EXCEPTION_RAISE("That does not have any meaning."); }; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST"); 
    const int arglist_subtree = fields_values[0]; 
    const int arg_subtree     = fields_values[1]; 
    //dputs_array(stderr__fileno, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST[arglist_subtree = ", int_string__stack(arglist_subtree)," ; arg_subtree = ", int_string__stack(arg_subtree), "]"); 
    const int left_nb  = pscript_code_parser__expr(this, syntax_env, arglist_subtree, false); 
    const int right_nb = arg_subtree < 0 ? 0 : pscript_code_parser__expr(this, syntax_env, arg_subtree, false); 
    return left_nb + right_nb; 
  }; break; 
  }; 
  return 1; 
}; 









static int pscript_code_parser__inst(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__inst_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__inst_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__inst_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__inst_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__inst_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: EXCEPTION_ASSERT(false); break;  


  case PSCRIPT_INST_LIST__TYPE__LIST__NIL: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__LIST__NIL", "\n");  
    EXCEPTION_RAISE("PSCRIPT_INST_LIST__TYPE__LIST__NIL"); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__LIST__CONS: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__LIST__CONS", "\n");  
    EXCEPTION_RAISE("PSCRIPT_INST_LIST__TYPE__LIST__CONS"); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__BEGIN: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__BEGIN", "\n");  
    EXCEPTION_RAISE("PSCRIPT_INST_LIST__TYPE__BEGIN"); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE", "\n");  
    EXCEPTION_RAISE("PSCRIPT_INST_LIST__TYPE__IF_NO_ELSE"); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__IF_ELSE: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__IF_ELSE", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_if_false    = fields_values[2]; 
    const int inst_list_head   = fields_values[3]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    pscript_code_parser__expr(this, syntax_env, test_expr_tree, false); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JFALSE); 
    const int test_break_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 
    pscript_code_parser__inst(this, syntax_env, inst_if_true); 
    if (inst_if_false >= 0) { 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JUMP); 
      const int if_true_exit_jump_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 
      const int if_false_start_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
      pscript_bytecode__rewrite_address(this -> bytecode_env, test_break_offset, if_false_start_label); 
      pscript_code_parser__inst(this, syntax_env, inst_if_false); 
      const int exit_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
      pscript_bytecode__rewrite_address(this -> bytecode_env, if_true_exit_jump_offset, exit_label); 
    } 
    else { 
      const int exit_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
      pscript_bytecode__rewrite_address(this -> bytecode_env, test_break_offset, exit_label); 
    }; 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__WHEN: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__WHEN", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    pscript_code_parser__expr(this, syntax_env, test_expr_tree, false);
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JFALSE); 
    const int test_break_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 
    pscript_code_parser__inst(this, syntax_env, inst_if_true);
    const int exit_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__rewrite_address(this -> bytecode_env, test_break_offset, exit_label); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__UNLESS: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__UNLESS", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_false    = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    pscript_code_parser__expr(this, syntax_env, test_expr_tree, false); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JTRUE); 
    const int test_break_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 
    pscript_code_parser__inst(this, syntax_env, inst_if_false); 
    const int exit_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__rewrite_address(this -> bytecode_env, test_break_offset, exit_label); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__WHILE: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__WHILE", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    const int inst_if_true     = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    const int while_start_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_code_parser__expr(this, syntax_env, test_expr_tree, false); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JFALSE); 
    const int test_break_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 
    pscript_code_parser__inst(this, syntax_env, inst_if_true); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JUMP); 
    pscript_bytecode__push_int(this -> bytecode_env, while_start_label); 
    const int while_end_label = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__rewrite_address(this -> bytecode_env, test_break_offset, while_end_label); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__FOR: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__FOR", "\n");  
    const int ident_token      = fields_values[0]; 
    const int init_expr        = fields_values[1]; 
    const int downto_huh       = fields_values[2]; 
    const int final_expr       = fields_values[3]; 
    const int step_expr        = fields_values[4]; 
    const int inst_if_true     = fields_values[5]; 
    const int inst_list_head   = fields_values[6]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 

    const int    ident_pstr  = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    const char * ident_cstr  = pscript_token__get_srcval_cstr(this -> token_env, ident_token); 
    const int    ident_env_i = pscript_env__lookup(this -> env, ident_pstr); 
    if (ident_env_i < 0) { EXCEPTION_RAISE("Pour une construction 'for', la variable d'indice doit être déclarée au préalable (en l'occurrence, la variable '", ident_cstr, "')." "\n"); }; 
    
    const int ident_env_type = pscript_env__get_type(this -> env, ident_env_i); 
    
    if (ident_env_type == PSCRIPT_ENV__VAR_LOCAL) {
        //const int localvar_i = pscript_env__get_value(this -> env, env_i); 
        const int localvar_i = pscript_env__get_value2(this -> env, ident_env_i); 
    	const int localvar_type = pscript_funproc__get_localvar_type(this -> funproc_env, funproc_i_being_compiled__global, localvar_i); 
    	if (localvar_type != pscript_type__get__integer(this -> type_env)) { EXCEPTION_RAISE("Pour une construction 'for', la variable d'indice doit être de type 'entier' (en l'occurrence, l'indice '", ident_cstr, "' est une ", pscript_string__get_cstr(this -> string_env, pscript_type__string(this -> type_env, localvar_type)), ")." "\n"); }; //int_string__stack(pscript_type__string(this -> type_env, localvar_type)), 
    }
    else { EXCEPTION_RAISE("Pour une construction 'for', la variable d'indice doit être une variable locale (en l'occurrence, l'indice '", ident_cstr, "' est une ", pscript_env__type__get_cstr(this -> env, ident_env_type),  ")." "\n"); }; 
    

    const int ident_env_value1 = pscript_env__get_value (this -> env, ident_env_i); 
    const int ident_env_value2 = pscript_env__get_value2(this -> env, ident_env_i); 

    const int localvar_i   = ident_env_value2; 
    const int type_i       = pscript_type__get__integer(this -> type_env); 
    const int indir_type_i = pscript_type__make__pointer(this -> type_env, type_i); 
    const int localvar_offset = pscript_funproc__get_localvar_offset(this -> funproc_env, funproc_i_being_compiled__global, localvar_i); 
    
 
    // RL: Init value. 
    EXCEPTION_ASSERT(init_expr >= 0); 
    pscript_code_parser__expr(this, syntax_env, init_expr, /*byref_huh*/false); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_STORE_LOCAL); 
#if 1
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    //pscript_bytecode__push_int (this -> bytecode_env, localvar_offset); 
    pscript_bytecode__push_mem_offset (this -> bytecode_env, localvar_offset); 
#else
    const int label__init_indice_bytecode_offset_to_be_rewritten = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__push_int8(this -> bytecode_env, 0); 
    pscript_bytecode__push_int (this -> bytecode_env, 0); 
#endif 

    // RL: Last indice 
    EXCEPTION_ASSERT(final_expr >= 0); 
    pscript_code_parser__expr(this, syntax_env, final_expr, /*byref_huh*/false); 
    
    // RL: Jump back 
    const int label__jmp_back__test_indice_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__DUP); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
#if 1
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    pscript_bytecode__push_mem_offset (this -> bytecode_env, localvar_offset); 
#else
    const int label__test_indice_bytecode_offset_to_be_rewritten = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__push_int8(this -> bytecode_env, 0); 
    pscript_bytecode__push_int (this -> bytecode_env, 0); 
#endif 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__SUPEQ_HUH); 
    
    // RL: Label if false 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JFALSE); 
    const int label__jfalse_bytecode_offset = pscript_bytecode__push_int(this -> bytecode_env, 0); 


    // RL: Instructions. 
    if (inst_if_true >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_if_true); 
    }; 


    // RL: End of loop body. 
    const int label__continue_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_LOAD_LOCAL); 
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    pscript_bytecode__push_mem_offset (this -> bytecode_env, localvar_offset); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    pscript_bytecode__push_int (this -> bytecode_env, 1); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__ADD); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_STORE_LOCAL); 
    pscript_bytecode__push_int8(this -> bytecode_env, type_i); 
    pscript_bytecode__push_mem_offset (this -> bytecode_env, localvar_offset); 
    
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JUMP); 
    pscript_bytecode__push_int(this -> bytecode_env, label__jmp_back__test_indice_bytecode_offset); 
    const int end_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__DROP); // The upper bound. 
    
    
#if 0 
    pscript_bytecode__rewrite_int8(this -> bytecode_env, label__init_indice_bytecode_offset_to_be_rewritten    ,    type_i); 
    pscript_bytecode__rewrite_int (this -> bytecode_env, label__init_indice_bytecode_offset_to_be_rewritten + 1,    localvar_offset); 
#endif 
    
    pscript_bytecode__rewrite_address(this -> bytecode_env,              label__jfalse_bytecode_offset, end_bytecode_offset); 
    
#if 0 
    pscript_bytecode__rewrite_int8(this -> bytecode_env, label__test_indice_bytecode_offset_to_be_rewritten    ,    type_i); 
    pscript_bytecode__rewrite_int (this -> bytecode_env, label__test_indice_bytecode_offset_to_be_rewritten + 1,    localvar_offset); 
#endif 
    
    
    
    //EXCEPTION_RAISE("Implementing 'for' needs local variables."); 
#if 0 
    dputs_array(filedes, "for ", pscript_token__get_srcval_cstr(token_env, ident_token), " = "); 
    pscript_code_parser__expr(this, syntax_env, init_expr, false); 
    dputs(filedes, downto_huh ? " downto " : " to "); 
    pscript_code_parser__expr(this, syntax_env, final_expr, false); 
    if (step_expr >= 0) { 
      pscript_code_parser__expr(this, syntax_env, step_expr, false); 
    }; 
    pscript_code_parser__inst(this, syntax_env, inst_if_true);
#endif 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__REPEAT: {  
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__REPEAT", "\n");  
    const int test_expr_tree   = fields_values[0]; 
    //const int inst_nb          = fields_values[1]; 
    const int inst_ref         = fields_values[1]; 
    const int inst_list_head   = fields_values[2]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    const int repeat_loop_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    if (inst_ref >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_ref); 
    }; 
    pscript_code_parser__expr(this, syntax_env, test_expr_tree, false);
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JFALSE); 
    pscript_bytecode__push_int(this -> bytecode_env, repeat_loop_bytecode_offset); 
  }; break;  
  case PSCRIPT_INST_LIST__TYPE__EXPR: { 
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_INST_LIST__TYPE__EXPR", "\n"); 
    const int expr_tree       = fields_values[0]; 
    const int inst_list_head  = fields_values[1]; 
    if (inst_list_head >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_list_head); 
    }; 
    const int nb_values_on_stack = pscript_code_parser__expr(this, syntax_env, expr_tree, false); 
    for (int i = 0; i < nb_values_on_stack; i++) { 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__DROP); 
    }; 
  }; break;  
    
    
  }; 

  return 0; 
}; 






// RL: Returns the number of items pushed onto the constant expressions stack. 
static int pscript_code_parser__token_const(pscript_t * this, const pscript_parser_env_t * syntax_env, const int token_i) { 
  const int    token_type        = pscript_token__get_type(this -> token_env, token_i); 
  const char * token_srcval_cstr = pscript_token__get_srcval_cstr(this -> token_env, token_i); 
  const int    token_srcval_pstr = pscript_token__get_srcval_pstr(this -> token_env, token_i); 
  switch (token_type) { 
  case PSCRIPT_TOKEN_TRUE:   pscript_const__push_bool(this -> const_env, true); break; 
  case PSCRIPT_TOKEN_FALSE:  pscript_const__push_bool(this -> const_env, false); break; 
  case PSCRIPT_TOKEN_ENTIER: { 
    const int value = atoi(token_srcval_cstr); 
    pscript_const__push_int(this -> const_env, value); 
  }; break; 
  case PSCRIPT_TOKEN_REEL__VIRG: 
  case PSCRIPT_TOKEN_REEL__DOT: 
  case PSCRIPT_TOKEN_REEL__E: 
    {
      const float value = atof(token_srcval_cstr); 
      pscript_const__push_float(this -> const_env, value); 
    }; break; 
  case PSCRIPT_TOKEN_NIL: pscript_const__push_nil(this -> const_env); break; 
  case PSCRIPT_TOKEN_IDENT: { 
    const int    ident_pstr  = token_srcval_pstr; 
    const char * ident_cstr  = token_srcval_cstr; 
    const int    ident_env_i = pscript_env__lookup(this -> env, ident_pstr); 
    if (ident_env_i < 0) { EXCEPTION_RAISE("The identifier '", ident_cstr, "' has not been defined yet. (Please not that, for constant values, there can't be forward references.)"); }; 
    
    const int ident_env_type = pscript_env__get_type(this -> env, ident_env_i); 

    switch (ident_env_type) { 
    default: 
    case PSCRIPT_ENV__VAR:         // RL: We could return the absolute address, which is constant. 
    case PSCRIPT_ENV__VAR_LOCAL:   // RL: The relative address should be constant. 
    case PSCRIPT_ENV__ARG:         // RL: The relative address should be constant. 
    case PSCRIPT_ENV__RETVAL:      // RL: The relative address should be constant. 
    case PSCRIPT_ENV__METHOD_THIS: // RL: The relative address should be constant.  
      EXCEPTION_RAISE("The identifier '", ident_cstr,"' is defined, but is a ", pscript_env__type__get_cstr(this -> env, ident_env_type), ", not a constant.");  
      break; 
    case PSCRIPT_ENV__CONST: { 
      const int const_i = pscript_env__get_const_indice(this -> env, ident_env_i); 
      pscript_const__stack__push_const_value(this -> const_env, const_i); 
    }; break; 

    case PSCRIPT_ENV__LIBPROC: do { 
	const int libproc_i           = pscript_env__get_value(this -> env, ident_env_i); 
	const int libproc_type_i      = pscript_libproc__get_type(this -> libproc_env, this -> type_env, libproc_i); 
	const int libproc_type_sizeof = pscript_type__sizeof(this -> type_env, libproc_type_i); 
	pscript_const__stack__push_value(this -> const_env, libproc_type_i, libproc_type_sizeof, (const char *) &libproc_i); 
      } while (false); break; 
      
    case PSCRIPT_ENV__FFIPROC: do { 
	const int ffiproc_i           = pscript_env__get_value(this -> env, ident_env_i); 
	const int ffiproc_type_i      = pscript_ffiproc__get_type(this -> ffiproc_env, ffiproc_i); 
	const int ffiproc_type_sizeof = pscript_type__sizeof(this -> type_env, ffiproc_type_i); 
#if 1 
	pscript_const__stack__push_value(this -> const_env, ffiproc_type_i, ffiproc_type_sizeof, (const char *) &ffiproc_i); 
#else 
	pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	pscript_bytecode__push_int8(this -> bytecode_env, ffiproc_type_i); 
	pscript_bytecode__push_int16(this -> bytecode_env, ffiproc_i); 
#endif 
      } while (false); break; 
      
    case PSCRIPT_ENV__FUNPROC: do { // RL: The address.  
	const int funproc_i      = pscript_env__get_value(this -> env, ident_env_i); 
	const int funproc_offset = pscript_funproc__get_offset(this -> funproc_env, funproc_i); 
	//dputs_array(stderr__fileno, "funproc_offset: ", int_string__stack(funproc_offset), "\n"); 
	const int funproc_type   = pscript_funproc__get_type(this -> funproc_env, funproc_i); 
	//const int proxy_type_i   = pscript_type__make__proxy(this -> type_env, funproc_type); 
	const int funproc_type_sizeof = pscript_type__sizeof(this -> type_env, funproc_type); 
	//const char
#if 1 
	pscript_const__stack__push_value(this -> const_env, funproc_type, funproc_type_sizeof, (const char *) &funproc_offset); 
#else 
	pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	pscript_bytecode__push_int8(this -> bytecode_env, proxy_type_i); 
	pscript_bytecode__push_integer(this -> bytecode_env, funproc_offset); 
#endif 
	break; 
      } while (false); break; 
      //case PSCRIPT_ENV__MAINPROC:  
    }; 
    
  }; break; 
  case PSCRIPT_TOKEN_STRING_C: 
    { 
      const int value_pstr = chaine_pstr__pscript__analyze_and_convert(this -> string_env, token_srcval_pstr); 
      pscript_const__push_string(this -> const_env, value_pstr); 
    }; break; 
  default: { 
    const char * token_type_cstr   = pscript_token__type_get_cstr(token_type); 
    EXCEPTION_RAISE(token_type_cstr, " tokens cannot be constant evaluated (value: ", token_srcval_cstr, ")"); 
  }; break; 
  }; 
  return 1; 
}; 


// RL: It returns how many expressions it has pushed onto the constant expressions stack. 
static int pscript_code_parser__expr_const_aux(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__expr_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__expr_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__expr_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: { 
    //EXCEPTION_ASSERT(chunk_type < PSCRIPT_TOKEN_COUNT); 
    EXCEPTION_ASSERT(chunk_type < TOKEN_MAX); 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__TOKEN", "\n"); 
    EXCEPTION_ASSERT(0 < descendants_nb); 
    if (1 == descendants_nb) {
      const int token_i = fields_values[0]; 
      pscript_code_parser__token_const(this, syntax_env, token_i); 
      break; 
    }; 
    if (3 == descendants_nb) { 
      if (chunk_type == PSCRIPT_TOKEN_OPENBRACKET) { 
	const int array_expr = fields_values[1]; 
	const int index_expr = fields_values[2]; 
	pscript_code_parser__expr_const_aux(this, syntax_env, array_expr); 
	pscript_code_parser__expr_const_aux(this, syntax_env, index_expr); 
	EXCEPTION_RAISE("Need to implement constant array access."); 
      } 
      else { 
	const int  left_expr = fields_values[1]; 
	const int right_expr = fields_values[2]; 
	pscript_code_parser__expr_const_aux(this, syntax_env, left_expr);
	pscript_code_parser__expr_const_aux(this, syntax_env, right_expr); 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_LOGICAL_OR    : pscript_const__stack__or       (this -> const_env); break; 
	case PSCRIPT_TOKEN_LOGICAL_AND   : pscript_const__stack__and      (this -> const_env); break; 
	case PSCRIPT_TOKEN_EQUAL : pscript_const__stack__equal_huh(this -> const_env); break; 
	case PSCRIPT_TOKEN_DIFF  : pscript_const__stack__diff_huh (this -> const_env); break; 
	case PSCRIPT_TOKEN_INF   : pscript_const__stack__inf_huh  (this -> const_env); break; 
	case PSCRIPT_TOKEN_SUP   : pscript_const__stack__sup_huh  (this -> const_env); break; 
	case PSCRIPT_TOKEN_INFEQ : pscript_const__stack__infeq_huh(this -> const_env); break; 
	case PSCRIPT_TOKEN_SUPEQ : pscript_const__stack__supeq_huh(this -> const_env); break; 
	case PSCRIPT_TOKEN_IPLUS : pscript_const__stack__add      (this -> const_env); break; 
	case PSCRIPT_TOKEN_IMOINS: pscript_const__stack__sub      (this -> const_env); break; 
	case PSCRIPT_TOKEN_RPLUS : pscript_const__stack__add      (this -> const_env); break; 
	case PSCRIPT_TOKEN_RMOINS: pscript_const__stack__sub      (this -> const_env); break; 
	case PSCRIPT_TOKEN_IMULT : pscript_const__stack__mul      (this -> const_env); break; 
	case PSCRIPT_TOKEN_IDIV  : pscript_const__stack__idiv     (this -> const_env); break; 
	case PSCRIPT_TOKEN_IMOD  : pscript_const__stack__imod     (this -> const_env); break; 
	case PSCRIPT_TOKEN_RMULT : pscript_const__stack__mul      (this -> const_env); break; 
	case PSCRIPT_TOKEN_RDIV  : pscript_const__stack__rdiv     (this -> const_env); break; 
	default: EXCEPTION_ASSERT(false); break; 
	}; 
      }; 
      break; 
    }; 
    if (2 == descendants_nb) { 
      if (int_member_array_nb(chunk_type, pscript_parser__postfix_operators__nb, pscript_parser__postfix_operators)) { 
	const int  left_expr = fields_values[1]; 
	pscript_code_parser__expr_const_aux(this, syntax_env, left_expr);
	EXCEPTION_ASSERT(false); 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_INC: 
	case PSCRIPT_TOKEN_DEC: 
	default: break; 
	}; 
	break; 
      } 
      else { 
	const int right_expr = fields_values[1]; 
	pscript_code_parser__expr_const_aux(this, syntax_env, right_expr); 
	switch (chunk_type) { 
	case PSCRIPT_TOKEN_LOGICAL_NOT: pscript_const__stack__not(this -> const_env); break; 
	case PSCRIPT_TOKEN_IPLUS : break; 
	case PSCRIPT_TOKEN_IMOINS: pscript_const__stack__opposite(this -> const_env); break; 
	case PSCRIPT_TOKEN_RPLUS : break; 
	case PSCRIPT_TOKEN_RMOINS: pscript_const__stack__opposite(this -> const_env); break; 
	default: EXCEPTION_ASSERT(false); break; 
	}; 
      }; 
      break; 
    }; 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__FUNCALL: { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL"); 
    const int fun_subtree = fields_values[0]; 
    const int arg_list    = fields_values[1]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__FUNCALL[fun_subtree = ", int_string__stack(fun_subtree)," ; arg_list = ", int_string__stack(arg_list), "]"); 
    if (arg_list >= 0) { 
      pscript_code_parser__expr_const_aux(this, syntax_env, arg_list); 
    }; 
    pscript_code_parser__expr_const_aux(this, syntax_env, fun_subtree); 
    EXCEPTION_RAISE("Need to implement constant function call."); 
  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__LAMBDA:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA"); 
    const int lambda_type_expr_tree = fields_values[0]; 
    const int localvar_ref          = fields_values[1]; 
    const int inst_ref              = fields_values[2]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__LAMBDA[lambda_type_expr_tree = ", int_string__stack(lambda_type_expr_tree)," ; localvar_ref = ", int_string__stack(localvar_ref)," ; inst_ref = ", int_string__stack(inst_ref), "]"); 
    
    
    
    const char * ident_cstr = "<lambda>"; 
    const int    ident_pstr = pscript_string__push(this -> string_env, ident_cstr); 
    
    
    
    EXCEPTION_RAISE("Need to implement constant lambda expression."); 

  }; break; 
  case PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST:  { 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST"); 
    const int arglist_subtree = fields_values[0]; 
    const int arg_subtree     = fields_values[1]; 
    //dputs_array(filedes, "PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST[arglist_subtree = ", int_string__stack(arglist_subtree)," ; arg_subtree = ", int_string__stack(arg_subtree), "]"); 
    int values_nb = 0; 
    values_nb += pscript_code_parser__expr_const_aux(this, syntax_env, arglist_subtree); 
    values_nb += pscript_code_parser__expr_const_aux(this, syntax_env, arg_subtree); 
    return values_nb; 
  }; break; 
  }; 
  return 1; 
}; 

// RL: It returns how many expressions it has pushed onto the constant expressions stack. 
static int pscript_code_parser__expr_const(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset, const int please_try_but_do_not_fail) { 
  int retval = INT_MIN; 
  PSCRIPT_EXCEPTION__BEGIN(pscript_code_parser__exception_env) { 
   case PSCRIPT_EXCEPTION_TYPE__TRY: retval = pscript_code_parser__expr_const_aux(this, syntax_env, chunk_offset); break; // RL: The code that will be run between handlers. 
   case EXCEPTION_CODE: if (please_try_but_do_not_fail) { retval = INT_MIN; break; }; PSCRIPT_EXCEPTION__PROPAGATE(pscript_code_parser__exception_env); break; 
  } PSCRIPT_EXCEPTION__END(pscript_code_parser__exception_env); // RL: The exception is propagated if no handler. Otherwise the handlers are dropped. 
  return retval; 
}; 




int pscript_code_parser__type__arglist_one(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__type_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE); 
  
  const int ident_token = fields_values[0]; 
  const int byref_huh   = fields_values[1]; 
  const int type_expr   = fields_values[2]; 
  
  //dputs_array(stderr__fileno, __func__, "--------------------------------------------- BYREF = ", bool_string(byref_huh), "\n"); 

  const int arg_name   = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
  const int arg_type_i = pscript_code_parser__type(this, syntax_env, type_expr);
  
  pscript_funproc__set_arg_push(this -> funproc_env, funproc_i, arg_name, byref_huh, arg_type_i); 
  
  return 0; 
}; 

int pscript_code_parser__type__arglist_nil(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__type_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL); 
  
  const int args_nb  = fields_values[0]; 

  const int arg_chunk_size = GET_CHUNK_SIZE__TYPE_TREE(PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE,-1); 
  int arg_chunk_offset = chunk_offset + chunk_size + args_nb * arg_chunk_size; 
  for (int i = 0; i < args_nb; i++) { 
    arg_chunk_offset -= arg_chunk_size; 
    pscript_code_parser__type__arglist_one(this, syntax_env, funproc_i, arg_chunk_offset); 
  }; 

  return 0; 
}; 

int pscript_code_parser__type__funproc(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__type_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), chunk_offset, i + field_shift); 
  }; 

  //dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "chunk_type: ", int_string__stack(chunk_type), "\n"); 
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_TYPE_TREE__TYPE__FUNPROC); 
  
  const int arg_list         = fields_values[0]; 
  const int retval_type_expr = fields_values[1]; 
  
  int retval_type_i; 
  if (retval_type_expr >= 0) { 
    retval_type_i = pscript_code_parser__type(this, syntax_env, retval_type_expr); 
  } 
  else { 
    retval_type_i = pscript_type__get__empty(this -> type_env); 
  }; 
  pscript_funproc__set_retval_type(this -> funproc_env, funproc_i, retval_type_i); 
  
  if (arg_list >= 0) { 
    pscript_code_parser__type__arglist_nil(this, syntax_env, funproc_i, arg_list); 
  }; 
  
  return 0; 
}; 




int pscript_code_parser__type__methode(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__type_env(syntax_env) -> memory__array_nb); 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), chunk_offset, i + field_shift); 
  }; 

  //dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "chunk_type: ", int_string__stack(chunk_type), "\n"); 
  EXCEPTION_ASSERT(chunk_type == PSCRIPT_TYPE_TREE__TYPE__METHODE); 
  
  const int of_type_expr     = fields_values[0]; 
  const int arg_list         = fields_values[1]; 
  const int retval_type_expr = fields_values[2]; 
  
  int retval_type_i; 
  if (retval_type_expr >= 0) { 
    retval_type_i = pscript_code_parser__type(this, syntax_env, retval_type_expr); 
  } 
  else { 
    retval_type_i = pscript_type__get__empty(this -> type_env); 
  }; 
  pscript_funproc__set_retval_type(this -> funproc_env, funproc_i, retval_type_i); 
  
  if (arg_list >= 0) { 
    pscript_code_parser__type__arglist_nil(this, syntax_env, funproc_i, arg_list); 
  }; 
  
  return 0; 
}; 




int pscript_code_parser__decl_local__funproc(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int chunk_offset) { 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__decl_local_env(syntax_env) -> memory__array_nb);  
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__decl_local_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__decl_local_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__decl_local_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 
    //int used_bytes = chunk_size; 
  
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__decl_local_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  
  
  EXCEPTION_ASSERT(PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE == chunk_type); 
  
  
  const int ident_token      = fields_values[0]; 
  const int typing_expr_tree = fields_values[1]; 
  const int tail             = fields_values[2]; 
  if (tail >= 0) { 
    pscript_code_parser__decl_local__funproc(this, syntax_env, funproc_i, tail); 
  }; 
  const int ident_pstr = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
  const int type_i     = pscript_code_parser__type(this, syntax_env, typing_expr_tree); 
  
  //pscript_funproc__set_localvar_type_nb__top(this -> funproc_env, ident_nb, type_i); 
  pscript_funproc__set_localvar_push(this -> funproc_env, funproc_i, /*localvar_name*/ident_pstr, /*localvar_type*/type_i); 
  
  
  
  
  
#if 0 
  switch (chunk_type) { 
  default: EXCEPTION_ASSERT(false); break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE: { 
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__VARIABLE", "\n"); 
    const int ident_token = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    const int tail = fields_values[2]; 
    if (tail >= 0) { 
      pscript_code_parser__decl_local(filedes, string_env, token_env, this, tail); 
    }; 
    dputs_array(filedes, " ", pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    pscript_code_parser__type(filedes, string_env, token_env, this, typing_expr_tree);
    dputs_array(filedes, ";"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL: { 
    dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__NIL", "\n"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS: { 
    dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__CONS", "\n"); 
  }; break;  
  case PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE: { 
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_LOCAL_LIST__TYPE__LOCALVAR__ONE", "\n"); 
    const int ident_token = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    dputs_array(filedes, " ", pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    pscript_code_parser__type(filedes, string_env, token_env, this, typing_expr_tree);
    dputs_array(filedes, ";"); 
  }; break;  
  }; 
#endif 
  
  
  return 0; 
}; 




int pscript_code_parser__type(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
#if 0 
  dputs_array(filedes, __func__, ": ", "chunk_offset: ", int_string__stack(chunk_offset), "\n");
  { 
    static int u = 0; 
    EXCEPTION_ASSERT(u == 0); 
    if (chunk_offset == 459) u++; 
  }; 
#endif 
  EXCEPTION_ASSERT(chunk_offset < pscript_parser__get__type_env(syntax_env) -> memory__array_nb);  
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0);  
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__type_env(syntax_env), chunk_offset); 
  const int descendants_nb = fields_nb - field_shift; 
  //dputs_array(filedes, "field_shift: ", int_string__stack(field_shift), "\n"); 
  //dputs_array(filedes, "fields_nb: ", int_string__stack(fields_nb), "\n"); 
  int fields_values[fields_nb - field_shift]; 

  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: { 
    EXCEPTION_ASSERT(chunk_type < PSCRIPT_TOKEN_COUNT); 
    EXCEPTION_ASSERT(3 == fields_nb); 
    EXCEPTION_ASSERT(1 == descendants_nb); 
    const int token_i = fields_values[0]; 
    switch (chunk_type) { 
    case PSCRIPT_TOKEN_BOOLEAN: return pscript_type__get__bool   (this -> type_env); break; 
    case PSCRIPT_TOKEN_INTEGER: return pscript_type__get__integer(this -> type_env); break; 
    case PSCRIPT_TOKEN_REAL:    return pscript_type__get__float  (this -> type_env); break; 
    case PSCRIPT_TOKEN_STRING:  return pscript_type__get__string (this -> type_env); break; 
    case PSCRIPT_TOKEN_VARIANT: return pscript_type__get__variant(this -> type_env); break; 
    case PSCRIPT_TOKEN_IDENT: do { 
	const int ident_token = fields_values[0]; 
	const int ident_pstr  = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
	const int type_env_i  = pscript_type__name__lookup(this -> type_env, ident_pstr); 
	if (type_env_i < 0) { EXCEPTION_RAISE("The type '", pscript_string__get_cstr(this -> string_env, ident_pstr),"' has not been defined yet."); }; 
	const int type_i = pscript_type__name__get_value(this -> type_env, type_env_i); 
	return type_i; 
      } while (false); break; 
    default: EXCEPTION_RAISE_AT_TOKEN(token_i, "The token (", int_string__stack(chunk_type), ")", pscript_token__type_cstr(chunk_type), " does not describe a type,"); break; 
    }; 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_CONST: { 
    const int  left_token = fields_values[0]; 
    const int right_token = fields_values[1]; 
    pscript_code_parser__token_const(this, syntax_env, left_token); 
    const int const_i_value = pscript_const__stack__top_get_value(this -> const_env); 
    pscript_const__stack__pop(this -> const_env); 
    pscript_code_parser__token_const(this, syntax_env, left_token); 
    const int const_j_value = pscript_const__stack__top_get_value(this -> const_env); 
    pscript_const__stack__pop(this -> const_env); 
    const int type_i = pscript_type__get_or_make__int_range(this -> type_env, const_i_value, const_j_value);   
    return type_i; 
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__SUBRANGE_EXPR: { 
    const int  left_expr = fields_values[0]; 
    const int right_expr = fields_values[1]; 
    pscript_code_parser__expr_const(this, syntax_env, left_expr, /*please_try_but_do_not_fail*/false); 
    const int const_i_value = pscript_const__stack__top_get_value(this -> const_env); 
    pscript_const__stack__pop(this -> const_env); 
    pscript_code_parser__expr_const(this, syntax_env, left_expr, /*please_try_but_do_not_fail*/false); 
    const int const_j_value = pscript_const__stack__top_get_value(this -> const_env); 
    pscript_const__stack__pop(this -> const_env); 
    const int type_i = pscript_type__get_or_make__int_range(this -> type_env, const_i_value, const_j_value);   
    return type_i; 
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__ARRAY: { 
    const int min_expr  = fields_values[0]; 
    const int max_expr  = fields_values[1]; 
    const int type_expr = fields_values[2]; 

    pscript_code_parser__expr_const(this, syntax_env, min_expr, /*please_try_but_do_not_fail*/false); 
    const int const_i_value = pscript_const__stack__top_get_value(this -> const_env); 
    pscript_const__stack__pop(this -> const_env); 
    int const_j_value; 
    if (max_expr >= 0) { 
      pscript_code_parser__expr_const(this, syntax_env, max_expr, /*please_try_but_do_not_fail*/false); 
      const_j_value = pscript_const__stack__top_get_value(this -> const_env); 
      pscript_const__stack__pop(this -> const_env); 
    }; 
    
    const int subtype_i = pscript_code_parser__type(this, syntax_env, /*chunk_offset*/type_expr); 
    int type_i; 
    if (max_expr >= 0) { 
      type_i = pscript_type__make__array(this -> type_env, subtype_i, const_i_value, const_j_value); 
    }
    else { 
      type_i = pscript_type__make__array(this -> type_env, subtype_i, 0, const_i_value - 1); 
    }; 
    return type_i; 
  }; break;  
  case PSCRIPT_TYPE_TREE__TYPE__RECORD: { 
    const int record_fields_nb = fields_values[0]; 
    const int type_i = pscript_type__make__record__empty(this -> type_env); 
    const int record_chunk_sizeof = GET_CHUNK_SIZE__TYPE_TREE(PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS,2); 
    int record_chunk_offset = chunk_offset; // - record_chunk_sizeof; 
    for (int i = 0; i < record_fields_nb; i++) { 
      record_chunk_offset -= record_chunk_sizeof; 
      const int record_chunk_type = pscript_chunk_env__get__type(pscript_parser__get__type_env(syntax_env), record_chunk_offset); 
      EXCEPTION_ASSERT(record_chunk_type == PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS); 
      const int ident_token   = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), record_chunk_offset, 1);  
      const int type_expr     = pscript_chunk_env__get__field_value(pscript_parser__get__type_env(syntax_env), record_chunk_offset, 2); 
      const int ident_pstr    = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
      const int record_type_i = pscript_code_parser__type(this, syntax_env, /*chunk_offset*/type_expr); 
      pscript_type__make__record__push_field(this -> type_env, type_i, ident_pstr, record_type_i); 
    }; 
    return type_i; 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_NIL: { 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS: { 
    //dputs_array(filedes, "PSCRIPT_TYPE_TREE__TYPE__RECORD_FIELD_CONS", "\n"); 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__FUNPROC: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__FUNPROC"); 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__METHODE: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__METHODE"); 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__NIL"); 
    EXCEPTION_ASSERT(false); 
#if 0 
    const int args_nb = fields_values[0]; 
    //dputs_array(stderr__fileno, "----------------> ARGS_NB: ", int_string__stack(args_nb), "\n"); 
    //int arg_chunk_offset = chunk_offset + chunk_size; 
    const int arg_chunk_size = GET_CHUNK_SIZE__TYPE_TREE(PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE,-1); 
    int arg_chunk_offset = chunk_offset + chunk_size + args_nb * arg_chunk_size; 
    for (int i = 0; i < args_nb; i++) { 
      arg_chunk_offset -= arg_chunk_size; 
      pscript_code_parser__type(this, syntax_env, arg_chunk_offset); 
      //arg_chunk_offset += arg_chunk_size; 
      if (i < args_nb - 1) { 
	dputs_array(filedes, "; "); 
      }; 
    };
#endif  
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__CONS"); 
    EXCEPTION_ASSERT(false); 
  }; break; 
  case PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE: { 
    //dputs(filedes, "PSCRIPT_TYPE_TREE__TYPE__ARG_LIST__ONE"); 
    EXCEPTION_ASSERT(false); 
    const int ident_token = fields_values[0]; 
    const int byref_huh   = fields_values[1]; 
    const int type_expr   = fields_values[2]; 
#if 0 
    if (byref_huh) { 
      dputs(filedes, "var "); 
    }; 
    dputs_array(filedes, pscript_token__get_srcval_cstr(token_env, ident_token), " : "); 
    pscript_code_parser__type(this, syntax_env, type_expr);
    //dputs_array(filedes, ";"); 
    //pscript_code_parser__type(this, syntax_env, chunk_offset + chunk_size);
#endif 
  }; break; 
    
  }; 
  return -1; 
}; 



static void pscript_code_parser__decl_toplevel__funproc_decl(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int of_type_i, const int typing_expr_tree, const int localvar_ref) { 
  // RL: 'lexical_env_i': To be able to deallocate the retval, arguments, and the localvars which are pushed onto the compile-time environment. 
  const int lexical_env_i = pscript_env__get_offset(this -> env); 
  pscript_funproc__set_lexical_env(this -> funproc_env, funproc_i, lexical_env_i); 
  
  // RL: Set 'this' 
  if (of_type_i >= 0) { 
    pscript_funproc__set_method_of(this -> funproc_env, funproc_i, of_type_i); 
  }; 
  
  // RL: Set 'retval_type' & args. 
  if (of_type_i < 0) { 
    pscript_code_parser__type__funproc(this, syntax_env, funproc_i, typing_expr_tree); 
  } 
  else { 
    pscript_code_parser__type__methode(this, syntax_env, funproc_i, typing_expr_tree); 
  };
  
  // RL: Set localvars. 
  if (localvar_ref >= 0) { 
    pscript_code_parser__decl_local__funproc(this, syntax_env, funproc_i, localvar_ref); 
  }; 
}; 

static void pscript_code_parser__decl_toplevel__funproc_def(pscript_t * this, const pscript_parser_env_t * syntax_env, const int funproc_i, const int inst_ref0, const int lambda_huh) { 
  funproc_i_being_compiled__global = funproc_i; 
  
  const int nesting_parent = pscript_funproc__lambda__nesting_funproc_i__get(this -> funproc_env, funproc_i); 
  int nesting_funproc__enclosing_jump__label = -1; 
  //if (nesting_parent >= 0) { // RL: Does not work as lambda objects inside mainprocs will have a nesting_parent of -1. 
  if (lambda_huh) { 
    // RL: Currently, the code of a lambda object is define inside of the code of the nesting function. 
    //     This is far from great, but that does the job. 
    // RL: To realize that, we have to put a jump for the nesting function. 
    //     Here it is. 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__JUMP); 
    nesting_funproc__enclosing_jump__label = pscript_bytecode__push_int(this -> bytecode_env, 0); 
  }; 

  const int funproc_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
  pscript_funproc__set_bytecode_offset(this -> funproc_env, funproc_i, funproc_bytecode_offset); 
    
    // RL: CALL MEMORY-FRAME 
    //     _________________ 
    //      - retval 
    //      - arg 
    //      - localvar 
    //      - stack of references to lexical env 
    //      - frame-size 
    //      - reference counter 
    //      - [EBP points here] 
    //  
    // RL: The memory-frame is upside-down in order to have retval at bottom - that way, we will not need to move it when returning. 
    // RL: The memory-frame is managed by the callee (allocation & de-allocation). 
    // RL: The callee removes the arguments from the expression-stack. 
    // RL: The callee pushes the retval onto the expression-stack (or its reference if it is not a scalar object). 
    // RL: The callee extends the compile-time environment (retval, args, localvars). 
    
    // RL: Memory-frame allocation 
    const int retval_type_i            = pscript_funproc__get_retval_type(this -> funproc_env, funproc_i); 
    const int retval_type_scalar_huh   = pscript_type__scalar_huh(this -> type_env, retval_type_i); //int_member_array(pscript_type__get_core_type(this -> type_env, retval_type_i), scalar_types); 
    const int method_of_type_i         = pscript_funproc__get_method_of(this -> funproc_env, funproc_i); 
    const int method_of_type_i__proxy  = method_of_type_i < 0 ? -1 : pscript_type__make__proxy(this -> type_env, method_of_type_i); 
    const int method_of_type_i__proxy__sizeof = method_of_type_i < 0 ? 0 : pscript_type__sizeof(this -> type_env, method_of_type_i__proxy); 
    const int retval_sizeof            = pscript_type__sizeof(this -> type_env, retval_type_i); 
    const int arg_all_sizeof           = pscript_funproc__get_arg_all_sizeof(this -> funproc_env, funproc_i); 
    const int localvar_all_sizeof      = pscript_funproc__get_localvar_all_sizeof(this -> funproc_env, funproc_i); 
    const int nesting_level            = pscript_funproc__lambda__nesting_level__get(this -> funproc_env, funproc_i);
    const int lexical_env_stack_sizeof = (sizeof(char)) + (2 + nesting_level) * (sizeof(int)); 
    const int frame_size_sizeof        = (sizeof(int)); 
    const int reference_counter_sizeof = (sizeof(int)); 
    const int total_allocated_mem_size = retval_sizeof + method_of_type_i__proxy__sizeof + arg_all_sizeof + localvar_all_sizeof + lexical_env_stack_sizeof + frame_size_sizeof + reference_counter_sizeof; 
    //assert(funproc_bytecode_offset != 1178); 
#if 1
    const int frame_size = total_allocated_mem_size; // RL: In some earlier versions, these two were different. 
#else 
    const int frame_size = total_allocated_mem_size - (retval_type_scalar_huh ? retval_sizeof : 0); 
    
    if (not(retval_type_scalar_huh)) { 
      // RL: 'retval' is then owned by the caller. 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_ALLOCATE); 
      pscript_bytecode__push_int(this -> bytecode_env, retval_sizeof); 
    }; 
#endif 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEMORY_FRAME__ALLOC_AND_INC); 
    pscript_bytecode__push_int(this -> bytecode_env, frame_size); 
    //if (nesting_parent < 0) { 
    if (!lambda_huh) { 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEMORY_FRAME__LEX_ENV__STATIC_INIT); 
    }
    else {  
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEMORY_FRAME__LEX_ENV__LAMBDA_INIT); 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__DROP); // RL: The lambda object. 
    }; 

    
    // RL: Computing offsets & adding the names to the compile-time environment. 
    const int rec_pstr = pscript_string__push(this -> string_env, "rec"); 
    pscript_env__push_proc(this -> env, rec_pstr, funproc_i); 

    int cumulated_offset = - total_allocated_mem_size; 
    const int retval_offset = cumulated_offset; 
    if (retval_sizeof > 0) { 
      pscript_env__push_retval(this -> env, funproc_i, 0); 
      pscript_funproc__set_retval_offset(this -> funproc_env, funproc_i, retval_offset); 
      cumulated_offset += retval_sizeof; 
    }; 
    
    const int methodof_offset = cumulated_offset; 
    if (method_of_type_i >= 0) { 
      const char * methodof_ident_cstr = "this"; 
      const int    methodof_ident_pstr = pscript_string__push(this -> string_env, methodof_ident_cstr); 
      pscript_env__push_method_this(this -> env, methodof_ident_pstr, funproc_i, 0); // method_of_type_i 
      //pscript_funproc__set_method_of__offset(this -> funproc_env, funproc_i, methodof_offset); 
      pscript_funproc__set_method_this__offset(this -> funproc_env, funproc_i, methodof_offset); 
      cumulated_offset += method_of_type_i__proxy__sizeof; 
    }; 
    
    const int arg_nb = pscript_funproc__get_arg_nb(this -> funproc_env, funproc_i); 
    int arg_offset[arg_nb]; 
    int arg_sizeof[arg_nb]; 
    for (int i = 0; i < arg_nb; i++) { 
      arg_offset[i] = cumulated_offset; 
      const int arg_name      = pscript_funproc__get_arg_name (this -> funproc_env, funproc_i, i); 
      const int arg_byref_huh = pscript_funproc__get_arg_byref(this -> funproc_env, funproc_i, i); 
      pscript_env__push_arg(this -> env, arg_name, funproc_i, i); 
      if (arg_byref_huh) { 
	//arg_sizeof[i] = pscript_type__sizeof__pointer(this -> type_env); 
	arg_sizeof[i] = pscript_type__sizeof__proxy(this -> type_env); 
      } 
      else { 
	arg_sizeof[i] = pscript_funproc__get_arg_sizeof(this -> funproc_env, funproc_i, i); 
      }; 
      pscript_funproc__set_arg_offset(this -> funproc_env, funproc_i, i, arg_offset[i]); 
      cumulated_offset += arg_sizeof[i]; 
    }; 
    
    const int localvar_nb = pscript_funproc__get_localvar_nb(this -> funproc_env, funproc_i); 
    int localvar_offset[localvar_nb]; 
    for (int i = 0; i < localvar_nb; i++) { 
      localvar_offset[i] = cumulated_offset; 
      const int localvar_name = pscript_funproc__get_localvar_name(this -> funproc_env, funproc_i, i); 
      pscript_env__push_localvar(this -> env, localvar_name, funproc_i, i); 
      pscript_funproc__set_localvar_offset(this -> funproc_env, funproc_i, i, localvar_offset[i]); 
      cumulated_offset += pscript_funproc__get_localvar_sizeof(this -> funproc_env, funproc_i, i); 
    }; 
    
    const int lexical_env_stack_offset = cumulated_offset; 
    cumulated_offset += lexical_env_stack_sizeof; 
    
    const int frame_size_offset = cumulated_offset; 
    cumulated_offset += frame_size_sizeof; 
    
    const int reference_counter_offset = cumulated_offset; 
    cumulated_offset += reference_counter_sizeof; 
    
    
    // RL: Pushing the expected number of arguments. 
    //     Therefore, the number of arguments is checked at compile-time & at run-time. 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__CHECK_ARG_NB); 
    if (method_of_type_i >= 0) { 
      pscript_bytecode__push_int8(this -> bytecode_env, arg_nb + 1); // RL: 'this' has to be included. 
    }
    else { 
      pscript_bytecode__push_int8(this -> bytecode_env, arg_nb); 
    }; 
    
    // RL: Now, moving the args which are on the stack into the local memory. 
    // RL: We can't just copy them altogether into the memory as the types may mismatched (type conversion might be needed).  
    // RL: The calling-convention is postfix (" 1 2 + "), therefore the lastest one is at the stack top. 
    for (int i = 0; i < arg_nb; i++) { 
      // RL: If a type conversion is needed, then it's performed by the VM within the instruction MEM_STORE_LOCAL. 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_STORE_LOCAL); 
      const int arg_i     = arg_nb - i - 1; 
      const int arg_type  = pscript_funproc__get_arg_type (this -> funproc_env, funproc_i, arg_i); 
      const int arg_byref = pscript_funproc__get_arg_byref(this -> funproc_env, funproc_i, arg_i); 
      if (arg_byref) { 
	const int lea_type_i = pscript_type__make__proxy(this -> type_env, arg_type); 
	pscript_bytecode__push_int8(this -> bytecode_env, lea_type_i); 
      } 
      else { 
	pscript_bytecode__push_int8(this -> bytecode_env, arg_type); 
      }; 
      //pscript_bytecode__push_int(this -> bytecode_env, arg_offset[arg_i]); 
      pscript_bytecode__push_mem_offset (this -> bytecode_env, arg_offset[arg_i]); 
    }; 

    // RL: Now, moving 'this' which are on the stack into the local memory. 
    if (method_of_type_i >= 0) { 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEM_STORE_LOCAL); 
      pscript_bytecode__push_int8(this -> bytecode_env, method_of_type_i__proxy); 
      pscript_bytecode__push_mem_offset (this -> bytecode_env, methodof_offset); 
    }; 

    

    
    // RL: Bytecode generation 
    if (inst_ref0 >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_ref0); 
    }; 
    


    // RL: Pop-ing the localvars & args from the compile-time environment. 
    { 
      const int former_env_i = pscript_funproc__get_lexical_env(this -> funproc_env, funproc_i); 
      pscript_env__deallocate(this -> env, former_env_i); 
    }; 
    
    // RL: De-allocating the local memory, and pushing 'retval' onto the stack. 
    { 
      const int retval_type = pscript_funproc__get_retval_type(this -> funproc_env, funproc_i); 
      pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__MEMORY_FRAME__DEC_AND_CONDITIONALLY_DEALLOC); 
      pscript_bytecode__push_int8(this -> bytecode_env, retval_type); 
    }; 
    
    // RL: For methods, object types are not computed at compile-time, 
    //     implying that we do not whether they will return anything. 
    //     Therefore, they must always return something. 
    if (method_of_type_i >= 0) { 
      const int retval_type    = pscript_funproc__get_retval_type(this -> funproc_env, funproc_i); 
      const int retval_sizeof  = pscript_type__sizeof(this -> type_env, retval_type); 
      if (0 == retval_sizeof) { 
	pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	pscript_bytecode__push_int8       (this -> bytecode_env, retval_type); 
      }; 
    }; 

    // RL: For lambdas, object types are not computed at compile-time, 
    //     implying that we do not whether they will return anything. 
    //     Therefore, they must always return something. 
    //if (nesting_parent >= 0) { 
    if (lambda_huh) { 
      const int retval_type    = pscript_funproc__get_retval_type(this -> funproc_env, funproc_i); 
      const int retval_sizeof  = pscript_type__sizeof(this -> type_env, retval_type); 
      if (0 == retval_sizeof) { 
	pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__PUSH); 
	pscript_bytecode__push_int8       (this -> bytecode_env, retval_type); 
      }; 
    }; 
    
    
    // RL: Bye. 
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__RET); 
    
    // RL: Setting bytecode size. 
    const int begin_funproc_bytecode_offset = pscript_funproc__get_bytecode_offset(this -> funproc_env, funproc_i); 
    const int   end_funproc_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    const int funproc_bytecode_size = end_funproc_bytecode_offset - begin_funproc_bytecode_offset; 
    pscript_funproc__set_bytecode_size(this -> funproc_env, funproc_i, funproc_bytecode_size); 
    
    //if (nesting_parent >= 0) { 
    if (lambda_huh) { 
      // RL: Code is nested. I will improve that later. In the meantime, the jump target address has to be set. 
      pscript_bytecode__rewrite_int(this -> bytecode_env, nesting_funproc__enclosing_jump__label, end_funproc_bytecode_offset); 
    }; 

    //if (nesting_parent < 0) { 
    if (!lambda_huh) { 
      funproc_i_being_compiled__global = -1; 
    } 
    else { 
      // RL: Code is nested. Restoring previous context. 
      funproc_i_being_compiled__global = nesting_parent; 
    }; 
}; 





int pscript_code_parser__decl_toplevel(pscript_t * this, const pscript_parser_env_t * syntax_env, const int chunk_offset) { 
  const int chunk_size  = pscript_chunk_env__get__chunk_size(pscript_parser__get__decl_toplevel_env(syntax_env), chunk_offset); 
  const int chunk_type  = pscript_chunk_env__get__type(pscript_parser__get__decl_toplevel_env(syntax_env), chunk_offset); 
  const int field_shift = pscript_parser_chunk_env__type__get_field_shift(chunk_type); 
  const int fields_nb   = pscript_chunk_env__get__fields_nb(pscript_parser__get__decl_toplevel_env(syntax_env), chunk_offset); 
  EXCEPTION_ASSERT(chunk_size > 0); 
  int fields_values[fields_nb - field_shift]; 
  for (int i = 0; i < fields_nb - field_shift; i++) { 
    fields_values[i] = pscript_chunk_env__get__field_value(pscript_parser__get__decl_toplevel_env(syntax_env), chunk_offset, i + field_shift); 
  }; 
  
  switch (chunk_type) { 
  default: EXCEPTION_RAISE("Unknown chunk_type: ", int_string__stack(chunk_type)); break; 
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE: { 
    //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__CONSTANTE", "\n"); 
    const int ident_token      = fields_values[0]; 
    const int expr_tree        = fields_values[1]; 
    const int ident_pstr       = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    int  const_env_i  = pscript_const__env__name__lookup(this -> const_env, ident_pstr); 
    if ( const_env_i >= 0) { EXCEPTION_RAISE("A constant identified by '", pscript_string__get_cstr(this -> string_env, ident_pstr), "' was already defined."); }; 
    int global_env_i  = pscript_env__name__lookup(this -> env, ident_pstr); 
    if (global_env_i >= 0) { EXCEPTION_RAISE("An item already uses the identifier '", pscript_string__get_cstr(this -> string_env, ident_pstr), "'."); }; 
    //pscript_const__stack__push_int(this -> const_env, expr_tree); // RL: TODO XXX FIXME: This expr has to be evaluated. 
    pscript_code_parser__expr_const(this, syntax_env, expr_tree, /*please_try_but_do_not_fail*/false); 
    const_env_i  = pscript_const__env__name__push_from_stack__nolookup(this -> const_env, ident_pstr); 
    global_env_i = pscript_env__push_const(this -> env, ident_pstr, const_env_i); 
  }; break;  
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__TYPE: { 
    const int ident_token      = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    const int ident_pstr       = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    const int type_env_i = pscript_type__name__lookup(this -> type_env, ident_pstr); 
    if (type_env_i >= 0) { EXCEPTION_RAISE("A type named '", pscript_string__get_cstr(this -> string_env, ident_pstr),"' has already been defined."); }; 
    pscript_type__name__push_nolookup(this -> type_env, ident_pstr, -1); 
    const int type_i = pscript_code_parser__type(this, syntax_env, /*chunk_offset*/typing_expr_tree); 
    pscript_type__name__define_udf__for_nb_names_on_top(this -> type_env, 1, type_i); 
  }; break;  
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE: { 
    //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__VARIABLE", "\n"); 
    const int ident_token      = fields_values[0]; 
    const int typing_expr_tree = fields_values[1]; 
    const int ident_pstr       = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    int var_env_i = pscript_var__env__name__lookup(this -> var_env, ident_pstr); 
    if (var_env_i >= 0) { EXCEPTION_RAISE("A variable named '", pscript_string__get_cstr(this -> string_env, ident_pstr), "' has already been defined."); }; 
    var_env_i = pscript_var__env__name__push_nolookup(this -> var_env, ident_pstr, -1); 
    const int type_i = typing_expr_tree >= 0 ? pscript_code_parser__type(this, syntax_env, /*chunk_offset*/typing_expr_tree) : pscript_type__get__variant(this -> type_env); 
    pscript_var__env__name__set_type__nb(this -> var_env, /*type_i*/type_i, /*ident_nb*/1); 
    const int global_env_i  = pscript_env__name__lookup(this -> env, ident_pstr); 
    if (global_env_i >= 0) { EXCEPTION_RAISE("An item already uses the identifier '", pscript_string__get_cstr(this -> string_env, ident_pstr), "'."); }; 
    pscript_env__push_var(this -> env, ident_pstr, var_env_i); 
  }; break; 
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC: { 
    //dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__MAINPROC", "\n"); 
    const int end_ref   = fields_values[0]; 
    const int inst_ref0 = fields_values[1]; 
    const int mainproc_i = pscript_mainproc__push_empty(this -> mainproc_env); 
    const int lexical_env_i = pscript_env__get_offset(this -> env); 
    pscript_mainproc__set_lexical_env(this -> mainproc_env, mainproc_i, lexical_env_i); 
    const int mainproc_bytecode_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    pscript_mainproc__set_bytecode_offset(this -> mainproc_env, mainproc_i, mainproc_bytecode_offset); 
    
    if (inst_ref0 >= 0) { 
      pscript_code_parser__inst(this, syntax_env, inst_ref0); 
    }; 
    
    pscript_bytecode__push_instruction(this -> bytecode_env, PVM_BYTECODE__HALT); 
    const int end_offset = pscript_bytecode__get_next_offset(this -> bytecode_env); 
    const int begin_offset = mainproc_bytecode_offset; 
    const int mainproc_bytecode_size = end_offset - begin_offset; 
    pscript_mainproc__set_bytecode_size(this -> mainproc_env, mainproc_i, mainproc_bytecode_size); 
  }; break;  
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC: { 
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__FUNPROC", "\n"); 
    const int    ident_token      = fields_values[0]; 
    const int    typing_expr_tree = fields_values[1]; 
    const int    localvar_nb_s    = fields_values[2]; 
    const int    localvar_ref     = fields_values[3]; 
    const int    end_ref          = fields_values[4]; 
    const int    inst_ref0        = fields_values[5]; 
    const int    ident_pstr       = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    const char * ident_cstr       = pscript_token__get_srcval_cstr(this -> token_env, ident_token); 
    
    // RL: Is there something in the environment? 
    {
      int global_env_i  = pscript_env__name__lookup(this -> env, ident_pstr); 
    if (global_env_i >= 0) { EXCEPTION_RAISE("An item already uses the identifier '", ident_cstr, "'."); }; 
    }; 

    const int funproc_i = pscript_funproc__push_empty(this -> funproc_env); 
    const int env_i     = pscript_env__push_proc(this -> env, ident_pstr, funproc_i); 
    pscript_funproc__set_name(this -> funproc_env, funproc_i, ident_pstr); 

    pscript_code_parser__decl_toplevel__funproc_decl(this, syntax_env, funproc_i, /*of_type*/-1, typing_expr_tree, localvar_ref); 
    
    pscript_code_parser__decl_toplevel__funproc_def(this, syntax_env, funproc_i, inst_ref0, /*lambda_huh*/false); 
    
  }; break;  
  case PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD: { 
    //dput_spaces(filedes); dputs_array(filedes, "PSCRIPT_DECL_TOPLEVEL_LIST__TYPE__METHOD", "\n"); 
    const int    ident_token      = fields_values[0]; 
    const int    of_type__expr    = fields_values[1]; 
    const int    typing_expr_tree = fields_values[2]; 
    const int    localvar_nb      = fields_values[3]; 
    const int    localvar_ref     = fields_values[4]; 
    const int    end_ref          = fields_values[5]; 
    const int    inst_ref0        = fields_values[6]; 
    const int    ident_pstr       = pscript_token__get_srcval_pstr(this -> token_env, ident_token); 
    const char * ident_cstr       = pscript_token__get_srcval_cstr(this -> token_env, ident_token); 

    // RL: Is there a method already having that name? 
    const int of_type = of_type__expr < 0 ? -1 : pscript_code_parser__type(this, syntax_env, of_type__expr); 
    { 
      const int lookedup_index = pscript_type__methode__lookup(this -> type_env, of_type, ident_pstr); 
      if (lookedup_index >= 0) { EXCEPTION_RAISE("An method of the type (", int_string__stack(of_type), ")'", pscript_type__get_cstr(this -> type_env, of_type), "' is already named '", ident_cstr, "'."); }; 
    };  
    
    const int funproc_i = pscript_funproc__push_empty(this -> funproc_env); 
    
    pscript_funproc__set_name(this -> funproc_env, funproc_i, ident_pstr); 
    
    pscript_type__methode__push_funproc(this -> type_env, of_type, ident_pstr, funproc_i); 
    
    pscript_code_parser__decl_toplevel__funproc_decl(this, syntax_env, funproc_i, /*of_type*/of_type, typing_expr_tree, localvar_ref); 
    
    pscript_code_parser__decl_toplevel__funproc_def(this, syntax_env, funproc_i, inst_ref0, /*lambda_huh*/false); 
  }; break;  
    
  }; // switch 
  return chunk_size; 
}; // fun 









int pscript_code_parser(pscript_t * this, const pscript_parser_env_t * syntax_env) { 
  pscript_code_parser__exception_env = pscript_exception_env__make(); 

  PSCRIPT_EXCEPTION__BEGIN(pscript_code_parser__exception_env) { 
    case PSCRIPT_EXCEPTION_TYPE__TRY: { 
      int chunk_offset = 0; 
      for (;;) { 
	if (chunk_offset >= pscript_parser__get__decl_toplevel_env(syntax_env) -> memory__array_nb) { break; }; 
	const int chunk_size = pscript_code_parser__decl_toplevel(this, syntax_env, chunk_offset); 
	chunk_offset += chunk_size;     
      }; 
    }; break;
  } PSCRIPT_EXCEPTION__END(pscript_code_parser__exception_env); 
  
  return 0; 
}; 


























