#include "pscript_global.h" 
#include "pscript_string.h" 

typedef int16_t int_pscript_string_mem_t; 
enum { INT_PSCRIPT_STRING_MEM_MAX = sizeof(int_pscript_string_mem_t) == 1 ? INT8_MAX : sizeof(int_pscript_string_mem_t) == 2 ? INT16_MAX : sizeof(int_pscript_string_mem_t) == 4 ? INT32_MAX : sizeof(int_pscript_string_mem_t) == 8 ? INT64_MAX : -1 }; 

enum { INT_PSCRIPT_STRING_MAX = sizeof(int_pscript_string_t) == 1 ? INT8_MAX : sizeof(int_pscript_string_t) == 2 ? INT16_MAX : sizeof(int_pscript_string_t) == 4 ? INT32_MAX : sizeof(int_pscript_string_t) == 8 ? INT64_MAX : -1 }; 
const int_pscript_string_t int_pscript_string_max = INT_PSCRIPT_STRING_MAX; 

enum { pscript_string__memory__size = INT16_MAX }; 
enum { pscript_string__size         = (1 << 10) }; 
struct pscript_string_env_t { 
  char    pscript_string__memory__array[pscript_string__memory__size]; 
  int16_t pscript_string__memory__nb; 
  
  int16_t pscript_string__mem_offset[pscript_string__size]; 
  int16_t pscript_string__len[pscript_string__size]; 
  int16_t pscript_string__nb; 
}; 

static void assert_compile__dakjdds4388ds1p(void) { 
  ASSERT_COMPILE(INT_PSCRIPT_STRING_MEM_MAX >= pscript_string__memory__size); 
  ASSERT_COMPILE(INT_PSCRIPT_STRING_MAX     >= pscript_string__size); 
  
  pscript_string_env_t * this;   
  ASSERT_COMPILE(sizeof(this -> pscript_string__memory__nb   ) >= sizeof(int_pscript_string_mem_t)); 
  ASSERT_COMPILE(sizeof(this -> pscript_string__mem_offset[0]) >= sizeof(int_pscript_string_mem_t)); 
  ASSERT_COMPILE(sizeof(this -> pscript_string__nb           ) >= sizeof(int_pscript_string_t    )); 
}; 

const int pscript_string_env__sizeof = sizeof(pscript_string_env_t); 

pscript_string_env_t * pscript_string_env__make(void) {
  MALLOC_BZERO(pscript_string_env_t,this); 

  *this -> pscript_string__memory__array = '\0';
  this -> pscript_string__memory__nb = 1; 
  
  *this -> pscript_string__mem_offset = 0; 
  *this -> pscript_string__len = 0; 
  this -> pscript_string__nb = 1; 

  return this; 
}; 
 
void pscript_string_env__delete(pscript_string_env_t * this) {
  free(this); 
}; 


// --- PSCRIPT_STRING__TOOLS --- 

static int pscript_string__cstrlen(const char * cstr) {
  if (NULL ==  cstr) return 0; 
  int len = 0; 
  for (;;) {
    if (*cstr == '\0') return len; 
    len++; 
    cstr++; 
  }; 
  assert(false); 
  return -1; 
}; 


 
// --- PSCRIPT_STRING__MEMORY --- 

// RL: For the '_len' versions, strings are not assumed to be null-terminated (feature used by the lexer). 
static int pscript_string__memory__push__len(pscript_string_env_t * this, const char * cstr, const int cstr_len) {
  if (0 == cstr_len) { return 0; }; 
  const int available_bytes = pscript_string__memory__size - this -> pscript_string__memory__nb; 
  const int needed_bytes = cstr_len + 1; 
  assert(available_bytes > needed_bytes); 
  const int index = this -> pscript_string__memory__nb; 
  //bcopy(cstr, this -> pscript_string__memory__array + index, (cstr_len + 1) * (sizeof(char))); 
  bcopy(cstr, this -> pscript_string__memory__array + index, (cstr_len) * (sizeof(char))); 
  *(this -> pscript_string__memory__array + index + cstr_len) = '\0'; 
  this -> pscript_string__memory__nb += needed_bytes; //cstr_len + 1; 
  return index; 
}; 
 
