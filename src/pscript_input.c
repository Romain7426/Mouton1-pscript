#include "pscript_global.h" 
#include "pscript_input.h" 

pscript_input_env_t * pscript_input_env__make_r(pscript_string_env_t * string_env, pscript_input_env_t * this) { 
  BZERO_THIS(this); 
  this -> string_env = string_env; 
  return this; 
}; 
 
pscript_input_env_t * pscript_input_env__make_b(pscript_string_env_t * string_env, const int bsize, void * bvalue) { 
  pscript_input_env_t * this = (pscript_input_env_t *) bvalue; 
  if (bsize < (int)sizeof(*this)) return NULL; 
  return pscript_input_env__make_r(string_env, this); 
}; 
 
pscript_input_env_t * pscript_input_env__make(pscript_string_env_t * string_env) { 
  MALLOC_THIS(pscript_input_env_t,this); 
  return pscript_input_env__make_r(string_env, this); 
}; 
 
void pscript_input_env__delete_r(pscript_input_env_t * this) { 
}; 
 
void pscript_input_env__delete(pscript_input_env_t * this) { 
  pscript_input_env__delete_r(this); 
  free(this); 
}; 
 

static void pscript_input__make__assign_buffer(pscript_input_env_t * this, const int input_i) { 
  if (this -> static_buffer__nb < pscript_input__static_buffer__size) { 
    const int index = this -> static_buffer__nb;  
    this -> static_buffer__nb++; 
    this -> buffer[input_i] = this -> static_buffer[index]; 
    this -> buffer_static_huh[input_i] = true; 
    this -> buffer_nb[input_i] = 0; 
  } 
  else { 
    this -> buffer[input_i] = malloc((pscript_input__buffer__size+1)*(sizeof(char))); 
    this -> buffer_static_huh[input_i] = false; 
    this -> buffer_nb[input_i] = 0; 
  }; 
}; 
 
static int pscript_input__make__alloc_and_init(pscript_input_env_t * this, const int input_type) { 
  assert(this -> nb < pscript_input__env_size); 
  const int input_i = this -> nb;  
  this -> nb++; 
  
  this -> type      [input_i] = input_type; 
  this -> mem_ro    [input_i] = NULL; 
  this -> mem_ro_len[input_i] = -1; 
  this -> fileno    [input_i] = -1; 
  this -> filename  [input_i] = -1; 
  
  pscript_input__make__assign_buffer(this, input_i); 
  this -> cumulated_read_nb[input_i] = 0; 
  this -> eof_huh          [input_i] = false; 
  
  this -> current_char0[input_i] = 0; 
  //this -> current_line1[input_i] = this -> token_line1[input_i]; //(int24_packed_t)1; 
  //this -> current_line1[input_i] = this -> token_line1[input_i]; //(int24_packed_t)1; 
  //this -> current_line1[input_i] = (int24_packed_t){ 1 , 0 , 0 }; //this -> token_line1[input_i]; //(int24_packed_t)1; 
  //*((char *) this -> current_line1[input_i]) = (int24_packed_t){ 1 , 0 , 0 }; //this -> token_line1[input_i]; //(int24_packed_t)1; 
  INT24_PACKED_ASSIGN_UNPACKED(this -> current_line1[input_i],1); 
  this -> current_column0[input_i] = 0; 
  this -> current_buffer0[input_i] = 0; 
  this -> token_char0[input_i] = 0; 
  //this -> token_line1[input_i] = 1; 
  INT24_PACKED_ASSIGN_UNPACKED(this -> token_line1[input_i],1); 
  this -> token_column0[input_i] = 0; 
  this -> token_buffer0[input_i] = 0; 
  
  this -> repl_huh[input_i] = false; 
  this -> repl_prompt_pstr[input_i] = -1; 
  this -> repl_prompt_cstr[input_i] = NULL; 
  
  return input_i; 
}; 
 
int pscript_input__make_from_file_pstr(pscript_input_env_t * this, const int filename_pstr) { 
  const char * filename_cstr = pscript_string__get_cstr(this -> string_env, filename_pstr); 
  
  const int fileno = open(filename_cstr, O_RDONLY); 
  if (fileno < 0) { return -1; }; 
  
  const int index = pscript_input__make__alloc_and_init(this, PSCRIPT_INPUT__TYPE__FILE); 
  this -> filename[index] = filename_pstr; 
  this -> fileno[index] = fileno; 
  
  return index; 
}; 
 
