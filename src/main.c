// =================================================================================================== 
// LIB 

//enum { debug_level_macro = 100 }; 
//const int debug_level = debug_level_macro; 
//enum { debug_level = 100 }; 
enum { debug_level__compile_time = 000 }; 
static int debug_level = debug_level__compile_time; 
static int verbose_level = -1; 

#include "lib__01__pre_header.ci" 
#include "lib__02__header.ci" 
#include "lib__03.ci"
#include "lib__04__string_stack.ci"
#include "lib__05__error_state.ci"
#include "lib__06__exception.ci"


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
//#include "input_reading.ci" 
#include "pscript.h" 

 


 
 
// -------------------------------- 
// PROGRAM 



static pscript_t * this = NULL; 



static int TPoint3D_printz__OLD(void * callback_data, const int argc, const char argv_core_type[], const int bool_nb, const char bool_stack[], const int integer_nb, const int integer_stack[], const int float_nb, const float float_stack[], const int char_nb, const char char_stack[], const int string_nb, const int string_stack[], const int pointer_nb, const int pointer_type[], const int pointer_stack[], const int proxy_nb, const int proxy_type[], const int proxy_stack[], const int array_nb, const int array_type[], const int array_offset[], const int array_mem_size, const char array_mem[], const int record_nb, const int record_type[], const int record_offset[], const int record_mem_size, const char record_mem[], const int udf_nb, const int udf_type[], const int udf_offset[], const int udf_mem_size, const char udf_mem[]) {
  pscript_t * this = callback_data; 
  assert(argc == 1); 
  //assert(argv_core_type[0] == PSCRIPT_TYPE__USER); 
  int udf_type0; 
  if (PSCRIPT_TYPE__PROXY == argv_core_type[0]) {
    udf_type0 = pscript_type__proxy__get_subtype(this -> type_env, proxy_type[0]); 
  }
  else if (PSCRIPT_TYPE__USER == argv_core_type[0]) {
    udf_type0 = udf_type[0]; 
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
  if (PSCRIPT_TYPE__PROXY == argv_core_type[0]) {
    const int udf_mem_offset = proxy_stack[0]; 
    udf_value = pscript_mem__get_pointer(this -> mem_env, udf_mem_offset); 
  }
  else if (PSCRIPT_TYPE__USER == argv_core_type[0]) {
    udf_value = udf_mem + udf_offset[0]; 
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
    const int udf_mem_offset = *(const int *) arg_mem + arg_offset[0]; 
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


 
static int stdlog_d = -1; //dup2(STDERR_FILENO, 3); 

static int program(void) {
  //input_reading__init(); 
  int allright_huh; 
  
  //pscript_t * this = NULL; 
  this = NULL; 
  this = pscript_make(); 
  
#if 1 
  assert(stdlog_d >= 0); 
  this -> stdlog__set__fdes(this, stdlog_d); 
#else   
  this -> stdlog__set__fdes(this, stderr); 
#endif 
  
#if 0  
  const int ffiproc_name = pscript_string__push(this -> string_env, "writeln"); 
  const int ffiproc_i = pscript_ffiproc__push(this -> ffiproc_env, ffiproc_name, 0, pscript_ffiproc_callback__writeln); 
  pscript_env__push_ffi(this -> env, ffiproc_name, ffiproc_i); 
#endif 

  
#if 1
  const int input_i = pscript_input__make_from_fileno(this -> input_env, input_file_fd, string__get(input_file_name)); 
  assert(input_i >= 0); 
#elif 0 
  allright_huh = this -> ReadDescriptionFile_fd(this, input_file_fd, string__get(input_file_name)); 
  if (allright_huh < 0) { return allright_huh; }; 
#else   
  allright_huh = this -> ReadDescriptionFile(this, "example.pas"); 
  if (allright_huh < 0) { return allright_huh; }; 
#endif 


  
  do { 
    allright_huh = pscript_token__parser(this -> input_env, input_i, this -> token_env); 
    if (not(allright_huh)) { 
      dputs_array(stderr_d, "Something got wrong.\n"); 
      break; 
    }; 
    
    

  
  
  
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__CHARS_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL READ CHARS: ", int_string__stack(this -> input_env -> cumulated_read_nb[input_i]), "\n"); 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__LINES_COUNT]) { 
      //dputs_array(stdlog_d, "TOTAL READ LINES: ", int_string__stack(this -> input_env -> cumulated_read_nb[input_i]), "\n"); 
      dputs(stdlog_d, "TOTAL READ LINES: "); 
      dputs(stdlog_d, "TODO XXX FIXME "); 
      //dputn(stdlog_d, this -> token_env -> line_nb); 
      dputs(stdlog_d, "\n"); 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__STRING_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED STRINGS: ",  int_string__stack(pscript_string__get_count(this -> string_env)), "\n"); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__STRING_STATS]) { 
      pscript_string__print_stats(stdlog_d, this -> string_env); 
    }; 
  
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__STRING_SHORT]) { 
      //string_stack__print_nb_strings(this -> string_env, 127); 
      pscript_string__print_nb_strings(stdlog_d, this -> string_env, 127); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__STRING_ALL]) { 
      //string_stack__print_all_strings(this -> string_env); 
      pscript_string__print_all_strings(stdlog_d, this -> string_env); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED TOKENS: ",  int_string__stack(pscript_token__get_count(this -> token_env)), "\n"); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
    
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_STATS]) { 
      pscript_token__print_stats(stdlog_d, this -> token_env); 
    }; 
 
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_SHORT]) { 
      //token_stack__print_nb_tokens(this -> token_env, 127); 
      pscript_token__print_nb_tokens(stdlog_d, this -> token_env, 127); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
  
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__TOKEN_ALL]) { 
      //token_stack__print_all_tokens(this -> token_env); 
      pscript_token__print_all_tokens(stdlog_d, this -> token_env); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 



    
    // RL: Syntax analysis & Tree generation  
    allright_huh = pscript_parser__parenthesis__check(this -> token_env); 
    if (allright_huh < 0) { return allright_huh; }; 
    
    allright_huh = pscript_parser__automaton(this -> parser_env, this -> token_env); 
    if (allright_huh < 0) { 
      dputs_array(stderr_d, "Something got wrong: ", int_string__stack(allright_huh), "\n"); 
      break; 
    }; 


  
  
    // RL: Code generation 
    // RL: TODO XXX FIXME: Separate code generation & symbol table filling 
    allright_huh = pscript_code_parser(this, this -> parser_env); 
    if (allright_huh < 0) { 
      dputs_array(stderr_d, "Something got wrong: ", int_string__stack(allright_huh), "\n"); 
      break; 
    }; 
    
#if 0 
    dputs_array(stdlog_d, "TOTAL COMPUTED TYPES: "     , int_string__stack(pscript_type__get_count    (this ->    type_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED CONSTANTS: " , int_string__stack(pscript_const__get_count   (this ->    const_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED VARIABLES: " , int_string__stack(pscript_var__get_count     (this ->      var_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED FUNPROC: "   , int_string__stack(pscript_funproc__get_count (this ->  funproc_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED MAINPROC: "  , int_string__stack(pscript_mainproc__get_count(this -> mainproc_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED FFIPROC: "   , int_string__stack(pscript_ffiproc__get_count(this -> ffiproc_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED LIBPROC: "   , int_string__stack(pscript_libproc__get_count(this -> libproc_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED GLOBAL ENV: ", int_string__stack(pscript_env__get_count(this -> env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED BYTEDATA(BSS): "  , int_string__stack(pscript_mem__toplevel_compiletime__bss__used(this -> mem_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED BYTEDATA(ZSS): "  , int_string__stack(pscript_mem__toplevel_compiletime__zss__used(this -> mem_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED BYTEDATA(ISS): "  , int_string__stack(pscript_mem__toplevel_compiletime__iss__used(this -> mem_env)), "\n"); 
    dputs_array(stdlog_d, "TOTAL COMPUTED BYTECODES: " , int_string__stack(pscript_bytecode__get_count(this -> bytecode_env)), "\n"); 
#endif 


    if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED BYTECODES: ", int_string__stack(pscript_bytecode__get_count(this -> bytecode_env)), "\n"); 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_SHORT]) { 
      bytecode_stack__print_nb(stdlog_d, this -> bytecode_env, this -> type_env, 127); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__BYTECODE_ALL]) { 
      bytecode_stack__print_all(stdlog_d, this -> bytecode_env, this -> type_env); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED FUNPROC: ", int_string__stack(pscript_funproc__get_count(this -> funproc_env)), "\n"); 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_SHORT]) { 
      funproc_stack__print_nb(stdlog_d, this -> funproc_env, this -> string_env, 127); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__FUNPROC_ALL]) { 
      funproc_stack__print_all(stdlog_d, this -> funproc_env, this -> string_env); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED MAINPROC: ", int_string__stack(pscript_mainproc__get_count(this -> mainproc_env)), "\n"); 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_SHORT]) { 
      mainproc_stack__print_nb(stdlog_d, this -> mainproc_env, this -> string_env, 127); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 

    if (0 < program_options[PROGRAM_OPTIONS_INDEX__MAINPROC_ALL]) { 
      mainproc_stack__print_all(stdlog_d, this -> mainproc_env, this -> string_env); 
      //return PROGRAM_EXIT__OPTIONS__CUT_SHORT; 
    }; 
  
    if (0 < program_options[PROGRAM_OPTIONS_INDEX__FFIPROC_COUNT]) { 
      dputs_array(stdlog_d, "TOTAL COMPUTED FFIPROC: ", int_string__stack(pscript_ffiproc__get_count(this -> ffiproc_env)), "\n"); 
    }; 

  
  
#if 1 
    { 
      const int ffiproc_name_pstr = pscript_string__push(this -> string_env, "printz"); 
      const int void_type_i       = pscript_type__get__void(this -> type_env); 
      const int ffiproc_i         = pscript_ffiproc__push(this -> ffiproc_env, ffiproc_name_pstr, /*arg_atleast*/0, /*retval_type*/void_type_i, /*callback_data*/this, /*callback*/TPoint3D_printz); 
      
      const int type_name_pstr = pscript_string__push(this -> string_env, "tpoint3d"); 
      const int type_name_i    = pscript_type__name__lookup(this -> type_env, type_name_pstr); 
      const int type_i         = pscript_type__name__get_value(this -> type_env, type_name_i); 
      const int method_i       = pscript_type__methode__push_ffiproc(this -> type_env, type_i, ffiproc_name_pstr, ffiproc_i); 
      
      pscript_ffiproc__set_method_of(this -> ffiproc_env, ffiproc_i, method_i); 
    }; 
#endif 
  
  
#if 0 
    fprintf(stdlog_d, "================ ENV PRINT =========" "\n"); 
    pscript_env__print_all(this -> env, this -> string_env); 
#endif 

  
#if 1
    const int mainproc_nb = pscript_mainproc__get_count(this -> mainproc_env); 
    for (int i = 0; i < mainproc_nb; i++) { 
      const int offset = pscript_mainproc__get_bytecode_offset(this -> mainproc_env, i); 
      allright_huh = this -> vm -> run(this -> vm, offset); 
      if (allright_huh == PSCRIPT_VM__ERROR__HALT) { continue; }; 
      this -> vm -> print_error(this -> vm, allright_huh); 
      break; 
    }; 
#elif 
    dputs_array(stderr__fileno, "INT8_MAX: ", int_string__stack(INT8_MAX), "\n"); 
    dputs_array(stderr__fileno, "INT8_MAX: ", int_string__stack(~(1 << 7)), "\n"); 
    dputs_array(stderr__fileno, "INT8_MAX: ", int_string__stack((1 << 7)), "\n"); 
    dputs_array(stderr__fileno, "INT8_MAX: ", int_string__stack(-(1 << 7)), "\n"); 
    const int mainproc_nb = pscript_mainproc__get_count(this -> mainproc_env); 
    dputs_array(stderr__fileno, "MAINPROC_NB: ", int_string__stack(mainproc_nb), "\n"); 
    for (int i = 0; i < mainproc_nb; i++) { 
      dputs_array(stderr__fileno, "MAINPROC_I: ", int_string__stack(i), "\n"); 
      const int offset = pscript_mainproc__get_bytecode_offset(this -> mainproc_env, i); 
      allright_huh = this -> vm -> run(this -> vm, offset); 
      dputs_array(stderr__fileno, "VM_RETURN: ", int_string__stack(allright_huh), "\n"); 
      dputs_array(stderr__fileno, "VM_RETURN: ", pscript_vm__error__cstr(allright_huh), "\n"); 
      dputs_array(stderr__fileno, "PSCRIPT_VM__ERROR__HALT: ", int_string__stack(PSCRIPT_VM__ERROR__HALT), "\n"); 
      if (allright_huh == PSCRIPT_VM__ERROR__HALT) { continue; }; 
      this -> vm -> print_error(this -> vm, allright_huh); 
      break; 
    }; 
#endif 
  
  
  
#if 0  
    error_state_t error_state; 
    allright_huh = token_analyzer(error_state); 
    allright_huh = syntax_tree__parser(); 
    allright_huh = bytecode__generate(); 
    const char * error_cstr = vm__run(); 
    if (NULL != error_cstr) { 
      dputs(stderr_d, error_cstr); 
      dput_eol(stderr); 
    }; 
    if (vm__data_stack__nb > 0) { 
      for (int i = 0; i < vm__data_stack__nb; i++) { 
	dputn(stdout, vm__data_stack__array[i]); 
	dputc(stdout, ' '); 
      }; 
      dput_eol(stdout); 
    }; 
#endif 

  } while (false); 
  
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



  input_file_fd    = stdin_d;
  output_file_fd   = stdout_d; 
  input_file_name  = string__push_or_lookup("<stdin>"); 
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
  dputs_array(stdout, "getdtablecount() = ", int_string__stack(getdtablecount()), "\n");
#endif 
#if 1 
  if (-1 == fcntl(3, F_GETFL) && errno == EBADF) { 
    stdlog_d = open("/dev/tty", O_WRONLY);  
  } 
  else { 
    stdlog_d = 3; 
  }; 
#if 0 
  dputs_array(stdout, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stderr_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stdlog_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  abort(); 
#endif 
#elif 
  stdlog_d = open("/dev/tty", O_WRONLY);  
  dputs_array(stdout, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stderr_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  dputs_array(stdlog_d, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
  abort(); 
#elif 0 
  stdlog_d = dup2(STDERR_FILENO, 3); 
  dputs_array(stdout, "stdlog_d = ", int_string__stack(stdlog_d), "\n");
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