static int pscript_string__memory__push(pscript_string_env_t * this, const char * cstr) { 
  const int cstr_len = pscript_string__cstrlen(cstr); 
  return pscript_string__memory__push__len(this, cstr, cstr_len); 
}; 
 
static const char * pscript_string__memory__get_cstr(const pscript_string_env_t * this, const int mem_offset) {
  assert(mem_offset >= 0); 
  assert(mem_offset < this -> pscript_string__memory__nb); 
  return this -> pscript_string__memory__array + mem_offset; 
}; 

 


// --- PSCRIPT_STRING__STACK --- 
 
// RL: For the '_len' versions, strings are not assumed to be null-terminated (feature used by the lexer). 
int pscript_string__push_no_lookup__len(pscript_string_env_t * this, const char * cstr, const int cstr_len) {
  if (0 == cstr_len) { return 0; }; 
  assert(pscript_string__size > this -> pscript_string__nb); 
  const int mem_offset = pscript_string__memory__push__len(this, cstr, cstr_len); 
  const int index = this -> pscript_string__nb; 
  this -> pscript_string__mem_offset[index] = mem_offset; 
  this -> pscript_string__len[index] = cstr_len; 
  this -> pscript_string__nb++; 
  return index; 
}; 
 
int pscript_string__push_no_lookup(pscript_string_env_t * this, const char * cstr) {
  const int cstr_len = pscript_string__cstrlen(cstr); 
  return pscript_string__push_no_lookup__len(this, cstr, cstr_len); 
}; 
 
const char * pscript_string__get_cstr(const pscript_string_env_t * this, const int str_index) { 
  assert(str_index >= 0); 
  assert(str_index < this -> pscript_string__nb); 
  const int mem_offset = this -> pscript_string__mem_offset[str_index]; 
  return pscript_string__memory__get_cstr(this, mem_offset); 
}; 

int pscript_string__get_len(const pscript_string_env_t * this, const int str_index) { 
  assert(str_index >= 0); 
  assert(str_index < this -> pscript_string__nb); 
  return this -> pscript_string__len[str_index]; 
}; 
 
// RL: For the '_len' versions, strings are not assumed to be null-terminated (feature used by the lexer). 
int pscript_string__lookup__len(const pscript_string_env_t * this, const char * cstr, const int cstr_len) { 
  if (0 == cstr_len) { return 0; }; 
  const char * p = this -> pscript_string__memory__array; 
  const int16_t * plen = this -> pscript_string__len; 
  for (int i = 0; i < this -> pscript_string__nb; i++) {
    if (cstr_len == *plen) { 
      //if (0 == strcmp(cstr, p)) { return i; }; 
      if (0 == bcmp(cstr, p, cstr_len)) { return i; }; 
    }; 
    p += *plen + 1; 
    plen++; 
  }; 
  return -1; 
}; 

int pscript_string__lookup(const pscript_string_env_t * this, const char * cstr) { 
  const int cstr_len = pscript_string__cstrlen(cstr); 
  return pscript_string__lookup__len(this, cstr, cstr_len); 
}; 

// RL: For the '_len' versions, strings are not assumed to be null-terminated (feature used by the lexer). 
int pscript_string__push_or_lookup__len(pscript_string_env_t * this, const char * cstr, const int cstr_len) { 
  if (0 == cstr_len) { return 0; }; 
  const int lookedup_index = pscript_string__lookup__len(this, cstr, cstr_len); 
  if (lookedup_index >= 0) { return lookedup_index; }; 
  return pscript_string__push_no_lookup__len(this, cstr, cstr_len); 
}; 

