#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_funproc.h" 
#include "pscript_funproc.ci" 



pscript_funproc_env_t * pscript_funproc_env__make(pscript_string_env_t * string_env, pscript_type_env_t * type_env) {
  MALLOC_BZERO(pscript_funproc_env_t,this); 
  this -> string_env = string_env; 
  this -> type_env = type_env; 
  return this;
}; 

void pscript_funproc_env__delete(pscript_funproc_env_t * this) {
  free(this);
};   

int pscript_funproc__get_count(const pscript_funproc_env_t * this) {
  return this -> funproc_nb; 
}; 


int pscript_funproc__get_type(pscript_funproc_env_t * this, const int funproc_i) {
  assert(funproc_i >= 0); 
  assert(funproc_i < this -> funproc_nb); 
  const int index = funproc_i; 
  
  if (this -> funproc_type[funproc_i] >= 0) { 
    return this -> funproc_type[funproc_i]; 
  }; 
  
  //int lambda__nesting_funproc_i[pscript_funproc__size]; // RL: -1 is top-level 
  
  //int method_of[pscript_funproc__size]; 

  if (this -> funproc_type[funproc_i] < 0) {
    this -> funproc_type[funproc_i] = pscript_type__make__funproc(this -> type_env, this -> arg_nb[index], this -> arg_name[index], this -> arg_byref[index], this -> arg_type[index], this -> retval_type[index]); 
  }; 
  return this -> funproc_type[funproc_i]; 
}; 

int pscript_funproc__get_name_pstr(const pscript_funproc_env_t * this, const int funproc_i) {
  assert(funproc_i >= 0); 
  assert(funproc_i < this -> funproc_nb); 
  return this -> funproc_name[funproc_i]; 
}; 

  


//int pscript_funproc__env__name__push_nolookup(pscript_funproc_env_t * this, const int name, const int type_i) {
int pscript_funproc__push_full(pscript_funproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int localvar_nb, const int localvar_name[], const int localvar_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size) {
  assert(this -> funproc_nb < pscript_funproc__size); 
  const int index = this -> funproc_nb; 
  
  this -> funproc_type[index] = -1; 

  this -> retval_type[index] = retval_type; 

  this -> method_of[index] = -1; 
  this -> lambda__nesting_funproc_i[index] = INT_MIN;

  assert(arg_nb < pscript_funproc__arg__size); 
  this -> arg_nb[index] = arg_nb; 
  for (int i = 0; i < arg_nb; i++) { 
    this -> arg_name[index][i] = arg_name[i]; 
    this -> arg_byref[index][i] = arg_byref[i]; 
    this -> arg_type[index][i] = arg_type[i]; 
  }; 

  assert(localvar_nb < pscript_funproc__localvar__size); 
  this -> localvar_nb[index] = localvar_nb; 
  for (int i = 0; i < localvar_nb; i++) { 
    this -> localvar_name[index][i] = localvar_name[i]; 
    this -> localvar_type[index][i] = localvar_type[i]; 
  }; 

  this -> funproc_name[index] = name; 
  this -> funproc_lexical_env[index] = lexical_env; 
  this -> funproc_src_code[index] = src_code; 
  this -> funproc_bytecode_offset[index] = bytecode_offset; 
  this -> funproc_bytecode_size[index] = bytecode_size; 

  this -> funproc_nb ++; 
  return index; 
}; 

int pscript_funproc__lookup(const pscript_funproc_env_t * this, const int name) {
  for (int i = 0; i < this -> funproc_nb; i++) {
    if (this -> funproc_name[i] == name) return i; 
  }; 
  return -1; 
}; 




