#ifndef PSCRIPT_ENV_H
#define PSCRIPT_ENV_H

#ifndef PSCRIPT_ENV_T 
#define PSCRIPT_ENV_T 
struct pscript_env_t; 
typedef struct pscript_env_t pscript_env_t; 
#endif 
 
// RL: This is compile-time environment. 
//     It's a stack: localvar are pop'ed when exiting the scope. 
//     Thus, at the end of the compilation, it should contain only top-level variables. 
//     . 
//     This thing is an object to solve binding at compile-time. 
enum { 
  PSCRIPT_ENV__VAR, 
  PSCRIPT_ENV__VAR_LOCAL, // RL: Localvars needs two data: nesting funproc + offset in that local run-time environment. 
  PSCRIPT_ENV__ARG,       // RL: Idem as localvars. 
  PSCRIPT_ENV__RETVAL,    // RL: Idem as localvars. 
  //PSCRIPT_ENV__METHOD_OF, 
  PSCRIPT_ENV__METHOD_THIS, 
  PSCRIPT_ENV__CONST, 
  PSCRIPT_ENV__FUNPROC, 
  //PSCRIPT_ENV__MAINPROC, 
  PSCRIPT_ENV__FFIPROC, 
  PSCRIPT_ENV__LIBPROC, 
  PSCRIPT_ENV__COUNT 
}; 

extern pscript_env_t * pscript_env__make(pscript_string_env_t * string_env); 
extern void pscript_env__delete(pscript_env_t * this); 

extern int  pscript_env__get_count(const pscript_env_t * this);

extern int  pscript_env__push_no_lookup(pscript_env_t * this, const int nom, const int type, const int value);
extern int  pscript_env__lookup(const pscript_env_t * this, const int nom);
extern void pscript_env__set_name(pscript_env_t * this, const int env_i, const int ident_pstr);
extern int  pscript_env__get_name(const pscript_env_t * this, const int env_i);
extern int  pscript_env__get_type(const pscript_env_t * this, const int env_i); 
extern int  pscript_env__get_value(const pscript_env_t * this, const int env_i); 
extern int  pscript_env__get_value2(const pscript_env_t * this, const int env_i); 
#define pscript_env__push pscript_env__push_no_lookup

extern int pscript_env__get_pscript_type(const pscript_env_t * this, const pscript_t * pscript_env, const int env_i); 

#if 0 
extern void pscript_env__set_current_funproc(pscript_env_t * this, const int funproc_i);
extern int  pscript_env__get_current_funproc(const pscript_env_t * this);
#endif 

//extern int          pscript_env__type__get_pstr(const pscript_env_t * this, const int env_type); 
extern const char * pscript_env__type__get_cstr(const pscript_env_t * this, const int env_type); 

extern int  pscript_env__get_offset(const pscript_env_t * this); 
extern void pscript_env__deallocate(pscript_env_t * this, const int env_offset); 


extern int pscript_env__get_etype(const pscript_env_t * this, const int ident_env_i); 

extern int pscript_env__get_const_huh(const pscript_env_t * this, const int env_i);
extern int pscript_env__get_mem_offset(const pscript_env_t * this, const int env_i);
extern int pscript_env__get_const_indice(const pscript_env_t * this, const int env_i);
extern int pscript_env__get_indice(const pscript_env_t * this, const int ident_env_i); 

extern int pscript_env__name__lookup(const pscript_env_t * this, const int nom_pstr); 

#define pscript_env__get_etype        pscript_env__get_type
#define pscript_env__get_indice       pscript_env__get_value
#define pscript_env__get_const_indice pscript_env__get_value
#define pscript_env__get_mem_offset   pscript_env__get_value
#define pscript_env__name__lookup     pscript_env__lookup


extern int pscript_env__push_const(pscript_env_t * this, const int nom_pstr, const int const_env_i); 
extern int pscript_env__push_var(pscript_env_t * this, const int nom_pstr, const int var_env_i); 
extern int pscript_env__push_proc(pscript_env_t * this, const int nom_pstr, const int funproc_i); 
//extern int pscript_env__push_methodof(pscript_env_t * this, const int some_value); 
extern int pscript_env__push_method_this(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int some_value); 
extern int pscript_env__push_retval(pscript_env_t * this, const int funproc_i, const int retval_i); 
extern int pscript_env__push_arg(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int arg_i); 
extern int pscript_env__push_localvar(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int localvar_i); 
//extern int pscript_env__push_main(pscript_env_t * this, const int nom_pstr, const int funproc_i); 
extern int pscript_env__push_ffi(pscript_env_t * this, const int nom_pstr, const int ffiproc_i); 




extern void pscript_env__print_nb(const int filedes, const pscript_env_t * this, const pscript_string_env_t * string_env, const int env_nb); 
extern void pscript_env__print_all(const int filedes, const pscript_env_t * this, const pscript_string_env_t * string_env); 




#endif /* PSCRIPT_ENV_H */
