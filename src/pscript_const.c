#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_const.h" 
#include "pscript_value.h" 



#if 1
pscript_const_env_t * pscript_const_env__make(pscript_t * pscript_env) {
  MALLOC_BZERO(pscript_const_env_t,this); 
  this -> pscript_env = pscript_env; 
  return this;
}; 
#else 
pscript_const_env_t * pscript_const_env__make(pscript_type_env_t * type_env) {
  MALLOC_BZERO(pscript_const_env_t,this); 
  this -> type_env = type_env; 
  return this;
}; 
#endif 

void pscript_const_env__delete(pscript_const_env_t * this) {
  free(this);
};   



int pscript_const__get_count(const pscript_const_env_t * this) { 
  return this -> env__nb; 
}; 



static int pscript_const__mem__alloc(pscript_const_env_t * this, const int type) {
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type); 
  assert(this -> mem_nb + type_sizeof <= pscript_const__mem__size); 
  const int mem_offset = this -> mem_nb; 
  //this -> mem_type[mem_offset] = type; 
  this -> mem_nb += type_sizeof; 
  return mem_offset; 
}; 



int pscript_const__env__name__push_nolookup(pscript_const_env_t * this, const int name, const int type_i) {
  assert(this -> env__nb < pscript_const__env__size); 
  const int index = this -> env__nb; 
  this -> env__name[index] = name; 
  this -> env__type[index] = type_i; 
  this -> env__mem_offset[index] = pscript_const__mem__alloc(this, type_i); 
  this -> env__nb ++; 
  return index; 
}; 

int pscript_const__env__name__lookup(const pscript_const_env_t * this, const int name) { 
  for (int i = 0; i < this -> env__nb; i++) {
    if (this -> env__name[i] == name) return i; 
  }; 
  return -1; 
}; 




static int pscript_const__stack__top_offset(pscript_const_env_t * this) {
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return stack_offset; 
}; 

void pscript_const__stack__drop(pscript_const_env_t * this) {
  assert(this -> stack__nb > 0); 
  const int index = this -> stack__nb - 1; 
  const int type_i = this -> stack__type[index]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  this -> stack__mem__nb -= type_sizeof; 
  this -> stack__nb --; 
}; 

static int pscript_const__stack__alloc(pscript_const_env_t * this, const int type) {
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type); 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  this -> stack__mem__nb += type_sizeof; 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  
  assert(this -> stack__nb < pscript_const__stack__size); 
  const int type_idx = this -> stack__nb; 
  this -> stack__type[type_idx] = type; 
  this -> stack__nb++; 
  
  return stack_offset; 
}; 





int pscript_const__stack__push_value(pscript_const_env_t * this, const int type1, const int value1_sizeof, const char value1[]) {
  const int type1_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1); 
  assert(type1_sizeof <= value1_sizeof); 
  const int stack_offset = pscript_const__stack__alloc(this, type1); 
  bcopy(value1, this -> stack__mem + stack_offset, type1_sizeof); 
  return stack_offset; 
}; 

int pscript_const__stack__push_bool(pscript_const_env_t * this, const int bool_value) {
  const int type_i = pscript_type__get__bool(this -> pscript_env -> type_env); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int stack_offset = pscript_const__stack__alloc(this, type_i); 
  this -> stack__mem[stack_offset] = bool_value; 
  return stack_offset; 
}; 

int pscript_const__stack__push_int(pscript_const_env_t * this, const int int_value) {
  const int type_i = pscript_type__get__integer(this -> pscript_env -> type_env); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int stack_offset = pscript_const__stack__alloc(this, type_i); 
  *(int *)(this -> stack__mem + stack_offset) = int_value; 
  return stack_offset; 
}; 

int pscript_const__stack__push_float(pscript_const_env_t * this, const float float_value) {
  const int type_i = pscript_type__get__float(this -> pscript_env -> type_env); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int stack_offset = pscript_const__stack__alloc(this, type_i); 
  *(float *)(this -> stack__mem + stack_offset) = float_value; 
  return stack_offset; 
}; 

