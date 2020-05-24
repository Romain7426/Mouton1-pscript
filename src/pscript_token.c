#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_token.h" 
#include "pscript_token_type.h" 

enum { INT_PSCRIPT_TOKEN_MAX = sizeof(int_pscript_token_t) == 1 ? INT8_MAX : sizeof(int_pscript_token_t) == 2 ? INT16_MAX : sizeof(int_pscript_token_t) == 4 ? INT32_MAX : sizeof(int_pscript_token_t) == 8 ? INT64_MAX : -1 }; 
const int_pscript_token_t int_pscript_token_max = INT_PSCRIPT_TOKEN_MAX; 


enum { PSCRIPT_TOKEN__SIZE = INT16_MAX }; //MAX(1 << 7,MIN((INT_MAX >> 1)+1, 11 << 15)) }; // 96k // Roughly, the number of produced tokens will be around 12.5% of the size of the source file. Therefore, if we have a 500k input file, around 69k would be produced. 


struct pscript_token_env_t { 
  pscript_string_env_t * string_env; 
  
  // STACK 
  int16_t        srcval [PSCRIPT_TOKEN__SIZE]; 
  uint8_t        type   [PSCRIPT_TOKEN__SIZE]; 
  int16_t        srcfile[PSCRIPT_TOKEN__SIZE]; 
  int32_t        offset0[PSCRIPT_TOKEN__SIZE]; 
  int24_packed_t line1  [PSCRIPT_TOKEN__SIZE]; 
  int16_t        column0[PSCRIPT_TOKEN__SIZE]; 
  int16_t        nb; 
  
  // SYNTAX_MASTER 
  const char *   master__array[PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE]; 
  int            master__len  [PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE]; 
  int8_t         master__nb; 

  // SYNTAX_KEYWORD 
  const char *   syntax_keyword__array[PSCRIPT_TOKEN__SYNTAX_KEYWORD__SIZE]; 
  int            syntax_keyword__len  [PSCRIPT_TOKEN__SYNTAX_KEYWORD__SIZE]; 
  int8_t         syntax_keyword__nb; 
}; 


static void assert_compile__dakjdds4388ds1p(void) { 
  ASSERT_COMPILE(INT_PSCRIPT_TOKEN_MAX >= PSCRIPT_TOKEN__SIZE); 
  
  pscript_token_env_t * this; 
  ASSERT_COMPILE(sizeof(this ->  srcval) >= sizeof(int_pscript_string_t    )); 
  ASSERT_COMPILE(sizeof(this ->    type) >= sizeof(int_pscript_token_type_t)); 
  ASSERT_COMPILE(sizeof(this -> srcfile) >= sizeof(int_pscript_string_t    )); 
  ASSERT_COMPILE(sizeof(this ->      nb) >= sizeof(int_pscript_token_t     )); 
  
  ASSERT_COMPILE(sizeof(this -> master__nb) == sizeof(int8_t)); 
  ASSERT_COMPILE(INT8_MAX >= PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE); 
  
  ASSERT_COMPILE(sizeof(this -> syntax_keyword__nb) == sizeof(int8_t)); 
  ASSERT_COMPILE(INT8_MAX >= PSCRIPT_TOKEN__SYNTAX_KEYWORD__SIZE); 
}; 

const int32_t pscript_token_env__sizeof = sizeof(pscript_token_env_t); 


 
pscript_token_env_t * pscript_token_env__make(pscript_string_env_t * string_env) {
  MALLOC_BZERO(pscript_token_env_t,this); 
  this -> string_env = string_env; 
  this -> nb = 0; 
  return this; 
}; 

void pscript_token_env__delete(pscript_token_env_t * this) {
  free(this); 
}; 



// *** TOKEN_STACK *** 

void pscript_token__raz(pscript_token_env_t * this) { 
  this -> nb = 0; 
}; 
 
int pscript_token__push(pscript_token_env_t * this, const int srcval_len, const char * srcval_rstr, const int token_type, const int srcfile, const int offset0, const int line1, const int column0) { 
  assert(PSCRIPT_TOKEN__SIZE > this -> nb); 
  const int idx          =  this -> nb; 
  const int srcval_pstr  = pscript_string__push__len(this -> string_env, srcval_rstr, srcval_len); 
  this -> srcval [idx]   = srcval_pstr; 
  this -> type   [idx]   =  token_type; 
  this -> srcfile[idx]   =     srcfile; 
  this -> offset0[idx]   =     offset0; 
  //this -> line1  [idx]   =       line1; 
  INT24_PACKED_ASSIGN_UNPACKED(this -> line1[idx], line1); 
  this -> column0[idx]   =     column0; 
  this -> nb++; 
  return idx; 
}; 

