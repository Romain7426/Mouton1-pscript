#include "pscript_global.h" 
#include "pscript.h" 
#include "pscript_stack.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 


pscript_stack_env_t * pscript_stack_env__make(pscript_type_env_t * type_env) {
  MALLOC_BZERO(pscript_stack_env_t,this); 
  this -> type_env = type_env; 
  this -> mem__nb = 0; 
  this -> stack__nb = 0; 
  return this;
}; 

void pscript_stack_env__delete(pscript_stack_env_t * this) {
  free(this);
}; 


static int pscript_stack__top_offset(const pscript_stack_env_t * this) {
  //const int stack_offset = pscript_stack__size - this -> stack__nb; 
  const int stack_offset = pscript_stack__mem__size - this -> mem__nb; 
  return stack_offset; 
}; 

int pscript_stack__get_top__offset(const pscript_stack_env_t * this) {
  return pscript_stack__top_offset(this); 
}; 

const char * pscript_stack__get_top__pointer(const pscript_stack_env_t * this) {
  //const int stack_offset = pscript_stack__size - this -> stack__nb; 
  const int stack_offset = pscript_stack__mem__size - this -> mem__nb; 
  return this -> mem__array +  stack_offset; 
}; 

int pscript_stack__get__type(const pscript_stack_env_t * this, const int depth) { 
  assert(this -> stack__nb > depth); 
  const int index = this -> stack__nb - depth - 1; 
  return this -> stack__type[index]; 
}; 
 
int pscript_stack__get__depth_sizeof(const pscript_stack_env_t * this, const int depth) { 
  assert(this -> stack__nb > depth); 
  int cumulated_offset = 0; 
  for (int i = 0; i < depth; i++) { 
    const int index = this -> stack__nb - i - 1; 
    const int type_i = this -> stack__type[index]; 
    cumulated_offset += pscript_type__sizeof(this -> type_env, type_i); 
  }; 
  return cumulated_offset; 
  const int stack_offset = pscript_stack__mem__size - this -> mem__nb - cumulated_offset; 
  return stack_offset; 
}; 
 
int pscript_stack__get__offset(const pscript_stack_env_t * this, const int depth) { 
  assert(this -> stack__nb > depth); 
  const int cumulated_offset = pscript_stack__get__depth_sizeof(this, depth); 
  const int stack_offset = pscript_stack__mem__size - this -> mem__nb + cumulated_offset; 
  return stack_offset; 
}; 
 
const char * pscript_stack__get__pointer(const pscript_stack_env_t * this, const int depth) { 
  const int stack_offset = pscript_stack__get__offset(this, depth); 
  return this -> mem__array + stack_offset; 
}; 

const char * pscript_stack__stack_offset__get__pointer(const pscript_stack_env_t * this, const int stack_offset) { 
  return this -> mem__array + stack_offset; 
}; 

char * pscript_stack__stack_offset__get__pointer_writable(pscript_stack_env_t * this, const int stack_offset) { 
  return this -> mem__array + stack_offset; 
}; 

 
void pscript_stack__get__value(const pscript_stack_env_t * this, const int depth, int * value_sizeof_ref, const int value_allocated_sizeof, char * value) { 
  assert(this -> stack__nb > depth); 
  const int type_i = this -> stack__type[this -> stack__nb - depth - 1]; 
  const int type_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
  assert(type_sizeof <= value_allocated_sizeof); 
  const char * stack_mem_ptr = pscript_stack__get__pointer(this, depth); 
  bcopy(stack_mem_ptr, value, type_sizeof); 
  if (NULL != value_sizeof_ref) { *value_sizeof_ref = type_sizeof; }; 
}; 

void pscript_stack__substitute(pscript_stack_env_t * this, const int depth, const int substituting_type_i, const int substituting_value_sizeof, const char * substituting_value) { 
  assert(this -> stack__nb > depth); 
  const int substituted_type_i = this -> stack__type[this -> stack__nb - depth - 1]; 
  const int substituted_type_sizeof = pscript_type__sizeof(this -> type_env, substituted_type_i); 
#if 0 
  const int substituting_type_sizeof = pscript_type__sizeof(this -> type_env, substituting_type_i); 
  assert(substituting_type_sizeof <= substituting_value_sizeof); 
#endif 
  const int extra_stack_mem_needed = substituting_value_sizeof - substituted_type_sizeof; 
  assert(this -> mem__nb + extra_stack_mem_needed <= pscript_stack__mem__size); 
  // RL: Extract from 'bcopy' manpage: «The two buffers may overlap.» (like 'memmove, unlike 'memcpy'). 
  const int top_offset = pscript_stack__top_offset(this); 
  const int top_offset_shifted = top_offset - extra_stack_mem_needed; 
  const int substituted_offset = pscript_stack__get__offset(this, depth); 
  const int cumulated_sizeof = pscript_stack__get__depth_sizeof(this, depth); 
  const int moved_size = cumulated_sizeof; 
  bcopy(this -> mem__array + top_offset, this -> mem__array + top_offset_shifted, moved_size); 
  this -> mem__nb += extra_stack_mem_needed; 
  char * stack_mem_ptr = this -> mem__array + pscript_stack__mem__size - this -> mem__nb + cumulated_sizeof; 
  bcopy(substituting_value, stack_mem_ptr, substituting_value_sizeof); 
  this -> stack__type[this -> stack__nb - depth - 1] = substituting_type_i; 
}; 
 




