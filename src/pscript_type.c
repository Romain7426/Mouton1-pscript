#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 


enum { pscript_type__record_field_size = 23 }; 
enum { pscript_type__proc_or_fun_arg_size = 31 }; 
enum { pscript_type__methodes__size = 31 }; 
struct pscript_type_cell_t; 
typedef struct pscript_type_cell_t pscript_type_cell_t; 
struct pscript_type_cell_t { 
  pscript_type_env_t * type_env; 
  
  int type_core; 

  union {   
    struct {
      int int_range__min; 
      int int_range__max; 
    }; 
    
    struct {
      int int_range__min__dyn__mem_offset; 
      int int_range__max__dyn__mem_offset; 
      //int int_range__min__dyn__expr_code; 
      //int int_range__max__dyn__expr_code; 
    }; 
    
    //int variant_subtype; 
    
    int pointer_subtype; 
    
    int proxy_subtype; 
    
    //struct pscript_type_cell_t * array_type; 
    struct { 
      int array_subtype; 
      int array_index_min; 
      int array_index_max; 
      int array_index_min__dyn__mem_offset; 
      int array_index_max__dyn__mem_offset; 
      //int array_index_min__dyn__expr_code; 
      //int array_index_max__dyn__expr_code; 
    }; 
  
    struct { 
      int record_field_name[pscript_type__record_field_size]; 
      //struct pscript_type_cell_t * record_field_type[pscript_type__record_field_size]; 
      int record_field_type[pscript_type__record_field_size]; 
      int record_field_offset[pscript_type__record_field_size]; 
      int record_field_nb; 
    }; 
   
    struct {
      //struct pscript_type_cell_t * funproc__arg[pscript_type__proc_or_fun_arg_size]; 
      int funproc__arg_name [pscript_type__proc_or_fun_arg_size]; 
      int funproc__arg_byref[pscript_type__proc_or_fun_arg_size]; 
      int funproc__arg_type [pscript_type__proc_or_fun_arg_size]; 
      int funproc__arg_nb; 
      //struct pscript_type_cell_t * fun_retval; 
      int funproc_retval; 
    }; 
    
    struct {
      //struct pscript_type_cell_t * ffiproc__arg[pscript_type__proc_or_fun_arg_size]; 
      int ffiproc__arg_name [pscript_type__proc_or_fun_arg_size]; 
      int ffiproc__arg_byref[pscript_type__proc_or_fun_arg_size]; 
      int ffiproc__arg_type [pscript_type__proc_or_fun_arg_size]; 
      int ffiproc__arg_nb; 
      //struct pscript_type_cell_t * fun_retval; 
      int ffiproc_retval; 
    }; 
    
    struct {
      //struct pscript_type_cell_t * libproc__arg[pscript_type__proc_or_fun_arg_size]; 
      //int libproc__arg_name [pscript_type__proc_or_fun_arg_size]; 
      int libproc__arg_byref[pscript_type__proc_or_fun_arg_size]; 
      int libproc__arg_type [pscript_type__proc_or_fun_arg_size]; 
      int libproc__arg_nb; 
      //struct pscript_type_cell_t * fun_retval; 
      //int libproc_retval; 
    }; 
    
    struct { 
      int lambda__subtype; 
      int lambda__lexical_env_nb; 
    }; 
    
    int funsignature__subtype; 
    
    struct { 
      //char * user_defined_type; 
      int user_defined_type__name; 
      int user_defined_type__subtype; 
    }; 
  }; // union 
  
  
  int  methode__name   [pscript_type__methodes__size]; 
  int  methode__funproc[pscript_type__methodes__size]; 
  char methode__type   [pscript_type__methodes__size]; 
  int  methode__nb; 

}; // struct 


enum { pscript_type_env__cell__size = INT8_MAX -1 }; //(1 << 6) }; 
enum { pscript_type_env__size = (1 << 5) }; 
struct pscript_type_env_t {  
  pscript_string_env_t * string_env; 
  
  // Types 
  pscript_type_cell_t cell__array[pscript_type_env__cell__size]; 
  int cell__sizeof[pscript_type_env__cell__size]; 
  int cell__string[pscript_type_env__cell__size]; 
  int cell__string_expanded[pscript_type_env__cell__size]; 
  int cell__nb; 
  
  // Env 
  int type__name[pscript_type_env__size]; 
  int type__cell[pscript_type_env__size]; 
  int type__nb; 
}; 

const int32_t pscript_type_env__sizeof = sizeof(pscript_type_env_t); 

const int_pscript_type_t int_pscript_type_max = pscript_type_env__size; 



static const char * pscript_type_core__cstr__array[] = { 
 "#<TYPE/NULL>", 
 "VOID", 
 "ANY", 
 "BOOL", 
 "INTEGER", 
 "INT_RANGE", 
 "INT_RANGE__DYN", 
 "FLOAT", 
 "CHAR", 
 "STRING", 
 "NIL_LIST", 
 "NIL_PTR", 
 "VARIANT", 
 "POINTER", // RL: A pointer is never the owner of an object. It does not have to do any memory management. 
 "PROXY", 
 "PROXY_BYTECODE", 
 "LIST", 
 "ARRAY", 
 "ARRAY__DYN", 
 "RECORD", 
 "FUNPROC", 
 "FFIPROC", 
 "LIBPROC", 
 "LAMBDA_META", 
 "LAMBDA", 
 "FUNSIGNATURE", 
 "USER", 
 NULL
}; 

enum { pscript_type_core__cstr__size = ARRAY_SIZE(pscript_type_core__cstr__array) }; 

static void assert_compile_time__d632sad4gdss(void) {
  ASSERT_COMPILE(pscript_type_core__cstr__size == PSCRIPT_TYPE__COUNT + 1); 
  ASSERT_COMPILE(pscript_type_env__cell__size < INT8_MAX); // If not so, then all the sizes to hold a 'type_i' variable must be changed (especially in the bytecode). 
}; 
 
const char * pscript_type_core__cstr(const int pscript_type_core) {
  return pscript_type_core__cstr__array[pscript_type_core]; 
};
 
 
static const char * pscript_type__methode_type__cstr__array[] = {
 "FUNPROC", 
 "FFIPROC", 
 NULL
};  

enum { pscript_type__methode_type__cstr__size = ARRAY_SIZE(pscript_type__methode_type__cstr__array) }; 
 
static void assert_compile_time__d632sad4gsdw(void) {
  ASSERT_COMPILE(pscript_type__methode_type__cstr__size == PSCRIPT_METHODE_TYPE__COUNT + 1); 
}; 
 
const char * pscript_methode_type__cstr(const int pscript_methode_type) {
  return pscript_type__methode_type__cstr__array[pscript_methode_type]; 
};


const int scalar_types[] = { SCALAR_TYPES__LIST }; 
 
int pscript_type__scalar_huh__core(const int pscript_type_core) { 
  //return int_member_array(pscript_type__get_core_type(this -> type_env, retval_type_i), scalar_types); 
  return int_member_array(pscript_type_core, scalar_types); 
}; 

int pscript_type__scalar_huh(const pscript_type_env_t * this, const int pscript_type) { 
  const int core_type = pscript_type__get_core_type(this, pscript_type); 
  return pscript_type__scalar_huh__core(core_type); 
}; 






pscript_type_env_t * pscript_type_env__make(pscript_string_env_t * string_env) {
  MALLOC_BZERO(pscript_type_env_t,this); 
  this -> string_env = string_env; 
  //this -> cell__nb = 1; // RL: Type 0 is reserved. 
  pscript_type__push_standard_types(this); 
  return this;
}; 

void pscript_type_env__delete(pscript_type_env_t * this) {
  free(this);
}; 


int pscript_type__get_count(const pscript_type_env_t * this) { 
  return this -> type__nb; 
}; 


