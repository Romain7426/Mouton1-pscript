#ifndef PSCRIPT_H
#define PSCRIPT_H
 
#ifndef PSCRIPT_T 
#define PSCRIPT_T 
struct pscript_t; 
typedef struct pscript_t pscript_t; 
#endif 

#include "pscript_string.h" 
#include "pscript_input.h" 
#include "pscript_token.h" 
#include "pscript_token_type.h" 
#include "pscript_token_parser.h" 
#include "pscript_chunk.h" 
#include "pscript_parser.h" 
#include "pscript_code_parser.h" 
#include "pscript_type.h" 
#include "pscript_const.h" 
#include "pscript_mem.h" 
#include "pscript_var.h" 
#include "pscript_funproc.h" 
#include "pscript_mainproc.h" 
#include "pscript_ffiproc.h" 
#include "pscript_libproc.h" 
#include "pscript_env.h" 
#include "pscript_bytecode.h" 
#include "pscript_stack.h" 
#include "pscript_vm.h" 
#include "pscript_value.h" 
#include "pscript_menu.h" 

struct pscript_t {
  int          filename_pstr; 
  const char * filename_cstr; 
  
  pscript_string_env_t * string_env; 
  
  pscript_input_env_t  * input_env; 

  pscript_token_env_t  * token_env; 
  
  pscript_parser_env_t * parser_env; 
 
  pscript_type_env_t   * type_env; 
  
  pscript_const_env_t  * const_env; 
  
  pscript_mem_env_t    * mem_env; 
  
  pscript_var_env_t    * var_env; 
  
  pscript_funproc_env_t  *  funproc_env; 
  
  pscript_mainproc_env_t * mainproc_env; 
  
  pscript_ffiproc_env_t  *  ffiproc_env; 
  
  pscript_libproc_env_t  *  libproc_env; 
  
  pscript_env_t * env; 
  
  pscript_bytecode_env_t * bytecode_env; 
  
  pscript_stack_env_t * stack_env; 
  
  pscript_vm_t * vm; 
  
  pscript_menu_env_t * menu_env; 
  
  pscript_t * (* make)(void); 
  void (* delete)(pscript_t * this); 

  void  * writeln_data; 
  int  (* writeln_callback)(void * writeln_data, const char * cstr); 
  void (* writeln_set)(struct pscript_t * this, void * writeln_data, int (* writeln_callback)(void * writeln_data, const char * cstr)); 
  int  (* writeln)(struct pscript_t * this, const char * cstr);

  void  * menu_data; 
  int     menu_async_huh; 
  int  (* menu_callback)(void * menu_data, const char * menu_title, const int menu_item_nb, const char * item[]); 
  void (* menu_set)(struct pscript_t * this, void * menu_data, const int menu_async_huh, int  (* menu_callback)(void * menu_data, const char * menu_title, const int menu_item_nb, const char * item[])); 
  int  (* menu)(struct pscript_t * this, const char * menu_title, const int menu_item_nb, const char * item[]);
  void (* menu__async_reverse_callback__push_return_value)(pscript_t * this, const int choice); 

  int (* ReadDescriptionFile)(struct pscript_t * this, const char * filename);
  int (* ReadDescriptionFile_fd)(struct pscript_t * this, const int fd, const char * filename);

  int stdlog_d; // Verbose about what is currently being done. 
  int stdout_d; // Production output. 
  int stderr_d; // Output to single out errors. 
  int write_to_stderr_as_well_huh; 
#if 1
  char * stdlog_filename; 
  char * stdout_filename;  
  char * stderr_filename;  
#else 
  int stdlog_owner_huh; 
  int stdout_owner_huh;  
  int stderr_owner_huh;  
#endif 
  char * stdlog_prefix; 
  char * stdout_prefix;  
  char * stderr_prefix;  
  void (* WriteStdlog)(pscript_t * this, const int argc, const char * argv[]); 
  void (* WriteStdout)(pscript_t * this, const int argc, const char * argv[]); 
  void (* WriteStderr)(pscript_t * this, const int argc, const char * argv[]); 

  void (* stdlog__set__fdes)(pscript_t * this, const int fdes); 
  void (* stdlog__set__file)(pscript_t * this, const char * filename); 
  void (* stdout__set__fdes)(pscript_t * this, const int fdes); 
  void (* stdout__set__file)(pscript_t * this, const char * filename); 
  void (* stderr__set__fdes)(pscript_t * this, const int fdes, const int write_to_stderr_as_well_huh); 
  void (* stderr__set__file)(pscript_t * this, const char * filename, const int write_to_stderr_as_well_huh); 
  void (* stdlog__unset)(pscript_t * this); 
  void (* stdout__unset)(pscript_t * this); 
  void (* stderr__unset)(pscript_t * this); 
  void (* stdlog__prefix_set)(pscript_t * this, const char * prefix); 
  void (* stdout__prefix_set)(pscript_t * this, const char * prefix); 
  void (* stderr__prefix_set)(pscript_t * this, const char * prefix); 
  void (* stdlog__prefix_unset)(pscript_t * this); 
  void (* stdout__prefix_unset)(pscript_t * this); 
  void (* stderr__prefix_unset)(pscript_t * this); 

}; 

extern pscript_t * pscript__make(void); 
extern void pscript__delete(pscript_t * this); 
#define pscript_make pscript__make 
#define pscript_delete pscript__delete 

