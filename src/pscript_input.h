#ifndef PSCRIPT_INPUT_H
#define PSCRIPT_INPUT_H

#ifndef PSCRIPT_INPUT_ENV_T 
#define PSCRIPT_INPUT_ENV_T 
struct pscript_input_env_t; 
typedef struct pscript_input_env_t pscript_input_env_t; 
#endif 
 
enum { 
  PSCRIPT_INPUT__TYPE__NULL, 
  PSCRIPT_INPUT__TYPE__MEM_RO, 
  PSCRIPT_INPUT__TYPE__FILE, 
  PSCRIPT_INPUT__TYPE__FILENO, // RL: So we are not the owner, and we should not close it. 
  PSCRIPT_INPUT__TYPE__COUNT, 
}; 

enum { pscript_input__env_size = 127 }; 
 
// RL: Convention: any token must fit into a buffer - meaning that its byte-size must be lower or equal of a buffer. 
// RL: Like any C-string, we will allocate buffers with an incremented size by 1 to be able to fit the '\0' null-char at the end. 
//     Therefore, the following size should take that into account (meaning that INT_MAX won't do). 
//enum { pscript_input__buffer__size = (MAX(INT8_MAX,MIN(INT_MAX,INT16_MAX))) >> 1) }; // RL: 16k - 1 
enum { pscript_input__buffer__size = (MAX(INT8_MAX,MIN(INT_MAX,INT16_MAX >> 2)) >> 1) }; // RL: 4k - 1 
enum { pscript_input__static_buffer__size = 4 }; 
 
struct pscript_input_env_t { 
  pscript_string_env_t * string_env; 
  
  char   static_buffer[pscript_input__static_buffer__size][pscript_input__buffer__size+1]; // = {}; // RL: "+1" to be able to hold a last '\0' null-char. 
  int8_t static_buffer__nb; // = 0; 
  
  int8_t         type[pscript_input__env_size]; 
  int8_t         nb; 
  
  const char *   mem_ro[pscript_input__env_size]; 
  int            mem_ro_len[pscript_input__env_size]; 
  int            fileno[pscript_input__env_size]; 
  int            filename[pscript_input__env_size]; 
  int32_t        cumulated_read_nb[pscript_input__env_size]; 
  int08_t        eof_huh[pscript_input__env_size]; 
  int08_t        buffer_static_huh[pscript_input__env_size]; 
  char *         buffer[pscript_input__env_size]; 
  int16_t        buffer_nb[pscript_input__env_size]; 
  int32_t        current_char0[pscript_input__env_size]; 
  int24_packed_t current_line1[pscript_input__env_size]; 
  int16_t        current_column0[pscript_input__env_size]; 
  int16_t        current_buffer0[pscript_input__env_size]; 
  int32_t        token_char0[pscript_input__env_size]; 
  int24_packed_t token_line1[pscript_input__env_size]; 
  int16_t        token_column0[pscript_input__env_size]; 
  int16_t        token_buffer0[pscript_input__env_size]; 
  
  int08_t      repl_huh[pscript_input__env_size]; 
  int          repl_prompt_pstr[pscript_input__env_size]; 
  const char * repl_prompt_cstr[pscript_input__env_size]; 
  
}; 

extern pscript_input_env_t * pscript_input_env__make(pscript_string_env_t * string_env); 
extern pscript_input_env_t * pscript_input_env__make_r(pscript_string_env_t * string_env, pscript_input_env_t * this); 
extern pscript_input_env_t * pscript_input_env__make_b(pscript_string_env_t * string_env, const int bsize, void * bvalue); 
extern void pscript_input_env__delete(pscript_input_env_t * this); 
extern void pscript_input_env__delete_r(pscript_input_env_t * this); 
 
extern int pscript_input__make_from_file_cstr(pscript_input_env_t * this, const char * filename_cstr); 
extern int pscript_input__make_from_file_pstr(pscript_input_env_t * this, const int    filename_pstr); 
 
extern int pscript_input__make_from_mem(pscript_input_env_t * this, const int mem_ro_len, const char * mem_ro); 
 
extern int pscript_input__make_from_fileno(pscript_input_env_t * this, const int fileno, const char * filename_cstr); 
 
extern void pscript_input__delete(pscript_input_env_t * this, const int input_i); 

extern int          pscript_input__eoh_huh        (pscript_input_env_t * this, const int input_i); 
extern int          pscript_input__getc           (pscript_input_env_t * this, const int input_i); 
extern void         pscript_input__move_backward  (pscript_input_env_t * this, const int input_i, const int chars_nb); 
extern const char * pscript_input__get_srcval_rstr(pscript_input_env_t * this, const int input_i); 
extern int          pscript_input__get_srcval_len (pscript_input_env_t * this, const int input_i); 
extern void         pscript_input__validate       (pscript_input_env_t * this, const int input_i); 
 
extern void pscript_input__repl__turn_off       (pscript_input_env_t * this, const int input_i); 
extern void pscript_input__repl__turn_on        (pscript_input_env_t * this, const int input_i); 
extern void pscript_input__repl__set_prompt_cstr(pscript_input_env_t * this, const int input_i, const char * prompt_cstr); 
extern void pscript_input__repl__set_prompt_pstr(pscript_input_env_t * this, const int input_i, const int    prompt_pstr); 
 
extern const char * pscript_input__get_filename_cstr(pscript_input_env_t * this, const int input_i); 
extern       int    pscript_input__get_filename_pstr(pscript_input_env_t * this, const int input_i); 



 
 






#endif /* PSCRIPT_INPUT_H */