int pscript_token__get_srcval_pstr(const pscript_token_env_t * this, const int token_idx){  
  return this -> srcval[token_idx]; 
}; 
 
const char * pscript_token__get_srcval_cstr(const pscript_token_env_t * this, const int token_idx){  
  return pscript_string__get_cstr(this -> string_env, this -> srcval[token_idx]); 
}; 
 
int pscript_token__get_type(const pscript_token_env_t * this, const int token_idx){  
  return this -> type[token_idx]; 
}; 
 
int pscript_token__get_srcfile_pstr(const pscript_token_env_t * this, const int token_idx){  
  return this -> srcfile[token_idx]; 
}; 
 
const char * pscript_token__get_srcfile_cstr(const pscript_token_env_t * this, const int token_idx){  
  return pscript_string__get_cstr(this -> string_env, this -> srcfile[token_idx]); 
}; 
 
int pscript_token__get_offset0(const pscript_token_env_t * this, const int token_idx){  
  return this -> offset0[token_idx]; 
}; 
 
int pscript_token__get_line1(const pscript_token_env_t * this, const int token_idx){ 
  //return this -> line1[token_idx]; 
  int line1; 
  INT24_PACKED_READ(this -> line1[token_idx],line1); 
  return line1; 
}; 
 
int pscript_token__get_column0(const pscript_token_env_t * this, const int token_idx){  
  return this -> column0[token_idx]; 
}; 



// *** STATS QUERY *** 

int pscript_token__get_count(const pscript_token_env_t * this) { 
  return this -> nb; 
}; 

int pscript_token__get_size(const pscript_token_env_t * this) { 
  return PSCRIPT_TOKEN__SIZE; 
}; 


// *** SYNTAX_MASTER *** 

static int pscript_token__master_push__common(pscript_token_env_t * this, const char * ident_lower_well_allocated) { 
  assert(this -> master__nb < PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE); 
  const int index = this -> master__nb; 
  this -> master__nb++; 
  this -> master__array[index] = ident_lower_well_allocated; 
  this -> master__len[index] = strlen(ident_lower_well_allocated); 
  return PSCRIPT_TOKEN__SYNTAX_MASTER__BASE + index; 
}; 
 
int pscript_token__master_push_cstr(pscript_token_env_t * this, const char * ident_cstr) { 
  const int  ident_len = strlen(ident_cstr); 
  char ident_lower_cstr[ident_len]; 
  for (int i = 0; i <= ident_len; i++) { 
    ident_lower_cstr[i] = tolower(ident_cstr[i]); 
  }; 
  const int    ident_lower_pstr           = pscript_string__push    (this -> string_env, ident_lower_cstr); 
  const char * ident_lower_well_allocated = pscript_string__get_cstr(this -> string_env, ident_lower_pstr); 
  return pscript_token__master_push__common(this, ident_lower_well_allocated); 
}; 

int pscript_token__master_push_pstr(pscript_token_env_t * this, const int ident_pstr) { 
  const char * ident_cstr = pscript_string__get_cstr(this -> string_env, ident_pstr); 
  return pscript_token__master_push_cstr(this, ident_cstr); 
}; 

int pscript_token__master_count(const pscript_token_env_t * this) { 
  return this -> master__nb; 
}; 

const int * pscript_token__master_len(const pscript_token_env_t * this) { 
  return this -> master__len; 
}; 
 
const char * * pscript_token__master_cstr(const pscript_token_env_t * this) { 
#if defined(__clang__)
  #pragma clang diagnostic push 
  #pragma clang diagnostic ignored "-Wcast-qual" 
#endif
  
  return (const char * * ) this -> master__array; 
  
#if defined(__clang__)
  #pragma clang diagnostic pop 
#endif
}; 
 



// *** SYNTAX_KEYWORD *** 

static int pscript_token__syntax_keyword_push__common(pscript_token_env_t * this, const char * ident_lower_well_allocated) { 
  assert(this -> syntax_keyword__nb < PSCRIPT_TOKEN__SYNTAX_KEYWORD__SIZE); 
  const int index = this -> syntax_keyword__nb; 
  this -> syntax_keyword__nb++; 
  this -> syntax_keyword__array[index] = ident_lower_well_allocated; 
  this -> syntax_keyword__len[index] = strlen(ident_lower_well_allocated); 
  return PSCRIPT_TOKEN__SYNTAX_KEYWORD__BASE + index; 
}; 
 
