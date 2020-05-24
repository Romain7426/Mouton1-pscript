#ifndef PSCRIPT_FFIPROC_H
#define PSCRIPT_FFIPROC_H

#ifndef PSCRIPT_FFIPROC_CALLBACK_T 
#define PSCRIPT_FFIPROC_CALLBACK_T 
#if 1 
typedef int pscript_ffiproc_callback_t(void * callback_data, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value); 
#else 
typedef int pscript_ffiproc_callback_t(void * callback_data, const int argc, const char argv_core_type[], const int bool_nb, const char bool_stack[], const int integer_nb, const int integer_stack[], const int float_nb, const float float_stack[], const int char_nb, const char char_stack[], const int string_nb, const int string_stack[], const int pointer_nb, const int pointer_type[], const int pointer_stack[], const int proxy_nb, const int proxy_type[], const int proxy_stack[], const int array_nb, const int array_type[], const int array_offset[], const int array_mem_size, const char array_mem[], const int record_nb, const int record_type[], const int record_offset[], const int record_mem_size, const char record_mem[], const int udf_nb, const int udf_type[], const int udf_offset[], const int udf_mem_size, const char udf_mem[]); 
#endif 
#endif 

#ifndef PSCRIPT_FFIPROC_ENV_T 
#define PSCRIPT_FFIPROC_ENV_T 
struct pscript_ffiproc_env_t; 
typedef struct pscript_ffiproc_env_t pscript_ffiproc_env_t; 
#endif 

enum { pscript_ffiproc__size      = (1 << 7) }; 
enum { pscript_ffiproc__arg__size = (1 << 5) }; 
struct pscript_ffiproc_env_t { 
  pscript_type_env_t * type_env; 
  
  int retval_type[pscript_ffiproc__size]; 
  
  int method_of__type[pscript_ffiproc__size]; 

  int arg_name   [pscript_ffiproc__size][pscript_ffiproc__arg__size]; 
  int arg_byref  [pscript_ffiproc__size][pscript_ffiproc__arg__size]; 
  int arg_type   [pscript_ffiproc__size][pscript_ffiproc__arg__size]; 
  int arg_nb     [pscript_ffiproc__size]; 
  int arg_atleast[pscript_ffiproc__size]; 
  
  int                          ffiproc_name         [pscript_ffiproc__size]; 
  int                          ffiproc_type         [pscript_ffiproc__size]; // self-type 
  void *                       ffiproc_callback_data[pscript_ffiproc__size]; 
  pscript_ffiproc_callback_t * ffiproc_callback     [pscript_ffiproc__size]; 
  int                          ffiproc_nb; 
}; 


extern pscript_ffiproc_env_t * pscript_ffiproc_env__make(pscript_type_env_t * type_env); 
extern void pscript_ffiproc_env__delete(pscript_ffiproc_env_t * this);  

extern int pscript_ffiproc__get_count(const pscript_ffiproc_env_t * this); 
extern void ffiproc_stack__print_nb(const int filedes, const pscript_ffiproc_env_t * this, const pscript_string_env_t * string_env, const int ffiproc_nb); 
extern void ffiproc_stack__print_all(const int filedes, const pscript_ffiproc_env_t * this, const pscript_string_env_t * string_env); 

extern int pscript_ffiproc__push(pscript_ffiproc_env_t * this, const int name, const int arg_atleast, const int retval_type, void * callback_data, pscript_ffiproc_callback_t * ffiproc_callback); 

extern int pscript_ffiproc__get_atleast_arg_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i); 

extern int pscript_ffiproc__get_retval_type(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_method_of_type(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_arg_byref(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i); 
 
extern int pscript_ffiproc__call(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value); 



extern int pscript_ffiproc__set_method_of_type(pscript_ffiproc_env_t * this, const int ffiproc_i, const int method_i); 
#define pscript_ffiproc__set_method_of pscript_ffiproc__set_method_of_type



extern int pscript_ffiproc__get_type(pscript_ffiproc_env_t * this, const int ffiproc_i); 


extern int pscript_ffiproc__lookup(const pscript_ffiproc_env_t * this, const int name); 
extern int pscript_ffiproc__set_arg_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_name, const int byref_huh, const int arg_type); 

#if 0 
extern int pscript_ffiproc__push_full(pscript_ffiproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int local_var_nb, const int local_var_name[], const int local_var_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size); 

extern int pscript_ffiproc__push_empty(pscript_ffiproc_env_t * this); 
extern int pscript_ffiproc__top(pscript_ffiproc_env_t * this); 
extern int pscript_ffiproc__set_name(pscript_ffiproc_env_t * this, const int ffiproc_i, const int name_pstr); 
extern int pscript_ffiproc__set_lexical_env(pscript_ffiproc_env_t * this, const int ffiproc_i, const int lexical_env); 
extern int pscript_ffiproc__set_retval_type(pscript_ffiproc_env_t * this, const int ffiproc_i, const int retval_type_i); 
extern int pscript_ffiproc__set_src_code(pscript_ffiproc_env_t * this, const int ffiproc_i, const int src_code); 
extern int pscript_ffiproc__set_bytecode_offset(pscript_ffiproc_env_t * this, const int ffiproc_i, const int bytecode_offset); 
extern int pscript_ffiproc__set_bytecode_size(pscript_ffiproc_env_t * this, const int ffiproc_i, const int bytecode_size); 
extern int pscript_ffiproc__set_arg_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_name, const int byref_huh, const int arg_type); 
extern int pscript_ffiproc__set_localvar_push(pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_name, const int localvar_type); 
extern int pscript_ffiproc__set_localvar_type_nb(pscript_ffiproc_env_t * this, const int ffiproc_i, const int ident_nb, const int type_i); 

#define pscript_ffiproc__set_arg_push__top(this, arg_name, byref_huh, arg_type) pscript_ffiproc__set_arg_push(this, pscript_ffiproc__top(this), arg_name, byref_huh, arg_type) 
#define pscript_ffiproc__set_localvar_type_nb__top(this, ident_nb, type_i) pscript_ffiproc__set_localvar_type_nb(this, pscript_ffiproc__top(this), ident_nb, type_i) 
#define pscript_ffiproc__set_localvar_push__top(this, localvar_name, localvar_type) pscript_ffiproc__set_localvar_push(this, pscript_ffiproc__top(this), localvar_name, localvar_type) 


extern int pscript_ffiproc__get_retval_type(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_lexical_env(const pscript_ffiproc_env_t * this, const int ffiproc_i); 

extern int pscript_ffiproc__get_arg_all_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_localvar_all_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_bytecode_offset(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_arg_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_arg_name(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i); 
extern int pscript_ffiproc__get_arg_byref(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i); 
extern int pscript_ffiproc__get_arg_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int arg_i); 
extern int pscript_ffiproc__get_localvar_nb(const pscript_ffiproc_env_t * this, const int ffiproc_i); 
extern int pscript_ffiproc__get_localvar_name(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_i); 
extern int pscript_ffiproc__get_localvar_sizeof(const pscript_ffiproc_env_t * this, const int ffiproc_i, const int localvar_i); 

#endif 


#endif /* PSCRIPT_FFIPROC_H */
