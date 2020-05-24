#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_value.h" 
#include "pscript_ffiproc.h" 


pscript_ffiproc_env_t * pscript_ffiproc_env__make(pscript_type_env_t * type_env) {
  MALLOC_BZERO(pscript_ffiproc_env_t,this); 
  this -> type_env = type_env; 
  return this;
}; 

void pscript_ffiproc_env__delete(pscript_ffiproc_env_t * this) {
  free(this);
};   

int pscript_ffiproc__get_count(const pscript_ffiproc_env_t * this) {
  return this -> ffiproc_nb; 
}; 

int pscript_ffiproc__get_type(pscript_ffiproc_env_t * this, const int ffiproc_i) {
  assert(ffiproc_i >= 0); 
  assert(ffiproc_i < this -> ffiproc_nb); 
  const int index = ffiproc_i; 
  if (this -> ffiproc_type[ffiproc_i] < 0) {
    this -> ffiproc_type[ffiproc_i] = pscript_type__make__ffiproc(this -> type_env, this -> arg_nb[index], this -> arg_name[index], this -> arg_byref[index], this -> arg_type[index], this -> retval_type[index]); 
  }; 
  return this -> ffiproc_type[ffiproc_i]; 
}; 

  




int pscript_ffiproc__push(pscript_ffiproc_env_t * this, const int name, const int arg_atleast, const int retval_type, void * callback_data, pscript_ffiproc_callback_t * callback) {
  assert(this -> ffiproc_nb < pscript_ffiproc__size); 
  const int index = this -> ffiproc_nb; 
  this -> ffiproc_name         [index] = name; 
  this -> ffiproc_type         [index] = -1; 
  this -> ffiproc_callback_data[index] = callback_data; 
  this -> ffiproc_callback     [index] = callback; 
  this -> arg_atleast          [index] = arg_atleast; 
  this -> retval_type          [index] = retval_type; 
  this -> method_of__type      [index] = -1; 
  this -> ffiproc_nb ++; 
  return index; 
}; 

int pscript_ffiproc__lookup(const pscript_ffiproc_env_t * this, const int name) {
  for (int i = 0; i < this -> ffiproc_nb; i++) {
    if (this -> ffiproc_name[i] == name) return i; 
  }; 
  return -1; 
}; 
 
 
int pscript_ffiproc__get_atleast_arg_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> arg_atleast[ffiproc_i]; 
}; 
 
int pscript_ffiproc__get_retval_type(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> retval_type[ffiproc_i]; 
}; 
 
int pscript_ffiproc__get_method_of_type(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> method_of__type[ffiproc_i]; 
}; 
 
int pscript_ffiproc__get_arg_byref(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i) { 
#if 0
  if (10 == ffiproc_i) { pmesserr("%s: " "ffiproc_i = %d - arg_i = %d - byref: %s " "\n", __func__, ffiproc_i, arg_i, this -> arg_byref[ffiproc_i][arg_i]?"TRUE":"FALSE"); }; 
#endif 
  if (arg_i > this -> arg_nb[ffiproc_i]) return false; 
  return this -> arg_byref[ffiproc_i][arg_i]; 
}; 
 
 
 
int pscript_ffiproc__set_method_of_type(pscript_ffiproc_env_t * this, const int ffiproc_i, const int method_i) { 
  this -> method_of__type[ffiproc_i] = method_i; 
  return ffiproc_i; 
}; 


int pscript_ffiproc__set_arg_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_name, const int byref_huh, const int arg_type) { 
  assert(this -> arg_nb[ffiproc_i] < pscript_ffiproc__arg__size); 
  const int arg_i = this -> arg_nb[ffiproc_i]; 
  
  this -> arg_name[ffiproc_i][arg_i] = arg_name; 
  this -> arg_byref[ffiproc_i][arg_i] = byref_huh; 
  this -> arg_type[ffiproc_i][arg_i] = arg_type; 
  
  this -> arg_nb[ffiproc_i]++; 
  //return ffiproc_i; 
  return arg_i;
}; 
 
 
 
 
int pscript_ffiproc__call(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value) { 
  void * callback_data = this -> ffiproc_callback_data[ffiproc_i]; 
  const int callback_value = this -> ffiproc_callback[ffiproc_i](callback_data, arg_nb, arg_type, arg_sizeof, arg_offset, arg_mem_size, arg_mem, retval_type, retval_allocated_size, retval_value); 
  return callback_value; 
}; 