int pscript_funproc__push_empty(pscript_funproc_env_t * this) {
  assert(this -> funproc_nb < pscript_funproc__size); 
  const int index = this -> funproc_nb; 
  if (0 != this -> funproc_nb) { 
    if (-1 == this -> retval_type[this -> funproc_nb-1]) {
    //if (-1 == this -> funproc_name[this -> funproc_nb-1]) {
    //if (-1 == this -> funproc_bytecode_offset[this -> funproc_nb-1]) {
      return this -> funproc_nb - 1; 
    };
  };
  
  pmessage("Pushing a new funproc (empty) - funproc_i: %d" "\n", index); 
  
  this -> retval_type[index] = -1; 
  this -> method_of[index] = -1; 
  this -> lambda__nesting_funproc_i[index] = -1;
  this -> arg_nb[index] = 0; 
  this -> localvar_nb[index] = 0; 
  this -> funproc_name[index] = -1; 
  this -> funproc_type[index] = -1; 
  this -> funproc_lexical_env[index] = -1; 
  this -> funproc_src_code[index] = -1; 
  this -> funproc_bytecode_offset[index] = -1; 
  this -> funproc_bytecode_size[index] = -1; 
  
  this -> funproc_nb ++; 
  return index; 
}; 









int pscript_funproc__top(pscript_funproc_env_t * this) { 
  assert(this -> funproc_nb > 0); 
  return this -> funproc_nb - 1; 
}; 
 