pscript_string_env_t * pscript_type__get_string_env(pscript_type_env_t * this) {
  return this -> string_env; 
}; 




int pscript_type__name__push_nolookup(pscript_type_env_t * this, const int name, const int type_i) {
  assert(this -> type__nb < pscript_type_env__size); 
  const int index = this -> type__nb; 
  const char * name_cstr = pscript_string__get_cstr(this -> string_env, name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower);
  this -> type__name[index] = name_lower; 
  this -> type__cell[index] = type_i; 
  this -> type__nb ++; 
  return index; 
}; 

int pscript_type__name__lookup(const pscript_type_env_t * this, const int name) { 
  const char * name_cstr = pscript_string__get_cstr(this -> string_env, name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower);
  for (int i = 0; i < this -> type__nb; i++) {
    if (this -> type__name[i] == name_lower) return i; 
  }; 
  return -1; 
}; 
 
int pscript_type__name__get_value(const pscript_type_env_t * this, const int type_env_i) {
  return this -> type__cell[type_env_i]; 
}; 

int pscript_type__get_core_type(const pscript_type_env_t * this, const int type_i) {
  return this -> cell__array[type_i].type_core; 
}; 

void pscript_type__name__set_type__for_nb_names_on_top(pscript_type_env_t * this, const int type_i, const int env_nb) {
  assert(env_nb <= this -> type__nb); 
  for (int i = 0; i < env_nb; i++) {
    this -> type__cell[this -> type__nb - i - 1] = type_i;     
  }; 
}; 


#if 1 
static int pscript_type__make__standard_type(pscript_type_env_t * this, const int subtype, const int bytesize, const char * type_string, const char * type_string_expanded) {
  const int name = pscript_string__push(this -> string_env, type_string); 
  const int lookedup_index = pscript_type__name__lookup(this, name); 
  assert(lookedup_index < 0); 
  assert(this -> cell__nb < pscript_type_env__cell__size); 
  const int index = this -> cell__nb; 
  this -> cell__array [index].type_env = this; 
  this -> cell__array [index].type_core = subtype; 
  this -> cell__sizeof[index] = bytesize; 
  this -> cell__string[index] = pscript_string__push(this -> string_env, type_string); 
  this -> cell__string_expanded[index] = pscript_string__push(this -> string_env, type_string_expanded);
  this -> cell__nb++; 
  pscript_type__name__push_nolookup(this, name, index); 
  return index; 
}; 
static int pscript_type__make__standard_type__no_string(pscript_type_env_t * this, const int subtype, const int bytesize) {
  assert(this -> cell__nb < pscript_type_env__cell__size); 
  const int index = this -> cell__nb; 
  this -> cell__array [index].type_env = this; 
  this -> cell__array [index].type_core = subtype; 
  this -> cell__sizeof[index] = bytesize; 
  this -> cell__string[index] = -1; 
  this -> cell__string_expanded[index] = -1;
  this -> cell__nb++; 
  return index; 
}; 
static int pscript_type__make__standard_type__no_no(pscript_type_env_t * this, const int subtype) {
  assert(this -> cell__nb < pscript_type_env__cell__size); 
  const int index = this -> cell__nb; 
  this -> cell__array [index].type_env = this; 
  this -> cell__array [index].type_core = subtype; 
  this -> cell__sizeof[index] = -1; 
  this -> cell__string[index] = -1; 
  this -> cell__string_expanded[index] = -1;
  this -> cell__nb++; 
  return index; 
}; 
#else 
static int pscript_type__make__standard_type(pscript_type_env_t * this, const int subtype) {
  assert(this -> cell__nb < pscript_type_env__cell__size); 
  const int index = this -> cell__nb; 
  this -> cell__array[index].type_env = this; 
  this -> cell__array[index].type_core = subtype; 
  this -> cell__sizeof[index] = -1; 
  this -> cell__string[index] = -1; 
  this -> cell__string_expanded[index] = -1;
  this -> cell__nb++; 
  return index; 
}; 
#endif 


static int standard_types__null_type = -1; 
static int standard_types__void      = -1; 
static int standard_types__any       = -1; 
static int standard_types__bool      = -1; 
static int standard_types__integer   = -1; 
static int standard_types__float     = -1; 
static int standard_types__char      = -1; 
static int standard_types__string    = -1; 
static int standard_types__nil_list  = -1; 
static int standard_types__nil_ptr   = -1; 
static int standard_types__variant   = -1; 
static int standard_types__list      = -1; 

#if 1
void pscript_type__push_standard_types(pscript_type_env_t * this) { 
if (-1 == standard_types__null_type) { standard_types__null_type = pscript_type__make__standard_type(this, PSCRIPT_TYPE__NULL,             0,      "#<TYPE/NULL>",       "#<TYPE/NULL>"); }; 
if (-1 == standard_types__void    ) { standard_types__void     = pscript_type__make__standard_type(this, PSCRIPT_TYPE__VOID,               0,               "VOID",               "VOID"); }; 
if (-1 == standard_types__any     ) { standard_types__any      = pscript_type__make__standard_type(this, PSCRIPT_TYPE__ANY,                0,                "ANY",                "ANY"); }; 
if (-1 == standard_types__bool    ) { standard_types__bool     = pscript_type__make__standard_type(this, PSCRIPT_TYPE__BOOL,               1,               "BOOL",               "BOOL"); }; 
if (-1 == standard_types__integer ) { standard_types__integer  = pscript_type__make__standard_type(this, PSCRIPT_TYPE__INTEGER,  sizeof(int),            "INTEGER",            "INTEGER"); }; 
if (-1 == standard_types__float   ) { standard_types__float    = pscript_type__make__standard_type(this, PSCRIPT_TYPE__FLOAT,  sizeof(float),              "FLOAT",              "FLOAT"); }; 
if (-1 == standard_types__char    ) { standard_types__char     = pscript_type__make__standard_type(this, PSCRIPT_TYPE__CHAR,               1,               "CHAR",               "CHAR"); }; 
if (-1 == standard_types__string  ) { standard_types__string   = pscript_type__make__standard_type(this, PSCRIPT_TYPE__STRING, sizeof(int_pscript_string_t), "STRING",          "STRING"); }; 
if (-1 == standard_types__nil_list) { standard_types__nil_list = pscript_type__make__standard_type(this, PSCRIPT_TYPE__NIL_LIST,           0,                "NIL",                "NIL"); }; 
if (-1 == standard_types__nil_ptr ) { standard_types__nil_ptr  = pscript_type__make__standard_type(this, PSCRIPT_TYPE__NIL_PTR,  sizeof(int_pscript_mem_t),           "NULL_PTR",           "NULL_PTR"); }; 
if (-1 == standard_types__variant ) { standard_types__variant  = pscript_type__make__standard_type(this, PSCRIPT_TYPE__VARIANT, sizeof(int_pscript_type_t) + sizeof(int_pscript_mem_t), "VARIANT", "VARIANT"); }; 
if (-1 == standard_types__list    ) { standard_types__list     = pscript_type__make__standard_type(this, PSCRIPT_TYPE__LIST, sizeof(int_pscript_type_t) + sizeof(int) + sizeof(int_pscript_type_t) + sizeof(int), "LIST", "LIST"); }; 
}; 
#else 
void pscript_type__push_standard_types(pscript_type_env_t * this) { 
  if (-1 == standard_types__null)    { standard_types__null    = pscript_type__make__standard_type(this, PSCRIPT_TYPE__NULL);    }; 
  if (-1 == standard_types__bool)    { standard_types__bool    = pscript_type__make__standard_type(this, PSCRIPT_TYPE__BOOL);    }; 
  if (-1 == standard_types__integer) { standard_types__integer = pscript_type__make__standard_type(this, PSCRIPT_TYPE__INTEGER); }; 
  if (-1 == standard_types__float)   { standard_types__float   = pscript_type__make__standard_type(this, PSCRIPT_TYPE__FLOAT);   }; 
  if (-1 == standard_types__char)    { standard_types__char    = pscript_type__make__standard_type(this, PSCRIPT_TYPE__CHAR);    }; 
  if (-1 == standard_types__string)  { standard_types__string  = pscript_type__make__standard_type(this, PSCRIPT_TYPE__STRING);  }; 
  if (-1 == standard_types__nil)     { standard_types__nil     = pscript_type__make__standard_type(this, PSCRIPT_TYPE__NIL);     }; 
}; 
#endif 