#if 0 
static int pscript_ffiproc__call_OLD(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_allocated_size, char * retval_value) { 
  
  const int argc = arg_nb; 
  char argv_core_type[arg_nb]; 
  int bool_nb = 0; 
  int integer_nb = 0; 
  int float_nb = 0; 
  int char_nb = 0; 
  int string_nb = 0; 
  int pointer_nb = 0; 
  int proxy_nb = 0; 
  int array_nb = 0; 
  int record_nb = 0; 
  int udf_nb = 0; 
  int array_mem_size = 0; 
  int record_mem_size = 0; 
  int udf_mem_size = 0; 
  
  for (int i = 0; i < arg_nb; i++) {
    const char core_type = pscript_type__get_core_type(this -> type_env, arg_type[i]); 
    argv_core_type[i] = core_type; 

#if 0     
    pmess("i : %d | core_type = [ %d ] %s" "\n", i, core_type, pscript_type_core__cstr(core_type)); 
#endif 
    switch (core_type) {
    default: 
      pmess("i : %d | core_type = [ %d ] %s" "\n", i, core_type, pscript_type_core__cstr(core_type)); 
      assert(false); 
      break; 
    case PSCRIPT_TYPE__BOOL:         bool_nb++; break;  
    case PSCRIPT_TYPE__INTEGER:   integer_nb++; break;  
    case PSCRIPT_TYPE__INT_RANGE: integer_nb++; break;  
    case PSCRIPT_TYPE__FLOAT:       float_nb++; break;  
    case PSCRIPT_TYPE__CHAR:         char_nb++; break;  
    case PSCRIPT_TYPE__STRING:     string_nb++; break;  
    case PSCRIPT_TYPE__NIL:       pointer_nb++; break;  
    case PSCRIPT_TYPE__POINTER:   pointer_nb++; break;  
    case PSCRIPT_TYPE__PROXY:       proxy_nb++; break;  
    case PSCRIPT_TYPE__ARRAY: {
      array_nb++; 
      array_mem_size += arg_sizeof[i]; 
    }; break;  
    case PSCRIPT_TYPE__RECORD: {
      record_nb++; 
      record_mem_size += arg_sizeof[i]; 
    }; break;  
    case PSCRIPT_TYPE__USER: {
      udf_nb++; 
      udf_mem_size += arg_sizeof[i]; 
    }; break;  
    };
  }; 

  char bool_stack[bool_nb]; 
  int integer_stack[integer_nb]; 
  float float_stack[float_nb]; 
  char char_stack[char_nb]; 
  int string_stack[string_nb]; 
  int pointer_type[pointer_nb]; 
  int pointer_stack[pointer_nb]; 
  int proxy_type[proxy_nb]; 
  int proxy_stack[proxy_nb]; 
  int array_type[array_nb]; 
  int array_offset[array_nb]; 
  char array_mem[array_mem_size]; 
  int record_type[record_nb]; 
  int record_offset[record_nb]; 
  char record_mem[record_mem_size]; 
  int udf_type[udf_nb]; 
  int udf_offset[udf_nb]; 
  char udf_mem[udf_mem_size]; 
  
  int bool_i = 0; 
  int integer_i = 0; 
  int float_i = 0; 
  int char_i = 0; 
  int string_i = 0; 
  int pointer_i = 0; 
  int proxy_i = 0; 
  int array_i = 0; 
  int record_i = 0; 
  int udf_i = 0; 
  int array_cumulated_offset = 0; 
  int record_cumulated_offset = 0; 
  int udf_cumulated_offset = 0; 
  for (int i = 0; i < arg_nb; i++) {
    const char core_type = argv_core_type[i]; 

    //const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], 
    const char * arg_value = arg_mem + arg_offset[i]; 
    
    switch (core_type) {
    default: assert(false); break; 
    case PSCRIPT_TYPE__BOOL:         bool_stack[   bool_i] = pscript_value__get_bool   (arg_sizeof[i], arg_value);    bool_i++; break;  
    case PSCRIPT_TYPE__INTEGER:   integer_stack[integer_i] = pscript_value__get_integer(arg_sizeof[i], arg_value); integer_i++; break;  
    case PSCRIPT_TYPE__INT_RANGE: integer_stack[integer_i] = pscript_value__get_integer(arg_sizeof[i], arg_value); integer_i++; break;  
    case PSCRIPT_TYPE__FLOAT:       float_stack[  float_i] = pscript_value__get_float  (arg_sizeof[i], arg_value);   float_i++; break;  
    case PSCRIPT_TYPE__CHAR:         char_stack[   char_i] = pscript_value__get_char   (arg_sizeof[i], arg_value);    char_i++; break;  
    case PSCRIPT_TYPE__STRING:     string_stack[ string_i] = pscript_value__get_string (arg_sizeof[i], arg_value);  string_i++; break;  
    case PSCRIPT_TYPE__NIL:       pointer_stack[pointer_i] = pscript_value__get_pointer(arg_sizeof[i], arg_value); pointer_type[pointer_i] = arg_type[i]; pointer_i++; break;  
    case PSCRIPT_TYPE__POINTER:   pointer_stack[pointer_i] = pscript_value__get_pointer(arg_sizeof[i], arg_value); pointer_type[pointer_i] = arg_type[i]; pointer_i++; break;  
    case PSCRIPT_TYPE__PROXY:       proxy_stack[  proxy_i] = pscript_value__get_proxy  (arg_sizeof[i], arg_value);   proxy_type[  proxy_i] = arg_type[i];   proxy_i++; break;  
    case PSCRIPT_TYPE__ARRAY: {
      array_offset[array_i] = array_cumulated_offset; 
      array_cumulated_offset += arg_sizeof[i]; 
      pscript_value__get_value(arg_sizeof[i], arg_value, arg_sizeof[i], array_mem + array_offset[array_i]); 
      array_i++; 
    }; break;  
    case PSCRIPT_TYPE__RECORD: {
      record_type[record_i] = arg_type[i]; 
      record_offset[record_i] = record_cumulated_offset; 
      record_cumulated_offset += arg_sizeof[i]; 
      pscript_value__get_value(arg_sizeof[i], arg_value, arg_sizeof[i], record_mem + record_offset[record_i]); 
      record_i++; 
    }; break;  
    case PSCRIPT_TYPE__USER: {
      udf_type[udf_i] = arg_type[i]; 
      udf_offset[udf_i] = udf_cumulated_offset; 
      udf_cumulated_offset += arg_sizeof[i]; 
      pscript_value__get_value(arg_sizeof[i], arg_value, arg_sizeof[i], udf_mem + udf_offset[udf_i]); 
      udf_i++; 
    }; break;  
    };
  }; 
  
  
  void * callback_data = this -> ffiproc_callback_data[ffiproc_i]; 
  const int callback_value = this -> ffiproc_callback[ffiproc_i](callback_data, argc, argv_core_type, bool_nb, bool_stack, integer_nb, integer_stack, float_nb, float_stack, char_nb, char_stack, string_nb, string_stack, pointer_nb, pointer_type, pointer_stack, proxy_nb, proxy_type, proxy_stack, array_nb, array_type, array_offset, array_mem_size, array_mem, record_nb, record_type, record_offset, record_mem_size, record_mem, udf_nb, udf_type, udf_offset, udf_mem_size, udf_mem); 
  
  return callback_value; 
}; 
#endif 
 





















