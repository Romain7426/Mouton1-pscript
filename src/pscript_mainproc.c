#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_mainproc.h" 

enum { pscript_mainproc__size = (1 << 6) }; 
struct pscript_mainproc_env_t { 
  int mainproc_lexical_env[pscript_mainproc__size]; 
  int mainproc_src_code[pscript_mainproc__size]; 
  int mainproc_bytecode_offset[pscript_mainproc__size]; 
  int mainproc_bytecode_size[pscript_mainproc__size]; 
  int mainproc_nb; 
}; 




pscript_mainproc_env_t * pscript_mainproc_env__make(pscript_type_env_t * type_env) {
  MALLOC_BZERO(pscript_mainproc_env_t,this); 
  return this;
}; 

void pscript_mainproc_env__delete(pscript_mainproc_env_t * this) {
  free(this);
};   

int pscript_mainproc__get_count(const pscript_mainproc_env_t * this) {
  return this -> mainproc_nb; 
}; 


int pscript_mainproc__push_empty(pscript_mainproc_env_t * this) {
  assert(this -> mainproc_nb < pscript_mainproc__size); 
  const int index = this -> mainproc_nb; 

#if 0 
  if (0 != this -> mainproc_nb) { 
    if (-1 == this -> retval_type[this -> mainproc_nb-1]) {
      return this -> mainproc_nb - 1; 
    };
  };
#endif 
  
  pmessage("Pushing a new mainproc (empty) - mainproc_i: %d" "\n", index); 
  
  this -> mainproc_lexical_env[index] = -1; 
  this -> mainproc_src_code[index] = -1; 
  this -> mainproc_bytecode_offset[index] = -1; 
  this -> mainproc_bytecode_size[index] = -1; 
  
  this -> mainproc_nb ++; 
  return index; 
}; 


int pscript_mainproc__get_bytecode_offset(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  return this -> mainproc_bytecode_offset[mainproc_i]; 
}; 