int pscript_type__get__void(const pscript_type_env_t * this) { 
  return standard_types__void; 
};  

int pscript_type__get__any(const pscript_type_env_t * this) { 
  return standard_types__any; 
};  

int pscript_type__get__bool(const pscript_type_env_t * this) { 
  return standard_types__bool; 
};  

int pscript_type__get__integer(const pscript_type_env_t * this) { 
  return standard_types__integer; 
};  

int pscript_type__get__float(const pscript_type_env_t * this) { 
  return standard_types__float; 
};  

int pscript_type__get__char(const pscript_type_env_t * this) { 
  return standard_types__char; 
};  

int pscript_type__get__string(const pscript_type_env_t * this) { 
  return standard_types__string; 
};  

int pscript_type__get__nil_list(const pscript_type_env_t * this) { 
  return standard_types__nil_list; 
};  

int pscript_type__get__nil_ptr(const pscript_type_env_t * this) { 
  return standard_types__nil_ptr; 
};  

int pscript_type__get__variant(const pscript_type_env_t * this) { 
  return standard_types__variant; 
}; 

int pscript_type__get__list(const pscript_type_env_t * this) { 
  return standard_types__list; 
}; 


 
int pscript_type__get__int_range(const pscript_type_env_t * this, const int i_min, const int i_max) { 
  pmessage("%s: " "i_min = %d - i_max = %d " "\n", __func__, i_min, i_max);
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core     != PSCRIPT_TYPE__INT_RANGE) continue; 
    if (this -> cell__array[i].int_range__min != i_min) continue; 
    if (this -> cell__array[i].int_range__max != i_max) continue; 
    return i; 
  }; 
  return -1; 
};  
 
int pscript_type__make__int_range__no_lookup(pscript_type_env_t * this, const int i_min__x, const int i_max__x) {
  //pmessage("%s: " "i_min = %d - i_max = %d " "\n", __func__, i_min__x, i_max__x);
  const int cell_index = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__INT_RANGE, sizeof(int)); 
  //pmessage("%s: " "The defined type is [ %d ] %d..%d." "\n", __func__, cell_index, this -> cell__array[cell_index].int_range__min, this -> cell__array[cell_index].int_range__max); 
  this -> cell__array[cell_index].int_range__min = i_min__x; 
  //pmessage("%s: " "The defined type is [ %d ] %d..%d." "\n", __func__, cell_index, this -> cell__array[cell_index].int_range__min, this -> cell__array[cell_index].int_range__max); 
  this -> cell__array[cell_index].int_range__max = i_max__x; 
  //pmessage("%s: " "The defined type is [ %d ] %d..%d." "\n", __func__, cell_index, i_min__x, i_max__x); 
  //pmessage("%s: " "The defined type is [ %d ] %d..%d." "\n", __func__, cell_index, this -> cell__array[cell_index].int_range__min, this -> cell__array[cell_index].int_range__max); 
  //this -> cell__array[cell_index].int_range__min = i_min__x; 
  //this -> cell__array[cell_index].int_range__max = i_max__x; 
  //pmessage("%s: " "The defined type is [ %d ] %d..%d." "\n", __func__, cell_index, this -> cell__array[cell_index].int_range__min, this -> cell__array[cell_index].int_range__max); 
  //pmessage("%s: " "The defined type is [ %d ] %s." "\n", __func__, cell_index, pscript_type__get_cstr(this, cell_index)); 
  return cell_index; 
}; 
 
int pscript_type__get_or_make__int_range(pscript_type_env_t * this, const int i_min, const int i_max) {
  pmessage("%s: " "i_min = %d - i_max = %d " "\n", __func__, i_min, i_max);
  const int lookedup_cell_index = pscript_type__get__int_range(this, i_min, i_max); 
  if (lookedup_cell_index >= 0) return lookedup_cell_index; 
  return pscript_type__make__int_range__no_lookup(this, i_min, i_max); 
}; 

int pscript_type__int_range__get_i_min(const pscript_type_env_t * this, const int type_i) { 
  return this -> cell__array[type_i].int_range__min; 
}; 
 
int pscript_type__int_range__get_i_max(const pscript_type_env_t * this, const int type_i) {
  return this -> cell__array[type_i].int_range__max; 
};  








int pscript_type__get__int_range_dyn(const pscript_type_env_t * this, const int i_min__mem_offset, const int i_max__mem_offset) { 
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core     != PSCRIPT_TYPE__INT_RANGE__DYN) continue; 
    if (this -> cell__array[i].int_range__min__dyn__mem_offset != i_min__mem_offset) continue; 
    if (this -> cell__array[i].int_range__max__dyn__mem_offset != i_max__mem_offset) continue; 
    return i; 
  }; 
  return -1; 
};  




static int pscript_type__make__funproc__lookup(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) {
  for (int i = 0; i < this -> cell__nb; i++) { 
    if (this -> cell__array[i].type_core       != PSCRIPT_TYPE__FUNPROC) continue; 
    if (this -> cell__array[i].funproc_retval  !=           retval_type) continue; 
    if (this -> cell__array[i].funproc__arg_nb !=                arg_nb) continue; 
    int j; 
    for (j = 0; j < arg_nb; j++) {
      if (this -> cell__array[i].funproc__arg_type [j] != arg_type [j]) break; 
      if (this -> cell__array[i].funproc__arg_byref[j] != arg_byref[j]) break; 
    }; 
    if (j != arg_nb) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__funproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) { 
#if 0 
  if (arg_nb < 1) { 
    dputs_array(stderr__fileno, __FILE__, STRINGIFY(__LINE__), __func__, ": [arg_nb: ", int_string__stack(arg_nb)," ; arg_name: NA  ; byref_huh: NA ; arg_type: NA ]" "\n"); 
  } 
  else { 
    dputs_array(stderr__fileno, __FILE__, STRINGIFY(__LINE__), __func__, ": [arg_nb: ", int_string__stack(arg_nb)," ; arg_name[0]: ", pscript_string__get_cstr(this -> string_env, arg_name[0]), " ; arg_byref[0]: ", arg_byref[0] ? "TRUE" : "FALSE", " ; arg_type[0]: ", int_string__stack(arg_type[0]), " ]" "\n"); 
  }; 
#endif 
  const int lookedup_index = pscript_type__make__funproc__lookup(this, arg_nb, arg_name, arg_byref, arg_type, retval_type); 
  if (lookedup_index >= 0) { 
    //dputs_array(stderr__fileno, __FILE__, STRINGIFY(__LINE__), __func__, " type_i = ", int_string__stack(lookedup_index), "\n"); 
    return lookedup_index; 
  }; 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__FUNPROC, sizeof(int)); 
  this -> cell__array[type_i].funproc_retval  = retval_type; 
  this -> cell__array[type_i].funproc__arg_nb =      arg_nb; 
  for (int j = 0; j < arg_nb; j++) {
    this -> cell__array[type_i].funproc__arg_name [j] = arg_name [j]; 
    this -> cell__array[type_i].funproc__arg_byref[j] = arg_byref[j]; 
    this -> cell__array[type_i].funproc__arg_type [j] = arg_type [j]; 
  }; 
  //dputs_array(stderr__fileno, __FILE__, STRINGIFY(__LINE__), __func__, " type_i = ", int_string__stack(type_i), "\n"); 
  return type_i; 
}; 