static int pscript_stack__mem__allocate(pscript_stack_env_t * this, const int value_sizeof) {
#if 1 
  assert(this -> mem__nb + value_sizeof <= pscript_stack__mem__size); 
  this -> mem__nb += value_sizeof; 
  const int stack_offset = pscript_stack__mem__size - this -> mem__nb; 
  return stack_offset; 
#else 
  assert(this -> stack__nb + value_sizeof <= pscript_stack__size); 
  this -> stack__nb += value_sizeof; 
  const int stack_offset = pscript_stack__size - this -> stack__nb; 
  return stack_offset; 
#endif 
}; 




static int pscript_stack__mem__push_value(pscript_stack_env_t * this, const int value_sizeof, const char * value) {
  if (0 == value_sizeof) { return pscript_stack__top_offset(this); }; 
  const int stack_offset = pscript_stack__mem__allocate(this, value_sizeof); 
  bcopy(value, this -> mem__array + stack_offset, value_sizeof); 
  return stack_offset; 
}; 



int pscript_stack__push_value(pscript_stack_env_t * this, const int type_i, const int value_sizeof, const char * value) {
  assert(this -> stack__nb < pscript_stack__size); 
  const int index = this -> stack__nb; 
  this -> stack__nb++; 
  const int stack_offset = pscript_stack__mem__push_value(this, value_sizeof, value); 
  this -> stack__type[index] = type_i; 
  return index; 
}; 
 
int pscript_stack__push_uninitialized(pscript_stack_env_t * this, const int type_i) { 
  assert(this -> stack__nb < pscript_stack__size); 
  const int index = this -> stack__nb; 
  this -> stack__type[index] = type_i; 
  this -> stack__nb++; 
  const int type_sizeof  = pscript_type__sizeof(this -> type_env, type_i); 
  const int stack_offset = pscript_stack__mem__allocate(this, type_sizeof); 
  return stack_offset; 
}; 




void pscript_stack__mem__drop(pscript_stack_env_t * this, const int value_sizeof) {
  assert(this -> mem__nb >= value_sizeof); 
  this -> mem__nb -= value_sizeof; 
}; 


void pscript_stack__drop(pscript_stack_env_t * this) {
  assert(this -> stack__nb >= 1); 
  const int type_sizeof = pscript_type__sizeof(this -> type_env, this -> stack__type[this -> stack__nb - 1]);
  pscript_stack__mem__drop(this, type_sizeof); 
  this -> stack__nb --;; 
}; 

int pscript_stack__get_top__type(const pscript_stack_env_t * this) {
  assert(this -> stack__nb >= 1); 
  return this -> stack__type[this -> stack__nb - 1]; 
}; 

void pscript_stack__get_top__value(const pscript_stack_env_t * this, int * value_sizeof_ref, const int value_allocated_sizeof, char * value) {
  assert(this -> stack__nb >= 1); 
  const int type_i = this -> stack__type[this -> stack__nb - 1]; 
  const int type_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
  const char * stack_mem = pscript_stack__get_top__pointer(this); 
  assert(type_sizeof <= value_allocated_sizeof); 
  if (NULL != value_sizeof_ref) { *value_sizeof_ref = type_sizeof; }; 
  bcopy(stack_mem, value, type_sizeof); 
}; 





#if 0 
enum      { pscript__stack__memory__size = (1 << 15) };  
static char pscript__stack__memory__array[pscript__stack__size] = { }; 
static char pscript__stack__memory__type[pscript__stack__size] = { }; 
static int  pscript__stack__memory__nb = 0; 
 
 
 
 
 

static int  pscript__stack__push(const int buffer_size, const char * buffer) {
  assert(NULL !=  buffer); 
  if (0 == buffer_size) { return 0; }; 
  assert(pscript__stack__size - pscript__stack__nb > buffer_size); 
  const int offset = pscript__stack__nb;
  bcopy(buffer, pscript__stack__array + offset, (buffer_size) * (sizeof(char))); 
  pscript__stack__nb += buffer_size; 
  return offset; 
}; 
 
static int  pscript__stack__allocate(const int size) {
  if (0 == size) { return 0; }; 
  assert(pscript__stack__size - pscript__stack__nb > size); 
  const int offset = pscript__stack__nb;
  pscript__stack__nb += size; 
  return offset; 
}; 
 
static void pscript__stack__set(const int offset, const int buffer_size, const char * buffer) {
  assert(NULL !=  buffer); 
  if (0 == buffer_size) { return; }; 
  assert(pscript__stack__nb >= offset + buffer_size); 
  bcopy(buffer, pscript__stack__array + offset, (buffer_size) * (sizeof(char))); 
}; 
 




