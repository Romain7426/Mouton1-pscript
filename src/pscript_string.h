#ifndef PSCRIPT_STRING_H
#define PSCRIPT_STRING_H

#ifndef PSCRIPT_STRING_ENV_T 
#define PSCRIPT_STRING_ENV_T 
struct  pscript_string_env_t; 
typedef struct pscript_string_env_t pscript_string_env_t; 
#endif 
extern  const  int                   pscript_string_env__sizeof; 
typedef        int16_t               int_pscript_string_t; 
extern  const  int_pscript_string_t  int_pscript_string_max; // RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that a string will fit into a 'int_pscript_string_t'. 

extern pscript_string_env_t * pscript_string_env__make(void); 
extern void pscript_string_env__delete(pscript_string_env_t * this); 

extern int pscript_string__push_no_lookup(pscript_string_env_t * this, const char * cstr); 
extern int pscript_string__push_no_lookup__len(pscript_string_env_t * this, const char * cstr, const int cstr_len); 
extern const char * pscript_string__get_cstr(const pscript_string_env_t * this, const int str_index); 
extern int pscript_string__get_len(const pscript_string_env_t * this, const int str_index); 
#define pscript_string__strlen pscript_string__get_len 
extern int pscript_string__lookup(const pscript_string_env_t * this, const char * cstr); 
extern int pscript_string__lookup__len(const pscript_string_env_t * this, const char * cstr, const int cstr_len); 
extern int pscript_string__push_or_lookup(pscript_string_env_t * this, const char * cstr); 
extern int pscript_string__push_or_lookup__len(pscript_string_env_t * this, const char * cstr, const int cstr_len); 
#define pscript_string__push      pscript_string__push_or_lookup
#define pscript_string__push__len pscript_string__push_or_lookup__len
// RL: For the '_len' versions, strings are not assumed to be null-terminated (feature used by the lexer). 


extern int pscript_string__get_count(const pscript_string_env_t * this); 

extern void pscript_string__print_stats      (const int filedes, const pscript_string_env_t * this); 
extern void pscript_string__print_one_string (const int filedes, const pscript_string_env_t * this, const int string_i); 
extern void pscript_string__print_nb_strings (const int filedes, const pscript_string_env_t * this, const int string_nb); 
extern void pscript_string__print_all_strings(const int filedes, const pscript_string_env_t * this); 

extern int  pscript_string__dump_raw   (const int filedes, const pscript_string_env_t * this); 
extern int  pscript_string__restore_raw(const int filedes,       pscript_string_env_t * this); 


#endif /* PSCRIPT_STRING_H */
