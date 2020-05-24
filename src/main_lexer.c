// =================================================================================================== 
// LIB 

//enum { debug_level_macro = 100 }; 
//const int debug_level = debug_level_macro; 
//enum { debug_level = 100 }; 
enum { debug_level__compile_time = 000 }; 
static int debug_level = debug_level__compile_time; 
static int verbose_level = -1; 

#if 0 
#include "lib__01__pre_header.ci" 
#include "lib__02__header.ci" 
#include "lib__03.ci"
#include "lib__04__string_stack.ci"
#include "lib__05__error_state.ci"
#include "lib__06__exception.ci"
#endif 

//#include "input_reading.ci" 
#include "pscript_global.h" 
#include "pscript_input.h" 
#include "pscript_token.h" 
#include "pscript_token_type.h" 
#include "pscript_token_automata.h" 
#include "pscript_token_parser.h" 
#include "pscript_chunk.h" 
#include "pscript_parser.h" 
#include "pscript_code_parser.h" 


//=================================================================================================== 
// PROGRAM META 

#define PROGRAM_NAME "PScript (Pascal Script)"
static const char program_name[] = PROGRAM_NAME; 
enum { program_name__cstrlen = ARRAY_SIZE(program_name) - 1 }; 
#define PROGRAM_COPYRIGHT_YEAR1 "2004"
#define PROGRAM_COPYRIGHT_YEAR2 "2020"

enum { PROGRAM_VERSION_MAJOR    = 1 }; 
enum { PROGRAM_VERSION_MINOR    = 1 }; 
enum { PROGRAM_VERSION_REVISION = 0 }; 

static const char program_copyright[] = 
//"ISC License" "\n"
//"http://...." "\n"
//"" "\n"
  //"_Software_ : Pascal Script" "\n"
  //"_Authors_ : Father Christmas" "\n"
  //"_Description_ : ...XXX..." "\n"
  //"" "\n"
PROGRAM_NAME "\n" 
  //"Copyright (c) " PROGRAM_COPYRIGHT_YEAR1 " - " PROGRAM_COPYRIGHT_YEAR2 " _Authors_ (as defined above)" "\n"
"Copyright (c) " PROGRAM_COPYRIGHT_YEAR1 " - " PROGRAM_COPYRIGHT_YEAR2 " " "\n"
"All rights reserved." "\n"
"" "\n"
  //"e-mail to contact the _Authors_ : someone@provider.com" "\n"
  //"" "\n"
  //"" "\n"
"Redistribution and use in source and binary forms, with or without" "\n"
"modification, are permitted provided that the following conditions" "\n"
"are met:" "\n"
"1. Redistributions of source code must retain the above copyright" "\n"
"   notice, this list of conditions and the following disclaimer." "\n"
"2. Redistributions in binary form must reproduce the above copyright" "\n"
"   notice, this list of conditions and the following disclaimer in the" "\n"
"   documentation and/or other materials provided with the distribution." "\n"
"" "\n"
"THIS SOFTWARE IS PROVIDED BY THE _AUTHORS_ ``AS IS'' AND ANY EXPRESS" "\n"
"OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED" "\n"
"WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE" "\n"
"DISCLAIMED. IN NO EVENT SHALL THE _AUTHORS_ BE LIABLE FOR ANY DIRECT," "\n"
"INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES" "\n"
"(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR" "\n"
"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)" "\n"
"HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT," "\n"
"STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING" "\n"
"IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE" "\n"
"POSSIBILITY OF SUCH DAMAGE." "\n" 
"" "\n"
"NB: We mostly do not care of what you do with this code as long as " "\n" 
"    you do not prevent us nor others from using it." "\n"
;
enum { program_copyright__strlen = ARRAY_SIZE(program_copyright) - 1 }; 


static const char program_description[] =     
  "" PROGRAM_NAME ": TODO XXX " "\n" 
//  "  " "If no  input filename is given on the command line, then read from stdin." "\n" 
//  "  " "If no output filename is given on the command line, then write to stdout." "\n"
; 

// =================================================================================================== 
// PROGRAM 
 
enum { 
      PROGRAM_EXIT__SUCCESS = 0, 
      PROGRAM_EXIT__OPTIONS__UNKNOWN, 
      PROGRAM_EXIT__OPTIONS__TOO_MANY_FILENAMES, 
      PROGRAM_EXIT__OPTIONS__CUT_SHORT, 
      PROGRAM_EXIT__CANNOT_OPEN_INPUT_FILE, 
      PROGRAM_EXIT__CANNOT_OPEN_OUTPUT_FILE, 
}; 

static int    input_file_fd   = -1; 
static int   output_file_fd   = -1; 
static str_t  input_file_name = -1; 
static str_t output_file_name = -1; 
 
static int     repl_mode = false; 
#ifdef JE_USE_GNU_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
// NetBSD libedit
//  -> http://www.cs.utah.edu/~bigler/code/libedit.html
//#include <editline.h>
#include <editline/readline.h>
// J'ai une drôle version de editline qui est juste un remplacement de gnu readline.
// Elle a besoin de curses pour linked.
// gnu readline -> gpl
// editline -> bsd
// ncurses -> x11, proche de BSD (BSD-like)
#endif
//  -I /home/romain/projects/libedit-20090923-3.0/destroot/include
// /home/romain/projects/libedit-20090923-3.0/destroot/lib/libedit.a -ltermcap -lm  
//      using_history(); // before anything 
//    tmp = readline("calc> ");
//    add_history(tmp);
//#undef stderr 
//#undef stdin 
//#undef stdout  


 

 
#include "main_options.ci" 



