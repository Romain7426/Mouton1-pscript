#include "pscript_global.h" 
#include "pscript_env.h" 

enum { pscript_env__size = (1 << 9) };  
struct pscript_env_t { 
  pscript_string_env_t * string_env; 
  int name[pscript_env__size]; 
  int type[pscript_env__size]; 
  int value[pscript_env__size]; 
  int value2[pscript_env__size]; // RL: Needed for LOCALVAR. ARG, RETVAL (local run-time environment + offset in that). 
  int nb; 
  
  //int current_funproc_i; 
}; 


static const char * pscript_env__type__cstr__array[] = {
  "VAR", 
  "VAR_LOCAL", 
  "ARG", 
  "RETVAL", 
  //"METHOD_OF", 
  "METHOD_THIS", 
  "CONST", 
  "FUNPROC", 
//  "MAINPROC", 
  "FFIPROC", 
  "LIBPROC", 
  NULL
}; 

enum { pscript_env__type__cstr__size = ARRAY_SIZE(pscript_env__type__cstr__array) }; 

static void assert_compile_time__ds3d632sad4gdss(void) {
  ASSERT_COMPILE(pscript_env__type__cstr__size == PSCRIPT_ENV__COUNT + 1); 
}; 
 
#if 0
static int pscript_env__type__cstr__pstr[pscript_env__type__cstr__size] = {}; 
#endif 


pscript_env_t * pscript_env__make(pscript_string_env_t * string_env) {
  MALLOC_BZERO(pscript_env_t,this); 
  this -> string_env = string_env; 
  this -> nb = 0; 
  //this -> current_funproc_i = -1; 
  return this;
}; 

void pscript_env__delete(pscript_env_t * this) {
  free(this);
};   


int  pscript_env__get_count(const pscript_env_t * this) { 
  return this -> nb; 
}; 
 

int pscript_env__push_no_lookup(pscript_env_t * this, const int nom, const int type, const int value) {
  assert(this -> nb < pscript_env__size); 
  const int index = this -> nb; 
  const char * nom_cstr = pscript_string__get_cstr(this -> string_env, nom); 
  const char * nom_cstr_lower = cstring__tolower(nom_cstr); 
  const int nom_lower = pscript_string__push(this -> string_env, nom_cstr_lower);
  this -> name[index] = nom_lower; 
  this -> type[index] = type; 
  this -> value[index] = value; 
  this -> nb++; 
  return index; 
};

int pscript_env__lookup(const pscript_env_t * this, const int nom) {
  const char * nom_cstr = pscript_string__get_cstr(this -> string_env, nom); 
  const char * nom_cstr_lower = cstring__tolower(nom_cstr); 
  const int nom_lower = pscript_string__push(this -> string_env, nom_cstr_lower);
  for (int i = 0; i < this -> nb; i++) {
    const int index = this -> nb - i - 1; 
    if (this -> name[index] == nom_lower) return index; 
  }; 
  return -1; 
}; 

void pscript_env__set_name(pscript_env_t * this, const int env_i, const int nom) {
  const char * nom_cstr = pscript_string__get_cstr(this -> string_env, nom); 
  const char * nom_cstr_lower = cstring__tolower(nom_cstr); 
  const int nom_lower = pscript_string__push(this -> string_env, nom_cstr_lower);
  this -> name[env_i] = nom_lower; 
}; 

int pscript_env__get_name(const pscript_env_t * this, const int env_i) {
  return this -> name[env_i]; 
}; 

int pscript_env__get_type(const pscript_env_t * this, const int env_i) {
  return this -> type[env_i]; 
}; 