int pscript_type__funproc__get_retval_type(const pscript_type_env_t * this, const int type_i) {
  assert(PSCRIPT_TYPE__FUNPROC == this -> cell__array[type_i].type_core); 
  return this -> cell__array[type_i].funproc_retval; 
}; 

int pscript_type__funproc__get_arg_nb(const pscript_type_env_t * this, const int type_i) { 
  assert(PSCRIPT_TYPE__FUNPROC == this -> cell__array[type_i].type_core); 
  return this -> cell__array[type_i].funproc__arg_nb; 
}; 

int pscript_type__funproc__get_arg_byref_huh(const pscript_type_env_t * this, const int type_i, const int arg_i) { 
  assert(PSCRIPT_TYPE__FUNPROC == this -> cell__array[type_i].type_core); 
  assert(arg_i < this -> cell__array[type_i].funproc__arg_nb); 
  return this -> cell__array[type_i].funproc__arg_byref[arg_i]; 
}; 





static int pscript_type__make__ffiproc__lookup(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core       != PSCRIPT_TYPE__FFIPROC) continue; 
    if (this -> cell__array[i].ffiproc_retval  !=           retval_type) continue; 
    if (this -> cell__array[i].ffiproc__arg_nb !=                arg_nb) continue; 
    int j; 
    for (j = 0; j < arg_nb; j++) {
      if (this -> cell__array[i].ffiproc__arg_type [j] != arg_type [j]) break; 
      if (this -> cell__array[i].ffiproc__arg_byref[j] != arg_byref[j]) break; 
    }; 
    if (j != arg_nb) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__ffiproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) {
  const int lookedup_index = pscript_type__make__ffiproc__lookup(this, arg_nb, arg_name, arg_byref, arg_type, retval_type); 
  if (lookedup_index >= 0) { return lookedup_index; }; 
  //const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__FFIPROC, sizeof(int)); 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__FFIPROC, sizeof(int16_t)); 
  this -> cell__array[type_i].ffiproc_retval  = retval_type; 
  this -> cell__array[type_i].ffiproc__arg_nb =      arg_nb; 
  for (int j = 0; j < arg_nb; j++) {
    this -> cell__array[type_i].ffiproc__arg_name [j] = arg_name [j]; 
    this -> cell__array[type_i].ffiproc__arg_byref[j] = arg_byref[j]; 
    this -> cell__array[type_i].ffiproc__arg_type [j] = arg_type [j]; 
  }; 
  return type_i; 
}; 



//static int pscript_type__make__libproc__lookup(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) {
static int pscript_type__make__libproc__lookup(pscript_type_env_t * this, const int arg_nb, const int arg_byref[]) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core       != PSCRIPT_TYPE__LIBPROC) continue; 
    //if (this -> cell__array[i].libproc_retval  !=           retval_type) continue; 
    if (this -> cell__array[i].libproc__arg_nb !=                arg_nb) continue; 
    int j; 
    for (j = 0; j < arg_nb; j++) {
      //if (this -> cell__array[i].libproc__arg_type [j] != arg_type [j]) break; 
      if (this -> cell__array[i].libproc__arg_byref[j] != arg_byref[j]) break; 
    }; 
    if (j != arg_nb) continue; 
    return i; 
  }; 
  return -1; 
}; 

//int pscript_type__make__libproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) {
int pscript_type__make__libproc(pscript_type_env_t * this, const int arg_nb, const int arg_byref[]) {
  //const int lookedup_index = pscript_type__make__libproc__lookup(this, arg_nb, arg_name, arg_byref, arg_type, retval_type); 
  const int lookedup_index = pscript_type__make__libproc__lookup(this, arg_nb, arg_byref); 
  if (lookedup_index >= 0) { return lookedup_index; }; 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__LIBPROC, sizeof(int)); 
  //this -> cell__array[type_i].libproc_retval  = retval_type; 
  this -> cell__array[type_i].libproc__arg_nb =      arg_nb; 
  for (int j = 0; j < arg_nb; j++) {
    //this -> cell__array[type_i].libproc__arg_name [j] = arg_name [j]; 
    this -> cell__array[type_i].libproc__arg_byref[j] = arg_byref[j]; 
    //this -> cell__array[type_i].libproc__arg_type [j] = arg_type [j]; 
  }; 
  return type_i; 
}; 



static int pscript_type__make__lambda__lookup(pscript_type_env_t * this, const int lexical_env_nb, const int subtype) { 
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core              != PSCRIPT_TYPE__LAMBDA) continue; 
    if (this -> cell__array[i].lambda__subtype        != subtype             ) continue; 
    if (this -> cell__array[i].lambda__lexical_env_nb != lexical_env_nb      ) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__lambda(pscript_type_env_t * this, const int lexical_env_nb, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type) { 
  const int subtype = pscript_type__make__funproc(this, arg_nb, arg_name, arg_byref, arg_type, retval_type); 
  const int lookedup_index = pscript_type__make__lambda__lookup(this, lexical_env_nb, subtype); 
  if (lookedup_index >= 0) return lookedup_index; 
  const int this_lambda_sizeof = (sizeof(int)) + (sizeof(char)) + lexical_env_nb * (sizeof(int)); 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__LAMBDA, this_lambda_sizeof); 
  this -> cell__array[type_i].lambda__subtype = subtype; 
  this -> cell__array[type_i].lambda__lexical_env_nb = lexical_env_nb; 
  return type_i; 
}; 
 
 

static int pscript_type__make__pointer__lookup(const pscript_type_env_t * this, const int subtype) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core != PSCRIPT_TYPE__POINTER) continue; 
    if (this -> cell__array[i].pointer_subtype   != subtype) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__pointer(pscript_type_env_t * this, const int subtype) {
  const int lookedup_index = pscript_type__make__pointer__lookup(this, subtype); 
  if (lookedup_index >= 0) return lookedup_index; 
  //const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__POINTER, sizeof(int)); 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__POINTER, pscript_type__sizeof(this, standard_types__nil_ptr)); 
  this -> cell__array[type_i].pointer_subtype = subtype; 
  return type_i; 
}; 



static int pscript_type__make__proxy__lookup(const pscript_type_env_t * this, const int subtype) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core     != PSCRIPT_TYPE__PROXY) continue; 
    if (this -> cell__array[i].proxy_subtype != subtype) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__proxy(pscript_type_env_t * this, const int subtype) {
  const int lookedup_index = pscript_type__make__proxy__lookup(this, subtype); 
  if (lookedup_index >= 0) return lookedup_index; 
  //const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__PROXY, sizeof(int)); 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__PROXY, pscript_type__sizeof(this, standard_types__nil_ptr)); 
  this -> cell__array[type_i].proxy_subtype = subtype; 
  return type_i; 
}; 



static int pscript_type__make__proxy_bytecode__lookup(const pscript_type_env_t * this, const int subtype) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core     != PSCRIPT_TYPE__PROXY_BYTECODE) continue; 
    if (this -> cell__array[i].proxy_subtype != subtype) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__proxy_bytecode(pscript_type_env_t * this, const int subtype) {
  const int lookedup_index = pscript_type__make__proxy_bytecode__lookup(this, subtype); 
  if (lookedup_index >= 0) return lookedup_index; 
  const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__PROXY_BYTECODE, sizeof(int)); 
  //const int type_i = pscript_type__make__standard_type__no_string(this, PSCRIPT_TYPE__PROXY_BYTECODE, pscript_type__sizeof(this, standard_types__nil_ptr)); 
  this -> cell__array[type_i].proxy_subtype = subtype; 
  return type_i; 
}; 



