#ifndef PSCRIPT_VAR_H
#define PSCRIPT_VAR_H

#ifndef PSCRIPT_VAR_ENV_T 
#define PSCRIPT_VAR_ENV_T 
struct pscript_var_env_t; 
typedef struct pscript_var_env_t pscript_var_env_t; 
#endif 

extern pscript_var_env_t * pscript_var_env__make(pscript_type_env_t * type_env, pscript_mem_env_t * mem_env); 
extern void pscript_var_env__delete(pscript_var_env_t * this);  

extern int pscript_var__get_count(const pscript_var_env_t * this);  

extern int pscript_var__stack__push_bool  (pscript_var_env_t * this, const int bool_value); 
extern int pscript_var__stack__push_int   (pscript_var_env_t * this, const int int_value); 
extern int pscript_var__stack__push_float (pscript_var_env_t * this, const int float_value); 
extern int pscript_var__stack__push_string(pscript_var_env_t * this, const int pstr); 
extern int pscript_var__stack__push_nil   (pscript_var_env_t * this); 

extern int pscript_var__stack__push_pointer(pscript_var_env_t * this); 
extern int pscript_var__stack__push_array(pscript_var_env_t * this); 
extern int pscript_var__stack__push_record(pscript_var_env_t * this); 
 
extern int pscript_var__stack__pop(pscript_var_env_t * this); 
extern int pscript_var__stack__top_get_type(pscript_var_env_t * this); 
extern int pscript_var__stack__top_get_value(pscript_var_env_t * this); 

extern int pscript_var__env__name__push_nolookup(pscript_var_env_t * this, const int name, const int type_i); 
extern int pscript_var__env__name__lookup(const pscript_var_env_t * this, const int name); 
#define pscript_var__env__name__push pscript_var__env__name__push_nolookup

extern void pscript_var__env__name__set_type__nb(pscript_var_env_t * this, const int type_i, const int ident_nb); 



extern int pscript_var__get_type(const pscript_var_env_t * this, const int ident_var_i); 
extern int pscript_var__get_mem_offset(const pscript_var_env_t * this, const int ident_var_i); 


#if 0 
extern int   pscript_var__top_indice(void); 

extern int   pscript_var__get_type(const int const_i); 
extern int   pscript_var__get_value_int(const int const_i); 
extern float pscript_var__get_value_float(const int const_i); 
extern char  pscript_var__get_value_char(const int const_i); 
extern int   pscript_var__get_value_str(const int const_i); 

extern void  pscript_var__stack__pick(const int const_i);
#endif 

#endif /* PSCRIPT_VAR_H */