int pscript_env__get_pscript_type(const pscript_env_t * this, const pscript_t * pscript_env, const int env_i) {
  int pscript_type = -1; 
  const int env_type  = this -> type[env_i]; 
  const int env_value = this -> value[env_i]; 
  //const int funproc_i = this -> current_funproc_i; 
  const int funproc_i = this -> value[env_i]; 
  switch (env_type) {
  default: assert(false); break; 
  case PSCRIPT_ENV__VAR:       do { pscript_type = pscript_var__get_type(pscript_env -> var_env, env_value); } while (false); break;  
  case PSCRIPT_ENV__VAR_LOCAL: do { pscript_type = pscript_funproc__get_localvar_type(pscript_env -> funproc_env, funproc_i, env_value); } while (false); break;  
  case PSCRIPT_ENV__ARG:       do { pscript_type = pscript_funproc__get_arg_type(pscript_env -> funproc_env, funproc_i, env_value); } while (false); break;  
  case PSCRIPT_ENV__RETVAL:    do { pscript_type = pscript_funproc__get_retval_type(pscript_env -> funproc_env, funproc_i); } while (false); break;  
  case PSCRIPT_ENV__METHOD_THIS: do { pscript_type = pscript_funproc__get_method_of__type(pscript_env -> funproc_env, funproc_i); } while (false); break;  
  case PSCRIPT_ENV__CONST:     do { pscript_type = pscript_const__get_type(pscript_env -> const_env, env_value); } while (false); break;  
  case PSCRIPT_ENV__FUNPROC:   do { pscript_type = pscript_funproc__get_type(pscript_env -> funproc_env, env_value); } while (false); break;  
  //case PSCRIPT_ENV__MAINPROC: do { } while (false); break;  
  //case PSCRIPT_ENV__FFIPROC:   do { } while (false); break;  
  //case PSCRIPT_ENV__LIBPROC:   do { } while (false); break;  
  }; 
  return pscript_type; 
}; 


int pscript_env__get_value(const pscript_env_t * this, const int env_i) {
  return this -> value[env_i]; 
}; 

int pscript_env__get_value2(const pscript_env_t * this, const int env_i) {
  return this -> value2[env_i]; 
}; 


int pscript_env__get_const_huh(const pscript_env_t * this, const int env_i) {
  return this -> type[env_i] == PSCRIPT_ENV__CONST; 
}; 

const char * pscript_env__type__get_cstr(const pscript_env_t * this, const int env_type) {
  return pscript_env__type__cstr__array[env_type]; 
}; 

#if 0  
int pscript_env__type__get_pstr(const pscript_env_t * this, const int env_type) {
}; 
#endif 




int pscript_env__get_offset(const pscript_env_t * this) {
  return this -> nb; 
}; 

void pscript_env__deallocate(pscript_env_t * this, const int env_offset) {
  this -> nb = env_offset; 
}; 

int pscript_env__push_const(pscript_env_t * this, const int nom_pstr, const int const_env_i) { 
  return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__CONST, const_env_i); 
}; 

int pscript_env__push_var(pscript_env_t * this, const int nom_pstr, const int var_env_i) { 
  return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__VAR, var_env_i); 
}; 

int pscript_env__push_proc(pscript_env_t * this, const int nom_pstr, const int funproc_i) { 
  return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__FUNPROC, funproc_i); 
}; 

#if 0
int pscript_env__push_methodof(pscript_env_t * this, const int some_value) { 
  const char * nom_cstr = "this";
  const int nom_pstr = pscript_string__push(this -> string_env, nom_cstr); 
  //pmesslog("Pushing method_of '%s' in the env (some_value: %d)" "\n", pscript_string__get_cstr(this -> string_env, nom_pstr), some_value); 
  return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__METHOD_OF, some_value); 
}; 
#else 
int pscript_env__push_method_this(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int some_value) { 
  //pmesslog("Pushing method_this '%s' in the env (funproc_i: %d - some_value: %d)" "\n", pscript_string__get_cstr(this -> string_env, nom_pstr), funproc_i, some_value); 
  const int env_i = pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__METHOD_THIS, 0); 
  this -> value [env_i] =  funproc_i; 
  this -> value2[env_i] = some_value; 
  return env_i; 
}; 
#endif 

int pscript_env__push_retval(pscript_env_t * this, const int funproc_i, const int retval_i) { 
  const char * nom_cstr = "retval";
  const int nom_pstr = pscript_string__push(this -> string_env, nom_cstr); 
  //pmesslog("Pushing retval '%s' in the env (funproc_i: %d - retval_i: %d)" "\n", pscript_string__get_cstr(this -> string_env, nom_pstr), funproc_i, retval_i); 
  //return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__RETVAL, retval_i); 
  const int env_i = pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__RETVAL, 0); 
  this -> value [env_i] = funproc_i; 
  this -> value2[env_i] =  retval_i; 
  return env_i; 
}; 