// -------------------------------- 
// PROGRAM 

enum { PSCRIPT_INSTANCE__SIZE = INT8_MAX - 1 }; 
enum { PSCRIPT_INSTANCE__SYSTEM_STACK__SIZE = SIGSTKSZ }; 
static void *       pscript_instance__system_stack[PSCRIPT_INSTANCE__SIZE]; 
static int          pscript_instance__nb = 1; // RL: '0' is the main stack. 
static int          pscript_instance__active = 0; 
static sigjmp_buf   pscript_instance__sjlj_env[PSCRIPT_INSTANCE__SIZE]; 
static sigjmp_buf * pscript_instance__sjlj_being_created = NULL; 
//static int          pscript_instance___being_created_index = INT_MIN; 
static void (* pscript_instance__sjlj_being_started)(void) = NULL; 



static int pscript_instance__detect_current_instance(void) { 
  int a; 
  for (int i = 1; i < pscript_instance__nb; i++) { 
    if ((void *)&a < pscript_instance__system_stack[i]) { continue; }; 
    if ((char *)&a < (char *)pscript_instance__system_stack[i] + PSCRIPT_INSTANCE__SYSTEM_STACK__SIZE) { return i; }; 
  }; 
  return 0; 
}; 


enum { PSCRIPT_INSTANCE__CALLER_STACK__SIZE = INT8_MAX - 1 }; 
static int pscript_instance__caller_stack__array[PSCRIPT_INSTANCE__CALLER_STACK__SIZE]; 
static int pscript_instance__caller_stack__nb = 0; 

static int pscript_instance__caller_stack__push(void) { 
  if (pscript_instance__caller_stack__nb >= PSCRIPT_INSTANCE__CALLER_STACK__SIZE) { return INT_MIN; }; 
  const int index = pscript_instance__caller_stack__nb; 
  pscript_instance__caller_stack__nb++; 
  const int current_instance = pscript_instance__detect_current_instance(); 
  pscript_instance__caller_stack__array[index] = current_instance;
  return current_instance; 
}; 

static int pscript_instance__caller_stack__top(void) { 
  if (pscript_instance__caller_stack__nb <= 0) { return INT_MIN; }; 
  const int index = pscript_instance__caller_stack__nb - 1; 
  return pscript_instance__caller_stack__array[index]; 
}; 

static int pscript_instance__caller_stack__drop(void) { 
  if (pscript_instance__caller_stack__nb <= 0) { return INT_MIN; }; 
  pscript_instance__caller_stack__nb--; 
  return pscript_instance__caller_stack__nb; 
}; 

static int pscript_instance__execute_in_another_instance(const int next_instance, void (*fun)(void)) { 
  // RL: First, we have to save the current instance. 
  //     Then move to the next one. 
  if (next_instance == pscript_instance__active) { 
    fun(); 
    return next_instance; 
  }; 
  
  if (next_instance <= 0) { return INT_MIN; }; 
  
  if (next_instance >= pscript_instance__nb) { return INT_MIN; }; 

  pscript_instance__caller_stack__push(); 
  
  const int current_instance = pscript_instance__active; 

  pscript_instance__sjlj_being_started = fun; 

  if (0 == sigsetjmp(pscript_instance__sjlj_env[pscript_instance__active], /*savemask?*/0)) { 
    // RL: Context saved. We're still in the same context. Now we gonna switch. 
    pscript_instance__active = next_instance; 
    siglongjmp(pscript_instance__sjlj_env[next_instance], 1); 
    // RL: Guard: It's a goto, so we should not come back. 
    abort(); 
  }; 
  // RL: We were switched back. Returning to our business. 
  pscript_instance__sjlj_being_started = NULL; 
  pscript_instance__active = current_instance; 
  return current_instance; 
}; 

static int pscript_instance__switch(const int next_instance) { 
  // RL: First, we have to save the current instance. 
  //     Then move to the next one. 
  if (next_instance == pscript_instance__active) { return next_instance; }; 

  if (next_instance >= pscript_instance__nb) { return INT_MIN; }; 
  
  const int current_instance = pscript_instance__active; 

  if (0 == sigsetjmp(pscript_instance__sjlj_env[pscript_instance__active], /*savemask?*/0)) { 
    // RL: Context saved. We're still in the same context. Now we gonna switch. 
    pscript_instance__active = next_instance; 
    siglongjmp(pscript_instance__sjlj_env[next_instance], 1); 
    // RL: Guard: It's a goto, so we should not come back. 
    abort(); 
  }; 
  // RL: We were switched back. Returning to our business. 
  pscript_instance__active = current_instance; 
  return current_instance; 
}; 




