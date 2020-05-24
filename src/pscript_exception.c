#include "pscript_global.h" 
#include "pscript_exception.h" 

const char * pscript_exception_type__cstr_array[] = {  
  "TRY", 
  "ASSERT", 
  "CODE_PARSER", 
  NULL 
}; 

enum { pscript_exception_type__cstr_array__size = ARRAY_SIZE(pscript_exception_type__cstr_array) }; 

static void assert_compile__dkudsuf2387645kjhsdfr(void) {
  ASSERT_COMPILE(pscript_exception_type__cstr_array__size == PSCRIPT_EXCEPTION_TYPE__COUNT + 1); 
}; 

const char * pscript_exception_type__get_cstr(const int exception_type) { 
  if (exception_type < 0) return ""; 
  if (exception_type >= PSCRIPT_EXCEPTION_TYPE__COUNT) return ""; 
  return pscript_exception_type__cstr_array[exception_type]; 
}; 



const int_pscript_exception_type_t int_pscript_exception_type__max    = PSCRIPT_EXCEPTION_TYPE__MAX; 
const int_pscript_exception_type_t     pscript_exception_type__max    = PSCRIPT_EXCEPTION_TYPE__MAX; 
const int_pscript_exception_type_t int_pscript_exception_type__sizeof = PSCRIPT_EXCEPTION_TYPE__SIZEOF; 
const int_pscript_exception_type_t     pscript_exception_type__sizeof = PSCRIPT_EXCEPTION_TYPE__SIZEOF; 
const int_pscript_exception_type_t     pscript_exception_type__count  = PSCRIPT_EXCEPTION_TYPE__COUNT; 

enum { PSCRIPT_EXCEPTION__SIZE = 126 }; //RL: Nestedness. 
struct pscript_exception_env_t { 
  sigjmp_buf     stack_array[PSCRIPT_EXCEPTION__SIZE]; 
  int8_t         stack_nb; 
  int8_t         stack_size; 
  
  int_pscript_exception_type_t raised_code; 
  char                         raised_msg[512]; 
  int                          raised_data_size; 
  char                         raised_data[(1 << 12)]; 
  
  int8_t malloced_huh; 
}; 


const int16_t pscript_exception_env__sizeof = sizeof(pscript_exception_env_t); 


pscript_exception_env_t * pscript_exception_env__make(void) { 
  MALLOC_BZERO(pscript_exception_env_t,this); 
  this -> stack_size = PSCRIPT_EXCEPTION__SIZE; 
  this -> malloced_huh = true;
  return this; 
}; 

void pscript_exception_env__delete(pscript_exception_env_t * this) { 
  if (this -> malloced_huh) { 
    free(this); 
  }; 
}; 


sigjmp_buf * pscript_exception__push__get_buf(pscript_exception_env_t * this) { 
  //dputs_array(stderr__fileno, __func__, "\n"); 
  if (this -> stack_nb >= this -> stack_size) { abort(); }; // RL: How are we supposed to recover from this? In throwing an exception? 
  sigjmp_buf * p = this -> stack_array + this -> stack_nb; 
  this -> stack_nb++; 
  return p; 
}; 


void pscript_exception__pop(pscript_exception_env_t * this) { 
  //dputs_array(stderr__fileno, __func__, "\n"); 
  if (this -> stack_nb < 1) { abort(); }; // RL: How are we supposed to recover from this? In throwing an exception? 
  this -> stack_nb--; 
}; 

void pscript_exception__propagate(pscript_exception_env_t * this) { 
  if (this -> stack_nb < 1) { 
    dputs_array(stderr__fileno, "Uncaught exception (an exception was raised while there is no handler to catch it):" "\n"); 
    dputs_array(stderr__fileno, "\t" "Exception code     : ", "(", int_string__stack(this -> raised_code), ")", pscript_exception_type__get_cstr(this -> raised_code), "\n"); 
    dputs_array(stderr__fileno, "\t" "Exception msg      : ", this -> raised_msg, "\n"); 
    dputs_array(stderr__fileno, "\t" "Exception data size: ", int_string__stack(this -> raised_data_size), "\n"); 
    abort(); // RL: How are we supposed to recover from this? In throwing an exception? 
  }; 
  
  siglongjmp(this -> stack_array[this -> stack_nb - 1], this -> raised_code); 
}; 


void pscript_exception__raise(pscript_exception_env_t * this, const int code, const char * msg, const int data_size, const char * data) { 
  this -> raised_code = code; 
  strlcpy(this -> raised_msg, msg, ARRAY_SIZE(this -> raised_msg)); 
  this -> raised_data_size = data_size; 
  bcopy(data, this -> raised_data, MIN(data_size, (int)ARRAY_SIZE(this -> raised_data))); 
  pscript_exception__propagate(this); 
}; 



