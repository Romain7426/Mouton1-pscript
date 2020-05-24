#ifndef PSCRIPT_MAINPROC_H
#define PSCRIPT_MAINPROC_H

#ifndef PSCRIPT_MAINPROC_ENV_T 
#define PSCRIPT_MAINPROC_ENV_T 
struct pscript_mainproc_env_t; 
typedef struct pscript_mainproc_env_t pscript_mainproc_env_t; 
#endif 

extern pscript_mainproc_env_t * pscript_mainproc_env__make(pscript_type_env_t * type_env); 
extern void pscript_mainproc_env__delete(pscript_mainproc_env_t * this);  

extern int pscript_mainproc__get_count(const pscript_mainproc_env_t * this); 
extern void mainproc_stack__print_nb(const int filedes, const pscript_mainproc_env_t * this, const pscript_string_env_t * string_env, const int mainproc_nb); 
extern void mainproc_stack__print_all(const int filedes, const pscript_mainproc_env_t * this, const pscript_string_env_t * string_env); 

extern int pscript_mainproc__push_empty(pscript_mainproc_env_t * this); 
extern int pscript_mainproc__get_bytecode_offset(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__set_bytecode_offset(pscript_mainproc_env_t * this, const int mainproc_i, const int bytecode_offset); 
extern int pscript_mainproc__set_bytecode_size(pscript_mainproc_env_t * this, const int mainproc_i, const int bytecode_size); 
extern int pscript_mainproc__set_lexical_env(pscript_mainproc_env_t * this, const int mainproc_i, const int lexical_env); 

#if 0 
extern int pscript_mainproc__lookup(const pscript_mainproc_env_t * this, const int name); 
extern int pscript_mainproc__push_full(pscript_mainproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int local_var_nb, const int local_var_name[], const int local_var_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size); 

extern int pscript_mainproc__top(pscript_mainproc_env_t * this); 
extern int pscript_mainproc__set_name(pscript_mainproc_env_t * this, const int mainproc_i, const int name_pstr); 
extern int pscript_mainproc__set_retval_type(pscript_mainproc_env_t * this, const int mainproc_i, const int retval_type_i); 
extern int pscript_mainproc__set_src_code(pscript_mainproc_env_t * this, const int mainproc_i, const int src_code); 
extern int pscript_mainproc__set_arg_push(pscript_mainproc_env_t * this, const int mainproc_i, const int arg_name, const int byref_huh, const int arg_type); 
extern int pscript_mainproc__set_localvar_push(pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_name, const int localvar_type); 
extern int pscript_mainproc__set_localvar_type_nb(pscript_mainproc_env_t * this, const int mainproc_i, const int ident_nb, const int type_i); 

#define pscript_mainproc__set_arg_push__top(this, arg_name, byref_huh, arg_type) pscript_mainproc__set_arg_push(this, pscript_mainproc__top(this), arg_name, byref_huh, arg_type) 
#define pscript_mainproc__set_localvar_type_nb__top(this, ident_nb, type_i) pscript_mainproc__set_localvar_type_nb(this, pscript_mainproc__top(this), ident_nb, type_i) 
#define pscript_mainproc__set_localvar_push__top(this, localvar_name, localvar_type) pscript_mainproc__set_localvar_push(this, pscript_mainproc__top(this), localvar_name, localvar_type) 


extern int pscript_mainproc__get_retval_type(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__get_lexical_env(const pscript_mainproc_env_t * this, const int mainproc_i); 

extern int pscript_mainproc__get_arg_all_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__get_localvar_all_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__get_arg_nb(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__get_arg_name(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i); 
extern int pscript_mainproc__get_arg_byref(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i); 
extern int pscript_mainproc__get_arg_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i, const int arg_i); 
extern int pscript_mainproc__get_localvar_nb(const pscript_mainproc_env_t * this, const int mainproc_i); 
extern int pscript_mainproc__get_localvar_name(const pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_i); 
extern int pscript_mainproc__get_localvar_sizeof(const pscript_mainproc_env_t * this, const int mainproc_i, const int localvar_i); 
#endif 


#endif /* PSCRIPT_MAINPROC_H */