static void pscript_instance__handler_to_capture_sigaltstack(const int /*ignored*/signal_number) { 
  // RL: We are on the new stack. 
  //     We will save it using sigsetjmp. 
  // RL: To save that new stack, we must temporarily use a global variable: 
  //     no parametre can be given to 'raise'. 
  //const int our_index = pscript_instance___being_created_index; 
  //sigjmp_buf * sjlj = pscript_instance__sjlj_env + our_index; 
  //if (0 == sigsetjmp(*sjlj, /*savemask?*/0)) { 
  if (0 == sigsetjmp(*pscript_instance__sjlj_being_created, /*savemask?*/0)) { 
    // RL: We've just saved the current context. 
    //     We won't start it. 
    //     We achieved what we wanted. 
    //     We will start it when we will come back, meaning that the return value of 'sigsetjmp' will be non-zero. 
    //     As such, for the moment, we peacefully return to 'raise', and exit this signal handling context.  
    return; 
  }; 
  
  // RL: We've just come back to this stack. 
  //     We are not in signal handling anymore. 
  // RL: This is the first call to that context. 
  //     So we will call the expected function. 
  // RL: Same thing here: data is globally passed. 
  for (;;) { 
    const int caller_instance = pscript_instance__caller_stack__top(); 
    void (* fun)(void) = pscript_instance__sjlj_being_started; 
    fun(); 
    //dputs_array(stderr__fileno, "BACK\n"); 
    pscript_instance__caller_stack__drop(); 
    pscript_instance__switch(caller_instance);     
  }; 
  
  // RL: The function is not supposed to return. 
  //     Otherwise, we might return to the 'raise' context, I guess, which is not valid anymore. 
  //     Therefore, we put a guard here. 
  //     (The system should have set a trap as well though [returns to 'thrkill']. Even though, the debugger got lost in that situation.) 
  //abort(); 
}; 
 
static int pscript_instance__push(void) { 
  if (pscript_instance__nb >= PSCRIPT_INSTANCE__SIZE) { return INT_MIN; }; 
  
  const int index = pscript_instance__nb; 
  pscript_instance__nb++; 
  
  pscript_instance__sjlj_being_created = pscript_instance__sjlj_env + index; 
  
  int alright_huh = false; 
  do {
    struct sigaction new_handler;
    struct sigaction old_handler;

    stack_t new_stack;
    stack_t old_stack;

    new_stack.ss_flags = 0; 
    new_stack.ss_size  = PSCRIPT_INSTANCE__SYSTEM_STACK__SIZE; 
    new_stack.ss_sp    = malloc(new_stack.ss_size); 
    if (NULL == new_stack.ss_sp) { break; }; 
    pscript_instance__system_stack[index] = new_stack.ss_sp; 
    if (0 != sigaltstack(&new_stack, &old_stack)) { break; }; 
    new_handler.sa_handler = pscript_instance__handler_to_capture_sigaltstack; 
    new_handler.sa_flags   = SA_ONSTACK; 
    sigemptyset(&new_handler.sa_mask); 
    if (0/*success*/ == sigaction(SIGUSR1, &new_handler, &old_handler)) { 
        if (0/*success*/ == raise(SIGUSR1)) { 
	  // RL: We were in the handler, and we came back. 
	  alright_huh = true; 
        }; 
        sigaction(SIGUSR1, &old_handler, NULL); 
    };
    sigaltstack(&old_stack, NULL); 
  } while (false); 

  pscript_instance__sjlj_being_created = NULL; 
  
  if (!alright_huh) { 
    pscript_instance__nb--; 
    return INT_MIN; 
  }; 
  
  return index; 
}; 




static void pscript_instance__entry_point_dummy(void) { 
  int a; 
  //dprintf(stderr__fileno, "Address of a variable on the current stack = %p" "\n", &a); 
  dprintf(stderr__fileno, "pscript_instance__detect_current_instance() = %d" "\n", pscript_instance__detect_current_instance()); 
  //pscript_instance__switch(pscript_instance__active + 1); 
  pscript_instance__execute_in_another_instance(pscript_instance__active + 1, pscript_instance__entry_point_dummy); 
}; 


// RL: Test detecting the current local-global context. 
static int program__local_global(void) { 
  
  pscript_instance__push(); 
  pscript_instance__push(); 
  pscript_instance__push(); 
  pscript_instance__push(); 

#if 0 
  dputs_array(stderr__fileno, "PSCRIPT_INSTANCE__SYSTEM_STACK__SIZE = ", int_string__stack(PSCRIPT_INSTANCE__SYSTEM_STACK__SIZE), "\n"); 
  dputs_array(stderr__fileno, "pscript_instance__nb = ", int_string__stack(pscript_instance__nb), "\n"); 
  for (int i = 0; i < pscript_instance__nb; i++) { 
    dprintf(stderr__fileno, "pscript_instance__system_stack[%d] = %p" "\n", i, pscript_instance__system_stack[i]); 
  }; 

  dprintf(stderr__fileno, "pscript_instance__detect_current_instance() = %d" "\n", pscript_instance__detect_current_instance()); 
#endif 

#if 1
  //static int pscript_instance__execute_in_another_instance(const int next_instance, void (*fun)(void)); 
  pscript_instance__execute_in_another_instance(1, pscript_instance__entry_point_dummy); 
#else   
  pscript_instance__sjlj_being_started = pscript_instance__entry_point_dummy; 
  
  //siglongjmp(pscript_instance__sjlj_env[1], 1); 
  pscript_instance__switch(1); 
  
  pscript_instance__sjlj_being_started = NULL; 
#endif 
  
  return 0; 
}; 


// -------------------------------- 
// PROGRAM 