int pscript_string__push_or_lookup(pscript_string_env_t * this, const char * cstr) { 
#if 0 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: &cstr = %p\n", &cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %p\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dprintf(stderr__fileno, __FILE__ ":" STRINGIFY(__LINE__) ":<>: cstr@ = %s\n", cstr); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr = ", cstr, "\n"); 
  //dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "cstr@ = ", int_string__stack((int)cstr), "\n"); 
#endif 
  const int cstr_len = pscript_string__cstrlen(cstr); 
  return pscript_string__push_or_lookup__len(this, cstr, cstr_len); 
}; 










// *** STATS QUERY *** 

int pscript_string__get_count(const pscript_string_env_t * this) { 
  return this -> pscript_string__nb; 
}; 

// *** PRINT *** 

void pscript_string__print_one_string(const int filedes, const pscript_string_env_t * this, const int string_i) {
  //const int filedes = fileno(yypscriptout); 
  const char * string_i__str = int_string__stack(string_i); 
  const int string_i__str__spaces_nb = MAX(0, 6 - strlen(string_i__str)); 
  const int string_type = 0; //this -> pscript_string__type[string_i]; 
  const char * string_type_str = ""; //pscript_string__type_get_cstr(string_type); //pscript_parser__string_type__get_cstr(string_type); 
  const int nb_spaces = 0; //MAX(0, 17 - strlen(string_type_str)); 
  const int srcval_len = pscript_string__get_len(this, string_i); 
  const char * string_len_str = int_string__stack(srcval_len); 
  const int string_len_str_len = strlen(string_len_str); 
  const int string_len_str_len_spaces_nb = MAX(0, 6 - string_len_str_len); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, string_i__str__spaces_nb); 
  dputs(filedes, string_i__str); 
  dputs(filedes, ": "); 
  dputs(filedes, string_type_str); 
  dput_spaces(filedes, nb_spaces); 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, string_len_str_len_spaces_nb); 
  dputs(filedes, string_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, pscript_string__get_cstr(this, string_i)); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	pscript_string__get_srcval_cstr(this, string_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
}; 