int pscript_mainproc__set_bytecode_offset(pscript_mainproc_env_t * this, const int mainproc_i, const int bytecode_offset) { 
  this -> mainproc_bytecode_offset[mainproc_i] = bytecode_offset; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_bytecode_size(pscript_mainproc_env_t * this, const int mainproc_i, const int bytecode_size) { 
  this -> mainproc_bytecode_size[mainproc_i] = bytecode_size; 
  return mainproc_i; 
}; 

int pscript_mainproc__set_lexical_env(pscript_mainproc_env_t * this, const int mainproc_i, const int lexical_env) { 
  this -> mainproc_lexical_env[mainproc_i] = lexical_env; 
  return mainproc_i; 
}; 
 
 
 

#if 0 
//int pscript_mainproc__env__name__push_nolookup(pscript_mainproc_env_t * this, const int name, const int type_i) {
int pscript_mainproc__push_full(pscript_mainproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int localvar_nb, const int localvar_name[], const int localvar_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size) {
  assert(this -> mainproc_nb < pscript_mainproc__size); 
  const int index = this -> mainproc_nb; 
  
  this -> retval_type[index] = retval_type; 

  assert(arg_nb < pscript_mainproc__arg__size); 
  this -> arg_nb[index] = arg_nb; 
  for (int i = 0; i < arg_nb; i++) { 
    this -> arg_name[index][i] = arg_name[i]; 
    this -> arg_byref[index][i] = arg_byref[i]; 
    this -> arg_type[index][i] = arg_type[i]; 
  }; 

  assert(localvar_nb < pscript_mainproc__localvar__size); 
  this -> localvar_nb[index] = localvar_nb; 
  for (int i = 0; i < localvar_nb; i++) { 
    this -> localvar_name[index][i] = localvar_name[i]; 
    this -> localvar_type[index][i] = localvar_type[i]; 
  }; 

  this -> mainproc_name[index] = name; 
  this -> mainproc_lexical_env[index] = lexical_env; 
  this -> mainproc_src_code[index] = src_code; 
  this -> mainproc_bytecode_offset[index] = bytecode_offset; 
  this -> mainproc_bytecode_size[index] = bytecode_size; 

  this -> mainproc_nb ++; 
  return index; 
}; 

int pscript_mainproc__lookup(const pscript_mainproc_env_t * this, const int name) {
  for (int i = 0; i < this -> mainproc_nb; i++) {
    if (this -> mainproc_name[i] == name) return i; 
  }; 
  return -1; 
}; 











int pscript_mainproc__top(pscript_mainproc_env_t * this) { 
  assert(this -> mainproc_nb > 0); 
  return this -> mainproc_nb - 1; 
}; 
 
int pscript_mainproc__set_name(pscript_mainproc_env_t * this, const int mainproc_i, const int name_pstr) { 
  this -> mainproc_name[mainproc_i] = name_pstr; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_retval_type(pscript_mainproc_env_t * this, const int mainproc_i, const int retval_type_i) { 
  this -> retval_type[mainproc_i] = retval_type_i; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_src_code(pscript_mainproc_env_t * this, const int mainproc_i, const int src_code) { 
  this -> mainproc_src_code[mainproc_i] = src_code; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_arg_push(pscript_mainproc_env_t * this, const int mainproc_i, const int arg_name, const int byref_huh, const int arg_type) { 
  assert(this -> arg_nb[mainproc_i] < pscript_mainproc__arg__size); 
  const int arg_i = this -> arg_nb[mainproc_i]; 
  
  this -> arg_name[mainproc_i][arg_i] = arg_name; 
  this -> arg_byref[mainproc_i][arg_i] = byref_huh; 
  this -> arg_type[mainproc_i][arg_i] = arg_type; 
  
  this -> arg_nb[mainproc_i]++; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_localvar_push(pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_name, const int localvar_type) { 
  assert(this -> localvar_nb[mainproc_i] < pscript_mainproc__localvar__size); 
  const int localvar_i = this -> localvar_nb[mainproc_i]; 
  
  this -> localvar_name[mainproc_i][localvar_i] = localvar_name; 
  this -> localvar_type[mainproc_i][localvar_i] = localvar_type; 
  
  this -> localvar_nb[mainproc_i]++; 
  return mainproc_i; 
}; 
 
int pscript_mainproc__set_localvar_type_nb(pscript_mainproc_env_t * this, const int mainproc_i, const int ident_nb, const int type_i) { 
  assert(this -> localvar_nb[mainproc_i] > ident_nb); 
  for (int i = 0; i < ident_nb; i++) { 
    const int localvar_i = this -> localvar_nb[mainproc_i] - ident_nb + i; 
    this -> localvar_type[mainproc_i][i] = type_i; 
  }; 
  return mainproc_i; 
}; 
 




int pscript_mainproc__get_retval_type(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  return this -> retval_type[mainproc_i]; 
}; 
 
int pscript_mainproc__get_lexical_env(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  return this -> mainproc_lexical_env[mainproc_i]; 
}; 
 


int pscript_mainproc__get_arg_all_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  int arg_all_sizeof = 0; 
  for (int i = 0; i < this -> arg_nb[mainproc_i]; i++) { 
    arg_all_sizeof += pscript_type__sizeof(this -> type_env, this -> arg_type[mainproc_i][i]); 
  }; 
  return arg_all_sizeof; 
}; 
 
int pscript_mainproc__get_localvar_all_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  int localvar_all_sizeof = 0; 
  for (int i = 0; i < this -> localvar_nb[mainproc_i]; i++) { 
    localvar_all_sizeof += pscript_type__sizeof(this -> type_env, this -> localvar_type[mainproc_i][i]); 
  }; 
  return localvar_all_sizeof; 
}; 

int pscript_mainproc__get_arg_nb(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  return this -> arg_nb[mainproc_i];
}; 
 
int pscript_mainproc__get_arg_name(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i) { 
  return this -> arg_name[mainproc_i][arg_i];
}; 
 
int pscript_mainproc__get_arg_byref(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i) { 
  return this -> arg_byref[mainproc_i][arg_i];
}; 
 
int pscript_mainproc__get_arg_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i) { 
  return pscript_type__sizeof(this -> type_env, this -> arg_type[mainproc_i][arg_i]);
}; 
 
int pscript_mainproc__get_localvar_nb(const pscript_mainproc_env_t * this, const int mainproc_i) { 
  return this -> localvar_nb[mainproc_i];
}; 
 
int pscript_mainproc__get_localvar_name(const pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_i) { 
  return this -> localvar_name[mainproc_i][localvar_i];
}; 
 
int pscript_mainproc__get_localvar_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_i) { 
  return pscript_type__sizeof(this -> type_env, this -> localvar_type[mainproc_i][localvar_i]);
}; 


#endif 




 


#if 1 

#if 0 
  int mainproc_name[pscript_mainproc__size]; 
  int mainproc_lexical_env[pscript_mainproc__size]; 
  int mainproc_src_code[pscript_mainproc__size]; 
  int mainproc_bytecode_offset[pscript_mainproc__size]; 
  int mainproc_bytecode_size[pscript_mainproc__size]; 
  int mainproc_nb; 
#endif 

static void mainproc_stack__print_one_mainproc(const int filedes, const pscript_mainproc_env_t * this, const pscript_string_env_t * string_env, const int mainproc_i) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  
  const char * mainproc_i__str = int_string__stack(mainproc_i); 
  const int mainproc_i__str__spaces_nb = MAX(0, 4 - strlen(mainproc_i__str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, mainproc_i__str__spaces_nb); 
  dputs(filedes, mainproc_i__str); 
  dputs(filedes, ": "); 

#if 0  
  const int mainproc_name_pstr = this -> mainproc_name[mainproc_i]; 
  const char * mainproc_name_cstr = pscript_string__get_cstr(string_env, mainproc_name_pstr); 
  const int mainproc_name_cstr__nb_spaces = MAX(0, 17 - strlen(mainproc_name_cstr)); 
  dputs(filedes, mainproc_name_cstr); 
  dput_spaces(filedes, mainproc_name_cstr__nb_spaces); 
  dputs(filedes, ": "); 
#endif 

  const char * mainproc_lenv__str = int_string__stack(this -> mainproc_lexical_env[mainproc_i]); 
  const int mainproc_lenv__str__spaces_nb = MAX(0, 4 - strlen(mainproc_lenv__str)); 
  dputs(filedes, "lexical env: "); 
  dput_spaces(filedes, mainproc_lenv__str__spaces_nb); 
  dputs(filedes, mainproc_lenv__str); 
  dputs(filedes, ": "); 
  
  const char * mainproc_boffset__str = int_string__stack(this -> mainproc_bytecode_offset[mainproc_i]); 
  const int mainproc_boffset__str__spaces_nb = MAX(0, 4 - strlen(mainproc_boffset__str)); 
  dputs(filedes, "Bytecode offset: "); 
  dput_spaces(filedes, mainproc_boffset__str__spaces_nb); 
  dputs(filedes, mainproc_boffset__str); 
  dputs(filedes, ": "); 
  
  const char * mainproc_bsize__str = int_string__stack(this -> mainproc_bytecode_size[mainproc_i]); 
  const int mainproc_bsize__str__spaces_nb = MAX(0, 4 - strlen(mainproc_bsize__str)); 
  dputs(filedes, "Bytecode size: "); 
  dput_spaces(filedes, mainproc_bsize__str__spaces_nb); 
  dputs(filedes, mainproc_bsize__str); 
  dputs(filedes, ": "); 
  

#if 0 
  const int srcval_len = pscript_string__get_len(this -> string_env, mainproc_stack__get_srcval(this, mainproc_i)); 
  const char * mainproc_len_str = int_string__stack(srcval_len); 
  const int mainproc_len_str_len = strlen(mainproc_len_str); 
  const int mainproc_len_str_len_spaces_nb = MAX(0, 6 - mainproc_len_str_len); 
  dputs(filedes, mainproc_type_str); 
  dput_spaces(filedes, nb_spaces); 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, mainproc_len_str_len_spaces_nb); 
  dputs(filedes, mainproc_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, mainproc_stack__get_srcval_cstr(this, mainproc_i)); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	mainproc_stack__get_srcval_cstr(this, mainproc_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
#endif 
}; 

static void mainproc_stack__print_stats__base(const int filedes, const pscript_mainproc_env_t * this, const int base) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  dputs(filedes, "MAINPROC_STACK("); 
  dputn(filedes, this -> mainproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> mainproc_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void mainproc_stack__print_stats(const int filedes, const pscript_mainproc_env_t * this) {
  mainproc_stack__print_stats__base(filedes, this, pscript_mainproc__size); 
}; 

void mainproc_stack__print_nb(const int filedes, const pscript_mainproc_env_t * this, const pscript_string_env_t * string_env, const int mainproc_nb) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  dputs(filedes, "MAINPROC_STACK("); 
  dputn(filedes, this -> mainproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_mainproc__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> mainproc_nb)) / ((double) pscript_mainproc__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> mainproc_nb,mainproc_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    mainproc_stack__print_one_mainproc(filedes, this, string_env, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void mainproc_stack__print_all(const int filedes, const pscript_mainproc_env_t * this, const pscript_string_env_t * string_env) {
  mainproc_stack__print_nb(filedes, this, string_env, this -> mainproc_nb); 
}; 
#endif 