static int TPoint3D_printz(void * callback_data, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value) { 
  pscript_t * this = callback_data; 
  assert(arg_nb == 1); 
  //assert(argv_core_type[0] == PSCRIPT_TYPE__USER); 
  const int arg_core_type0 = pscript_type__get_core_type(this -> type_env, arg_type[0]); 
  int udf_type0; 
  if (PSCRIPT_TYPE__PROXY == arg_core_type0) { 
    udf_type0 = pscript_type__proxy__get_subtype(this -> type_env, arg_type[0]); 
  } 
  else if (PSCRIPT_TYPE__USER == arg_core_type0) { 
    udf_type0 = arg_type[0]; 
  }
  else {
    assert(false); 
    udf_type0 = -1; 
  }; 
  const char * udf_name_cstr = pscript_type__user_defined_type__get_name_cstr(this -> type_env, udf_type0); 
  assert(0 == strcmp("tpoint3d", udf_name_cstr)); 
  const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, udf_type0); 
  const int subtype_core = pscript_type__get_core_type(this -> type_env, subtype); 
  assert(PSCRIPT_TYPE__RECORD == subtype_core); 
  const int field_pstr = pscript_string__push(this -> string_env, "z"); 
  const int field_index = pscript_type__record__get_field_index(this -> type_env, subtype, field_pstr); 
  assert(field_index >= 0); 
  const int field_type = pscript_type__record__get_field_type(this -> type_env, subtype, field_index); 
  assert(field_type == pscript_type__get__float(this -> type_env)); 
  const int field_offset = pscript_type__record__get_field_offset(this -> type_env, subtype, field_index); 
  const char * udf_value; 
  if (PSCRIPT_TYPE__PROXY == arg_core_type0) {
    const int udf_mem_offset = *(const int_pscript_mem_t *) (arg_mem + arg_offset[0]); 
    udf_value = pscript_mem__get_pointer(this -> mem_env, udf_mem_offset); 
  }
  else if (PSCRIPT_TYPE__USER == arg_core_type0) {
    udf_value = arg_mem + arg_offset[0]; 
  }
  else {
    assert(false); 
    udf_value = NULL; 
  }; 

  const float field_value = *(const float *) (udf_value + field_offset); 
  const char * field_value_cstr = float_string__stack(field_value); 
  PScript_WriteStdout2(this, "The Z value of this TPoint3D object is ", field_value_cstr, ".", "\n"); 
  return 0; 
}; 



int pscript__ffiproc_callback__menu(void * callback_data, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value) { 
  pscript_t * this = callback_data; 
  const char * arg_string[arg_nb]; 
  
  for (int i = 0; i < arg_nb; i++) { 
    //extern const char * pscript_value__cstr  (pscript_type_env_t * type_env, pscript_string_env_t * string_env, const int type1, const int type1_sizeof, const char value1[]); 
    const int    type1        = arg_type[i]; 
    const int    type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
    const char * value1       = arg_mem + arg_offset[i]; 
    //arg_string[i] = pscript_value__get_cstr(this -> type_env, this -> string_env, this -> mem_env, type1, type1_sizeof, value1); 
    arg_string[i]             = pscript_value__get_cstr(this, type1, type1_sizeof, value1); 
    //arg_string[i]           = pscript_value__cstr_raw(this, type1, type1_sizeof, value1); 
  }; 
  
#if 0 
  dputs_array(stderr_d, __func__, "\n"); 
  for (int i = 0; i < arg_nb; i++) { 
    dputs_array(stderr_d, "\t" "arg_string[", int_string__stack(i), "] = ", arg_string[i]); 
    dputs(stderr_d, "\n"); 
  }; 
#endif 

#if 1 
  {
    this -> WriteStdout(this, 2, (const char * []){ arg_string[0], "\n" } ); 
    for (int i = 1; i < arg_nb; i++) { 
      dputs_array(this -> stdout_d, "\t" "[", int_string__stack(i-1), "]" " ", arg_string[i], "\n"); 
    }; 
    enum { buff_size = 32 }; 
    char buffer[buff_size]; 
    *buffer = '\0'; 
    //read(this -> stdin_d, buffer, buff_size); 
    read(stdin_d, buffer, buff_size); 
    const int choice = MAX(-1,MIN(arg_nb-1,atoi(buffer))); 
    bcopy((const char *)&choice, retval_value, MIN(retval_allocated_size,(int)sizeof(choice))); 
    return 0; 
  }; 
#elif 0 
  {
    const char * to_be_printed = strconcat__stack_nb(arg_nb, arg_string); 
    //pmesserr("%s: " "%s" "\n", __func__, to_be_printed); 
    this -> writeln(this, to_be_printed); 
  }; 
#else 
  for (int i = 0; i < argc; i++) {
    dputs(stdout_d, arg_string[i]); 
  }; 
  dputs(stdout_d, "\n"); 
#endif 
  
  *((int *)retval_value) = 8385;
  
  return 0; 
}; 







static int stdlog_d = -1; //dup2(STDERR_FILENO, 3); 