static int pscript_type__make__array__lookup(const pscript_type_env_t * this, const int subtype, const int i_min, const int i_max) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core       != PSCRIPT_TYPE__ARRAY) continue; 
    if (this -> cell__array[i].array_subtype   != subtype) continue; 
    if (this -> cell__array[i].array_index_min != i_min) continue; 
    if (this -> cell__array[i].array_index_max != i_max) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__array(pscript_type_env_t * this, const int subtype, const int i_min, const int i_max) {
  const int lookedup_index = pscript_type__make__array__lookup(this, subtype, i_min, i_max); 
  if (lookedup_index >= 0) return lookedup_index; 
  const int type_i = pscript_type__make__standard_type__no_no(this, PSCRIPT_TYPE__ARRAY); 
  this -> cell__array[type_i].array_subtype = subtype; 
  this -> cell__array[type_i].array_index_min = i_min; 
  this -> cell__array[type_i].array_index_max = i_max; 
  return type_i; 
}; 



#if 1
int pscript_type__get__user_defined_type(const pscript_type_env_t * this, const int type_name) {
  const int lookedup_index = pscript_type__name__lookup(this, type_name); 
  if (lookedup_index < 0) return -1; 
  return this -> type__cell[lookedup_index]; 
}; 

static int pscript_type__make__user_defined_type__lookup(const pscript_type_env_t * this, const int udf_name) { 
  const char * name_cstr = pscript_string__get_cstr(this -> string_env, udf_name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower);
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core               != PSCRIPT_TYPE__USER) continue; 
    if (this -> cell__array[i].user_defined_type__name != name_lower) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__user_defined_type(pscript_type_env_t * this, const int udf_name, const int subtype) { 
  const char * udf_name_cstr = pscript_string__get_cstr(this -> string_env, udf_name);
  do { 
    const int env_lookedup_index = pscript_type__name__lookup(this, udf_name); 
    if (env_lookedup_index >= 0) { 
      const int cell_lookedup_index = this -> type__cell[env_lookedup_index]; 
      if (cell_lookedup_index < 0) { break; }; 
      const int cell_lookedup_typecore = pscript_type__get_core_type(this, cell_lookedup_index); 
      if (cell_lookedup_typecore != PSCRIPT_TYPE__USER) {
	const char * cell_lookedup_cstr = pscript_type__get_cstr(this, cell_lookedup_index); 
	pmesserr("The name '%s' is already bound  (as an alias) to the type [ %d ] %s. " "\n", udf_name_cstr, cell_lookedup_index, cell_lookedup_cstr); 
	return -1; 
      }; 
      
      const int cell_lookedup_subtype = this -> cell__array[cell_lookedup_index].user_defined_type__subtype; 
      if (cell_lookedup_subtype != subtype) { 
	const char * cell_lookedup_subtype_cstr = pscript_type__get_cstr(this, cell_lookedup_subtype); 
	const char * subtype_cstr = pscript_type__get_cstr(this, subtype); 
	pmesserr("The name '%s' is already defined as the type [ %d ] %s (different from  [ %d ] %s). " "\n", udf_name_cstr, cell_lookedup_subtype, cell_lookedup_subtype_cstr, subtype, subtype_cstr); 
	return -1; 
      }; 
      
      const char * subtype_cstr = pscript_type__get_cstr(this, subtype); 
      pmessage("The name '%s' is already defined as the same type [ %d ] %s -- ignoring that harmless second definition. " "\n", udf_name_cstr, subtype, subtype_cstr); 
      //return env_lookedup_index; 
      return cell_lookedup_index; 
  }; 
  
    const int cell_lookedup_index = pscript_type__make__user_defined_type__lookup(this, udf_name); 
    if (cell_lookedup_index >= 0) { 
      const int cell_lookedup_subtype = this -> cell__array[cell_lookedup_index].user_defined_type__subtype; 
      if (cell_lookedup_subtype != subtype) { 
	const char * cell_lookedup_subtype_cstr = pscript_type__get_cstr(this, cell_lookedup_subtype); 
	const char * subtype_cstr = pscript_type__get_cstr(this, subtype); 
	pmesserr("The name '%s' is already defined as the type [ %d ] %s (different from [ %d ] %s). " "\n", udf_name_cstr, cell_lookedup_subtype, cell_lookedup_subtype_cstr, subtype, subtype_cstr); 
	return -1; 
      }; 
      
      const char * subtype_cstr = pscript_type__get_cstr(this, subtype); 
      pmessage("The name '%s' is already defined as the same type [ %d ] %s -- ignoring that harmless second definition. " "\n", udf_name_cstr, subtype, subtype_cstr); 
      
      return cell_lookedup_index; 
    }; 
  } while (false); 

  const char * name_cstr = pscript_string__get_cstr(this -> string_env, udf_name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower); 
  const int type_i = pscript_type__make__standard_type__no_no(this, PSCRIPT_TYPE__USER); 
  this -> cell__array[type_i].user_defined_type__name = name_lower; 
  this -> cell__array[type_i].user_defined_type__subtype = subtype; 
  this -> cell__string[type_i] = udf_name; 
  return type_i; 
}; 

int pscript_type__user_defined_type__get_subtype(pscript_type_env_t * this, const int type_i) {
  if (PSCRIPT_TYPE__USER != this -> cell__array[type_i].type_core) { 
    const char * type_cstr = pscript_type__get_cstr(this, type_i); 
    pmesserr("The type [ %d ] %s is not an user-defined type. " "\n", type_i, type_cstr); 
    return -2; 
  }; 
  return this -> cell__array[type_i].user_defined_type__subtype; 
}; 

int pscript_type__user_defined_type__get_name(pscript_type_env_t * this, const int type_i) {
  if (PSCRIPT_TYPE__USER != this -> cell__array[type_i].type_core) { 
    const char * type_cstr = pscript_type__get_cstr(this, type_i); 
    pmesserr("The type [ %d ] %s is not an user-defined type. " "\n", type_i, type_cstr); 
    return -2; 
  }; 
  return this -> cell__array[type_i].user_defined_type__name; 
}; 

const char * pscript_type__user_defined_type__get_name_cstr(pscript_type_env_t * this, const int type_i) {
  const int name_pstr = pscript_type__user_defined_type__get_name(this, type_i); 
  return pscript_string__get_cstr(this -> string_env, name_pstr); 
}; 


void pscript_type__name__define_udf__for_nb_names_on_top(pscript_type_env_t * this, const int ident_nb, const int subtype) {
  assert(ident_nb <= this -> type__nb); 
  for (int i = 0; i < ident_nb; i++) {
    const int index = this -> type__nb - i - 1; 
    assert(-1 == this -> type__cell[index]); 
    const int udf_name_i = this -> type__name[index]; 
    const int udf_cell_i = pscript_type__make__user_defined_type(this, udf_name_i, subtype); 
    this -> type__cell[index] = udf_cell_i; 
  };   
}; 


#else 
static int pscript_type__make__user_defined_type__lookup(const pscript_type_env_t * this, const int type_name) {
  for (int i = 0; i < this -> cell__nb; i++) {
    if (this -> cell__array[i].type_core               != PSCRIPT_TYPE__USER) continue; 
    if (this -> cell__array[i].user_defined_type__name != type_name) continue; 
    return i; 
  }; 
  return -1; 
}; 

int pscript_type__make__user_defined_type(pscript_type_env_t * this, const int type_name) {
  const int lookedup_index = pscript_type__make__user_defined_type__lookup(this, type_name); 
  if (lookedup_index >= 0) return lookedup_index; 
  const int type_i = pscript_type__make__standard_type__no_no(this, PSCRIPT_TYPE__USER); 
  this -> cell__array[type_i].user_defined_type__name = type_name; 
  this -> cell__array[type_i].user_defined_type__type_i = pscript_type__name__lookup(this, type_name); 
  this -> cell__string[type_i] = type_name; 
  return type_i; 
}; 
#endif 