int pscript_input__make_from_file_cstr(pscript_input_env_t * this, const char * filename_cstr) { 
  const int filename_pstr = pscript_string__push(this -> string_env, filename_cstr); 
  return pscript_input__make_from_file_pstr(this, filename_pstr); 
}; 
  
int pscript_input__make_from_mem(pscript_input_env_t * this, const int mem_ro_len, const char * mem_ro) { 
  const int index = pscript_input__make__alloc_and_init(this, PSCRIPT_INPUT__TYPE__MEM_RO); 
  this -> mem_ro[index] = mem_ro; 
  this -> mem_ro_len[index] = mem_ro_len; 
  return index; 
}; 
 
int pscript_input__make_from_fileno(pscript_input_env_t * this, const int fileno, const char * filename_cstr) { 
  const int index = pscript_input__make__alloc_and_init(this, PSCRIPT_INPUT__TYPE__FILENO); 
  const int filename_pstr = pscript_string__push(this -> string_env, filename_cstr); 
  this -> fileno[index] = fileno; 
  this -> filename[index] = filename_pstr; 
  
  this -> repl_huh[index] = (1 == isatty(fileno)); 
  if (this -> repl_huh[index]) { 
    const char * repl_prompt = "> "; 
    const int    repl_promp_pstr = pscript_string__push(this -> string_env, repl_prompt); 
    this -> repl_prompt_pstr[index] = repl_promp_pstr; 
    this -> repl_prompt_cstr[index] = pscript_string__get_cstr(this -> string_env, repl_promp_pstr); 
  }; 
  
  return index; 
}; 
 
 
void pscript_input__delete(pscript_input_env_t * this, const int input_i) { 
  assert(input_i >= 0); 
  assert(input_i < this -> nb); 
  if (not(this -> buffer_static_huh[input_i])) { 
    free(this -> buffer); 
  }; 
}; 
 

int pscript_input__eof_huh(pscript_input_env_t * this, const int input_i) { 
  return this -> eof_huh[input_i]; 
}; 

static void pscript_input__buffer__shift_remaining_data_to_the_beginning(pscript_input_env_t * this, const int input_i) { 
  if (0 == this -> token_buffer0[input_i]) { return; }; 
  if (0 == this -> buffer_nb[input_i]) { return; }; 
  const int token_len = this -> current_buffer0[input_i] - this -> token_buffer0[input_i]; 
  bcopy(this -> buffer[input_i] + this -> token_buffer0[input_i], this -> buffer[input_i], token_len); 
  this -> buffer_nb[input_i]      -= this -> token_buffer0[input_i]; 
  this -> token_buffer0[input_i]   = 0; 
  this -> current_buffer0[input_i] = token_len; 
}; 
 