static int program(void) { 
  //dputs(stderr_d, "Hi /pol/!\n"); 
  
  pscript_string_env_t  * string_env; 
  pscript_input_env_t   *  input_env; 
  pscript_token_env_t   *  token_env; 
  //pscript_type_env_t    *   type_env; 
  pscript_parser_env_t  * parser_env; 
  pscript_t * this; 
  
#if 0 
  string_env = pscript_string_env__make(); 
  input_env  = pscript_input_env__make (string_env); 
  token_env  = pscript_token_env__make (string_env); 
  //type_env   = pscript_type_env__make  (string_env); 
  parser_env = pscript_parser_env__make(); 
#else 
  this = NULL; 
  this = pscript_make(); 
  string_env = this -> string_env; 
  input_env  = pscript_input_env__make (string_env); 
  token_env  = this -> token_env; 
  parser_env = pscript_parser_env__make(); 
#endif 
  
  
  const int token_menu = pscript_token__master_push_cstr(token_env, "menu"); 
  const int token_item = pscript_token__syntax_keyword_push_cstr(token_env, "item"); 
  
  //allright_huh = this -> ReadDescriptionFile_fd(this, input_file_fd, string__get(input_file_name)); 
  
  const int input_i = pscript_input__make_from_fileno(input_env, input_file_fd, string__get(input_file_name)); 
  assert(input_i >= 0); 
  
  do { 
    int allright_huh; 
    allright_huh = pscript_token__parser(input_env, input_i, token_env); 
    if (not(allright_huh)) { 
      dputs_array(stderr_d, "Something got wrong.\n"); 
      break; 
    }; 
    
    
    if (false) { 
      dputs_array(stderr_d, "pscript_string_env__sizeof: ", int_string__stack(pscript_string_env__sizeof), "\n"); 
      pscript_string_env_t * restored_string_env = pscript_string_env__make(); 
      allright_huh = pscript_string__restore_raw(stdin_d, restored_string_env); 
      dputs_array(stderr_d, "pscript_string__restore_raw: ", int_string__stack(allright_huh), "\n"); 
      assert(allright_huh >= 0); 
      const int bcmp_res = bcmp(string_env, restored_string_env, pscript_string_env__sizeof); //sizeof(*string_env)); 
      dputs_array(stderr_d, "bcmp: ", bool_string(0 == bcmp_res), "\n"); // RL: Success! :-) 
      //dputs_array(stderr_d, "TOTAL COMPUTED STRINGS: ", int_string__stack(pscript_string__get_count(string_env)), "\n"); 
      //dputs_array(stderr_d, "TOTAL COMPUTED STRINGS: ", int_string__stack(pscript_string__get_count(restored_string_env)), "\n"); 
      //pscript_string__print_all_strings(stderr_d, string_env); 
      //pscript_string__print_all_strings(stderr_d, restored_string_env); 
      break; 
    }; 
    if (false) { 
      pscript_string__dump_raw(stdout_d, string_env); 
      break; 
    }; 

    if (false) { 
      dputs_array(stderr_d, "pscript_token_env__sizeof: ", int_string__stack(pscript_token_env__sizeof), "\n"); 
      pscript_token_env_t * restored_token_env = pscript_token_env__make(string_env); 
      allright_huh = pscript_token__restore_raw(stdin_d, restored_token_env); 
      dputs_array(stderr_d, "pscript_token__restore_raw: ", int_string__stack(INT_MAX & allright_huh), "\n"); 
      assert(allright_huh >= 0); 
      const int bcmp_res = bcmp(token_env, restored_token_env, pscript_token_env__sizeof); //sizeof(*token_env)); 
      dputs_array(stderr_d, "bcmp: ", bool_string(0 == bcmp_res), "\n"); // RL: Success! :-) 
      break; 
    }; 
    if (false) { 
      pscript_token__dump_raw(stdout_d, token_env); 
      break; 
    }; 
    
    

    //dputs_array(stderr_d, "TOTAL READ CHARS: ", int_string__stack(token_env -> char_nb), "\n"); 
    //dputs_array(stderr_d, "TOTAL READ LINES: ", int_string__stack(token_env -> line_nb), "\n"); 
    dputs_array(stderr_d, "TOTAL READ CHARS: ", int_string__stack(input_env -> cumulated_read_nb[input_i]), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED STRINGS: ", int_string__stack(pscript_string__get_count(string_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED TOKENS: ",  int_string__stack(pscript_token__get_count(token_env)), "\n"); 
    pscript_string__print_stats(stderr_d, string_env); 
    pscript_token__print_stats(stderr_d, token_env); 
#if 1
    pscript_string__print_all_strings(stdlog_d, string_env); 
    pscript_token__print_all_tokens(stdlog_d, token_env); 
#endif 

    if (allright_huh < 0) { return allright_huh; }; 

  
    
    // RL: SYNTAX ANALYSIS 

    // RL: Adding the 'menu' syntax. 
    { 
      const int node__item_arg_list__tail  = pscript_parser__custom_syntax__node_push(parser_env); 
      const int node__item_arg_list__head  = pscript_parser__custom_syntax__node_push(parser_env); 
      const int node__item_content         = pscript_parser__custom_syntax__node_push(parser_env); 
      const int node__item_list            = pscript_parser__custom_syntax__node_push(parser_env); 
      //const int node__menu_title           = pscript_parser__custom_syntax__node_push(parser_env); 
      const int node__menu2                = pscript_parser__custom_syntax__node_push(parser_env); 
      const int node__menu                 = pscript_parser__custom_syntax__node_push(parser_env); 
      
      const int rule__item_arg_list__tail2 = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_arg_list__tail, PSCRIPT_TOKEN_COMMA, 0, node__item_arg_list__tail); 
      const int rule__item_arg_list__tail1 = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_arg_list__tail); 
      const int rule__item_arg_list__head  = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_arg_list__head, 0, node__item_arg_list__tail); 
      const int rule__item_content__cons   = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_content, PSCRIPT_TOKEN_OPENPAR, node__item_arg_list__head, PSCRIPT_TOKEN_CLOSEPAR); 
      //const int rule__item__cons2         = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item, PSCRIPT_TOKEN_OPENPAR, node__item_arg_list, PSCRIPT_TOKEN_CLOSEPAR, PSCRIPT_TOKEN_PTVIRG); 
      //const int rule__item__cons3         = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item, node__item_arg_list); 
      const int rule__item_list__cons2     = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_list, token_item, node__item_content, PSCRIPT_TOKEN_PTVIRG, node__item_list); 
      const int rule__item_list__cons1     = pscript_parser__custom_syntax__rule_push_array(parser_env, node__item_list); 
      //const int rule__menu_title__cons1   = pscript_parser__custom_syntax__rule_push_array(parser_env, node__menu_title, PSCRIPT_TOKEN_STRING_C); 
      //const int rule__menu_title__cons2   = pscript_parser__custom_syntax__rule_push_array(parser_env, node__menu_title, PSCRIPT_TOKEN_STRING_P); 
      //const int rule__menu__cons1         = pscript_parser__custom_syntax__rule_push_array(parser_env, node__menu, token_menu, node__menu_title, PSCRIPT_TOKEN_BEGIN, node__item_list, PSCRIPT_TOKEN_END); 
      const int rule__menu2__cons1         = pscript_parser__custom_syntax__rule_push_array(parser_env, node__menu2, 0, PSCRIPT_TOKEN_BEGIN, node__item_list, PSCRIPT_TOKEN_END); 
      const int rule__menu__cons1          = pscript_parser__custom_syntax__rule_push_array(parser_env, node__menu, token_menu, node__menu2); 
      