int pscript_type__make__record__empty(pscript_type_env_t * this) {
  const int type_i = pscript_type__make__standard_type__no_no(this, PSCRIPT_TYPE__RECORD); 
  return type_i; 
}; 
 
int pscript_type__make__record__push_field(pscript_type_env_t * this, int record_i, const int field_name, const int field_type) {
  const int field_nb = this -> cell__array[record_i].record_field_nb; 
  assert(field_nb < pscript_type__record_field_size); 
  const int index = field_nb; 
  const int previous_offset = field_nb == 0 ? 0 : this -> cell__array[record_i].record_field_offset[index-1]; 
  const int previous_sizeof = field_nb == 0 ? 0 : pscript_type__sizeof(this, this -> cell__array[record_i].record_field_type[index-1]); 
  this -> cell__array[record_i].record_field_name[index] = field_name; 
  this -> cell__array[record_i].record_field_type[index] = field_type; 
  this -> cell__array[record_i].record_field_offset[index] = previous_offset + previous_sizeof; 
  this -> cell__array[record_i].record_field_nb++; 
  return record_i; 
}; 
 
 
int pscript_type__record__get_field_nb    (const pscript_type_env_t * this, const int record_i) {
  return this -> cell__array[record_i].record_field_nb; 
}; 

int pscript_type__record__get_field_index (const pscript_type_env_t * this, const int record_i, const int field_ident_pstr) {
  assert(record_i >= 0); 
  assert(record_i < this -> cell__nb); 
  assert(PSCRIPT_TYPE__RECORD == this -> cell__array[record_i].type_core); 
  const int field_nb = this -> cell__array[record_i].record_field_nb; 
  for (int i = 0; i < field_nb; i++) {
    if (this -> cell__array[record_i].record_field_name[i] == field_ident_pstr) return i; 
  }; 
  return -1; 
}; 
 
int pscript_type__record__get_field_name(const pscript_type_env_t * this, const int record_i, const int field_i) {
  return this -> cell__array[record_i].record_field_name[field_i]; 
}; 
 
int pscript_type__record__get_field_type(const pscript_type_env_t * this, const int record_i, const int field_i) {
  return this -> cell__array[record_i].record_field_type[field_i]; 
}; 
 
int pscript_type__record__get_field_offset(const pscript_type_env_t * this, const int record_i, const int field_i) {
  return this -> cell__array[record_i].record_field_offset[field_i]; 
}; 
 
int pscript_type__record__get_field_sizeof(pscript_type_env_t * this, const int record_i, const int field_i) {
  return pscript_type__sizeof(this, this -> cell__array[record_i].record_field_type[field_i]); 
}; 







int pscript_type__sizeof__pointer(pscript_type_env_t * this) {
  //return pscript_type__sizeof(this, standard_types__nil); 
  //return pscript_type__sizeof(this, standard_types__null); 
  //return sizeof(int); 
  return pscript_type__sizeof(this, standard_types__nil_ptr); 
}; 

int pscript_type__sizeof__proxy(pscript_type_env_t * this) {
  //return pscript_type__sizeof(this, standard_types__nil); 
  //return pscript_type__sizeof(this, standard_types__null); 
  //return sizeof(int); 
  return pscript_type__sizeof(this, standard_types__nil_ptr); 
}; 

int pscript_type__sizeof__proxy_bytecode(pscript_type_env_t * this) {
  //return pscript_type__sizeof(this, standard_types__nil); 
  //return pscript_type__sizeof(this, standard_types__null); 
  return sizeof(int); 
  //return pscript_type__sizeof(this, standard_types__nil_ptr); 
}; 

//static int pscript_type__sizeof__transparent_proxy_huh(pscript_type_env_t * this, const int type_i, const int transparent_proxy_huh) {
int pscript_type__sizeof(pscript_type_env_t * this, const int type_i) { 
  //if (not(transparent_proxy_huh)) 
  if (this -> cell__sizeof[type_i] >= 0) { return this -> cell__sizeof[type_i]; }; 
  
  int bytesize = -1; 
  switch (this -> cell__array[type_i].type_core) {
  case PSCRIPT_TYPE__VOID: bytesize = 0; break; 
  case PSCRIPT_TYPE__BOOL: bytesize = 1; break; 
  case PSCRIPT_TYPE__INTEGER: bytesize = sizeof(int); break; 
  case PSCRIPT_TYPE__INT_RANGE: bytesize = sizeof(int); break; 
  case PSCRIPT_TYPE__FLOAT: bytesize = sizeof(float); break; 
  case PSCRIPT_TYPE__CHAR: bytesize = sizeof(char); break; 
  case PSCRIPT_TYPE__STRING:   bytesize = sizeof(int_pscript_string_t); break; 
  case PSCRIPT_TYPE__NIL_LIST: bytesize = 0; break; 
  case PSCRIPT_TYPE__NIL_PTR:  bytesize = sizeof(int_pscript_mem_t); break;  
  case PSCRIPT_TYPE__POINTER:  bytesize = sizeof(int_pscript_mem_t); break; //bytesize = pscript_type__sizeof(this, this -> pointer); break; 
  case PSCRIPT_TYPE__PROXY_BYTECODE:  bytesize = sizeof(int); break;  
  case PSCRIPT_TYPE__PROXY: do { 
#if 1 
	bytesize = sizeof(int_pscript_mem_t); 
#else 
      if (not(transparent_proxy_huh)) { 
	bytesize = sizeof(int); 
      } 
      else { 
	//dputs_array(stderr__fileno, "TYPE_I: ", pscript_type__get_cstr(this, type_i), "\n"); 
	const int subtype_i = pscript_type__proxy__get_subtype(this, type_i); 
	bytesize = pscript_type__sizeof__transparent_proxy_huh(this, subtype_i, transparent_proxy_huh); 
      }; 
#endif 
    } while (false); break; 
  case PSCRIPT_TYPE__ARRAY: { 
    const int subtype_sizeof = pscript_type__sizeof(this, this -> cell__array[type_i].array_subtype); 
    const int nb_cells       = this -> cell__array[type_i].array_index_max - this -> cell__array[type_i].array_index_min + 1; 
    bytesize = subtype_sizeof * nb_cells; 
  }; 
    break; 
  case PSCRIPT_TYPE__RECORD: {
    bytesize = 0; 
    for (int i = 0; i < this -> cell__array[type_i].record_field_nb; i++) {
      bytesize += pscript_type__sizeof(this, this -> cell__array[type_i].record_field_type[i]); 
    }; 
  }; 
    break; 
#if 1
  case PSCRIPT_TYPE__FUNPROC: 
    //bytesize = pscript_type__sizeof(this -> fun_retval); 
    bytesize = sizeof(int); break; 
#else 
  case PSCRIPT_TYPE__FUN: 
    //bytesize = pscript_type__sizeof(this -> fun_retval); 
    bytesize = sizeof(int); break; 
  case PSCRIPT_TYPE__PROC: 
#if 0 
    for (int i = 0; i < this -> proc_or_fun__arg_nb; i++) {
      bytesize += pscript_type__sizeof(this, this -> cell__array[type_i].proc_or_fun__arg[i]); 
    }; 
    break;
#endif 
    bytesize = sizeof(int); break; 
#endif 
  case PSCRIPT_TYPE__USER: { 
    const int udf_type_i = this -> cell__array[type_i].user_defined_type__subtype; 
    assert(udf_type_i >= 0); 
    bytesize = pscript_type__sizeof(this, udf_type_i); 
    }; 
    break; 
  default: assert(false); bytesize = -1; break; 
  }; 

  //if (not(transparent_proxy_huh)) 
  this -> cell__sizeof[type_i] = bytesize; 
  return bytesize;   
}; 