int pscript_token__syntax_keyword_push_cstr(pscript_token_env_t * this, const char * ident_cstr) { 
  const int  ident_len = strlen(ident_cstr); 
  char ident_lower_cstr[ident_len]; 
  for (int i = 0; i <= ident_len; i++) { 
    ident_lower_cstr[i] = tolower(ident_cstr[i]); 
  }; 
  const int    ident_lower_pstr           = pscript_string__push    (this -> string_env, ident_lower_cstr); 
  const char * ident_lower_well_allocated = pscript_string__get_cstr(this -> string_env, ident_lower_pstr); 
  return pscript_token__syntax_keyword_push__common(this, ident_lower_well_allocated); 
}; 

int pscript_token__syntax_keyword_push_pstr(pscript_token_env_t * this, const int ident_pstr) { 
  const char * ident_cstr = pscript_string__get_cstr(this -> string_env, ident_pstr); 
  return pscript_token__syntax_keyword_push_cstr(this, ident_cstr); 
}; 

int pscript_token__syntax_keyword_count(const pscript_token_env_t * this) { 
  return this -> syntax_keyword__nb; 
}; 

const int * pscript_token__syntax_keyword_len(const pscript_token_env_t * this) { 
  return this -> syntax_keyword__len; 
}; 
 
const char * * pscript_token__syntax_keyword_cstr(const pscript_token_env_t * this) { 
#if defined(__clang__)
  #pragma clang diagnostic push 
  #pragma clang diagnostic ignored "-Wcast-qual" 
#endif
  
  return (const char * * ) this -> syntax_keyword__array; 
  
#if defined(__clang__)
  #pragma clang diagnostic pop 
#endif
}; 
 



// *** PRINT *** 

void pscript_token__print_one_token(const int filedes, const pscript_token_env_t * this, const int token_i) {
  //const int filedes = fileno(yypscriptout); 
  const char * token_i__str                = int_string__stack(token_i); 
  const int    token_i__str__spaces_nb     = MAX(0, 6 - strlen(token_i__str)); 
  const int    token_type                  = this -> type[token_i]; 
  const char * token_type_i_str            = int_string__stack(token_type); //pscript_parser__token_type__get_cstr(token_type); 
  const int    token_type_i_str__spaces_nb = MAX(0, 3 - strlen(token_type_i_str)); 
  const char * token_type_str              = pscript_token__type_get_cstr(token_type); //pscript_parser__token_type__get_cstr(token_type); 
  const int    token_type_str__spaces_nb   = MAX(0, 18 - strlen(token_type_str)); 
  const char * token_srcval_cstr           = pscript_token__get_srcval_cstr(this, token_i); 
  const int    token_srcval_cstr__spaces_nb = MAX(0, 47 - strlen(token_srcval_cstr)); 
  const int    srcval_len                  = pscript_string__get_len(this -> string_env, pscript_token__get_srcval_pstr(this, token_i)); 
  const char * token_len_str               = int_string__stack(srcval_len); 
  const int    token_len_str_len           = strlen(token_len_str); 
  const int    token_len_str_len_spaces_nb = MAX(0, 6 - token_len_str_len); 

  int    token_line1; INT24_PACKED_READ(this -> line1[token_i], token_line1);
  const char * token_line1_cstr               = int_string__stack(token_line1); 
  const int    token_line1_cstr_len           = strlen(token_line1_cstr); 
  const int    token_line1_cstr_len_spaces_nb = MAX(0, 6 - token_line1_cstr_len); 

  const int    token_column0                    = this -> column0[token_i]; 
  const char * token_column0_cstr               = int_string__stack(token_column0); 
  const int    token_column0_cstr_len           = strlen(token_column0_cstr); 
  const int    token_column0_cstr_len_spaces_nb = MAX(0, 6 - token_column0_cstr_len); 

  const int    token_offset0                    = this -> offset0[token_i]; 
  const char * token_offset0_cstr               = int_string__stack(token_offset0); 
  const int    token_offset0_cstr_len           = strlen(token_offset0_cstr); 
  const int    token_offset0_cstr_len_spaces_nb = MAX(0, 6 - token_offset0_cstr_len); 

  const int    token_srcfile_pstr               = this -> srcfile[token_i]; 
  const char * token_srcfile_cstr               = pscript_token__get_srcfile_cstr(this, token_i); 
  const int    token_srcfile_cstr_len           = strlen(token_srcfile_cstr); 
  const int    token_srcfile_cstr_len_spaces_nb = MAX(0, 6 - token_srcfile_cstr_len); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, token_i__str__spaces_nb); 
  dputs(filedes, token_i__str); 
  dputs(filedes, ": "); 
  dputs(filedes, token_type_str); 
  dputs(filedes, "("); 
  //dput_spaces(filedes, token_type_i_str__spaces_nb); 
  dputs(filedes, token_type_i_str); 
  dputs(filedes, ")"); 
  //dput_spaces(filedes, token_type_str__spaces_nb); 
  dput_spaces(filedes, token_type_i_str__spaces_nb + token_type_str__spaces_nb); 