#if 1 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__tail2, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 1, 2); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__tail1, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__head, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 1); 
      //pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_content__cons, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_content__cons, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons2, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 1, 3); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons1, 0, -1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu2__cons1, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 2); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu__cons1, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, 0, 1); 
#elif 1 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__tail2, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 1, 2); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__tail1, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__head, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_content__cons, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons2, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 1, 3); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons1, 0, -1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu2__cons1, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 2); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu__cons1, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, 0, 1); 
#else
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__tail, 0, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_arg_list__cons, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 2); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item__cons1, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item__cons2, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item__cons3, 0, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons1, 0, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__item_list__cons2, PSCRIPT_EXPR_TREE__TYPE__COMMA_LIST, 0, 1); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu_title__cons1, 0, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu_title__cons2, 0, 0); 
      pscript_parser__custom_syntax__tree_set__array(parser_env, rule__menu__cons1, PSCRIPT_EXPR_TREE__TYPE__FUNCALL, 0, 2); 
#endif 
      
      pscript_parser__custom_syntax__master_root_set(parser_env, token_menu, rule__menu__cons1); 
    }; 
      
    
    allright_huh = pscript_parser__parenthesis__check(token_env); 
    if (allright_huh < 0) { return allright_huh; }; 
    
    allright_huh = pscript_parser__automaton(parser_env, token_env); 
    if (allright_huh < 0) { 
      dputs_array(stderr_d, "Something got wrong: ", int_string__stack(allright_huh), "\n"); 
      break; 
    }; 
#if 1 
    dputs_array(stdlog_d, "//Parsing was successful: ", int_string__stack(allright_huh), "\n"); 
#endif 

#if 1 
    pscript_parser__print_all(stdlog_d, parser_env); 
#endif 
    
    pscript_parser__print_stats(stderr_d, parser_env); 

#if 1 
    pscript_parser__print_pretty(stdlog_d, string_env, token_env, parser_env); 