int pscript_type__sizeof__(pscript_type_env_t * this, const int type_i) { 
  return 0; //pscript_type__sizeof__transparent_proxy_huh(this, type_i, false); 
}; 

int pscript_type__sizeof__transparent_proxy(pscript_type_env_t * this, const int type_i) { 
  return 0; //pscript_type__sizeof__transparent_proxy_huh(this, type_i, true); 
}; 

 
int pscript_type__transparent_proxy__get_type(pscript_type_env_t * this, const int type_i) { 
  int subtype_i = type_i; 
  for (;;) { 
    const int subtype_core = pscript_type__get_core_type(this, subtype_i); 
    if (PSCRIPT_TYPE__PROXY != subtype_core) { return subtype_i; }; 
    subtype_i = pscript_type__proxy__get_subtype(this, subtype_i); 
  }; 
}; 
 
int pscript_type__transparent_proxy__get_core_type(pscript_type_env_t * this, const int type_i) { 
  const int subtype_i = pscript_type__transparent_proxy__get_type(this, type_i); 
  const int subtype_core = pscript_type__get_core_type(this, subtype_i); 
  return subtype_core; 
}; 
 
int pscript_type__transparent_proxy__get_sizeof(pscript_type_env_t * this, const int type_i) { 
  const int subtype_i = pscript_type__transparent_proxy__get_type(this, type_i); 
  const int subtype_sizeof = pscript_type__sizeof(this, subtype_i); 
  return subtype_sizeof; 
}; 
 



 
int pscript_type__string(pscript_type_env_t * this, const int type_i) {
  if (this -> cell__string[type_i] >= 0) return this -> cell__string[type_i]; 
  
  int type_string = -1; 
  const int type_core = this -> cell__array[type_i].type_core; 
  switch (type_core) {
  case PSCRIPT_TYPE__VOID: type_string = pscript_string__push(this -> string_env, "VOID"); break; 
  case PSCRIPT_TYPE__BOOL: type_string = pscript_string__push(this -> string_env, "BOOL"); break; 
  case PSCRIPT_TYPE__INTEGER: type_string = pscript_string__push(this -> string_env, "INTEGER"); break; 
  case PSCRIPT_TYPE__INT_RANGE: { 
    const char * int_min_cstr = int_string__stack(this -> cell__array[type_i].int_range__min); 
    //const int int_min_pstr = pscript_string__push(this -> string_env, int_min_cstr); 
    const char * int_max_cstr = int_string__stack(this -> cell__array[type_i].int_range__max); 
    //const int int_max_pstr = pscript_string__push(this -> string_env, int_max_cstr); 
    const char * int_range_cstr = strconcat__stack(int_min_cstr, "..", int_max_cstr); 
    type_string = pscript_string__push(this -> string_env, int_range_cstr); 
    pmessage("%s: " "int_range_cstr = %s" "\n", __func__, int_range_cstr); 
  };
    break; 
  case PSCRIPT_TYPE__FLOAT: type_string = pscript_string__push(this -> string_env, "FLOAT"); break; 
  case PSCRIPT_TYPE__CHAR: type_string = pscript_string__push(this -> string_env, "CHAR"); break; 
  case PSCRIPT_TYPE__STRING: type_string = pscript_string__push(this -> string_env, "STRING"); break; 
  case PSCRIPT_TYPE__NIL_LIST:  type_string = pscript_string__push(this -> string_env, "NIL"); break; 
  case PSCRIPT_TYPE__NIL_PTR: type_string = pscript_string__push(this -> string_env, "NULL"); break; 
  case PSCRIPT_TYPE__POINTER: {
    const int subtype_string = pscript_type__string(this, this -> cell__array[type_i].pointer_subtype); 
    const char * subtype_cstr = pscript_string__get_cstr(this -> string_env, subtype_string); 
    const char * pointer_cstr = strconcat__stack("POINTER TO ", subtype_cstr); 
    type_string = pscript_string__push(this -> string_env, pointer_cstr); 
  }; 
    break; 
  case PSCRIPT_TYPE__PROXY_BYTECODE: 
  case PSCRIPT_TYPE__PROXY: {
    const int subtype_string = pscript_type__string(this, this -> cell__array[type_i].proxy_subtype); 
    const char * subtype_cstr = pscript_string__get_cstr(this -> string_env, subtype_string); 
    const char * proxy_cstr = strconcat__stack("PROXY TO ", subtype_cstr); 
    type_string = pscript_string__push(this -> string_env, proxy_cstr); 
  }; 
    break; 
  case PSCRIPT_TYPE__ARRAY: {
    const int subtype_string = pscript_type__string(this, this -> cell__array[type_i].array_subtype); 
    const char * int_min_cstr = int_string__stack(this -> cell__array[type_i].array_index_min); 
    //const int int_min_pstr = pscript_string__push(this -> string_env, int_min_cstr); 
    const char * int_max_cstr = int_string__stack(this -> cell__array[type_i].array_index_max); 
    //const int int_max_pstr = pscript_string__push(this -> string_env, int_max_cstr); 
    const char * subtype_cstr = pscript_string__get_cstr(this -> string_env, subtype_string); 
    const char * array_cstr = strconcat__stack("ARRAY[ ", int_min_cstr, "..", int_max_cstr, "] OF ", subtype_cstr); 
    type_string = pscript_string__push(this -> string_env, array_cstr); 
  }; 
    break; 
  case PSCRIPT_TYPE__RECORD: {
    const int field_nb = this -> cell__array[type_i].record_field_nb; 
    const char * subtype_string[field_nb]; 
    for (int i = 0; i < field_nb; i++) {
      const int field_name = this -> cell__array[type_i].record_field_name[i]; 
      const char * field_name_cstr = pscript_string__get_cstr(this -> string_env, field_name); 
      const int field_type = this -> cell__array[type_i].record_field_type[i]; 
      const int field_type_string = pscript_type__string(this, field_type); 
      const char * field_type_cstr = pscript_string__get_cstr(this -> string_env, field_type_string); 
      subtype_string[i] = strconcat__stack(" ", field_name_cstr, ": ", field_type_cstr, ";"); 
    }; 
    const char * fields_cstr = strconcat__stack_nb(field_nb, subtype_string); 
    const char * record_cstr = strconcat__stack("RECORD[", fields_cstr, " ]"); 
    type_string = pscript_string__push(this -> string_env, record_cstr); 
  }; 
    break; 
  //case PSCRIPT_TYPE__FUN: 
  case PSCRIPT_TYPE__FUNPROC: {
    const int arg_nb = this -> cell__array[type_i].funproc__arg_nb; 
    const char * subtype_string[arg_nb]; 
    for (int i = 0; i < arg_nb; i++) {
      const int    arg_name        = this -> cell__array[type_i].funproc__arg_name[i]; 
      const char * arg_name_cstr   = pscript_string__get_cstr(this -> string_env, arg_name); 
      const int    arg_byref       = this -> cell__array[type_i].funproc__arg_byref[i]; 
      const char * arg_byref_cstr  = arg_byref ? "var ":""; 
      const int    arg_type        = this -> cell__array[type_i].funproc__arg_type[i]; 
      const int    arg_type_string = pscript_type__string(this, arg_type); 
      const char * arg_type_cstr   = pscript_string__get_cstr(this -> string_env, arg_type_string); 
      subtype_string[i] = strconcat__stack(" ", arg_byref_cstr, arg_name_cstr, ": ", arg_type_cstr, ";"); 
    }; 
    const char * args_cstr = strconcat__stack_nb(arg_nb, subtype_string); 
    const char * funproc_cstr = NULL; 
    const int retval_type = this -> cell__array[type_i].funproc_retval; 
    //if (type_core == PSCRIPT_TYPE__FUN) { 
    if (retval_type > 0) { 
      const int retval_type_string = pscript_type__string(this, this -> cell__array[type_i].funproc_retval); 
      const char * retval_type_cstr = pscript_string__get_cstr(this -> string_env, retval_type_string); 
      funproc_cstr = strconcat__stack("function(", args_cstr, "): ", retval_type_cstr); 
    } 
    else {
      funproc_cstr = strconcat__stack("procedure(", args_cstr, ")"); 
    };
    type_string = pscript_string__push(this -> string_env, funproc_cstr); 
  }; 
    break; 
  case PSCRIPT_TYPE__FFIPROC: {
    const int arg_nb = this -> cell__array[type_i].ffiproc__arg_nb; 
    const char * subtype_string[arg_nb]; 
    for (int i = 0; i < arg_nb; i++) {
      const int    arg_name        = this -> cell__array[type_i].ffiproc__arg_name[i]; 
      const char * arg_name_cstr   = pscript_string__get_cstr(this -> string_env, arg_name); 
      const int    arg_type        = this -> cell__array[type_i].ffiproc__arg_type[i]; 
      const int    arg_type_string = pscript_type__string(this, arg_type); 
      const char * arg_type_cstr   = pscript_string__get_cstr(this -> string_env, arg_type_string); 
      subtype_string[i] = strconcat__stack(" ", arg_name_cstr, ": ", arg_type_cstr, ";"); 
    }; 
    const char * args_cstr    = strconcat__stack_nb(arg_nb, subtype_string); 
    const char * ffiproc_cstr = NULL; 
    const int    retval_type  = this -> cell__array[type_i].ffiproc_retval; 
    if (retval_type > 0) { 
      const int    retval_type_pstr = pscript_type__string(this, this -> cell__array[type_i].ffiproc_retval); 
      const char * retval_type_cstr = pscript_string__get_cstr(this -> string_env, retval_type_pstr); 
      ffiproc_cstr = strconcat__stack("ffiproc(", args_cstr, "): ", retval_type_cstr); 
    } 
    else {
      ffiproc_cstr = strconcat__stack("ffiproc(", args_cstr, ")"); 
    };
    type_string = pscript_string__push(this -> string_env, ffiproc_cstr); 
  }; 
    break; 
  case PSCRIPT_TYPE__USER: { 
#if 1
    const int udf_name = this -> cell__array[type_i].user_defined_type__name; 
    type_string = udf_name; 
#else 
    const int udf_i = this -> cell__array[type_i].user_defined_type; 
    const int udf_name = this -> type__name[udf_i]; 
    type_string = udf_name; 
#endif 
  };
    break; 
  default: assert(false); type_string = -1; break; 
  }; 

  this -> cell__string[type_i] = type_string; 
  return type_string;   
}; 

