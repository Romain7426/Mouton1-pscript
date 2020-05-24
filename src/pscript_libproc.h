#ifndef PSCRIPT_LIBPROC_H
#define PSCRIPT_LIBPROC_H

#ifndef PSCRIPT_LIBPROC_ENV_T 
#define PSCRIPT_LIBPROC_ENV_T 
struct pscript_libproc_env_t; 
typedef struct pscript_libproc_env_t pscript_libproc_env_t; 
#endif 

#include "pscript.h" 

#ifndef PSCRIPT_LIBPROC_CALLBACK_T 
#define PSCRIPT_LIBPROC_CALLBACK_T
//typedef int pscript_libproc_callback_t(void * callback_data); 
//typedef int pscript_libproc_callback_t(pscript_t * this); 
typedef int pscript_libproc_callback_t(pscript_t * this, const int on_stack_arg_nb); 
#endif 

enum { pscript_libproc__size = (1 << 5) };  
enum { pscript_libproc__arg__size = (1 << 4) };  
struct pscript_libproc_env_t { 
  pscript_t * pscript_env; 

  int halt_offset; 
  
  //int arg_name [pscript_libproc__size][pscript_libproc__arg__size]; 
  int arg_byref  [pscript_libproc__size][pscript_libproc__arg__size]; 
  int arg_nb     [pscript_libproc__size]; 
  int arg_atleast[pscript_libproc__size]; 
  
  int                         libproc_name     [pscript_libproc__size]; 
  int                         libproc_type     [pscript_libproc__size]; // self-type 
  //pscript_t *                  libproc_callback_data[pscript_libproc__size]; 
  pscript_libproc_callback_t * libproc_callback[pscript_libproc__size]; // run-time (VM) 
  //pscript_libproc_callback_t * libproc_compile[pscript_libproc__size]; 
  //pscript_libproc_callback_t * libproc_vm[pscript_libproc__size]; 
  int                         libproc_nb; 
}; 


extern pscript_libproc_env_t * pscript_libproc_env__make(pscript_t * pscript_env); 
extern void pscript_libproc_env__delete(pscript_libproc_env_t * this);  

extern int pscript_libproc__push_standard_lib(pscript_libproc_env_t * this); 

extern int  pscript_libproc__get_count(const pscript_libproc_env_t * this); 
extern void pscript_libproc__print_nb(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env, const int libproc_nb); 
extern void pscript_libproc__print_all(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env); 

//extern int pscript_libproc__push(pscript_libproc_env_t * this, const int name, const int arg_nb, const int arg_atleast, void * callback_data, pscript_libproc_callback_t * callback); 
extern int          pscript_libproc__push(pscript_libproc_env_t * this, const int name, const int arg_nb, const int arg_atleast, pscript_libproc_callback_t * callback); 
extern int          pscript_libproc__get_name_pstr(const pscript_libproc_env_t * this, const int libproc_i); 
extern const char * pscript_libproc__get_name_cstr(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env, const int libproc_i); 
extern int          pscript_libproc__get_arg_nb(const pscript_libproc_env_t * this, const int libproc_i); 
extern int          pscript_libproc__get_arg_atleast(const pscript_libproc_env_t * this, const int libproc_i); 
extern int          pscript_libproc__get_arg_byref(const pscript_libproc_env_t * this, const int libproc_i, const int arg_i); 

extern int pscript_libproc__call(const pscript_libproc_env_t * this, const int libproc_i, const int on_stack_arg_nb); 


extern int pscript_libproc__get_type(pscript_libproc_env_t * this, pscript_type_env_t * type_env, const int libproc_i); 



#endif /* PSCRIPT_LIBPROC_H */