int pscript_const__stack__push_string(pscript_const_env_t * this, const int pstr) {
  const int type_i = pscript_type__get__string(this -> pscript_env -> type_env); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int stack_offset = pscript_const__stack__alloc(this, type_i); 
  //*(int *)(this -> stack__mem + stack_offset) = pstr; 
  bcopy(&pstr, this -> stack__mem + stack_offset, type_sizeof); 
  return stack_offset; 
}; 

int pscript_const__stack__push_nil(pscript_const_env_t * this) {
  const int type_i       = pscript_type__get__nil_list(this -> pscript_env -> type_env); 
  const int type_sizeof  = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int stack_offset = pscript_const__stack__alloc(this, type_i); 
  //*(int *)(this -> stack__mem + stack_offset) = 0; 
  //bcopy(value1, this -> stack__mem + stack_offset, type_sizeof); 
  bzero(this -> stack__mem + stack_offset, type_sizeof); 
  return stack_offset; 
}; 



int pscript_const__stack__pop(pscript_const_env_t * this) {
  assert(this -> stack__nb > 0); 
  const int type_idx = this -> stack__nb - 1; 
  const int type_i = this -> stack__type[type_idx]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  this -> stack__mem__nb -= type_sizeof; 
  this -> stack__nb--; 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return stack_offset; 
}; 

int pscript_const__stack__top_get_type(const pscript_const_env_t * this) {
  assert(this -> stack__nb > 0); 
  const int type_idx = this -> stack__nb - 1; 
  const int type_i = this -> stack__type[type_idx]; 
  return type_i; 
}; 

static int8_t pscript_const__stack__top_get_value__int8(const pscript_const_env_t * this) {
  const int type_sizeof = 1; 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return *(const int8_t *)(this -> stack__mem + stack_offset); 
}; 

static int16_t pscript_const__stack__top_get_value__int16(const pscript_const_env_t * this) {
  const int type_sizeof = 2; 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return *(const int16_t *)(this -> stack__mem + stack_offset); 
}; 

static int32_t pscript_const__stack__top_get_value__int32(const pscript_const_env_t * this) {
  const int type_sizeof = 4; 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return *(const int32_t *)(this -> stack__mem + stack_offset); 
}; 

static int64_t pscript_const__stack__top_get_value__int64(const pscript_const_env_t * this) {
  const int type_sizeof = 8; 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  return *(const int64_t *)(this -> stack__mem + stack_offset); 
}; 

int pscript_const__stack__top_get_value(const pscript_const_env_t * this) {
  assert(this -> stack__nb > 0); 
  const int type_idx = this -> stack__nb - 1; 
  const int type_i = this -> stack__type[type_idx]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  assert((size_t)type_sizeof <= sizeof(int)); 
  //const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  int intvalue; 
  switch (type_sizeof) {
  case 1: intvalue = pscript_const__stack__top_get_value__int8 (this); break; 
  case 2: intvalue = pscript_const__stack__top_get_value__int16(this); break; 
  case 4: intvalue = pscript_const__stack__top_get_value__int32(this); break; 
  case 8: intvalue = pscript_const__stack__top_get_value__int64(this); break; 
  default: assert(false); intvalue = 0; break; 
  }; 
  return intvalue; 
}; 

int pscript_const__stack__top_get_value2(const pscript_const_env_t * this, const int value_allocated_sizeof, char * value) {
  assert(this -> stack__nb > 0); 
  const int type_idx    = this -> stack__nb - 1; 
  const int type_i      = this -> stack__type[type_idx]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  assert(type_sizeof <= value_allocated_sizeof); 
  assert(this -> stack__mem__nb + type_sizeof <= pscript_const__stack__mem__size); 
  const int stack_offset = pscript_const__stack__mem__size - this -> stack__mem__nb; 
  bcopy(this -> stack__mem + stack_offset, value, type_sizeof); 
  return type_sizeof; 
}; 