#if 0 



//int pscript_ffiproc__env__name__push_nolookup(pscript_ffiproc_env_t * this, const int name, const int type_i) {
int pscript_ffiproc__push_full(pscript_ffiproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int localvar_nb, const int localvar_name[], const int localvar_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size) {
  assert(this -> ffiproc_nb < pscript_ffiproc__size); 
  const int index = this -> ffiproc_nb; 
  
  this -> retval_type[index] = retval_type; 

  assert(arg_nb < pscript_ffiproc__arg__size); 
  this -> arg_nb[index] = arg_nb; 
  for (int i = 0; i < arg_nb; i++) { 
    this -> arg_name[index][i] = arg_name[i]; 
    this -> arg_byref[index][i] = arg_byref[i]; 
    this -> arg_type[index][i] = arg_type[i]; 
  }; 

  assert(localvar_nb < pscript_ffiproc__localvar__size); 
  this -> localvar_nb[index] = localvar_nb; 
  for (int i = 0; i < localvar_nb; i++) { 
    this -> localvar_name[index][i] = localvar_name[i]; 
    this -> localvar_type[index][i] = localvar_type[i]; 
  }; 

  this -> ffiproc_name[index] = name; 
  this -> ffiproc_lexical_env[index] = lexical_env; 
  this -> ffiproc_src_code[index] = src_code; 
  this -> ffiproc_bytecode_offset[index] = bytecode_offset; 
  this -> ffiproc_bytecode_size[index] = bytecode_size; 

  this -> ffiproc_nb ++; 
  return index; 
}; 