//extern int pscript_type__string_expanded(pscript_type_env_t * this, const int type_i); 

const char * pscript_type__get_cstr(pscript_type_env_t * this, const int type_i) {
  const int type_pstr = pscript_type__string(this, type_i); 
  return pscript_string__get_cstr(this -> string_env, type_pstr); 
}; 















int pscript_type__pointer__get_subtype(const pscript_type_env_t * this, const int type_i) {
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__POINTER); 
  return this -> cell__array[type_i].pointer_subtype; 
}; 

int pscript_type__proxy__get_subtype(const pscript_type_env_t * this, const int type_i) {
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__PROXY); 
  return this -> cell__array[type_i].proxy_subtype; 
}; 

int pscript_type__proxy_bytecode__get_subtype(const pscript_type_env_t * this, const int type_i) {
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__PROXY_BYTECODE); 
  return this -> cell__array[type_i].proxy_subtype; 
}; 

#if 0 
int pscript_type__variant__get_subtype(const pscript_type_env_t * this, const int type_i) {
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__VARIANT); 
  return this -> cell__array[type_i].variant_subtype; 
}; 
#endif 







int pscript_type__array__get_subtype(const pscript_type_env_t * this, const int type_i) { 
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__ARRAY); 
  return this -> cell__array[type_i].array_subtype; 
}; 

int pscript_type__array__get_i_min  (const pscript_type_env_t * this, const int type_i) {
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__ARRAY); 
  return this -> cell__array[type_i].array_index_min; 
}; 
 
int pscript_type__array__get_i_max  (const pscript_type_env_t * this, const int type_i) { 
  assert(type_i < this -> cell__nb); 
  const int core_type = this -> cell__array[type_i].type_core; 
  assert(core_type == PSCRIPT_TYPE__ARRAY); 
  return this -> cell__array[type_i].array_index_max; 
}; 
 















int pscript_type__methode__lookup(pscript_type_env_t * this, const int type_i, const int name) {
  const char * name_cstr = pscript_string__get_cstr(this -> string_env, name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower);
  //pmessage("Looking up for a method '%s' in the type [ %d ] '%s'." "\n", name_cstr, type_i, pscript_type__get_cstr(this, type_i)); 
  //pmessage("METHOD_NB: %d" "\n", this -> cell__array[type_i].methode__nb); 
  for (int i = 0; i < this -> cell__array[type_i].methode__nb; i++) {
    //pmessage("method[%d] = %s" "\n", i, pscript_string__get_cstr(this -> string_env, this -> cell__array[type_i].methode__name[i])); 
    if (this -> cell__array[type_i].methode__name[i] == name_lower) return i; 
  }; 
  return -1; 
}; 

int pscript_type__methode__push_no_lookup(pscript_type_env_t * this, const int type_i, const int name, const int methode__type, const int funproc_i) { 
  assert(this -> type__nb < pscript_type_env__size); 
  const int index = this -> cell__array[type_i].methode__nb; 
  const char * name_cstr = pscript_string__get_cstr(this -> string_env, name); 
  const char * name_cstr_lower = cstring__tolower(name_cstr); 
  const int name_lower = pscript_string__push(this -> string_env, name_cstr_lower);
  this -> cell__array[type_i].methode__name[index] = name_lower; 
  this -> cell__array[type_i].methode__funproc[index] = funproc_i; 
  this -> cell__array[type_i].methode__type[index] = methode__type; 
  this -> cell__array[type_i].methode__nb ++; 
  return index; 
}; 

int pscript_type__methode__push_lookup(pscript_type_env_t * this, const int type_i, const int name, const int methode_type, const int funproc_i) { 
  const int lookedup_index = pscript_type__methode__lookup(this, type_i, name); 
  if (lookedup_index >= 0) {
    if (this -> cell__array[type_i].methode__funproc[lookedup_index] == funproc_i) return lookedup_index; 
    pmessage("The type [ %d ] '%s' already has a method named '%s'." "\n", type_i, pscript_type__get_cstr(this, type_i), pscript_string__get_cstr(this -> string_env, name)); 
    return -1; 
  }; 
  return pscript_type__methode__push_no_lookup(this, type_i, name, methode_type, funproc_i); 
}; 


int pscript_type__methode__get_nb(const pscript_type_env_t * this, const int type_i) {
  return this -> cell__array[type_i].methode__nb; 
}; 

int pscript_type__methode__get_name(const pscript_type_env_t * this, const int type_i, const int method_i) {
  return this -> cell__array[type_i].methode__name[method_i]; 
}; 

int pscript_type__methode__get_funproc(const pscript_type_env_t * this, const int type_i, const int method_i) {
  return this -> cell__array[type_i].methode__funproc[method_i]; 
}; 

int pscript_type__methode__get_ffiproc(const pscript_type_env_t * this, const int type_i, const int method_i) {
  return this -> cell__array[type_i].methode__funproc[method_i]; 
}; 

int pscript_type__methode__get_type(const pscript_type_env_t * this, const int type_i, const int method_i) {
  return this -> cell__array[type_i].methode__type[method_i]; 
}; 



































