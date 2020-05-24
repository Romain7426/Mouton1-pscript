#include "pscript_global.h" 
#include "pscript.h" 
#include "pscript_bytecode.h" 
#include "pscript_type.h" 
#include "pvm_bytecode__type.ci" 




pscript_bytecode_env_t * pscript_bytecode_env__make(void) {
  MALLOC_BZERO(pscript_bytecode_env_t,this); 
  return this;
}; 

void pscript_bytecode_env__delete(pscript_bytecode_env_t * this) {
  free(this);
};   



int pscript_bytecode__get_count(const pscript_bytecode_env_t * this) {
  return this -> bytecode_nb; 
}; 



int pscript_bytecode__push_value(pscript_bytecode_env_t * this, const int value_sizeof, const char value[]) { 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = value[i]; 
  }; 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 

int pscript_bytecode__push_value0(pscript_bytecode_env_t * this, const int value_sizeof) { 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  bzero(this -> bytecode_array + offset, value_sizeof); 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 

int pscript_bytecode__push_int16(pscript_bytecode_env_t * this, const int16_t intvalue) { 
  //return pscript_bytecode__push_value(this, sizeof(intvalue), &intvalue);
  return pscript_bytecode__push_value(this, sizeof(intvalue), (const char *)&intvalue);
}; 
 
int pscript_bytecode__push_char(pscript_bytecode_env_t * this, const char intvalue) { 
  return pscript_bytecode__push_value(this, sizeof(intvalue), (const char *)&intvalue); 
}; 
 
int pscript_bytecode__push_nil_list(pscript_bytecode_env_t * this, const int intvalue) { 
  //return pscript_bytecode__push_value(this, sizeof(intvalue), (const char *)&intvalue);
  assert(false); 
  return pscript_bytecode__push_value(this, 1, (const char *)&intvalue);
}; 
 
int pscript_bytecode__push_nil_ptr(pscript_bytecode_env_t * this, const int intvalue) { 
  //return pscript_bytecode__push_value(this, sizeof(intvalue), (const char *)&intvalue);
  assert(false); 
  return pscript_bytecode__push_value(this, 4, (const char *)&intvalue);
}; 
 
int pscript_bytecode__push_mem_offset(pscript_bytecode_env_t * this, const int mem_offset) { 
  return pscript_bytecode__push_value(this, sizeof(int_pscript_mem_t), (const char *)&mem_offset);
}; 

int pscript_bytecode__push_bytecode_offset(pscript_bytecode_env_t * this, const int bytecode_offset) { 
  return pscript_bytecode__push_value(this, sizeof(int), (const char *)&bytecode_offset);
}; 




int pscript_bytecode__push_instruction(pscript_bytecode_env_t * this, const uint8_t bytecode) { 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 

  assert(this -> bytecode_nb < pscript_bytecode__size); 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 
  const int offset = this -> bytecode_nb; 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 
  this -> bytecode_array[offset] = bytecode; 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 
  this -> bytecode_nb++; 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 
  return offset; 
  //dputs_array(stderr__fileno, __func__, STRINGIFY(__LINE__), "\n"); 
}; 
 
int pscript_bytecode__push_bool(pscript_bytecode_env_t * this, const int boolval) { 
  assert(this -> bytecode_nb < pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  this -> bytecode_array[offset] = boolval; 
  this -> bytecode_nb++; 
  return offset; 
}; 
 
int pscript_bytecode__push_int(pscript_bytecode_env_t * this, const int intval) { 
  const int value_sizeof = sizeof(intval); 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  int p = intval; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = p & 0xFF; 
    p >>= 8; 
  }; 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 
 
int pscript_bytecode__push_int8(pscript_bytecode_env_t * this, const int8_t intval) { 
  const int value_sizeof = sizeof(intval); 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  int p = intval; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = p & 0xFF; 
    p >>= 8; 
  }; 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 
 
int pscript_bytecode__push_float(pscript_bytecode_env_t * this, const float floatval) { 
  const int value_sizeof = sizeof(floatval); 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  const uint8_t * p = (const uint8_t *) &floatval; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = *p; 
    p++; 
  }; 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 
 
int pscript_bytecode__push_string(pscript_bytecode_env_t * this, const int pstr) { 
  const int value_sizeof = sizeof(int_pscript_string_t);//sizeof(pstr); 
  assert(this -> bytecode_nb + value_sizeof <= pscript_bytecode__size); 
  const int offset = this -> bytecode_nb; 
  int p = pstr; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = p & 0xFF; 
    p >>= 8; 
  }; 
  this -> bytecode_nb += value_sizeof; 
  return offset; 
}; 
 
int pscript_bytecode__push_const(pscript_bytecode_env_t * this, const pscript_t * pscript_env, const int const_i) { 
  const int const_type = pscript_const__get_type(pscript_env -> const_env, const_i); 
  const int type_core = pscript_type__get_core_type(pscript_env -> type_env, const_type); 
  int offset = -1; 
  switch (type_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__BOOL:      offset = pscript_bytecode__push_bool    (this, pscript_const__get_value_bool (pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__INTEGER:   offset = pscript_bytecode__push_int     (this, pscript_const__get_value_int  (pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__INT_RANGE: offset = pscript_bytecode__push_int     (this, pscript_const__get_value_int  (pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__FLOAT:     offset = pscript_bytecode__push_float   (this, pscript_const__get_value_float(pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__CHAR:      offset = pscript_bytecode__push_char    (this, pscript_const__get_value_char (pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__STRING:    offset = pscript_bytecode__push_string  (this, pscript_const__get_value_str  (pscript_env -> const_env, const_i)); break; 
  case PSCRIPT_TYPE__NIL_LIST:  offset = pscript_bytecode__push_value0(this, 0); break; 
  case PSCRIPT_TYPE__NIL_PTR:   offset = pscript_bytecode__push_value0(this, sizeof(int_pscript_mem_t)); break; 
  }; 
  return offset; 
}; 
 
int pscript_bytecode__push_pointer_nil(pscript_bytecode_env_t * this) { 
  //return pscript_bytecode__push_int(this, 0); 
  return pscript_bytecode__push_value0(this, sizeof(int_pscript_mem_t)); 
}; 
 


int pscript_bytecode__get_next_offset(const pscript_bytecode_env_t * this) { 
  return this -> bytecode_nb; 
}; 
 

int pscript_bytecode__rewrite_address(pscript_bytecode_env_t * this, const int where_offset, const int what_offset) { 
  assert(where_offset >= 0); 
  const int value_sizeof = sizeof(what_offset); 
  assert(where_offset + value_sizeof <= this -> bytecode_nb); 
  const int offset = where_offset; 
  int p = what_offset; 
  for (int i = 0; i < value_sizeof; i++) { 
    this -> bytecode_array[offset+i] = p & 0xFF; 
    p >>= 8; 
  }; 
  return offset; 
}; 
 
int pscript_bytecode__rewrite_value(pscript_bytecode_env_t * this, const int bytecode_offset, const int value_sizeof, const char value[]) {
  assert(bytecode_offset >= 0); 
  assert(bytecode_offset + value_sizeof <= this -> bytecode_nb); 
  char * b = this -> bytecode_array + bytecode_offset; 
  const char * p = value; 
  for (int i = 0; i < value_sizeof; i++) { 
    *b = *p; 
    b++; 
    p++; 
  }; 
  return bytecode_offset; 
}; 

int pscript_bytecode__rewrite_int8(pscript_bytecode_env_t * this, const int bytecode_offset, const int8_t value) {
  return pscript_bytecode__rewrite_value(this, bytecode_offset, sizeof(value), &value); 
}; 

int pscript_bytecode__rewrite_int(pscript_bytecode_env_t * this, const int bytecode_offset, const int value) {
  return pscript_bytecode__rewrite_value(this, bytecode_offset, sizeof(value), (const char *) &value); 
};

 






#if 0 





static void     bytecode_stack__print_nb(stderr, 127); 
bytecode_type__print_all_as_array(const pscript_bytecode_env_t * this, const int out_stream) {
  for (int i = 0; i < pvm_bytecode__size; i++) { 
    const int bytecode_value = pvm_bytecode__value[i];
    const char * bytecode_cstr = bytecode_type__get_cstr(i);
    const size_t len = strlen(bytecode_cstr); 
    const int align_len = 33; 
    const int base10_len = (bytecode_value < 10 ? 1 : (bytecode_value < 100 ? 2 : 3)); 
    const int nbspaces = max(0, align_len - len) + (3 - base10_len);  //(2 - ((int)(bytecode_value / 10)));   //(i < 10 ? 2 : (i < 100 ? 1 : 0)); 
    const char * ib2 = int_to_str_base2__stack(bytecode_value); 
    const int ib2_len = strlen(ib2); 
    const char * ib16 = int_to_str_base16__stack(bytecode_value); 
    const int ib16_len = strlen(ib16); 
    dputs(out_stream, bytecode_cstr); 
    dput_spaces(out_stream, nbspaces);
    dputn(out_stream, bytecode_value);
    dput_spaces(out_stream, 9 - ib2_len);
    dputs(out_stream, ib2);
    dput_spaces(out_stream, 9 - ib16_len);
    dputs(out_stream, ib16);
    dputc(out_stream, '\n');
  };   
}; 

#endif











int bytecode_stack__print_one_bytecode__get_arg_len(const pscript_bytecode_env_t * this, pscript_type_env_t * type_env, const int bytecode_offset, const int bytecode_value) {
  int arg_len = 0; 
  switch (bytecode_value) {
  case PVM_BYTECODE__VMCALL: arg_len = 4; break;  
  case PVM_BYTECODE__SYSCALL: arg_len = 4; break;  
  case PVM_BYTECODE__PUSH: {
    //const int8_t value_sizeof = this -> bytecode_array[bytecode_offset + 1]; 
    const int8_t type_i = this -> bytecode_array[bytecode_offset + 1]; 
    const int8_t value_sizeof = pscript_type__sizeof(type_env, type_i); 
    //dputs_array(stderr__fileno, " TYPE: ", pscript_type__get_cstr(type_env, type_i), "\n"); 
    //dputs_array(stderr__fileno, " SIZEOF: ", int_string__stack(value_sizeof), "\n"); 
    arg_len = 1 + value_sizeof; 
    }; break; 
  case PVM_BYTECODE__JTRUE: arg_len = 4; break;  
  case PVM_BYTECODE__JFALSE: arg_len = 4; break;  
  case PVM_BYTECODE__JUMP: arg_len = 4; break;  
  case PVM_BYTECODE__CALL: arg_len = 5; break;  // @ is in the bytecode. 
  case PVM_BYTECODE__CALL_STACK_OBJECT: arg_len = 1; break;  // @ is in the stack. 
  case PVM_BYTECODE__CALL_FFI: arg_len = 3; break;  // @ is in the bytecode. 
  case PVM_BYTECODE__CALL_LIB: arg_len = 3; break;  // @ is in the bytecode. 
  case PVM_BYTECODE__CALL_METHOD: arg_len = 3; break;  // @ is in the bytecode. 
  //case PVM_BYTECODE__CALL_CLIENT: arg_len = 4; break;  
  //case PVM_BYTECODE__CALL_CLIENT_METHOD: arg_len = 4; break;  
  case PVM_BYTECODE__CHECK_ARG_NB: arg_len = 1; break;  // ARG_NB is in the bytecode 
  //case PVM_BYTECODE__ENSURE_TYPE: arg_len = 4; break;  // ARG_NB is in the bytecode 
  case PVM_BYTECODE__LEA_LOCAL: arg_len = 1 + sizeof(int_pscript_mem_t); break;  // @ is in the bytecode 
  //case PVM_BYTECODE__ARRAY_GET_ELT0_ADDR: arg_len = 4; break;  
  case PVM_BYTECODE__MEM_ALLOCATE: arg_len = 4; break;  // size is in the bytecode 
  case PVM_BYTECODE__MEM_DEALLOCATE: arg_len = 4; break;  // size is in the bytecode 
  case PVM_BYTECODE__MEM_LOAD: arg_len = 1 + sizeof(int_pscript_mem_t); break;  
  case PVM_BYTECODE__MEM_STORE: arg_len = 1 + sizeof(int_pscript_mem_t); break;  // @ is in the bytecode: arg_len = 4; break;  [0] is value (the size must be taken into account) 
  case PVM_BYTECODE__MEM_LOAD_LOCAL: arg_len = 1 + sizeof(int_pscript_mem_t); break;  // @ is in the bytecode 
  case PVM_BYTECODE__MEM_STORE_LOCAL: arg_len = 1 + sizeof(int_pscript_mem_t); break;  // type then @ in the bytecode 
  case PVM_BYTECODE__RECORD_GET_FIELD_VALUE_BY_NAME: arg_len = 2; break;  // type then @ in the bytecode 
  case PVM_BYTECODE__RECORD_GET_FIELD_ADDRESS_BY_NAME: arg_len = 2; break;  // type then @ in the bytecode 
  case PVM_BYTECODE__MEMORY_FRAME__ALLOC_AND_INC: arg_len = sizeof(int); break;  // type then @ in the bytecode 
  case PVM_BYTECODE__MEMORY_FRAME__DEC_AND_CONDITIONALLY_DEALLOC: arg_len = sizeof(int8_t); break;  // type then @ in the bytecode 
  case PVM_BYTECODE__MAKE_LAMBDA: arg_len = 2*sizeof(int8_t) + 2*sizeof(int); break;  // type then @ in the bytecode 
  }; 
  return arg_len;
}; 

int bytecode_stack__print_one_bytecode(const int fd, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env, const int bytecode_offset) {
  //const int filedes = fileno(yypascalout); 
  const int filedes = fd; //stderr; 
  const char * bytecode_offset__str = int_string__stack(bytecode_offset); 
  const int bytecode_offset__str__spaces_nb = MAX(0, 4 - strlen(bytecode_offset__str)); 
  const int bytecode_value = this -> bytecode_array[bytecode_offset]; 
  const char * bytecode_value_str = int_string__stack(bytecode_value); 
  const int bytecode_value__nb_spaces = MAX(0, 4 - strlen(bytecode_value_str)); 
  const int bytecode_type = this -> bytecode_array[bytecode_offset]; 
  const char * bytecode_type_str = pvm_bytecode__cstr(bytecode_type); 
  const int bytecode_type__nb_spaces = MAX(0, 24 - strlen(bytecode_type_str)); 
  //const int srcval_len = pscript_string__get_len(this -> string_env, bytecode_stack__get_srcval(this, bytecode_i)); 
  //const char * bytecode_len_str = int_string__stack(srcval_len); 
  //const int bytecode_len_str_len = strlen(bytecode_len_str); 
  //const int bytecode_len_str_len_spaces_nb = MAX(0, 6 - bytecode_len_str_len); 
  dputs(filedes, "@ "); 
  dput_spaces(filedes, bytecode_offset__str__spaces_nb); 
  dputs(filedes, bytecode_offset__str); 
  dputs(filedes, ": ["); 
  dput_spaces(filedes, bytecode_value__nb_spaces); 
  dputs(filedes, bytecode_value_str); 
  dputs(filedes, "] "); 
  dputs(filedes, bytecode_type_str); 
  dput_spaces(filedes, bytecode_type__nb_spaces); 
#if 0 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, bytecode_len_str_len_spaces_nb); 
  dputs(filedes, bytecode_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, bytecode_stack__get_srcval_cstr(this, bytecode_i)); 
#endif 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	bytecode_stack__get_srcval_cstr(this, bytecode_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 

  const int arg_nb = bytecode_stack__print_one_bytecode__get_arg_len(this, type_env, bytecode_offset, bytecode_value); 
  if (0 == arg_nb) return 1; 
  
  dputs(filedes, " - ARG: "); 
  for (int i = 0; i < arg_nb; i++) {
    dputn(filedes, this -> bytecode_array[bytecode_offset+i+1]);
    dputs(filedes, " "); 
  };
  
  return arg_nb+1; 
}; 

static void bytecode_stack__print_stats__base(const int filedes, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env, const int base) {
  //const int filedes = fileno(yypascalout); 
  //const int filedes = stderr; 
  dputs(filedes, "BYTECODE_STACK("); 
  dputn(filedes, this -> bytecode_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> bytecode_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

void bytecode_stack__print_stats(const int filedes, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env) {
  bytecode_stack__print_stats__base(filedes, this, type_env, pscript_bytecode__size); 
}; 

void bytecode_stack__print_nb(const int fd, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env, const int bytecode_nb) {
  //const int filedes = fileno(yypascalout); 
  const int filedes = fd; //stderr; 
  dputs(filedes, "BYTECODE_STACK("); 
  dputn(filedes, this -> bytecode_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_bytecode__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> bytecode_nb)) / ((double) pscript_bytecode__size))); 
  dputs(filedes, "% ) = { \n"); 
  //const int nb = MIN(this -> bytecode_nb,bytecode_nb); 
  const int nb = bytecode_nb; 
  int offset = 0; 
  for (int i = 0; i < nb; i++) {
    if (offset >= this -> bytecode_nb) break; 
    dputs(filedes, "  "); 
    offset += bytecode_stack__print_one_bytecode(fd, this, type_env, offset);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void bytecode_stack__print_all(const int fd, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env) {
  bytecode_stack__print_nb(fd, this, type_env, this -> bytecode_nb); 
}; 