int pscript_funproc__set_name(pscript_funproc_env_t * this, const int funproc_i, const int name_pstr) { 
  //pmessage("pscript_funproc__set_name: funproc_i = %d - name_pstr = %d" "\n", funproc_i, name_pstr); 
  this -> funproc_name[funproc_i] = name_pstr; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_lexical_env(pscript_funproc_env_t * this, const int funproc_i, const int lexical_env) { 
  this -> funproc_lexical_env[funproc_i] = lexical_env; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_retval_type(pscript_funproc_env_t * this, const int funproc_i, const int retval_type_i) { 
  this -> retval_type[funproc_i] = retval_type_i; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_src_code(pscript_funproc_env_t * this, const int funproc_i, const int src_code) { 
  this -> funproc_src_code[funproc_i] = src_code; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_bytecode_offset(pscript_funproc_env_t * this, const int funproc_i, const int bytecode_offset) { 
  this -> funproc_bytecode_offset[funproc_i] = bytecode_offset; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_bytecode_size(pscript_funproc_env_t * this, const int funproc_i, const int bytecode_size) { 
  this -> funproc_bytecode_size[funproc_i] = bytecode_size; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_arg_push(pscript_funproc_env_t * this, const int funproc_i, const int arg_name, const int byref_huh, const int arg_type) { 
#if 0 
  //pmessage("Pushing arg [funproc_i: %d ; name: %s ; byref_huh: %s ; arg_type: %d ]" "\n", funproc_i, pscript_string__get_cstr(this -> string_env, arg_name), byref_huh ? "TRUE" : "FALSE", arg_type); 
  dputs_array(stderr__fileno, __FILE__, STRINGIFY(__LINE__), __func__, ": Pushing arg [funproc_i: ", int_string__stack(funproc_i)," ; name: ", pscript_string__get_cstr(this -> string_env, arg_name), " ; byref_huh: ", byref_huh ? "TRUE" : "FALSE", " ; arg_type: ", int_string__stack(arg_type), " ]" "\n"); 
#endif 
  assert(this -> arg_nb[funproc_i] < pscript_funproc__arg__size); 
  const int arg_i = this -> arg_nb[funproc_i]; 
  
  this -> arg_name [funproc_i][arg_i] = arg_name; 
  this -> arg_byref[funproc_i][arg_i] = byref_huh; 
  this -> arg_type [funproc_i][arg_i] = arg_type; 
  
  this -> arg_nb[funproc_i]++; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_localvar_push(pscript_funproc_env_t * this, const int funproc_i, const int localvar_name, const int localvar_type) { 
  //pmessage("Pushing localvar [funproc_i: %d ; name: %s ; localvar_type: %d ]" "\n", funproc_i, pscript_string__get_cstr(this -> string_env, localvar_name), localvar_type); 
  assert(this -> localvar_nb[funproc_i] < pscript_funproc__localvar__size); 
  const int localvar_i = this -> localvar_nb[funproc_i]; 
  
  this -> localvar_name[funproc_i][localvar_i] = localvar_name; 
  this -> localvar_type[funproc_i][localvar_i] = localvar_type; 
  
  this -> localvar_nb[funproc_i]++; 
  return funproc_i; 
}; 
 
int pscript_funproc__set_localvar_type_nb(pscript_funproc_env_t * this, const int funproc_i, const int ident_nb, const int type_i) { 
  assert(this -> localvar_nb[funproc_i] >= ident_nb); 
  for (int i = 0; i < ident_nb; i++) { 
    const int localvar_i = this -> localvar_nb[funproc_i] - ident_nb + i; 
    this -> localvar_type[funproc_i][i] = type_i; 
  }; 
  return funproc_i; 
}; 
 




int pscript_funproc__get_retval_type(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> retval_type[funproc_i]; 
}; 
 
int pscript_funproc__get_lexical_env(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> funproc_lexical_env[funproc_i]; 
}; 
 


int pscript_funproc__get_arg_all_sizeof(const pscript_funproc_env_t * this, const int funproc_i) { 
  int arg_all_sizeof = 0; 
  for (int i = 0; i < this -> arg_nb[funproc_i]; i++) { 
    if (this -> arg_byref[funproc_i][i]) { 
      arg_all_sizeof += pscript_type__sizeof__proxy(this -> type_env); 
    }
    else { 
      arg_all_sizeof += pscript_type__sizeof(this -> type_env, this -> arg_type[funproc_i][i]); 
    }; 
  }; 
  return arg_all_sizeof; 
}; 
 
int pscript_funproc__get_localvar_all_sizeof(const pscript_funproc_env_t * this, const int funproc_i) { 
  int localvar_all_sizeof = 0; 
  for (int i = 0; i < this -> localvar_nb[funproc_i]; i++) { 
    localvar_all_sizeof += pscript_type__sizeof(this -> type_env, this -> localvar_type[funproc_i][i]); 
  }; 
  return localvar_all_sizeof; 
}; 

int pscript_funproc__get_bytecode_offset(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> funproc_bytecode_offset[funproc_i]; 
}; 
 
int pscript_funproc__get_arg_nb(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> arg_nb[funproc_i];
}; 
 
int pscript_funproc__get_retval_sizeof(const pscript_funproc_env_t * this, const int funproc_i) { 
  return pscript_type__sizeof(this -> type_env, this -> retval_type[funproc_i]);
}; 
 
int pscript_funproc__get_retval_offset(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> retval_offset[funproc_i]; 
}; 
 
void pscript_funproc__set_retval_offset(pscript_funproc_env_t * this, const int funproc_i, const int retval_offset) {
  this -> retval_offset[funproc_i] = retval_offset; 
}; 

int pscript_funproc__get_arg_name(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i) { 
  return this -> arg_name[funproc_i][arg_i];
}; 

const int * pscript_funproc__get_arg_name__array( const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> arg_name[funproc_i];
}; 

int pscript_funproc__get_arg_type(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i) { 
  return this -> arg_type[funproc_i][arg_i];
}; 
 
const int * pscript_funproc__get_arg_type__array( const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> arg_type[funproc_i];
}; 
  
int pscript_funproc__get_arg_byref(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i) { 
  return this -> arg_byref[funproc_i][arg_i];
}; 
 
const int * pscript_funproc__get_arg_byref__array(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> arg_byref[funproc_i];
}; 


 
int pscript_funproc__get_arg_sizeof(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i) { 
  return pscript_type__sizeof(this -> type_env, this -> arg_type[funproc_i][arg_i]);
}; 
 
int pscript_funproc__get_arg_offset(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i) {
  return this -> arg_offset[funproc_i][arg_i]; 
}; 
 
void pscript_funproc__set_arg_offset(pscript_funproc_env_t * this, const int funproc_i, const int arg_i, const int arg_offset) {
  this -> arg_offset[funproc_i][arg_i] = arg_offset; 
}; 

int pscript_funproc__get_localvar_nb(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> localvar_nb[funproc_i];
}; 
 
int pscript_funproc__get_localvar_name(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i) { 
  return this -> localvar_name[funproc_i][localvar_i];
}; 
 
int pscript_funproc__get_localvar_type(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i) { 
  return this -> localvar_type[funproc_i][localvar_i];
}; 
 
int pscript_funproc__get_localvar_sizeof(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i) { 
  return pscript_type__sizeof(this -> type_env, this -> localvar_type[funproc_i][localvar_i]);
}; 

int pscript_funproc__get_localvar_offset(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i) {
  return this -> localvar_offset[funproc_i][localvar_i]; 
}; 
 
void pscript_funproc__set_localvar_offset(pscript_funproc_env_t * this, const int funproc_i, const int localvar_i, const int localvar_offset) {
  this -> localvar_offset[funproc_i][localvar_i] = localvar_offset; 
}; 

void pscript_funproc__set_method_of(pscript_funproc_env_t * this, const int funproc_i, const int method_of_type_i) {
  this -> method_of[funproc_i] = method_of_type_i; 
}; 

int pscript_funproc__get_method_of(const pscript_funproc_env_t * this, const int funproc_i) {
  return this -> method_of[funproc_i]; 
}; 

int pscript_funproc__get_method_this(const pscript_funproc_env_t * this, const int funproc_i) { 
  const int proxy_subtype = this -> method_of[funproc_i]; 
  const int proxy_type_i = pscript_type__make__proxy(this -> type_env, proxy_subtype); 
  return proxy_type_i; 
}; 

void pscript_funproc__set_method_of__offset(pscript_funproc_env_t * this, const int funproc_i, const int offset) {
  this -> method_of__offset[funproc_i] = offset; 
}; 

int pscript_funproc__get_method_of__offset(const pscript_funproc_env_t * this, const int funproc_i) {
   return this -> method_of__offset[funproc_i]; 
}; 

void pscript_funproc__set_method_this__offset(pscript_funproc_env_t * this, const int funproc_i, const int offset) {
  this -> method_of__offset[funproc_i] = offset; 
}; 

int pscript_funproc__get_method_this__offset(const pscript_funproc_env_t * this, const int funproc_i) {
   return this -> method_of__offset[funproc_i]; 
}; 

int pscript_funproc__get_method_of__sizeof(pscript_funproc_env_t * this, const int funproc_i) {
  return pscript_type__sizeof(this -> type_env, this -> method_of[funproc_i]); 
}; 

int pscript_funproc__get_method_this__sizeof(pscript_funproc_env_t * this, const int funproc_i) {
  const int proxy_subtype = this -> method_of[funproc_i]; 
  const int proxy_type_i = pscript_type__make__proxy(this -> type_env, proxy_subtype); 
  return pscript_type__sizeof(this -> type_env, proxy_type_i); 
}; 

void pscript_funproc__lambda__nesting_funproc_i__set(pscript_funproc_env_t * this, const int funproc_i, const int parent_funproc_i) { 
  this -> lambda__nesting_funproc_i[funproc_i] = parent_funproc_i; 
}; 

int pscript_funproc__lambda__nesting_funproc_i__get(const pscript_funproc_env_t * this, const int funproc_i) { 
  return this -> lambda__nesting_funproc_i[funproc_i]; 
}; 
 
int pscript_funproc__lambda__nesting_level__get(const pscript_funproc_env_t * this, const int funproc_i) { 
  const int parent_funproc_i = this -> lambda__nesting_funproc_i[funproc_i]; 
  const int parent_funproc__nesting_level = 0 > parent_funproc_i ? -1 : pscript_funproc__lambda__nesting_level__get(this, parent_funproc_i); 
  return 1 + parent_funproc__nesting_level; 
}; 
 


 


#if 1 

#if 0 
  int funproc_name[pscript_funproc__size]; 
  int funproc_lexical_env[pscript_funproc__size]; 
  int funproc_src_code[pscript_funproc__size]; 
  int funproc_bytecode_offset[pscript_funproc__size]; 
  int funproc_bytecode_size[pscript_funproc__size]; 
  int funproc_nb; 
#endif 

static void funproc_stack__print_one_funproc(const int filedes, const pscript_funproc_env_t * this, const pscript_string_env_t * string_env, const int funproc_i) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  
  const char * funproc_i__str = int_string__stack(funproc_i); 
  const int funproc_i__str__spaces_nb = MAX(0, 4 - strlen(funproc_i__str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, funproc_i__str__spaces_nb); 
  dputs(filedes, funproc_i__str); 
  dputs(filedes, ": "); 
  
  const int funproc_name_pstr = this -> funproc_name[funproc_i]; 
  const char * funproc_name_cstr = pscript_string__get_cstr(string_env, funproc_name_pstr); 
  const int funproc_name_cstr__nb_spaces = MAX(0, 17 - strlen(funproc_name_cstr)); 
  dputs(filedes, funproc_name_cstr); 
  dput_spaces(filedes, funproc_name_cstr__nb_spaces); 
  dputs(filedes, ": "); 

  const char * funproc_lenv__str = int_string__stack(this -> funproc_lexical_env[funproc_i]); 
  const int funproc_lenv__str__spaces_nb = MAX(0, 4 - strlen(funproc_lenv__str)); 
  dputs(filedes, "lexical env: "); 
  dput_spaces(filedes, funproc_lenv__str__spaces_nb); 
  dputs(filedes, funproc_lenv__str); 
  dputs(filedes, ": "); 
  
  const char * funproc_boffset__str = int_string__stack(this -> funproc_bytecode_offset[funproc_i]); 
  const int funproc_boffset__str__spaces_nb = MAX(0, 4 - strlen(funproc_boffset__str)); 
  dputs(filedes, "Bytecode offset: "); 
  dput_spaces(filedes, funproc_boffset__str__spaces_nb); 
  dputs(filedes, funproc_boffset__str); 
  dputs(filedes, ": "); 
  
  const char * funproc_bsize__str = int_string__stack(this -> funproc_bytecode_size[funproc_i]); 
  const int funproc_bsize__str__spaces_nb = MAX(0, 4 - strlen(funproc_bsize__str)); 
  dputs(filedes, "Bytecode size: "); 
  dput_spaces(filedes, funproc_bsize__str__spaces_nb); 
  dputs(filedes, funproc_bsize__str); 
  dputs(filedes, ": "); 
  

#if 0 
  const int srcval_len = pscript_string__get_len(this -> string_env, funproc_stack__get_srcval(this, funproc_i)); 
  const char * funproc_len_str = int_string__stack(srcval_len); 
  const int funproc_len_str_len = strlen(funproc_len_str); 
  const int funproc_len_str_len_spaces_nb = MAX(0, 6 - funproc_len_str_len); 
  dputs(filedes, funproc_type_str); 
  dput_spaces(filedes, nb_spaces); 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, funproc_len_str_len_spaces_nb); 
  dputs(filedes, funproc_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, funproc_stack__get_srcval_cstr(this, funproc_i)); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	funproc_stack__get_srcval_cstr(this, funproc_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
#endif 
}; 

static void funproc_stack__print_stats__base(const int filedes, const pscript_funproc_env_t * this, const int base) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  
  dputs(filedes, "FUNPROC_STACK("); 
  dputn(filedes, this -> funproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> funproc_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void funproc_stack__print_stats(const int filedes, const pscript_funproc_env_t * this) {
  funproc_stack__print_stats__base(filedes, this, pscript_funproc__size); 
}; 

void funproc_stack__print_nb(const int filedes, const pscript_funproc_env_t * this, const pscript_string_env_t * string_env, const int funproc_nb) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  dputs(filedes, "FUNPROC_STACK("); 
  dputn(filedes, this -> funproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_funproc__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> funproc_nb)) / ((double) pscript_funproc__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> funproc_nb,funproc_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    funproc_stack__print_one_funproc(filedes, this, string_env, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void funproc_stack__print_all(const int filedes, const pscript_funproc_env_t * this, const pscript_string_env_t * string_env) {
  funproc_stack__print_nb(filedes, this, string_env, this -> funproc_nb); 
}; 
#endif 