static int pscript_input__buffer__fill(pscript_input_env_t * this, const int input_i) { 
  const int available = pscript_input__buffer__size - this -> buffer_nb[input_i]; 
  
  switch (this -> type[input_i]) { 
  default: assert(false); break; 
  case PSCRIPT_INPUT__TYPE__MEM_RO: goto pscript_input__buffer__fill__state__mem_ro; break; 
  case PSCRIPT_INPUT__TYPE__FILE:   goto pscript_input__buffer__fill__state__fileno; break; 
  case PSCRIPT_INPUT__TYPE__FILENO: goto pscript_input__buffer__fill__state__fileno; break; 
  }; 

  assert(false); 



 pscript_input__buffer__fill__state__fileno: {
    int read_nb = -1; 
    if (this -> repl_huh[input_i]) { 
      //dputs(stderr, repl_prompt); }; 
      char * tmp = NULL; 
      tmp = readline(this -> repl_prompt_cstr[input_i]); 
#if 0 
      dputs_array(stderr, "EDITLINE: tmp: ", tmp, "\n"); 
#endif 
      if ((NULL != tmp) && (*tmp != '\0')) { add_history(tmp); }; 
      if (NULL == tmp) { 
	read_nb = 0; 
	this -> repl_huh[input_i] = false; 
      } 
      else { 
	read_nb = strlcpy(this -> buffer[input_i] + this -> buffer_nb[input_i], tmp, available); 
      }; 
    } 
    else { 
      read_nb = read(this -> fileno[input_i], this -> buffer[input_i] + this -> buffer_nb[input_i], available); 
    }; 
#if 0 
    dputs_array(stderr, __func__, ": read_nb = ", int_string__stack(read_nb), "\n"); 
#endif 
    if (0 == read_nb) { this -> eof_huh[input_i] = true; return 0; }; 
    if (0  > read_nb) { return read_nb; }; 
    this -> buffer_nb[input_i] += read_nb; 
    this -> cumulated_read_nb[input_i] += read_nb; 
    //if (repl_mode) { input_reading__eof_huh__var = true; }; 
    return read_nb; 
  }; 

  assert(false); 

 pscript_input__buffer__fill__state__mem_ro: { 
    const int left_to_be_read = this -> mem_ro_len[input_i] - this -> cumulated_read_nb[input_i]; 
    if (0 >= left_to_be_read) { 
      this -> eof_huh[input_i] = true; 
      return 0; 
    }; 
    const int bcopy_nb = MIN(available,left_to_be_read);  
    bcopy(this -> mem_ro[input_i], this -> buffer[input_i] + this -> buffer_nb[input_i], bcopy_nb); 
    this -> buffer_nb[input_i] += bcopy_nb; 
    this -> cumulated_read_nb[input_i] += bcopy_nb; 
    return bcopy_nb; 
  }; 

  assert(false); 
  return -1; 
}; 

 
int pscript_input__getc(pscript_input_env_t * this, const int input_i) { 
#if 0 
  dputs_array(stderr, __func__, "\n"); 
  dputs_array(stderr, "this -> current_buffer0[input_i] = ", int_string__stack(this -> current_buffer0[input_i]), "\n"); 
  dputs_array(stderr, "this -> token_buffer0  [input_i] = ", int_string__stack(this -> token_buffer0[input_i]), "\n"); 
  dputs_array(stderr, "this -> buffer_nb      [input_i] = ", int_string__stack(this -> buffer_nb[input_i]), "\n"); 
  dputs_array(stderr, "this -> eof_huh        [input_i] = ", bool_string(this -> eof_huh[input_i]), "\n"); 
#endif 

  if (this -> current_buffer0[input_i] == this -> buffer_nb[input_i]) { 
    if (this -> eof_huh[input_i]) { return EOF; }; 
    if (this -> repl_huh[input_i] && (this -> buffer_nb[input_i] != 0)) { return EOF; }; 
    if (pscript_input__buffer__size == this -> buffer_nb[input_i]) { 
      if (0 == this -> token_buffer0[input_i]) { 
	assert_message(false, "Token is too long to fit in the buffer (buffer size: ...)"); 
      }; 
      pscript_input__buffer__shift_remaining_data_to_the_beginning(this, input_i); 
    }; 
    const int nb_chars_read = pscript_input__buffer__fill(this, input_i); 
#if 0 
    dputs_array(stderr, "nb_chars_read = [", int_string__stack(nb_chars_read), "] ", "\n"); 
#endif
    if (0 == nb_chars_read) { return EOF; }; 
    if (0  > nb_chars_read) { return -2; }; 
  }; 
#if 0 
  char_array__print(stderr, 20, input_buffer); dput_eol(stderr); 
#endif 
  //assert(input_buffer__token__index_end < input_buffer__nb); 
  //const int c = input_buffer[input_buffer__token__index_end]; 
  const int c = this -> buffer[input_i][this -> current_buffer0[input_i]]; 
  this -> current_buffer0[input_i]++; 

#if 0 
  // RL: Ignored for the moment as when chars are pushed back, this data become inaccurate. 
  if ('\n' == c) { 
    int line1; 
    INT24_PACKED_READ(this -> current_line1[input_i], line1); 
    line1++; 
    INT24_PACKED_WRITE(this -> current_line1[input_i], line1); 
  }; 
#endif 
  return c; 
}; 
 
void pscript_input__move_backward(pscript_input_env_t * this, const int input_i, const int chars_nb) { 
  assert(chars_nb >= 0); 
  assert(chars_nb <= this -> current_buffer0[input_i]); 
  this -> current_buffer0[input_i] -= chars_nb; 
  assert(this -> token_buffer0[input_i] <= this -> current_buffer0[input_i]); 
}; 
 