#endif 



  {
    const int ffiproc_name_pstr = pscript_string__push(this -> string_env, "menu"); 
    const int retval_type_i     = pscript_type__get__integer(this -> type_env); 
    const int ffiproc_i         = pscript_ffiproc__push(this -> ffiproc_env, ffiproc_name_pstr, /*arg_atleast*/1, /*retval_type*/retval_type_i, /*void callback_data*/this, /*callback*/pscript__ffiproc_callback__menu); 
    pscript_env__push_ffi(this -> env, ffiproc_name_pstr, ffiproc_i); 
  }; 



    allright_huh = pscript_code_parser(this, parser_env); 
    if (allright_huh < 0) { 
      dputs_array(stderr_d, "Something got wrong: ", int_string__stack(allright_huh), "\n"); 
      break; 
    }; 



    dputs_array(stderr_d, "TOTAL COMPUTED TYPES: "     , int_string__stack(pscript_type__get_count    (this ->    type_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED CONSTANTS: " , int_string__stack(pscript_const__get_count   (this ->    const_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED VARIABLES: " , int_string__stack(pscript_var__get_count     (this ->      var_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED FUNPROC: "   , int_string__stack(pscript_funproc__get_count (this ->  funproc_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED MAINPROC: "  , int_string__stack(pscript_mainproc__get_count(this -> mainproc_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED FFIPROC: "   , int_string__stack(pscript_ffiproc__get_count(this -> ffiproc_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED LIBPROC: "   , int_string__stack(pscript_libproc__get_count(this -> libproc_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED GLOBAL ENV: ", int_string__stack(pscript_env__get_count(this -> env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED BYTEDATA(BSS): "  , int_string__stack(pscript_mem__toplevel_compiletime__bss__used(this -> mem_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED BYTEDATA(ZSS): "  , int_string__stack(pscript_mem__toplevel_compiletime__zss__used(this -> mem_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED BYTEDATA(ISS): "  , int_string__stack(pscript_mem__toplevel_compiletime__iss__used(this -> mem_env)), "\n"); 
    dputs_array(stderr_d, "TOTAL COMPUTED BYTECODES: " , int_string__stack(pscript_bytecode__get_count(this -> bytecode_env)), "\n"); 

#if 0 
    funproc_stack__print_all(this -> funproc_env, this -> string_env); 
    mainproc_stack__print_all(this -> mainproc_env, this -> string_env); 
    //pscript_env__print_all(this -> env, this -> string_env); 
#endif 
    bytecode_stack__print_all(stdlog_d, this -> bytecode_env, this -> type_env); 


    
#if 1 
    // RL: 'printz' 
    { 
      const int ffiproc_name = pscript_string__push(this -> string_env, "printz"); 
    const int void_type_i = pscript_type__get__void(this -> type_env); 
    const int ffiproc_i = pscript_ffiproc__push(this -> ffiproc_env, ffiproc_name, /*arg_atleast*/0, /*retval_type*/void_type_i, /*callback_data*/this, /*callback*/TPoint3D_printz); 
    
    const int type_name = pscript_string__push(this -> string_env, "tpoint3d"); 
    const int type_name_i = pscript_type__name__lookup(this -> type_env, type_name); 
    const int type_i = pscript_type__name__get_value(this -> type_env, type_name_i); 
    const int method_i = pscript_type__methode__push_ffiproc(this -> type_env, type_i, ffiproc_name, ffiproc_i); 
    
    pscript_ffiproc__set_method_of(this -> ffiproc_env, ffiproc_i, method_i); 
  }; 
#endif 


    
    
    { 
      yypscript_global_objet_being_filled_up_while_parsing = this; // RL: For 'pmesserr' to work. 
      const int mainproc_nb = pscript_mainproc__get_count(this -> mainproc_env); 
      for (int i = 0; i < mainproc_nb; i++) { 
	const int offset = pscript_mainproc__get_bytecode_offset(this -> mainproc_env, i); 
	allright_huh = this -> vm -> run(this -> vm, offset); 
	if (allright_huh == PSCRIPT_VM__ERROR__HALT) { continue; }; 
	this -> vm -> print_error(this -> vm, allright_huh); 
	break; 
      }; 
    }; 
    
    

#if 0   
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__CHARS_COUNT]) { 
    dputs(stderr_d, "TOTAL READ CHARS: "); 
    dputn(stderr_d, this -> token_env -> char_nb); 
    dputs(stderr_d, "\n"); 
  }; 
  
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__LINES_COUNT]) { 
    dputs(stderr_d, "TOTAL READ LINES: "); 
    dputn(stderr_d, this -> token_env -> line_nb); 
    dputs(stderr_d, "\n"); 
  }; 
  
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_COUNT]) { 
    dputs(stderr_d, "TOTAL COMPUTED TOKENS: "); 
    dputn(stderr_d, this -> token_env -> token_stack__nb); 
    dputs(stderr_d, "\n"); 
  }; 
 
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_SHORT]) { 
    token_stack__print_nb_tokens(this -> token_env, 127); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 
  
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_ALL]) { 
    token_stack__print_all_tokens(this -> token_env); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 
  
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_COUNT]) { 
    dputs_array(stderr_d, "TOTAL COMPUTED BYTECODES: ", int_string__stack(pscript_bytecode__get_count(this -> bytecode_env)), "\n"); 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_SHORT]) { 
    bytecode_stack__print_nb(this -> bytecode_env, this -> type_env, 127); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_ALL]) { 
    bytecode_stack__print_all(this -> bytecode_env, this -> type_env); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_COUNT]) { 
    dputs_array(stderr_d, "TOTAL COMPUTED FUNPROC: ", int_string__stack(pscript_funproc__get_count(this -> funproc_env)), "\n"); 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_SHORT]) { 
    funproc_stack__print_nb(this -> funproc_env, this -> string_env, 127); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_ALL]) { 
    funproc_stack__print_all(this -> funproc_env, this -> string_env); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_COUNT]) { 
    dputs_array(stderr_d, "TOTAL COMPUTED MAINPROC: ", int_string__stack(pscript_mainproc__get_count(this -> mainproc_env)), "\n"); 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_SHORT]) { 
    mainproc_stack__print_nb(this -> mainproc_env, this -> string_env, 127); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 

  if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_ALL]) { 
    mainproc_stack__print_all(this -> mainproc_env, this -> string_env); 
    //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
  }; 
  
  if (0 < program_options[PROGRAM_OPTIONS_INDEX__FFIPROC_COUNT]) { 
    dputs_array(stderr_d, "TOTAL COMPUTED FFIPROC: ", int_string__stack(pscript_ffiproc__get_count(this -> ffiproc_env)), "\n"); 
  }; 
#endif 
  
  
  } while (false); 
  
  
  
  pscript_input__delete(input_env, input_i); 
  
  pscript_parser_env__delete(parser_env); 
  pscript_token_env__delete(token_env); 
  pscript_input_env__delete(input_env); 
  pscript_string_env__delete(string_env); 
  
  return 0; 
}; 
 
 
// =================================================================================================== 
// MAIN 




