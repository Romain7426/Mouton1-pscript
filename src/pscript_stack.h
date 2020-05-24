#ifndef PSCRIPT_STACK_H
#define PSCRIPT_STACK_H

#ifndef PSCRIPT_STACK_ENV_T 
#define PSCRIPT_STACK_ENV_T 
struct pscript_stack_env_t; 
typedef struct pscript_stack_env_t pscript_stack_env_t; 
#endif 

enum { pscript_stack__size = (1 << 7) }; 
enum { pscript_stack__mem__size = (pscript_stack__size << 3) }; 
struct pscript_stack_env_t { 
  pscript_type_env_t * type_env; 
  
  char mem__array[pscript_stack__mem__size]; 
  int mem__nb; 
  
  char stack__type[pscript_stack__size]; 
  int stack__nb; 
}; 


#if 1 

#elif 0  
typedef int pscript_stack_t; // An offset. Whatever is below is forbidden (can't be popped nor read). 
#else 
struct pscript_stack_t; 
typedef struct pscript_stack_t pscript_stack_t; 
struct pscript_stack_t { 
  char * stack; 
  int stack_size; 
  int stack_nb; 
}; 
#endif 


extern pscript_stack_env_t * pscript_stack_env__make(pscript_type_env_t * type_env); 
extern void pscript_stack_env__delete(pscript_stack_env_t * this); 

extern int pscript_stack__push_value(pscript_stack_env_t * this, const int type_i, const int value_sizeof, const char * value);  
extern int pscript_stack__push_uninitialized(pscript_stack_env_t * this, const int type_i);  

extern int          pscript_stack__get_top__type(const pscript_stack_env_t * this); 
extern void         pscript_stack__get_top__value(const pscript_stack_env_t * this, int * value_sizeof_ref, const int value_allocated_sizeof, char * value); 
extern int          pscript_stack__get_top__offset(const pscript_stack_env_t * this); 
extern const char * pscript_stack__get_top__pointer(const pscript_stack_env_t * this); 
 
extern int          pscript_stack__get__type   (const pscript_stack_env_t * this, const int depth); 
extern const char * pscript_stack__get__pointer(const pscript_stack_env_t * this, const int depth); 
extern void         pscript_stack__get__value  (const pscript_stack_env_t * this, const int depth, int * value_sizeof_ref, const int value_allocated_sizeof, char * value); 
#define pscript_stack__read pscript_stack__get__value 

extern void pscript_stack__substitute(pscript_stack_env_t * this, const int depth, const int substituting_type_i, const int substituting_value_sizeof, const char * substituting_value);  

extern void pscript_stack__drop(pscript_stack_env_t * this); 

extern const char * pscript_stack__stack_offset__get__pointer         (const pscript_stack_env_t * this, const int stack_offset); 
extern       char * pscript_stack__stack_offset__get__pointer_writable(      pscript_stack_env_t * this, const int stack_offset); 

#if 0 
extern void pscript_stack__push_integer(pscript_stack_t * this, const int n);  
extern void pscript_stack__push_int_range(pscript_stack_t * this, const int type_min, const int type_max, const int n);  
extern void pscript_stack__push_float(pscript_stack_t * this, const float x);  
extern void pscript_stack__push_char(pscript_stack_t * this, const char c); 
extern void pscript_stack__push_pstr(pscript_stack_t * this, const int pstr); 
extern void pscript_stack__push_cstr(pscript_stack_t * this, const char * cstr); 

extern int          pscript_stack__pop_integer(pscript_stack_t * this);  
extern int          pscript_stack__pop_int_range(pscript_stack_t * this); 
extern float        pscript_stack__pop_float(pscript_stack_t * this);  
extern char         pscript_stack__pop_char(pscript_stack_t * this); 
extern int          pscript_stack__pop_pstr(pscript_stack_t * this); 
extern const char * pscript_stack__pop_pstr_as_cstr(pscript_stack_t * this); 
extern const char * pscript_stack__pop_cstr(pscript_stack_t * this); 
#endif 




#endif /* PSCRIPT_STACK_H */