int pscript_const__env__name__push_from_stack__nolookup(pscript_const_env_t * this, const int name) { 
  //dputs_array(stderr, __func__, ": ", "name = ", int_string__stack(name), "\n"); 
  //pmessage("pscript_const__env__name__push_from_stack__nolookup: name = %s" "\n", pscript_string__get_cstr(this -> string_env, name)); 
  //pmessage("pscript_const__env__name__push_from_stack__nolookup: name = %d" "\n", name); 
  //fprintf(stderr_FILE, "%s: name = %d" "\n", __func__, name); 
  //fprintf(stderr_FILE, "PASCAL: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": " "%s: name = %d" "\n", __func__, name); 
  //if (yypascalout != NULL) fprintf(yypascalout, "PASCAL: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": " "%s: name = %d" "\n", __func__, name); 
  pmessage("%s: name = %d" "\n", __func__, name); 
  assert(this -> stack__nb > 0); 
  const int type_idx = this -> stack__nb - 1; 
  const int type_i = this -> stack__type[type_idx]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  const int const_i = pscript_const__env__name__push_nolookup(this, name, type_i); 
  const int mem_offset = this -> env__mem_offset[const_i]; 
  const int stack_offset = pscript_const__stack__top_offset(this); 

  bcopy(this -> stack__mem + stack_offset, this -> mem + mem_offset, type_sizeof); 
  
  pmessage("%s: " "type = [ %d ] " "\n", __func__, type_i); 
  //pmessage("pscript_const__env__name__push_from_stack__nolookup: type = [ %d ] ( %d ) " "\n", type_i, pscript_type__string(this -> pscript_env -> type_env, type_i)); 

#if 0 
  pscript_value__print__raw(type_sizeof, this -> stack__mem + stack_offset); dputs(stderr, "\n"); 
  pscript_value__print__raw(type_sizeof, this ->        mem +   mem_offset); dputs(stderr, "\n"); 
#endif 
  return const_i; 
}; 


int pscript_const__stack__push_const_value(pscript_const_env_t * this, const int const_i) {
  const int mem_offset = this -> env__mem_offset[const_i]; 
  //const int type_i = this -> mem_type[mem_offset]; 
  const int type_i = this -> env__type[const_i]; 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
  return pscript_const__stack__push_value(this, type_i, type_sizeof, this -> mem + mem_offset); 
}; 






static int pscript_const__get_mem_offset(const pscript_const_env_t * this, const int const_i) { 
  assert(const_i >= 0); 
  assert(const_i < this -> env__nb); 
  const int mem_offset = this -> env__mem_offset[const_i]; 
  return mem_offset; 
}; 
 
int pscript_const__get_type(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  return const_type; 
}; 
 
int pscript_const__get_value_bool(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  const int core_type = pscript_type__get_core_type(this -> pscript_env -> type_env, const_type); 
  assert(PSCRIPT_TYPE__BOOL == core_type); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, const_type); 
  return this -> mem[mem_offset]; 
}; 
 
int pscript_const__get_value_int(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  const int core_type = pscript_type__get_core_type(this -> pscript_env -> type_env, const_type); 
  assert(PSCRIPT_TYPE__INTEGER == core_type); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, const_type); 
  const int intvalue = *(const int *)(this -> mem + mem_offset);
  return intvalue; 
}; 
 
float pscript_const__get_value_float(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  const int core_type = pscript_type__get_core_type(this -> pscript_env -> type_env, const_type); 
  assert(PSCRIPT_TYPE__FLOAT == core_type); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, const_type); 
  const float floatvalue = *(const float *)(this -> mem + mem_offset);
  return floatvalue; 
}; 
 
char pscript_const__get_value_char(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  const int core_type = pscript_type__get_core_type(this -> pscript_env -> type_env, const_type); 
  assert(PSCRIPT_TYPE__CHAR == core_type); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, const_type); 
  const char charvalue = *(const char *)(this -> mem + mem_offset);
  return charvalue; 
}; 
 