static int main_program(const int argc, const char * argv[]) {
  int allright_huh; 

  allright_huh = program_options__parse(argc, argv); 
  if (0 != allright_huh) { return allright_huh; }; 
  
  allright_huh = program_options__analyse(argc, argv); 
  if (0 != allright_huh) { return allright_huh; }; 



  input_file_fd   = stdin_d;
  output_file_fd  = stdout_d; 
  input_file_name = string__push_or_lookup("<stdin>"); 
  output_file_name = string__push_or_lookup("<stdout>"); 
  //repl_mode = (1 == isatty(stdin)); 
  repl_mode = false; 

  if (1 <= program_options__filenames__nb) {
    const char * filename = argv[program_options__filenames[0]]; 
    do {
      if (('-' == *filename) && ('\0' == *(filename+1))) { break; }; 
      input_file_fd = open(filename, O_RDONLY); 
      if (0 > input_file_fd) {
	dputs_array(stderr_d, "Cannot open input file: ", filename, "\n"); 
	return PROGRAM_EXIT__CANNOT_OPEN_INPUT_FILE; 
      }; 
      input_file_name = string__push_or_lookup(filename); 
      repl_mode = false; 
    } while (false); 
  }; 
  if (2 <= program_options__filenames__nb) {
    const char * filename = argv[program_options__filenames[1]]; 
    do {
      if (('-' == *filename) && ('\0' == *(filename+1))) { break; }; 
      output_file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777); 
      if (0 > output_file_fd) {
	dputs_array(stderr_d, "Cannot open output file: ", filename, "\n"); 
	return PROGRAM_EXIT__CANNOT_OPEN_OUTPUT_FILE;
      }; 
      output_file_name = string__push_or_lookup(filename); 
    } while (false); 
  }; 
  
  
  if (repl_mode) { 
    using_history(); 
  }; 
  
  int allright; 
  do {
    allright = program(); 
  } while (repl_mode); 
  
  return allright; 
  //return PROGRAM_EXIT__SUCCESS; 
}; 


static void main_checks(void) { 
};

static void main_arg_print(const int argc, const char * argv[]) {
  dputs(stderr_d, "argc: "); dputn(stderr_d, argc); dputs(stderr_d, "\n"); 
  for (int i = 0; i < argc; i++) {
    dputs(stderr_d, "argv["); dputn(stderr_d, i); dputs(stderr_d, "] = "); dputs(stderr_d, argv[i]); dputs(stderr_d, "\n"); 
  }; 
}; 

#if 1
extern int main(const int argc, const char * argv[]) {
#if 0 
  dputs_array(stdout_d, "getdtablecount() = ", int_string__stack(getdtablecount()), "\n");
#endif 
#if 1 
  if (-1 == fcntl(3, F_GETFL) && errno == EBADF) { 
    stdlog_d = open("/dev/tty", O_WRONLY);  
  } 
  else { 
    stdlog_d = 3; 
  }; 
#if 0 
  dputs_array(stdout_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stderr_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stdlog_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  abort(); 
#endif 
#elif 
  stdlog_d = open("/dev/tty", O_WRONLY);  
  dputs_array(stdout_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stderr_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stdlog_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  abort(); 
#elif 0 
  stdlog_d = dup2(STDERR_FILENO, 3); 
  dputs_array(stdout_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stderr_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stdlog_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  abort(); 
#endif 
  srandomdev();
  exception__raz(); 
  if (false) { main_arg_print(argc, argv); }; 
  main_checks(); 
  return main_program(argc, argv); 
};
#elif 0

static int main__things(const int argc, const char * argv[]) {
  //printf("%d", INT64_MAX+1); exit(0); 
  //dputn(stderr_d, INT64_MAX+1); exit(0); 
  dputn(stderr_d, -INT32_MIN); exit(0); 
  dputn(stderr_d, INT_MAX); exit(0); 
  dputn(stderr_d, INT_BITSIZE); exit(0); 
  dputn(stderr_d, INT_BYTESIZE); exit(0); 
  dputn(stderr_d, INT32_MAX); exit(0); 
  dputn(stderr_d, (int) INT64_MAX); exit(0); 
  dputn(stderr_d, (int) INT64_MIN); exit(0); 
  return 0;
};
#else 

extern int main(const int argc, const char * argv[]) {
#if 0 
  ASSERT_COMPILE((ARRAY_SIZE(syntax_tree__type__string) == (SYNTAX_TREE__TYPE__COUNT+1))); 
#endif 
  dputn(stderr_d, ARRAY_SIZE(syntax_tree__type__string)); dput_eol(stderr); 
  dputn(stderr_d, SYNTAX_TREE__TYPE__COUNT); dput_eol(stderr); 


  dputn(stderr_d, TOKEN_TYPE__COUNT); dput_eol(stderr); 
  dputn(stderr_d, SYNTAX_TREE__TYPE__); dput_eol(stderr); 
  dputn(stderr_d, SYNTAX_TREE__TYPE__AXIOME); dput_eol(stderr); 
  dputn(stderr_d, SYNTAX_TREE__TYPE__COUNT_SHIFTED); dput_eol(stderr); 

  return 0; 
}; 
  

#endif 