int pscript_ffiproc__push_empty(pscript_ffiproc_env_t * this) {
  assert(this -> ffiproc_nb < pscript_ffiproc__size); 
  const int index = this -> ffiproc_nb; 
  if (0 != this -> ffiproc_nb) { 
    if (-1 == this -> retval_type[this -> ffiproc_nb-1]) {
      return this -> ffiproc_nb - 1; 
    };
  };
  
  pmessage("Pushing a new ffiproc (empty) - ffiproc_i: %d" "\n", index); 
  
  this -> retval_type[index] = -1; 
  this -> arg_nb[index] = 0; 
  this -> localvar_nb[index] = 0; 
  this -> ffiproc_name[index] = -1; 
  this -> ffiproc_lexical_env[index] = -1; 
  this -> ffiproc_src_code[index] = -1; 
  this -> ffiproc_bytecode_offset[index] = -1; 
  this -> ffiproc_bytecode_size[index] = -1; 

  this -> ffiproc_nb ++; 
  return index; 
}; 









int pscript_ffiproc__top(pscript_ffiproc_env_t * this) { 
  assert(this -> ffiproc_nb > 0); 
  return this -> ffiproc_nb - 1; 
}; 
 
int pscript_ffiproc__set_name(pscript_ffiproc_env_t * this, const int ffiproc_i, const int name_pstr) { 
  this -> ffiproc_name[ffiproc_i] = name_pstr; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_lexical_env(pscript_ffiproc_env_t * this, const int ffiproc_i, const int lexical_env) { 
  this -> ffiproc_lexical_env[ffiproc_i] = lexical_env; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_retval_type(pscript_ffiproc_env_t * this, const int ffiproc_i, const int retval_type_i) { 
  this -> retval_type[ffiproc_i] = retval_type_i; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_src_code(pscript_ffiproc_env_t * this, const int ffiproc_i, const int src_code) { 
  this -> ffiproc_src_code[ffiproc_i] = src_code; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_bytecode_offset(pscript_ffiproc_env_t * this, const int ffiproc_i, const int bytecode_offset) { 
  this -> ffiproc_bytecode_offset[ffiproc_i] = bytecode_offset; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_bytecode_size(pscript_ffiproc_env_t * this, const int ffiproc_i, const int bytecode_size) { 
  this -> ffiproc_bytecode_size[ffiproc_i] = bytecode_size; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_arg_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_name, const int byref_huh, const int arg_type) { 
  assert(this -> arg_nb[ffiproc_i] < pscript_ffiproc__arg__size); 
  const int arg_i = this -> arg_nb[ffiproc_i]; 
  
  this -> arg_name[ffiproc_i][arg_i] = arg_name; 
  this -> arg_byref[ffiproc_i][arg_i] = byref_huh; 
  this -> arg_type[ffiproc_i][arg_i] = arg_type; 
  
  this -> arg_nb[ffiproc_i]++; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_localvar_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_name, const int localvar_type) { 
  assert(this -> localvar_nb[ffiproc_i] < pscript_ffiproc__localvar__size); 
  const int localvar_i = this -> localvar_nb[ffiproc_i]; 
  
  this -> localvar_name[ffiproc_i][localvar_i] = localvar_name; 
  this -> localvar_type[ffiproc_i][localvar_i] = localvar_type; 
  
  this -> localvar_nb[ffiproc_i]++; 
  return ffiproc_i; 
}; 
 
int pscript_ffiproc__set_localvar_type_nb(pscript_ffiproc_env_t * this, const int ffiproc_i, const int ident_nb, const int type_i) { 
  assert(this -> localvar_nb[ffiproc_i] > ident_nb); 
  for (int i = 0; i < ident_nb; i++) { 
    const int localvar_i = this -> localvar_nb[ffiproc_i] - ident_nb + i; 
    this -> localvar_type[ffiproc_i][i] = type_i; 
  }; 
  return ffiproc_i; 
}; 
 




int pscript_ffiproc__get_retval_type(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> retval_type[ffiproc_i]; 
}; 
 
int pscript_ffiproc__get_lexical_env(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> ffiproc_lexical_env[ffiproc_i]; 
}; 
 


int pscript_ffiproc__get_arg_all_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  int arg_all_sizeof = 0; 
  for (int i = 0; i < this -> arg_nb[ffiproc_i]; i++) { 
    arg_all_sizeof += pscript_type__sizeof(this -> type_env, this -> arg_type[ffiproc_i][i]); 
  }; 
  return arg_all_sizeof; 
}; 
 
int pscript_ffiproc__get_localvar_all_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  int localvar_all_sizeof = 0; 
  for (int i = 0; i < this -> localvar_nb[ffiproc_i]; i++) { 
    localvar_all_sizeof += pscript_type__sizeof(this -> type_env, this -> localvar_type[ffiproc_i][i]); 
  }; 
  return localvar_all_sizeof; 
}; 

int pscript_ffiproc__get_bytecode_offset(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> ffiproc_bytecode_offset[ffiproc_i]; 
}; 
 
int pscript_ffiproc__get_arg_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> arg_nb[ffiproc_i];
}; 
 
int pscript_ffiproc__get_arg_name(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i) { 
  return this -> arg_name[ffiproc_i][arg_i];
}; 
 
int pscript_ffiproc__get_arg_byref(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i) { 
  return this -> arg_byref[ffiproc_i][arg_i];
}; 
 
int pscript_ffiproc__get_arg_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i) { 
  return pscript_type__sizeof(this -> type_env, this -> arg_type[ffiproc_i][arg_i]);
}; 
 
int pscript_ffiproc__get_localvar_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i) { 
  return this -> localvar_nb[ffiproc_i];
}; 
 