int pscript_const__get_value_str(const pscript_const_env_t * this, const int const_i) { 
  const int mem_offset = pscript_const__get_mem_offset(this, const_i); 
  //const int const_type = this -> mem_type[mem_offset]; 
  const int const_type = this -> env__type[const_i]; 
  const int core_type = pscript_type__get_core_type(this -> pscript_env -> type_env, const_type); 
  assert(PSCRIPT_TYPE__STRING == core_type); 
  const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, const_type); 
  //const int intvalue = *(const int *)(this -> mem + mem_offset);
  int intvalue = 0; 
  bcopy(this -> mem + mem_offset, &intvalue, type_sizeof); 

  return intvalue; 
}; 
 
int pscript_const__get_value_nil(const pscript_const_env_t * this, const int const_i) { 
  return 0; 
}; 







#define PSCRIPT_CONST__STACK__OPERATION__PREAMBLE			\
  assert(this -> stack__nb >= 2);					\
  const int type_index1 = this -> stack__nb - 2;			\
  const int type_index2 = this -> stack__nb - 1;			\
  const int type1 = this -> stack__type[type_index1];			\
  const int type2 = this -> stack__type[type_index2];			\
  const int type1_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1); \
  const int type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type2); \
  const int stack_offset2 = pscript_const__stack__top_offset(this);	\
  char value2[type2_sizeof];						\
  bcopy(this -> stack__mem + stack_offset2, value2, type2_sizeof);	\
  pscript_const__stack__drop(this);					\
  const int stack_offset1 = pscript_const__stack__top_offset(this);	\
  char value1[type1_sizeof];						\
  bcopy(this -> stack__mem + stack_offset1, value1, type1_sizeof);	\
  pscript_const__stack__drop(this);					\
  const int retval_sizeof_alloc = MAX(type1_sizeof, type2_sizeof);	\
  char retval_value[retval_sizeof_alloc];				\
  int retval_sizeof = -1;						\
  int retval_type = -1; 

#define PSCRIPT_CONST__STACK__OPERATION__PREAMBLE_1			\
  assert(this -> stack__nb >= 1);					\
  const int type_index1 = this -> stack__nb - 1;			\
  const int type1 = this -> stack__type[type_index1];			\
  const int type1_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1); \
  const int stack_offset1 = pscript_const__stack__top_offset(this);	\
  char value1[type1_sizeof];						\
  bcopy(this -> stack__mem + stack_offset1, value1, type1_sizeof);	\
  pscript_const__stack__drop(this);					\
  const int retval_sizeof_alloc = type1_sizeof;				\
  char retval_value[retval_sizeof_alloc];				\
  int retval_sizeof = -1;						\
  int retval_type = -1; 

#define PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE			\
  pscript_const__stack__push_value(this, retval_type, retval_sizeof, retval_value); 



void pscript_const__stack__and(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__bool_and(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 

void pscript_const__stack__or(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__bool_or(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__equal_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__equal_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__diff_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__diff_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__inf_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__inf_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__sup_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__sup_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__infeq_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__infeq_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__supeq_huh(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__supeq_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__add(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__add(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__sub(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__sub(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__mul(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__mul(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__idiv(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__idiv(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__imod(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__imod(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__not(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE_1;  
  pscript_value__operation__bool_not(this -> pscript_env, type1, type1_sizeof, value1, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 
 
void pscript_const__stack__opposite(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE_1;  
  pscript_value__operation__opposite(this -> pscript_env, type1, type1_sizeof, value1, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 

void pscript_const__stack__rdiv(pscript_const_env_t * this) { 
  PSCRIPT_CONST__STACK__OPERATION__PREAMBLE;  
  pscript_value__operation__rdiv(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
  PSCRIPT_CONST__STACK__OPERATION__POSTAMBLE;
}; 



 

#if 0  
#endif  









