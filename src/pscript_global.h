#ifndef PSCRIPT_GLOBAL_H
#define PSCRIPT_GLOBAL_H

#include "lib__01__pre_header.ci" 

#include <stdlib.h>
#include <stdint.h> // int8_t, etc., intmax_t , uintmax_t, 
#include <inttypes.h> // uint8_t, ..., uintmax_t i = UINTMAX_MAX; // this type always exists 
#include <stddef.h> // offsetof(type, member-designator) 
#include <stdio.h> // remove, int rename(const char *old, const char *new); , 
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <math.h>     // cosf, ..., int signbit(real-floating x); , isnormal(neither zero, subnormal, infinite, nor NaN)., int isnan(real-floating x); , int isinf(real-floating x); int isfinite(real-floating x); 
#include <ctype.h>    // tolower, toupper
#include <assert.h>  // dépend de la valeur de la macro NDEBUG 
#include <complex.h>  // types «complex», «double complex», «long double complex», «float complex» 
#include <ctype.h> 
#include <errno.h> 
#include <float.h> // limits 
#include <iso646.h>  // Alternative spellings: and &&, xor ^, etc. 
#include <limits.h> 
#include <setjmp.h> 
#include <signal.h> 
#include <time.h> // clock & time --- 
#include <wchar.h> 
#include <wctype.h> 
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <langinfo.h> 

#ifdef STDBOOL_H_EXISTS
#  include <stdbool.h> // define a «bool» type which extends to «_Bool»
#endif 

#ifdef FENV_H_EXISTS
#  include <fenv.h> 
#endif

#ifdef SYS_FENV_H_EXISTS
#  include <sys/fenv.h> 
#endif

#ifdef LANGINFO_H_EXISTS
#  include <langinfo.h> 
#endif 

#ifdef TGMATH_H_EXISTS
#  include <tgmath.h> 
#endif

#ifdef CTIME_R_IS_NEEDED_AND_IS_NOT_PROVIDED
#ifndef ctime_r
#  define ctime_r(a,b) ((b == NULL ? 0 : strcpy(b, "FAILED ")), *a = time(NULL), ctime(a))
#endif
#endif
 
#include <editline/readline.h>


#include "lib__02__header.ci" 
#include "lib__03.ci"
#include "lib__04__string_stack.ci"
#include "lib__05__error_state.ci"
#include "lib__06__exception.ci"

#define MALLOC_THIS(ctype_t,this) ctype_t * this = NULL; this = (ctype_t *) malloc(sizeof(*this)); 
#define BZERO_THIS(this) bzero(this, sizeof(*this)); 
#define MALLOC_BZERO(ctype_t,this) ctype_t * this = NULL; this = (ctype_t *) malloc(sizeof(*this)); bzero(this, sizeof(*this)); 
#define ASSIGN_METHOD(ctype_t,this,method_name) this -> method_name = glue3(ctype_t,__,method_name); 

extern char * strcopy(const char * str); 

#define BIGLIB_STRING(s) BIGLIB_STRINGX(s)
#define BIGLIB_STRINGX(s) #s

#include "pscript.h" 
#include "pscript_inttype.h" 

extern FILE * yypscriptin;
extern FILE * yypscriptout;
extern void yypscriptlexerinit(const int srcfile); 
extern void yypscriptrestart(FILE * new_file); 
extern pscript_t * yypscript_global_objet_being_filled_up_while_parsing; 
extern int yypscriptparse(void);

extern int pscript_get_nb_lignes(void);
extern int pscript_get_nb_chars(void);
extern int pscript_get_nb_chars_tot(void);
extern int pscript_get_yyleng(void);
extern char * pscript_get_yytext(void);
extern const char * pscript_parser__token_type__get_cstr(const int token_type_i); 


#if 1
#define pmess(...)    PScript_PrintfStdlog(yypscript_global_objet_being_filled_up_while_parsing, __VA_ARGS__)
#define pmesserr(...) PScript_PrintfStderr(yypscript_global_objet_being_filled_up_while_parsing, __VA_ARGS__)
#elif 0
#define pmess(...)    PScript_PrintfStdlog(yypscript_global_objet_being_filled_up_while_parsing, __VA_ARGS__)
#define pmesserr(...) PScript_PrintfStderr(yypscript_global_objet_being_filled_up_while_parsing, __VA_ARGS__)
#elif 0
#define pmess    PScript_WriteStdlog
#define pmesserr PScript_PrintfStderr
#else
#define pmess(...)							\
  if (yypscriptout != NULL) fprintf(yypscriptout, "PSCRIPT: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": "  __VA_ARGS__); 
//putc('\n', yypscriptout); 

//#define pscript_err(format_mess,...)  fprintf(yypscriptout, "ERREUR: " "PSCRIPT_DESCRIPTION: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " __FUNC__ ": " format_mess "\n", __VA_ARGS__);
#define pmesserr(...)							\
  if (yypscriptout != NULL) fprintf(yypscriptout, "ERREUR: " "PSCRIPT: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": " __VA_ARGS__); \
  fprintf(stderr_FILE,                                       "PSCRIPT: " __FILE__ ": " BIGLIB_STRING(__LINE__) ": " BIGLIB_STRING(__func__) ": " __VA_ARGS__); 
//putc('\n', yypscriptout); 
#endif 
#define pmessage pmess 
#define pmesslog pmess 



//extern int 


#endif /* PSCRIPT_GLOBAL_H */

