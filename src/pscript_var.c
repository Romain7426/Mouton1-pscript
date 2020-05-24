#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_mem.h" 
#include "pscript_var.h" 


//enum { pscript_var__mem__size = (1 << 15) }; 
enum { pscript_var__env__size = (1 << 7) }; 
struct pscript_var_env_t { 
  pscript_type_env_t * type_env; 
  pscript_mem_env_t * mem_env; 
  
#if 0 
  char mem[pscript_var__mem__size]; 
  int  mem_type[pscript_var__mem__size]; 
  int  mem_nb; 
  
  int stack__value_bool   [pscript_var__stack__size]; 
  int stack__value_int    [pscript_var__stack__size]; 
  int stack__value_float  [pscript_var__stack__size]; 
  int stack__value_char   [pscript_var__stack__size]; 
  int stack__value_str    [pscript_var__stack__size]; 
  int stack__value_nil    [pscript_var__stack__size]; 
  int stack__value_pointer[pscript_var__stack__size]; 
  int stack__value_array  [pscript_var__stack__size]; // offset dans la const_mem 
  int stack__value_record [pscript_var__stack__size]; // offset dans la const_mem 
  // proc & fun cannot be 'const' per se: this const is a compile-time const, while proc&fun are known only after relocation time (which means right before being run). 
  // user does not make sense, as it is built bottom-up. 
#endif 
  
  int env__name      [pscript_var__env__size]; 
  int env__type      [pscript_var__env__size]; 
  int env__mem_offset[pscript_var__env__size]; 
  int env__nb; 
}; 






pscript_var_env_t * pscript_var_env__make(pscript_type_env_t * type_env, pscript_mem_env_t * mem_env) { 
  MALLOC_BZERO(pscript_var_env_t,this); 
  this -> type_env = type_env; 
  this -> mem_env = mem_env; 
  return this; 
}; 
 
void pscript_var_env__delete(pscript_var_env_t * this) { 
  free(this); 
}; 
 
 
int pscript_var__get_count(const pscript_var_env_t * this) { 
  return this -> env__nb; 
}; 
 
 
int pscript_var__env__name__push_nolookup(pscript_var_env_t * this, const int name, const int type_i) { 
  assert(this -> env__nb < pscript_var__env__size); 
  const int index = this -> env__nb; 
  this -> env__name[index] = name; 
  this -> env__type[index] = type_i; 
  if (type_i >= 0) { 
    const int type_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
    //this -> env__mem_offset[index] = pscript_mem__alloc(this -> mem_env, type_sizeof); 
    if (type_i == pscript_type__get__variant(this -> type_env)) { 
      this -> env__mem_offset[index] = pscript_mem__toplevel_compiletime__zss__allocate(this -> mem_env, type_sizeof); 
    } 
    else { 
      this -> env__mem_offset[index] = pscript_mem__toplevel_compiletime__bss__allocate(this -> mem_env, type_sizeof); 
    }; 
  } 
  else { 
    this -> env__mem_offset[index] = -1; 
  }; 
  this -> env__nb ++; 
  return index; 
}; 

int pscript_var__env__name__lookup(const pscript_var_env_t * this, const int name) {
  for (int i = 0; i < this -> env__nb; i++) {
    if (this -> env__name[i] == name) return i; 
  }; 
  return -1; 
}; 


void pscript_var__env__name__set_type__nb(pscript_var_env_t * this, const int type_i, const int ident_nb) {
  assert(this -> env__nb >= ident_nb); 
  const int type_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
  const int in_zss_huh  = type_i == pscript_type__get__variant(this -> type_env); 
  for (int i = 0; i < ident_nb; i++) {
    const int index = this -> env__nb - ident_nb + i; 
    this -> env__type[index] = type_i; 
    //this -> env__mem_offset[index] = pscript_mem__alloc(this -> mem_env, type_sizeof); 
    //this -> env__mem_offset[index] = pscript_mem__toplevel_compiletime__allocate(this -> mem_env, type_sizeof); 
    if (in_zss_huh) { 
      this -> env__mem_offset[index] = pscript_mem__toplevel_compiletime__zss__allocate(this -> mem_env, type_sizeof); 
    } 
    else { 
      this -> env__mem_offset[index] = pscript_mem__toplevel_compiletime__bss__allocate(this -> mem_env, type_sizeof); 
    }; 
  }; 
}; 



int pscript_var__get_type(const pscript_var_env_t * this, const int var_i) {
  return this -> env__type[var_i]; 
}; 

int pscript_var__get_mem_offset(const pscript_var_env_t * this, const int var_i) {
  return this -> env__mem_offset[var_i]; 
}; 




