#ifndef PSCRIPT_FUNPROC_H
#define PSCRIPT_FUNPROC_H

#ifndef PSCRIPT_FUNPROC_ENV_T 
#define PSCRIPT_FUNPROC_ENV_T 
struct pscript_funproc_env_t; 
typedef struct pscript_funproc_env_t pscript_funproc_env_t; 
#endif 

extern pscript_funproc_env_t * pscript_funproc_env__make(pscript_string_env_t * string_env, pscript_type_env_t * type_env); 
extern void pscript_funproc_env__delete(pscript_funproc_env_t * this); 

extern int pscript_funproc__lookup(const pscript_funproc_env_t * this, const int name); 
extern int pscript_funproc__push_full(pscript_funproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int local_var_nb, const int local_var_name[], const int local_var_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size); 

extern int pscript_funproc__get_count(const pscript_funproc_env_t * this); 
extern void funproc_stack__print_nb(const int filedes, const pscript_funproc_env_t * this, const pscript_string_env_t * string_env, const int funproc_nb); 
extern void funproc_stack__print_all(const int filedes, const pscript_funproc_env_t * this, const pscript_string_env_t * string_env); 

extern int pscript_funproc__get_type(pscript_funproc_env_t * this, const int funproc_i); 

extern int pscript_funproc__push_empty(pscript_funproc_env_t * this); 
extern int pscript_funproc__top(pscript_funproc_env_t * this); 
extern int pscript_funproc__set_name(pscript_funproc_env_t * this, const int funproc_i, const int name_pstr); 
extern int pscript_funproc__set_lexical_env(pscript_funproc_env_t * this, const int funproc_i, const int lexical_env); 
extern int pscript_funproc__set_retval_type(pscript_funproc_env_t * this, const int funproc_i, const int retval_type_i); 
extern int pscript_funproc__set_src_code(pscript_funproc_env_t * this, const int funproc_i, const int src_code); 
extern int pscript_funproc__set_bytecode_offset(pscript_funproc_env_t * this, const int funproc_i, const int bytecode_offset); 
extern int pscript_funproc__set_bytecode_size(pscript_funproc_env_t * this, const int funproc_i, const int bytecode_size); 
extern int pscript_funproc__set_arg_push(pscript_funproc_env_t * this, const int funproc_i, const int arg_name, const int byref_huh, const int arg_type); 
extern int pscript_funproc__set_localvar_push(pscript_funproc_env_t * this, const int funproc_i, const int localvar_name, const int localvar_type); 
extern int pscript_funproc__set_localvar_type_nb(pscript_funproc_env_t * this, const int funproc_i, const int ident_nb, const int type_i); 


#define pscript_funproc__set_arg_push__top(this, arg_name, byref_huh, arg_type) pscript_funproc__set_arg_push(this, pscript_funproc__top(this), arg_name, byref_huh, arg_type) 
#define pscript_funproc__set_localvar_type_nb__top(this, ident_nb, type_i) pscript_funproc__set_localvar_type_nb(this, pscript_funproc__top(this), ident_nb, type_i) 
#define pscript_funproc__set_localvar_push__top(this, localvar_name, localvar_type) pscript_funproc__set_localvar_push(this, pscript_funproc__top(this), localvar_name, localvar_type) 


extern int pscript_funproc__get_retval_type(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_lexical_env(const pscript_funproc_env_t * this, const int funproc_i); 

extern int pscript_funproc__get_name_pstr(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_arg_all_sizeof(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_localvar_all_sizeof(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_bytecode_offset(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_retval_sizeof(const pscript_funproc_env_t * this, const int funproc_i); //, const int retval_i); 
extern int pscript_funproc__get_retval_offset(const pscript_funproc_env_t * this, const int funproc_i); //, const int retval_i); 
extern int pscript_funproc__get_arg_nb(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_arg_name(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i); 
extern int pscript_funproc__get_arg_type(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i); 
extern int pscript_funproc__get_arg_byref(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i); 
extern int pscript_funproc__get_arg_sizeof(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i); 
extern int pscript_funproc__get_arg_offset(const pscript_funproc_env_t * this, const int funproc_i, const int arg_i); 
extern int pscript_funproc__get_localvar_nb(const pscript_funproc_env_t * this, const int funproc_i); 
extern int pscript_funproc__get_localvar_name(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i); 
extern int pscript_funproc__get_localvar_type(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i); 
extern int pscript_funproc__get_localvar_sizeof(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i); 
extern int pscript_funproc__get_localvar_offset(const pscript_funproc_env_t * this, const int funproc_i, const int localvar_i); 

extern const int * pscript_funproc__get_arg_name__array( const pscript_funproc_env_t * this, const int funproc_i); 
extern const int * pscript_funproc__get_arg_type__array( const pscript_funproc_env_t * this, const int funproc_i); 
extern const int * pscript_funproc__get_arg_byref__array(const pscript_funproc_env_t * this, const int funproc_i); 

extern void pscript_funproc__set_retval_offset(pscript_funproc_env_t * this, const int funproc_i, const int retval_offset); 
extern void pscript_funproc__set_arg_offset(pscript_funproc_env_t * this, const int funproc_i, const int arg_i, const int arg_offset); 
extern void pscript_funproc__set_localvar_offset(pscript_funproc_env_t * this, const int funproc_i, const int localvar_i, const int localvar_offset); 


#define pscript_funproc__get_offset pscript_funproc__get_bytecode_offset



extern void pscript_funproc__set_method_of(pscript_funproc_env_t * this, const int funproc_i, const int method_of_type_i); 
extern int  pscript_funproc__get_method_of(const pscript_funproc_env_t * this, const int funproc_i); 
#if 0 
extern void pscript_funproc__set_method_of__offset(pscript_funproc_env_t * this, const int funproc_i, const int offset); 
extern int  pscript_funproc__get_method_of__offset(const pscript_funproc_env_t * this, const int funproc_i); 
extern int  pscript_funproc__get_method_of__sizeof(pscript_funproc_env_t * this, const int funproc_i); 
#endif 
#define pscript_funproc__get_method_of__type pscript_funproc__get_method_of
 
extern int  pscript_funproc__get_method_this        (const pscript_funproc_env_t * this, const int funproc_i); 
extern void pscript_funproc__set_method_this__offset(      pscript_funproc_env_t * this, const int funproc_i, const int offset); 
extern int  pscript_funproc__get_method_this__offset(const pscript_funproc_env_t * this, const int funproc_i); 
extern int  pscript_funproc__get_method_this__sizeof(      pscript_funproc_env_t * this, const int funproc_i); 
#define pscript_funproc__get_method_this__type pscript_funproc__get_method_this

extern void pscript_funproc__lambda__nesting_funproc_i__set(      pscript_funproc_env_t * this, const int funproc_i, const int parent_funproc_i); 
extern int  pscript_funproc__lambda__nesting_funproc_i__get(const pscript_funproc_env_t * this, const int funproc_i); 
extern int  pscript_funproc__lambda__nesting_level__get    (const pscript_funproc_env_t * this, const int funproc_i); 
#define pscript_funproc__lambda__nesting_level pscript_funproc__lambda__nesting_level__get


#endif /* PSCRIPT_FUNPROC_H */