static void pscript_string__print_stats__mem__base(const int filedes, const pscript_string_env_t * this, const int base) {
  //const int filedes = fileno(yypscriptout); 
  dputs(filedes, "STRING_MEM("); 
  dputn(filedes, this -> pscript_string__memory__nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> pscript_string__memory__nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void pscript_string__print_stats__stack__base(const int filedes, const pscript_string_env_t * this, const int base) {
  //const int filedes = fileno(yypscriptout); 
  dputs(filedes, "STRING_STACK("); 
  dputn(filedes, this -> pscript_string__nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> pscript_string__nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

void pscript_string__print_stats(const int filedes, const pscript_string_env_t * this) {
  pscript_string__print_stats__stack__base(filedes, this, pscript_string__size); 
  pscript_string__print_stats__mem__base(filedes, this, pscript_string__memory__size); 
}; 

void pscript_string__print_nb_strings(const int filedes, const pscript_string_env_t * this, const int string_nb) {
  //const int filedes = fileno(yypscriptout); 
  dputs(filedes, "STRING_STACK("); 
  dputn(filedes, this -> pscript_string__nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_string__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> pscript_string__nb)) / ((double) pscript_string__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> pscript_string__nb,string_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    pscript_string__print_one_string(filedes, this, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void pscript_string__print_all_strings(const int filedes, const pscript_string_env_t * this) {
  pscript_string__print_nb_strings(filedes, this, this -> pscript_string__nb); 
}; 
 



// *** RAW DUMP & RESTORE *** 

#if 0 
enum { pscript_string__memory__size = INT16_MAX }; 
enum { pscript_string__size = (1 << 10) }; 
struct pscript_string_env_t {
  char pscript_string__memory__array[pscript_string__memory__size]; 
  int16_t pscript_string__memory__nb; 
  
  int16_t pscript_string__mem_offset[pscript_string__size]; 
  int16_t pscript_string__len[pscript_string__size]; 
  int16_t pscript_string__nb; 
}; 
#endif 

#ifdef ENDIAN_BIG 
#error 
#else 
#define BUFFER_WRITE(type_t,a) { *((type_t *)p) = (a); p += sizeof(type_t); } 
#define BUFFER_READ(type_t,a) { a = *((type_t *)p); p += sizeof(type_t); } 
#endif 

enum { pscript_string__dump_raw__buffer_size = 63 }; 
int pscript_string__dump_raw(const int filedes, const pscript_string_env_t * this) { 
  int written_chars_count = 0; 
  //int should_have_written_chars_count = 0; 
  char buffer[pscript_string__dump_raw__buffer_size]; 
  char * p = buffer; 
  BUFFER_WRITE(int8_t,sizeof(this -> pscript_string__memory__nb)); 
  BUFFER_WRITE(int8_t,sizeof(this -> pscript_string__nb)); 
  BUFFER_WRITE(int8_t,sizeof(this -> pscript_string__len[0])); 
  BUFFER_WRITE(int16_t,this -> pscript_string__memory__nb); 
  BUFFER_WRITE(int16_t,this -> pscript_string__nb); 
  written_chars_count += write(filedes, buffer, p - buffer); 
  written_chars_count += write(filedes, this -> pscript_string__memory__array, this -> pscript_string__memory__nb); 
  written_chars_count += write(filedes, this -> pscript_string__mem_offset,    sizeof(this -> pscript_string__mem_offset[0]) * this -> pscript_string__nb); 
  written_chars_count += write(filedes, this -> pscript_string__len,           sizeof(this -> pscript_string__len[0]       ) * this -> pscript_string__nb); 
  const int should_have_written_chars_count = (p - buffer) + (this -> pscript_string__memory__nb) + (sizeof(this -> pscript_string__mem_offset[0]) * this -> pscript_string__nb) + (sizeof(this -> pscript_string__len[0]       ) * this -> pscript_string__nb); 
  if (written_chars_count == should_have_written_chars_count) { return written_chars_count; } 
  return INT_MIN & written_chars_count; 
}; 


int pscript_string__restore_raw(const int filedes, pscript_string_env_t * this) { 
  if (this -> pscript_string__nb > 1) { return false; }; 
  char buffer[pscript_string__dump_raw__buffer_size]; 
  char * p = buffer; 
  int read_nb; 
  read_nb = read(filedes, buffer, 3); 
  if (read_nb < 3) { return false; }; 
  const int8_t int_mem_sizeof = buffer[0]; 
  const int8_t int_nb_sizeof = buffer[1]; 
  const int8_t int_len_sizeof = buffer[2]; 
  if (int_mem_sizeof != sizeof(this -> pscript_string__memory__nb)) { return false; }; 
  if (int_nb_sizeof  != sizeof(this -> pscript_string__nb))         { return false; }; 
  if (int_len_sizeof != sizeof(this -> pscript_string__len[0]))     { return false; }; 
  int wanna_read = int_mem_sizeof + int_nb_sizeof; 
  read_nb = read(filedes, buffer, wanna_read); 
  if (read_nb < wanna_read) { return false; }; 
  p = buffer; 
  BUFFER_READ(int16_t,this -> pscript_string__memory__nb); 
  BUFFER_READ(int16_t,this -> pscript_string__nb); 
  
  wanna_read = this -> pscript_string__memory__nb;                                         read_nb = read(filedes, this -> pscript_string__memory__array, wanna_read); if (read_nb < wanna_read) { return INT_MIN & read_nb; }; 
  wanna_read = sizeof(this -> pscript_string__mem_offset[0]) * this -> pscript_string__nb; read_nb = read(filedes, this -> pscript_string__mem_offset,    wanna_read); if (read_nb < wanna_read) { return INT_MIN & read_nb; }; 
  wanna_read = sizeof(this -> pscript_string__len[0])        * this -> pscript_string__nb; read_nb = read(filedes, this -> pscript_string__len,           wanna_read); if (read_nb < wanna_read) { return INT_MIN & read_nb; }; 
  
  return read_nb; 
}; 