void pscript_stack__push_integer(pscript_stack_t stack, const int n) { 
  assert(pscript__stack__memory__nb + sizeof(int) < pscript__stack__memory__size); 
  const int i = pscript__stack__memory__nb; 
  pscript__stack__memory__type[i] = PSCRIPT_TYPE__INTEGER; 
  (int *)(pscript__stack__memory__array + i) = n; 
  pscript__stack__memory__nb += sizeof(int); 
};  
   
void pscript_stack__push_int_range(pscript_stack_t stack, const int type_min, const int type_max, const int n) { 
  assert(pscript__stack__memory__nb + sizeof(int) < pscript__stack__memory__size); 
  const int i = pscript__stack__memory__nb; 
  pscript__stack__memory__type[i] = PSCRIPT_TYPE__INT_RANGE; 
  (int *)(pscript__stack__memory__array + i) = n; 
  pscript__stack__memory__nb += sizeof(int); 
};  
   
void pscript_stack__push_float(pscript_stack_t stack, const float x) { 
  assert(pscript__stack__memory__nb + sizeof(float) < pscript__stack__memory__size); 
  const int i = pscript__stack__memory__nb; 
  pscript__stack__memory__type[i] = PSCRIPT_TYPE__FLOAT; 
  (float *)(pscript__stack__memory__array + i) = x; 
  pscript__stack__memory__nb += sizeof(float); 
};  
   
void pscript_stack__push_char(pscript_stack_t stack, const char c) { 
  assert(pscript__stack__memory__nb + sizeof(char) < pscript__stack__memory__size); 
  const int i = pscript__stack__memory__nb; 
  pscript__stack__memory__type[i] = PSCRIPT_TYPE__CHAR; 
  (char *)(pscript__stack__memory__array + i) = c; 
  pscript__stack__memory__nb += sizeof(char); 
};  
   
void pscript_stack__push_pstr(pscript_stack_t stack, const int pstr) { 
  assert(pscript__stack__memory__nb + sizeof(int) < pscript__stack__memory__size); 
  const int i = pscript__stack__memory__nb; 
  pscript__stack__memory__type[i] = PSCRIPT_TYPE__STRING; 
  (int *)(pscript__stack__memory__array + i) = pstr; 
  pscript__stack__memory__nb += sizeof(int); 
};  
   
void pscript_stack__push_cstr(pscript_stack_t stack, const char * cstr) { 
  const int i = pscript_string__push(cstr); 
  pscript_stack__push_pstr(stack, i); 
};  
   
 

int pscript_stack__pop_integer(pscript_stack_t stack) { 
  assert(pscript__stack__memory__nb - sizeof(int) >= 0); 
  const int i = pscript__stack__memory__nb - 1; 
  assert(PSCRIPT_TYPE__INTEGER == pscript__stack__memory__type[i]);
  const int val = *(int *)(pscript__stack__memory__array + i); 
  pscript__stack__memory__nb -= sizeof(int); 
  return val; 
}; 
   
int pscript_stack__pop_int_range(pscript_stack_t stack) { 
  assert(pscript__stack__memory__nb - sizeof(int) >= 0); 
  const int i = pscript__stack__memory__nb - 1; 
  assert(PSCRIPT_TYPE__INT_RANGE == pscript__stack__memory__type[i]);
  const int val = *(int *)(pscript__stack__memory__array + i); 
  pscript__stack__memory__nb -= sizeof(int); 
  return val; 
}; 
 
float pscript_stack__pop_float(pscript_stack_t stack) { 
  assert(pscript__stack__memory__nb - sizeof(float) >= 0); 
  const int i = pscript__stack__memory__nb - 1; 
  assert(PSCRIPT_TYPE__FLOAT == pscript__stack__memory__type[i]);
  const float val = *(float *)(pscript__stack__memory__array + i); 
  pscript__stack__memory__nb -= sizeof(float); 
  return val; 
};  
   
char pscript_stack__pop_char(pscript_stack_t stack) { 
  assert(pscript__stack__memory__nb - sizeof(char) >= 0); 
  const int i = pscript__stack__memory__nb - 1; 
  assert(PSCRIPT_TYPE__CHAR == pscript__stack__memory__type[i]);
  const char val = *(char *)(pscript__stack__memory__array + i); 
  pscript__stack__memory__nb -= sizeof(char); 
  return val; 
};  
   
int pscript_stack__pop_pstr(pscript_stack_t stack) { 
  assert(pscript__stack__memory__nb - sizeof(int) >= 0); 
  const int i = pscript__stack__memory__nb - 1; 
  assert(PSCRIPT_TYPE__STRING == pscript__stack__memory__type[i]);
  const int val = *(int *)(pscript__stack__memory__array + i); 
  pscript__stack__memory__nb -= sizeof(int); 
  return val; 
};  
   
const char * pscript_stack__pop_pstr_as_cstr(pscript_stack_t stack) { 
  const int pstr = pscript_stack__pop_pstr(stack); 
  return pscript_string__get_cstr(pstr); 
};  
   
const char * pscript_stack__pop_cstr(pscript_stack_t stack) { 
  return pscript_stack__pop_pstr_as_cstr(stack); 
};  
   




#endif 