extern void pscript__stdlog__unset(pscript_t * this); 
extern void pscript__stdout__unset(pscript_t * this); 
extern void pscript__stderr__unset(pscript_t * this); 

extern void pscript__stdlog__prefix_set(pscript_t * this, const char * prefix); 
extern void pscript__stdout__prefix_set(pscript_t * this, const char * prefix); 
extern void pscript__stderr__prefix_set(pscript_t * this, const char * prefix); 

extern void pscript__stdlog__prefix_unset(pscript_t * this); 
extern void pscript__stdout__prefix_unset(pscript_t * this); 
extern void pscript__stderr__prefix_unset(pscript_t * this); 

extern void pscript__stdlog__set__fdes(pscript_t * this, const int fdes); 
extern void pscript__stdlog__set__file(pscript_t * this, const char * filename); 
extern void pscript__stdout__set__fdes(pscript_t * this, const int fdes); 
extern void pscript__stdout__set__file(pscript_t * this, const char * filename); 
extern void pscript__stderr__set__fdes(pscript_t * this, const int fdes, const int write_to_stderr_as_well_huh); 
extern void pscript__stderr__set__file(pscript_t * this, const char * filename, const int write_to_stderr_as_well_huh); 

//extern int pscript__dputs(pscript_t * this, const int fdes, const int argc, const char * argv[]); 
extern void pscript__WriteStdlog(pscript_t * this, const int argc, const char * argv[]); 
extern void pscript__WriteStdout(pscript_t * this, const int argc, const char * argv[]); 
extern void pscript__WriteStderr(pscript_t * this, const int argc, const char * argv[]); 


#ifndef BIGLIB_STRING
#define BIGLIB_STRING(s) BIGLIB_STRINGX(s)
#define BIGLIB_STRINGX(s) #s
#endif 

#define PScript_WriteStdlog2(this,...) pscript__WriteStdlog(this,(ARRAY_SIZE(((const char * []) { __VA_ARGS__ }))), ((const char * []) { __VA_ARGS__ }))
#define PScript_WriteStdout2(this,...) pscript__WriteStdout(this,(ARRAY_SIZE(((const char * []) { __VA_ARGS__ }))), ((const char * []) { __VA_ARGS__ }))
#define PScript_WriteStderr2(this,...) pscript__WriteStderr(this,(ARRAY_SIZE(((const char * []) { __VA_ARGS__ }))), ((const char * []) { __VA_ARGS__ }))

#define PScript_WriteStdlog(this,...) PScript_WriteStdlog2(this, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " __VA_ARGS__) 
#define PScript_WriteStdout(this,...) PScript_WriteStdout2(this, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " __VA_ARGS__) 
#define PScript_WriteStderr(this,...) PScript_WriteStderr2(this, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " __VA_ARGS__) 

//putc('\n', yypascalout); 
#define PScript_PrintfStdlog(this,...)					\
  if (this != NULL) if (this -> stdlog_d >= 0) {			\
      if (this -> stdlog_prefix != NULL) dputs(this -> stdlog_d, this -> stdlog_prefix); \
      dprintf(this -> stdlog_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); \
    }; 
#define PScript_PrintfStdout(this,...)					\
  if (this != NULL) if (this -> stdout_d >= 0) {			\
      if (this -> stdout_prefix != NULL) dputs(this -> stdout_d, this -> stdout_prefix); \
      dprintf(this -> stdout_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); \
    }; 
#define PScript_PrintfStderr(this,...)					\
  if (this != NULL) {							\
    if (this -> stdlog_d >= 0) {					\
      if (this -> stdlog_prefix != NULL) dputs(this -> stdlog_d, this -> stdlog_prefix); \
      if (this -> stderr_prefix != NULL) dputs(this -> stdlog_d, this -> stderr_prefix); \
      dprintf(this -> stdlog_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); \
    };									\
    if (this -> stderr_d >= 0) {					\
      if (this -> stderr_prefix != NULL) dputs(this -> stderr_d, this -> stderr_prefix); \
      dprintf(this -> stderr_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); \
    };									\
    if (this -> write_to_stderr_as_well_huh) {				\
      if (this -> stderr_prefix != NULL) dputs(stderr_d, this -> stderr_prefix); \
      dprintf(stderr_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); \
    };									\
  }									\
  else {								\
    if (this -> stderr_prefix != NULL) dputs(stderr_d, this -> stderr_prefix); \
    dprintf(stderr_d, __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": " __VA_ARGS__); \
  }; 


extern void pscript__writeln_set(pscript_t * this, void * writeln_data, int (* writeln_callback)(void * writeln_data, const char * cstr)); 
extern int  pscript__writeln(pscript_t * this, const char * cstr);

extern void pscript__menu_set(struct pscript_t * this, void * menu_data, const int menu_async_huh, int  (* menu_callback)(void * menu_data, const char * menu_title, const int menu_item_nb, const char * item[])); 
extern int  pscript__menu(struct pscript_t * this, const char * menu_title, const int menu_item_nb, const char * item[]);
extern int  pscript__menu2(pscript_t * this, const int menu_i); 
extern void pscript__menu__async_reverse_callback__push_return_value(pscript_t * this, const int choice); 


extern int pscript__ReadDescriptionFile(pscript_t * this, const char * filename);
extern int pscript__ReadDescriptionFile_fd(pscript_t * this, const int fd, const char * filename);






#endif /* PSCRIPT_H */