int pscript_ffiproc__get_localvar_name(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_i) { 
  return this -> localvar_name[ffiproc_i][localvar_i];
}; 
 
int pscript_ffiproc__get_localvar_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_i) { 
  return pscript_type__sizeof(this -> type_env, this -> localvar_type[ffiproc_i][localvar_i]);
}; 
 
#endif 








#if 1 

#if 0 
  int ffiproc_name[pscript_ffiproc__size]; 
  int ffiproc_lexical_env[pscript_ffiproc__size]; 
  int ffiproc_src_code[pscript_ffiproc__size]; 
  int ffiproc_bytecode_offset[pscript_ffiproc__size]; 
  int ffiproc_bytecode_size[pscript_ffiproc__size]; 
  int ffiproc_nb; 
#endif 

static void ffiproc_stack__print_one_ffiproc(const int filedes, const pscript_ffiproc_env_t * this, const pscript_string_env_t * string_env, const int ffiproc_i) {
  //const int filedes = fileno(yypascalout); 
  
  const char * ffiproc_i__str            = int_string__stack(ffiproc_i); 
  const int    ffiproc_i__str__spaces_nb = MAX(0, 4 - strlen(ffiproc_i__str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, ffiproc_i__str__spaces_nb); 
  dputs(filedes, ffiproc_i__str); 
  dputs(filedes, ": "); 
  
  const int    ffiproc_name_pstr            = this -> ffiproc_name[ffiproc_i]; 
  const char * ffiproc_name_cstr            = pscript_string__get_cstr(string_env, ffiproc_name_pstr); 
  const int    ffiproc_name_cstr__nb_spaces = MAX(0, 17 - strlen(ffiproc_name_cstr)); 
  dputs(filedes, ffiproc_name_cstr); 
  dput_spaces(filedes, ffiproc_name_cstr__nb_spaces); 
  dputs(filedes, ": "); 

#if 0 
  const char * ffiproc_lenv__str = int_string__stack(this -> ffiproc_lexical_env[ffiproc_i]); 
  const int ffiproc_lenv__str__spaces_nb = MAX(0, 4 - strlen(ffiproc_lenv__str)); 
  dputs(filedes, "lexical env: "); 
  dput_spaces(filedes, ffiproc_lenv__str__spaces_nb); 
  dputs(filedes, ffiproc_lenv__str); 
  dputs(filedes, ": "); 
  
  const char * ffiproc_boffset__str = int_string__stack(this -> ffiproc_bytecode_offset[ffiproc_i]); 
  const int ffiproc_boffset__str__spaces_nb = MAX(0, 4 - strlen(ffiproc_boffset__str)); 
  dputs(filedes, "Bytecode offset: "); 
  dput_spaces(filedes, ffiproc_boffset__str__spaces_nb); 
  dputs(filedes, ffiproc_boffset__str); 
  dputs(filedes, ": "); 
  
  const char * ffiproc_bsize__str = int_string__stack(this -> ffiproc_bytecode_size[ffiproc_i]); 
  const int ffiproc_bsize__str__spaces_nb = MAX(0, 4 - strlen(ffiproc_bsize__str)); 
  dputs(filedes, "Bytecode size: "); 
  dput_spaces(filedes, ffiproc_bsize__str__spaces_nb); 
  dputs(filedes, ffiproc_bsize__str); 
  dputs(filedes, ": "); 
#endif 
  

#if 0 
  const int srcval_len = pscript_string__get_len(this -> string_env, ffiproc_stack__get_srcval(this, ffiproc_i)); 
  const char * ffiproc_len_str = int_string__stack(srcval_len); 
  const int ffiproc_len_str_len = strlen(ffiproc_len_str); 
  const int ffiproc_len_str_len_spaces_nb = MAX(0, 6 - ffiproc_len_str_len); 
  dputs(filedes, ffiproc_type_str); 
  dput_spaces(filedes, nb_spaces); 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, ffiproc_len_str_len_spaces_nb); 
  dputs(filedes, ffiproc_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, ffiproc_stack__get_srcval_cstr(this, ffiproc_i)); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	ffiproc_stack__get_srcval_cstr(this, ffiproc_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
#endif 
}; 

static void ffiproc_stack__print_stats__base(const int filedes, const pscript_ffiproc_env_t * this, const int base) {
  //const int filedes = fileno(yypascalout); 
  dputs(filedes, "FFIPROC_STACK("); 
  dputn(filedes, this -> ffiproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> ffiproc_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void ffiproc_stack__print_stats(const int filedes, const pscript_ffiproc_env_t * this) {
  ffiproc_stack__print_stats__base(filedes, this, pscript_ffiproc__size); 
}; 

void ffiproc_stack__print_nb(const int filedes, const pscript_ffiproc_env_t * this, const pscript_string_env_t * string_env, const int ffiproc_nb) {
  //const int filedes = fileno(yypascalout); 
  dputs(filedes, "FFIPROC_STACK("); 
  dputn(filedes, this -> ffiproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_ffiproc__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> ffiproc_nb)) / ((double) pscript_ffiproc__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> ffiproc_nb,ffiproc_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    ffiproc_stack__print_one_ffiproc(filedes, this, string_env, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void ffiproc_stack__print_all(const int filedes, const pscript_ffiproc_env_t * this, const pscript_string_env_t * string_env) {
  ffiproc_stack__print_nb(filedes, this, string_env, this -> ffiproc_nb); 
}; 
#endif 
 
 

