#ifndef PSCRIPT_EXCEPTION_H
#define PSCRIPT_EXCEPTION_H

enum { 
  PSCRIPT_EXCEPTION_TYPE__TRY, 
  PSCRIPT_EXCEPTION_TYPE__ASSERT, 
  PSCRIPT_EXCEPTION_TYPE__CODE_PARSER, 
  PSCRIPT_EXCEPTION_TYPE__COUNT 
}; 

// RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that an exception will fit into a 'int_pscript_exception_t'. 
enum { PSCRIPT_EXCEPTION_TYPE__INTTYPE = PSCRIPT_INT8 }; 
enum { PSCRIPT_EXCEPTION_TYPE__SIZEOF  = PSCRIPT_SIZEOF(PSCRIPT_EXCEPTION_TYPE__INTTYPE) }; 
enum { PSCRIPT_EXCEPTION_TYPE__MAX     = PSCRIPT_INTTYPE_MAX(PSCRIPT_EXCEPTION_TYPE__INTTYPE) };
#if PSCRIPT_EXCEPTION_TYPE__INTTYPE == PSCRIPT_INT8 
typedef       int8_t                   int_pscript_exception_type_t; 
#endif 

extern  const int_pscript_exception_type_t  int_pscript_exception_type__max; 
extern  const int_pscript_exception_type_t      pscript_exception_type__max; 
extern  const int_pscript_exception_type_t  int_pscript_exception_type__sizeof; 
extern  const int_pscript_exception_type_t      pscript_exception_type__sizeof; 
extern  const int_pscript_exception_type_t      pscript_exception_type__count; 

extern const char * pscript_exception_type__cstr_array[]; 

extern const char * pscript_exception_type__get_cstr(const int exception_type); 
#define pscript_exception_type__cstr pscript_exception_type__get_cstr





#ifndef PSCRIPT_EXCEPTION_ENV_T 
#define PSCRIPT_EXCEPTION_ENV_T 
struct         pscript_exception_env_t; 
typedef struct pscript_exception_env_t pscript_exception_env_t; 
#endif 
extern  const int16_t                  pscript_exception_env__sizeof; 

extern pscript_exception_env_t * pscript_exception_env__make(void); 
extern void pscript_exception_env__delete(pscript_exception_env_t * this); 


// push // switch // pop // 

// RL: This function actually allocates a new item on the stack. 
//     But it cannot set it, as it must be done by 'sigsetjmp', 
//     and 'sigsetjmp' must be called in the calling function, not 
//     in the called function (issue with the return address which 
//     gets pop'ed). 
extern sigjmp_buf * pscript_exception__push__get_buf(pscript_exception_env_t * this); 

extern void pscript_exception__pop(pscript_exception_env_t * this); 

extern void pscript_exception__raise(pscript_exception_env_t * this, const int code, const char * msg, const int data_size, const char * data); 

extern void pscript_exception__propagate(pscript_exception_env_t * this); 


// RL: Be very careful when you do a 'break', 'continue', 'return', or 'goto'  
//     inside those blocks, as it could mess up the stack. Basically, 
//     they should not be used to escape those blocks, 
//     and PSCRIPT_EXCEPTION__END should always be run. 
#define PSCRIPT_EXCEPTION__BEGIN(exception_env)				\
  { do { switch (sigsetjmp(*pscript_exception__push__get_buf((exception_env)),0)) { \
  default: pscript_exception__pop((exception_env)); pscript_exception__propagate((exception_env)); break; 
#define PSCRIPT_EXCEPTION__END(exception_env) \
  }; } while (false); pscript_exception__pop((exception_env)); }; 
 
#define PSCRIPT_EXCEPTION__PROPAGATE(exception_env) pscript_exception__pop((exception_env)); pscript_exception__propagate((exception_env)); 

// RL: So an exception block should look like this: 
#if 0 
PSCRIPT_EXCEPTION__BEGIN(exception_env) { 
  case PSCRIPT_EXCEPTION_TYPE__TRY: /* RL: The code that will be run between handlers. */ break; 
  case PSCRIPT_EXCEPTION_TYPE__MY_EXCEPTION_1:  /* RL: Do something. */ break; 
  case PSCRIPT_EXCEPTION_TYPE__MY_EXCEPTION_2:  /* RL: Do something. */ break; 
} PSCRIPT_EXCEPTION__END(exception_env); // RL: The exception is propagated if no handler. Otherwise the handlers are dropped. 
// RL: Be very careful when using 'return', 'continue', 'break', and 'goto' within the handlers: 'PSCRIPT_EXCEPTION__END' must always be called otherwise the stack will be broken. 
// RL: TODO XXX FIXME: Should I implement a 'pscript_exception__current_level' to circumvent that? Does that matter? 

#endif 


  




#endif /* PSCRIPT_EXCEPTION_H */