const char * pscript_input__get_srcval_rstr(pscript_input_env_t * this, const int input_i) { 
  return (this -> buffer[input_i]) + (this -> token_buffer0[input_i]); 
}; 
 
int pscript_input__get_srcval_len(pscript_input_env_t * this, const int input_i) { 
  return this -> current_buffer0[input_i] - this -> token_buffer0[input_i]; 
}; 
 
void pscript_input__validate(pscript_input_env_t * this, const int input_i) { 
#if 0 
  this -> token_buffer0[input_i] = this -> current_buffer0[input_i]; 
  this -> token_char0  [input_i] = this -> current_char0  [input_i]; 
  //this -> token_line1  [input_i] = this -> current_line1  [input_i]; 
  INT24_PACKED_ASSIGN_PACKED(this -> token_line1  [input_i],this -> current_line1  [input_i]); 
  this -> token_column0[input_i] = this -> current_column0[input_i]; 
#else 
  // RL: First, the fields 'current_line1' & 'current_column0' & 'current_char0' were not maintained. 
  //     So we have to compute them now. 
  int line_buffer_offset0 = -1; 
  int eol_count = 0; 
  for (int i = this -> current_buffer0[input_i] - 1; i >= this -> token_buffer0[input_i]; i--) { 
    const int c = this -> buffer[input_i][i]; 
    if ('\n' == c) { 
      line_buffer_offset0 = i; 
      eol_count++; 
      break; 
    }; 
  }; 
  for (int j = this -> token_buffer0[input_i]; j < line_buffer_offset0; j++) { 
    const int d = this -> buffer[input_i][j]; 
    if ('\n' == d) { 
      eol_count++; 
    }; 
  }; 

  int line1; 
  INT24_PACKED_READ(this -> token_line1[input_i], line1); 
  line1 += eol_count; 
  INT24_PACKED_WRITE(this -> token_line1[input_i], line1); 

  this -> token_char0  [input_i] += (this -> current_buffer0[input_i] - this -> token_buffer0[input_i]); 
  INT24_PACKED_WRITE(this -> token_line1[input_i],line1); 
  if (line_buffer_offset0 >= 0) { 
    this -> token_column0[input_i] = this -> current_buffer0[input_i] - line_buffer_offset0 - 1; // RL: 'line_buffer_offset0' is misnamed - it's actually the offset of the previous '\n'. 
  } 
  else { 
    this -> token_column0[input_i] += (this -> current_buffer0[input_i] - this -> token_buffer0[input_i]); 
  }; 
  this -> token_buffer0[input_i] = this -> current_buffer0[input_i]; 
  
#endif 
}; 
 
 
void pscript_input__repl__turn_off(pscript_input_env_t * this, const int input_i) { 
  this -> repl_huh[input_i] = false; 
}; 
 
void pscript_input__repl__turn_on(pscript_input_env_t * this, const int input_i) { 
  this -> repl_huh[input_i] = true; 
}; 
 
void pscript_input__repl__set_prompt_pstr(pscript_input_env_t * this, const int input_i, const int prompt_pstr) { 
  const char * prompt_cstr = pscript_string__get_cstr(this -> string_env, prompt_pstr); 
  this -> repl_prompt_pstr[input_i] = prompt_pstr; 
  this -> repl_prompt_cstr[input_i] = prompt_cstr; 
}; 

void pscript_input__repl__set_prompt_cstr(pscript_input_env_t * this, const int input_i, const char * prompt_cstr) { 
  const int prompt_pstr = pscript_string__push(this -> string_env, prompt_cstr); 
  pscript_input__repl__set_prompt_pstr(this, input_i, prompt_pstr); 
}; 
 

const char * pscript_input__get_filename_cstr(pscript_input_env_t * this, const int input_i) { 
  const int filename_pstr = this -> filename[input_i]; 
  const char * filename_cstr = pscript_string__get_cstr(this -> string_env, filename_pstr); 
  return filename_cstr; 
}; 

int pscript_input__get_filename_pstr(pscript_input_env_t * this, const int input_i) { 
  const int filename_pstr = this -> filename[input_i]; 
  return filename_pstr; 
}; 

