#ifndef PSCRIPT_TOKEN_H
#define PSCRIPT_TOKEN_H

#ifndef PSCRIPT_TOKEN_ENV_T 
#define PSCRIPT_TOKEN_ENV_T 
struct         pscript_token_env_t; 
typedef struct pscript_token_env_t pscript_token_env_t; 
#endif 
extern  const int32_t              pscript_token_env__sizeof; 
//typedef char surrogate_pscript_token_env_t[pscript_token_env__sizeof]; 
typedef       int16_t              int_pscript_token_t; 
extern  const int_pscript_token_t  int_pscript_token_max; // RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that a token will fit into a 'int_pscript_token_t'. 

extern pscript_token_env_t * pscript_token_env__make  (pscript_string_env_t * string_env); 
extern void                  pscript_token_env__delete(pscript_token_env_t  * this); 

extern void pscript_token__raz (pscript_token_env_t * this); 
extern int  pscript_token__push(pscript_token_env_t * this, const int srcval_len, const char * srcval_rstr, const int token_type, const int srcfile, const int offset0, const int line1, const int column0); 

extern const char * pscript_token__get_srcval_cstr (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_srcval_pstr (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_type        (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_srcfile_pstr(const pscript_token_env_t * this, const int token_idx); 
extern const char * pscript_token__get_srcfile_cstr(const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_offset0     (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_line1       (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_column0     (const pscript_token_env_t * this, const int token_idx); 
extern int          pscript_token__get_count       (const pscript_token_env_t * this); 
extern int          pscript_token__get_size        (const pscript_token_env_t * this); 

extern void pscript_token__print_stats     (const int filedes, const pscript_token_env_t * this); 
extern void pscript_token__print_one_token (const int filedes, const pscript_token_env_t * this, const int token_i); 
extern void pscript_token__print_nb_tokens (const int filedes, const pscript_token_env_t * this, const int token_nb); 
extern void pscript_token__print_all_tokens(const int filedes, const pscript_token_env_t * this); 

extern int pscript_token__dump_raw   (const int filedes, const pscript_token_env_t * this); 
extern int pscript_token__restore_raw(const int filedes,       pscript_token_env_t * this); 

extern int pscript_token__master_push_cstr(pscript_token_env_t * this, const char * ident_cstr); 
extern int pscript_token__master_push_pstr(pscript_token_env_t * this, const int    ident_pstr); 
extern int            pscript_token__master_count(const pscript_token_env_t * this); 
extern const int *    pscript_token__master_len  (const pscript_token_env_t * this); 
extern const char * * pscript_token__master_cstr (const pscript_token_env_t * this); 

extern int pscript_token__syntax_keyword_push_cstr(pscript_token_env_t * this, const char * ident_cstr); 
extern int pscript_token__syntax_keyword_push_pstr(pscript_token_env_t * this, const int    ident_pstr); 
extern int            pscript_token__syntax_keyword_count(const pscript_token_env_t * this); 
extern const int *    pscript_token__syntax_keyword_len  (const pscript_token_env_t * this); 
extern const char * * pscript_token__syntax_keyword_cstr (const pscript_token_env_t * this); 

#endif /* PSCRIPT_TOKEN_H */