#if 0 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, token_len_str_len_spaces_nb); 
  dputs(filedes, token_len_str); 
  dputs(filedes, " ] : "); 
#endif 
  dputs(filedes, " : "); 
  dputs(filedes, token_srcval_cstr); 
  dput_spaces(filedes, token_srcval_cstr__spaces_nb); 
  dputs(filedes, " [ srcfile: "); 
  dput_spaces(filedes, token_srcfile_cstr_len_spaces_nb); 
  dputs(filedes, "<"); 
  dputs(filedes, token_srcfile_cstr); 
  dputs(filedes, ">@"); 
  dputs(filedes, token_offset0_cstr); 
  dput_spaces(filedes, token_offset0_cstr_len_spaces_nb); 
  dputs(filedes, " - line1: "); 
  dput_spaces(filedes, token_line1_cstr_len_spaces_nb); 
  dputs(filedes, token_line1_cstr); 
  dputs(filedes, " - column0: "); 
  dput_spaces(filedes, token_column0_cstr_len_spaces_nb); 
  dputs(filedes, token_column0_cstr); 
  dputs(filedes, " ] "); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	pscript_token__get_srcval_cstr(this, token_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
}; 

static void pscript_token__print_stats__base(const int filedes, const pscript_token_env_t * this, const int base) {
  //const int filedes = fileno(yypscriptout); 
  dputs(filedes, "TOKEN_STACK("); 
  dputn(filedes, this -> nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

void pscript_token__print_stats(const int filedes, const pscript_token_env_t * this) {
  pscript_token__print_stats__base(filedes, this, PSCRIPT_TOKEN__SIZE); 
}; 

void pscript_token__print_nb_tokens(const int filedes, const pscript_token_env_t * this, const int token_nb) {
  //const int filedes = fileno(yypscriptout); 
  dputs(filedes, "TOKEN_STACK("); 
  dputn(filedes, this -> nb); 
  dputs(filedes, " / "); 
  dputn(filedes, PSCRIPT_TOKEN__SIZE); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> nb)) / ((double) PSCRIPT_TOKEN__SIZE))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> nb,token_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    pscript_token__print_one_token(filedes, this, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void pscript_token__print_all_tokens(const int filedes, const pscript_token_env_t * this) {
  pscript_token__print_nb_tokens(filedes, this, this -> nb); 
}; 
 


// *** RAW DUMP & RESTORE *** 

#if 0 
struct pscript_token_env_t { 
  int16_t        srcval [PSCRIPT_TOKEN__SIZE]; 
  int8_t         type   [PSCRIPT_TOKEN__SIZE]; 
  int16_t        srcfile[PSCRIPT_TOKEN__SIZE]; 
  int32_t        offset0[PSCRIPT_TOKEN__SIZE]; 
  int24_packed_t line1  [PSCRIPT_TOKEN__SIZE]; 
  int16_t        column0[PSCRIPT_TOKEN__SIZE]; 
  int16_t        nb; 
}; 
#endif 

#ifdef ENDIAN_BIG 
#error 
#else 
#define BUFFER_WRITE(type_t,a) { *((type_t *)p) = (a); p += sizeof(type_t); } 
//#define BUFFER_READ(type_t,a) { a = *((type_t *)p); p += sizeof(type_t); } 
#define BUFFER_READ(type_t,a) a = *((type_t *)p); p += sizeof(type_t); 
#endif 
#define WRITE_ARRAY(a,s) { const int s_ = (s); written_chars_count += write(filedes, (a), (s_)); should_have_written_chars_count += s_; }
#define READ_ARRAY(a,s)  { const int s_ = (s);    read_chars_count += read (filedes, (a), (s_)); should_have_read_chars_count    += s_; }


enum { pscript_token__dump_raw__buffer_size = 63 }; 
int pscript_token__dump_raw(const int filedes, const pscript_token_env_t * this) { 
  int written_chars_count = 0; 
  int should_have_written_chars_count = 0; 
  char buffer[pscript_token__dump_raw__buffer_size]; 
  char * p = buffer; 
  BUFFER_WRITE(int8_t,sizeof(this ->  srcval[0])); 
  BUFFER_WRITE(int8_t,sizeof(this ->    type[0])); 
  BUFFER_WRITE(int8_t,sizeof(this -> srcfile[0])); 
  BUFFER_WRITE(int8_t,sizeof(this -> offset0[0])); 
  BUFFER_WRITE(int8_t,sizeof(this ->   line1[0])); 
  BUFFER_WRITE(int8_t,sizeof(this -> column0[0])); 
  BUFFER_WRITE(int8_t,sizeof(this ->      nb   )); 
  BUFFER_WRITE(int16_t,this -> nb); 
  written_chars_count += write(filedes, buffer, p - buffer); should_have_written_chars_count += (p - buffer); 
  WRITE_ARRAY(this -> srcval,  (this -> nb) * sizeof(this ->  srcval[0])); 
  WRITE_ARRAY(this -> type,    (this -> nb) * sizeof(this ->    type[0])); 
  WRITE_ARRAY(this -> srcfile, (this -> nb) * sizeof(this -> srcfile[0])); 
  WRITE_ARRAY(this -> offset0, (this -> nb) * sizeof(this -> offset0[0])); 
  WRITE_ARRAY(this -> line1,   (this -> nb) * sizeof(this ->   line1[0])); 
  WRITE_ARRAY(this -> column0, (this -> nb) * sizeof(this -> column0[0])); 
  if (written_chars_count == should_have_written_chars_count) { return written_chars_count; } 
  return INT_MIN & written_chars_count; 
}; 

//#define return assert(false); 
int pscript_token__restore_raw(const int filedes, pscript_token_env_t * this) { 
  if (this -> nb > 0) { return INT_MIN; }; 
  char buffer[pscript_token__dump_raw__buffer_size]; 
  char * p = buffer; 
  int read_chars_count = 0; 
  int should_have_read_chars_count = 0; 
  READ_ARRAY(buffer, 9); 
  //dputs_array(stderr, __func__, ": ", "read_chars_count: ", int_string__stack(read_chars_count), "\n"); 
#if 0 
  const int8_t int_srcval_sizeof  = buffer[0]; 
  const int8_t int_type_sizeof    = buffer[1]; 
  const int8_t int_srcfile_sizeof = buffer[2]; 
  const int8_t int_offset0_sizeof = buffer[3]; 
  const int8_t int_line1_sizeof   = buffer[4]; 
  const int8_t int_column0_sizeof = buffer[5]; 
  const int8_t int_nb_sizeof      = buffer[6]; 
#endif 
  const int8_t BUFFER_READ(int8_t, int_srcval_sizeof ); 
  const int8_t BUFFER_READ(int8_t, int_type_sizeof   ); 
  const int8_t BUFFER_READ(int8_t, int_srcfile_sizeof); 
  const int8_t BUFFER_READ(int8_t, int_offset0_sizeof); 
  const int8_t BUFFER_READ(int8_t, int_line1_sizeof  ); 
  const int8_t BUFFER_READ(int8_t, int_column0_sizeof); 
  const int8_t BUFFER_READ(int8_t, int_nb_sizeof     ); 
  if (int_srcval_sizeof  != sizeof(this ->  srcval[0])) { return INT_MIN | read_chars_count; }; 
  if (int_type_sizeof    != sizeof(this ->    type[0])) { return INT_MIN | read_chars_count; }; 
  if (int_srcfile_sizeof != sizeof(this -> srcfile[0])) { return INT_MIN | read_chars_count; }; 
  if (int_offset0_sizeof != sizeof(this -> offset0[0])) { return INT_MIN | read_chars_count; }; 
  if (int_line1_sizeof   != sizeof(this ->   line1[0])) { return INT_MIN | read_chars_count; }; 
  if (int_column0_sizeof != sizeof(this -> column0[0])) { return INT_MIN | read_chars_count; }; 
  if (int_nb_sizeof      != sizeof(this ->      nb   )) { return INT_MIN | read_chars_count; }; 
  BUFFER_READ(int16_t,this -> nb); 
  READ_ARRAY(this -> srcval,  (this -> nb) * sizeof(this ->  srcval[0])); 
  READ_ARRAY(this -> type,    (this -> nb) * sizeof(this ->    type[0])); 
  READ_ARRAY(this -> srcfile, (this -> nb) * sizeof(this -> srcfile[0])); 
  READ_ARRAY(this -> offset0, (this -> nb) * sizeof(this -> offset0[0])); 
  READ_ARRAY(this -> line1,   (this -> nb) * sizeof(this ->   line1[0])); 
  READ_ARRAY(this -> column0, (this -> nb) * sizeof(this -> column0[0])); 
  if (read_chars_count == should_have_read_chars_count) { return read_chars_count; } 
  return INT_MIN | read_chars_count; 
  return 0; 
}; 