int pscript_env__push_arg(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int arg_i) { 
  //pmesslog("Pushing arg '%s' in the env (funproc_i: %d - arg_i: %d)" "\n", pscript_string__get_cstr(this -> string_env, nom_pstr), funproc_i, arg_i); 
  //return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__ARG, arg_i); 
  const int env_i = pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__ARG, 0); 
  this -> value [env_i] = funproc_i; 
  this -> value2[env_i] =     arg_i; 
  return env_i; 
}; 

int pscript_env__push_localvar(pscript_env_t * this, const int nom_pstr, const int funproc_i, const int localvar_i) { 
  //return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__VAR_LOCAL, localvar_i); 
  const int env_i = pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__VAR_LOCAL, 0); 
  this -> value [env_i] =  funproc_i; 
  this -> value2[env_i] = localvar_i; 
  return env_i; 
}; 
 
int pscript_env__push_ffi(pscript_env_t * this, const int nom_pstr, const int ffiproc_i) { 
  return pscript_env__push_no_lookup(this, nom_pstr, PSCRIPT_ENV__FFIPROC, ffiproc_i); 
}; 




#if 0 
void pscript_env__set_current_funproc(pscript_env_t * this, const int funproc_i) {
  this -> current_funproc_i = funproc_i; 
}; 

int pscript_env__get_current_funproc(const pscript_env_t * this) {
  return this -> current_funproc_i; 
}; 
#endif 



















#if 0 
static int pscript_env__nom[pscript_env__size] = { }; 
static int pscript_env__const_huh[pscript_env__size] = { }; 
static int pscript_env__mem_offset_or_const_indice[pscript__env__size] = { }; 
static int pscript_env__nb = 0; 


int pscript_env__push(const int nom, const int const_huh, const int mem_offset_or_const_indice) {
  assert(pscript_env__nb < pscript_env__size); 
  const int indice = pscript_env__nb; 
  pscript_env__nom[indice] = nom; 
  pscript_env__const_huh[indice] = const_huh; 
  pscript_env__mem_offset_or_const_indice[indice] = mem_offset_or_const_indice;   
  pscript_env__nb++; 
  return indice; 
}; 

int pscript_env__lookup(const int nom) {
  for (int i = 0; i < pscript_env__nb; i++) {
    if (nom == pscript_env__nom[i]) { return i; }; 
  }; 
  return -1; 
}; 

int pscript_env__get_nom(const int env_i) {
  return pscript_env__nom[env_i]; 
}; 

int pscript_env__get_const_huh(const int env_i) {
  return pscript_env__const_huh[env_i]; 
}; 

int pscript_env__get_mem_offset(const int env_i) {
  return pscript_env__mem_offset_or_const_indice[env_i]; 
}; 

int pscript_env__get_const_indice(const int env_i) {
  return pscript_env__mem_offset_or_const_indice[env_i]; 
}; 


int pscript_env__set_name(const int name); 
int pscript_env__set_type_all__nocopy(pscript_type_t * arg_type); 





enum               {   pscript_funproc__size = (1 << 9) }; 
enum               {   pscript_funproc__arg__size = (1 << 9) }; 
static           int   pscript_funproc__name[pscript_funproc__size] = { }; 
static           int   pscript_funproc__env_offset[pscript_funproc__size] = { }; 
static           int   pscript_funproc__argc[pscript_funproc__size] = { }; 
static           int   pscript_funproc__argv_name[pscript_funproc__size][pscript_funproc__arg__size] = { }; 
static           int   pscript_funproc__argv_byref_huh[pscript_funproc__size][pscript_funproc__arg__size] = { }; 
static pscript_type_t * pscript_funproc__argv_type[pscript_funproc__size][pscript_funproc__arg__size] = { }; 
static pscript_type_t * pscript_funproc__retval_type[pscript_funproc__size] = { }; 
static           int   pscript_funproc__localvar_name[pscript_funproc__size][pscript_funproc__arg__size] = { }; 
static pscript_type_t * pscript_funproc__localvar_type[pscript_funproc__size][pscript_funproc__arg__size] = { }; 
static           int   pscript_funproc__code[pscript_funproc__size] = { }; 

int pscript_funproc__push(const int name, const int env_offset, const int argc, const int arg_byref[], const int arg_name[], const pscript_type_t * arg_type[], const pscript_type_t * retval_type); 

int pscript_funproc__push_empty(void); 
//int pscript_funproc__push_arg__nocopy(const int funproc_i, const int name, const int byref_huh, pscript_type_t * arg_type); 
int pscript_funproc__set_arg__nocopy(const int name, const int byref_huh, pscript_type_t * arg_type); 
int pscript_funproc__set_name(const int name); 
int pscript_funproc__set_retval_type__nocopy(pscript_type_t * retval_type); 

int pscript_funproc__set_code(const int code); 
int pscript_funproc__set_localvar__type_all__nocopy(pscript_type_t * arg_type); 
int pscript_funproc__set_localvar__name(const int name); 
#endif 















#if 1 
static void pscript_env__print_one_env(const int filedes, const pscript_env_t * this, const pscript_string_env_t * string_env, const int env_i) {
  //const int filedes = yypscriptout == NULL ? stderr : fileno(yypscriptout); 
  //const int filedes = stderr; 
  
  const char * env_i__str            = int_string__stack(env_i); 
  const int    env_i__str__spaces_nb = MAX(0, 4 - strlen(env_i__str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, env_i__str__spaces_nb); 
  dputs(filedes, env_i__str); 
  dputs(filedes, ": "); 
  
  const int    env_name_pstr            = this -> name[env_i]; 
  const char * env_name_cstr            = pscript_string__get_cstr(string_env, env_name_pstr); 
  const int    env_name_cstr__nb_spaces = MAX(0, 17 - strlen(env_name_cstr)); 
  dputs(filedes, env_name_cstr); 
  dput_spaces(filedes, env_name_cstr__nb_spaces); 
  dputs(filedes, ": "); 
  
  const char * env_type__str            = int_string__stack(this -> type[env_i]); 
  const int    env_type__str__spaces_nb = MAX(0, 4 - strlen(env_type__str)); 
  dputs(filedes, "Type: "); 
  dput_spaces(filedes, env_type__str__spaces_nb); 
  dputs(filedes, env_type__str); 
  dputs(filedes, ": "); 
  
  const char * env_value__str            = int_string__stack(this -> value[env_i]); 
  const int    env_value__str__spaces_nb = MAX(0, 4 - strlen(env_value__str)); 
  dputs(filedes, "Value: "); 
  dput_spaces(filedes, env_value__str__spaces_nb); 
  dputs(filedes, env_value__str); 
  dputs(filedes, ": "); 
}; 

static void pscript_env__print_stats__base(const int filedes, const pscript_env_t * this, const int base) {
  //const int filedes = fileno(yypscriptout); 
  //const int filedes = yypscriptout == NULL ? stderr : fileno(yypscriptout); 
  //const int filedes = stderr; 
  dputs(filedes, "ENV_STACK("); 
  dputn(filedes, this -> nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void pscript_env__print_stats(const int filedes, const pscript_env_t * this) {
  pscript_env__print_stats__base(filedes, this, pscript_env__size); 
}; 

void pscript_env__print_nb(const int filedes, const pscript_env_t * this, const pscript_string_env_t * string_env, const int env_nb) {
  //const int filedes = fileno(yypscriptout); 
  //const int filedes = stderr; 
  //const int filedes = yypscriptout == NULL ? stderr : fileno(yypscriptout); 
  dputs(filedes, "ENV_STACK("); 
  dputn(filedes, this -> nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_env__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> nb)) / ((double) pscript_env__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> nb,env_nb); 
  for (int i = 0; i < nb; i++) { 
    dputs(filedes, "  "); 
    pscript_env__print_one_env(filedes, this, string_env, i); 
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void pscript_env__print_all(const int filedes, const pscript_env_t * this, const pscript_string_env_t * string_env) {
  pscript_env__print_nb(filedes, this, string_env, this -> nb); 
}; 

#endif 

