#include "pscript_global.h"
#include "pscript.h"
#include "pscript_value.h"
#include "pscript_token_automata.h"
#include "pvm_bytecode__type.ci"


// RL: TODO XXX FIXME: To be removed 
pscript_t * yypscript_global_objet_being_filled_up_while_parsing = NULL; 


#if 1
static int  pscript__ffiproc_callback__writeln(void * callback_data, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value); 
#else
static int pscript__ffiproc_callback__writeln(void * callback_data, const int argc, const char argv_core_type[], const int bool_nb, const char bool_stack[], const int integer_nb, const int integer_stack[], const int float_nb, const float float_stack[], const int char_nb, const char char_stack[], const int string_nb, const int string_stack[], const int pointer_nb, const int pointer_type[], const int pointer_stack[], const int proxy_nb, const int proxy_type[], const int proxy_stack[], const int array_nb, const int array_type[], const int array_offset[], const int array_mem_size, const char array_mem[], const int record_nb, const int record_type[], const int record_offset[], const int record_mem_size, const char record_mem[], const int udf_nb, const int udf_type[], const int udf_offset[], const int udf_mem_size, const char udf_mem[]); 
#endif 



static  void pscript__add_newline(pscript_t * this) { 
  const char * ident_cstr = "newline"; 
  const int ident_pstr = pscript_string__push(this -> string_env, ident_cstr); 
  const int type_i = pscript_type__get__string(this -> type_env); 
  const char * value_cstr = "\n"; 
  const int value_pstr = pscript_string__push(this -> string_env, value_cstr); 
#if 1 
  { 
    pscript_const__stack__push_string(this -> const_env, value_pstr); 
    const int const_i = pscript_const__env__name__push_from_stack__nolookup(this -> const_env, ident_pstr); 
    pscript_const__stack__drop(this -> const_env); 
    pscript_env__push_const(this -> env, ident_pstr, const_i); 
  };
#else 
  { 
    // That works as well. 
    const int var_i = pscript_var__env__name__push(this -> var_env, ident_pstr, type_i); 
    const int mem_offset = pscript_var__get_mem_offset(this -> var_env, var_i); 
    pscript_mem__write(this -> mem_env, mem_offset, sizeof(value_pstr), (const char *)&value_pstr); 
    pscript_env__push_var(this -> env, ident_pstr, var_i); 
  }; 
#endif 
}; 
 
pscript_t * pscript_make(void) {
  MALLOC_BZERO(pscript_t,this); 
  
  ASSIGN_METHOD(pscript,this,make); 
  ASSIGN_METHOD(pscript,this,delete); 
  ASSIGN_METHOD(pscript,this,ReadDescriptionFile); 
  ASSIGN_METHOD(pscript,this,ReadDescriptionFile_fd); 
  ASSIGN_METHOD(pscript,this,WriteStdlog); 
  ASSIGN_METHOD(pscript,this,WriteStdout); 
  ASSIGN_METHOD(pscript,this,WriteStderr); 
  ASSIGN_METHOD(pscript,this,stdlog__set__fdes); 
  ASSIGN_METHOD(pscript,this,stdlog__set__file); 
  ASSIGN_METHOD(pscript,this,stdout__set__fdes); 
  ASSIGN_METHOD(pscript,this,stdout__set__file); 
  ASSIGN_METHOD(pscript,this,stderr__set__fdes); 
  ASSIGN_METHOD(pscript,this,stderr__set__file); 
  ASSIGN_METHOD(pscript,this,stdlog__unset); 
  ASSIGN_METHOD(pscript,this,stdout__unset); 
  ASSIGN_METHOD(pscript,this,stderr__unset); 
  ASSIGN_METHOD(pscript,this,stdlog__prefix_set); 
  ASSIGN_METHOD(pscript,this,stdout__prefix_set); 
  ASSIGN_METHOD(pscript,this,stderr__prefix_set); 
  ASSIGN_METHOD(pscript,this,stdlog__prefix_unset); 
  ASSIGN_METHOD(pscript,this,stdout__prefix_unset); 
  ASSIGN_METHOD(pscript,this,stderr__prefix_unset); 
  ASSIGN_METHOD(pscript,this,writeln_set); 
  ASSIGN_METHOD(pscript,this,writeln); 
  ASSIGN_METHOD(pscript,this,menu_set); 
  ASSIGN_METHOD(pscript,this,menu); 
  ASSIGN_METHOD(pscript,this,menu__async_reverse_callback__push_return_value); 
  
  this -> string_env = pscript_string_env__make(); 
  this -> input_env  = pscript_input_env__make(this -> string_env); 
  this -> token_env  = pscript_token_env__make(this -> string_env); 
  pscript_token_automata__set_master(pscript_token_automata, this -> token_env); 
  this -> parser_env = pscript_parser_env__make(); 
  this -> type_env   = pscript_type_env__make(this -> string_env); 
  //this -> const_env = pscript_const_env__make(this -> type_env); 
  this -> const_env  = pscript_const_env__make(this); 
  this -> mem_env = pscript_mem_env__make(this -> type_env); 
  this -> var_env = pscript_var_env__make(this -> type_env, this -> mem_env); 
  this -> funproc_env = pscript_funproc_env__make(this -> string_env, this -> type_env); 
  this -> mainproc_env = pscript_mainproc_env__make(this -> type_env); 
  this -> ffiproc_env = pscript_ffiproc_env__make(this -> type_env); 
  this -> libproc_env = pscript_libproc_env__make(this); 
  this -> env = pscript_env__make(this -> string_env);   
  this -> bytecode_env = pscript_bytecode_env__make(); 
  this -> stack_env = pscript_stack_env__make(this -> type_env); 
  this -> vm = pscript_vm__make(this); 
  this -> menu_env = pscript_menu_env__make(this -> string_env); 
  
  this -> stdlog_d = -1; 
  this -> stdout_d = stdout_d; 
  this -> stderr_d = stderr_d; 
  this -> write_to_stderr_as_well_huh = false; 

#if 1
  this -> stdlog_filename = NULL; 
  this -> stdout_filename = NULL;  
  this -> stderr_filename = NULL;  
#else   
  this -> stdlog_owner_huh = false; 
  this -> stdout_owner_huh = false; 
  this -> stderr_owner_huh = false; 
#endif 

  this -> stdlog_prefix = NULL; 
  this -> stdout_prefix = NULL;  
  this -> stderr_prefix = strcopy("PScript: Erreur: ");  



  pscript__add_newline(this); 

  {
    pscript_libproc__push_standard_lib(this -> libproc_env); 
  }; 

  {
    const int ffiproc_name = pscript_string__push(this -> string_env, "writeln"); 
    const int retval_type_i = pscript_type__get__void(this -> type_env); 
    const int ffiproc_i = pscript_ffiproc__push(this -> ffiproc_env, ffiproc_name, /*arg_atleast*/0, /*retval_type*/retval_type_i, /*void callback_data*/this, /*callback*/pscript__ffiproc_callback__writeln); 
    pscript_env__push_ffi(this -> env, ffiproc_name, ffiproc_i); 
  }; 


  
  // RL: TODO XXX FIXME: To be removed. 
  yypscript_global_objet_being_filled_up_while_parsing = this; // RL: For 'pmesserr' to work. 

  
  return this; 
};

void pscript_delete(pscript_t * this) { 
  pscript__stderr__prefix_unset(this); 
  pscript__stdout__prefix_unset(this); 
  pscript__stdlog__prefix_unset(this); 
  
  pscript__stderr__unset(this); 
  pscript__stdout__unset(this); 
  pscript__stdlog__unset(this); 
  
  pscript_menu_env__delete(this -> menu_env); 
  pscript_vm__delete(this -> vm); 
  pscript_stack_env__delete(this -> stack_env); 
  pscript_bytecode_env__delete(this -> bytecode_env); 
  pscript_env__delete(this -> env);   
  pscript_libproc_env__delete(this -> libproc_env); 
  pscript_ffiproc_env__delete(this -> ffiproc_env); 
  pscript_mainproc_env__delete(this -> mainproc_env); 
  pscript_funproc_env__delete(this -> funproc_env); 
  pscript_var_env__delete(this -> var_env); 
  pscript_mem_env__delete(this -> mem_env); 
  pscript_const_env__delete(this -> const_env); 
  pscript_type_env__delete(this -> type_env); 
  pscript_token_env__delete(this -> token_env); 
  pscript_string_env__delete(this -> string_env); 
  free(this); 
};


void pscript__stdlog__prefix_set(pscript_t * this, const char * prefix) { 
  pscript__stdlog__prefix_unset(this); 
  if (prefix == NULL) return; 
  this -> stdlog_prefix = strcopy(prefix); 
}; 
  
void pscript__stdout__prefix_set(pscript_t * this, const char * prefix) { 
  pscript__stdout__prefix_unset(this); 
  if (prefix == NULL) return; 
  this -> stdout_prefix = strcopy(prefix); 
}; 
  
void pscript__stderr__prefix_set(pscript_t * this, const char * prefix) { 
  pscript__stderr__prefix_unset(this); 
  if (prefix == NULL) return; 
  this -> stderr_prefix = strcopy(prefix); 
}; 
  

void pscript__stdlog__prefix_unset(pscript_t * this) { 
  if (this -> stdlog_prefix == NULL) return;  
  free(this -> stdlog_prefix); 
  this -> stdlog_prefix = NULL; 
}; 
  
void pscript__stdout__prefix_unset(pscript_t * this) { 
  if (this -> stdout_prefix == NULL) return; 
  free(this -> stdout_prefix); 
  this -> stdout_prefix = NULL; 
}; 
  
void pscript__stderr__prefix_unset(pscript_t * this) { 
  if (this -> stderr_prefix == NULL) return; 
  free(this -> stderr_prefix); 
  this -> stderr_prefix = NULL; 
}; 
  



void pscript__stdlog__unset(pscript_t * this) { 
  if (this -> stdlog_filename != NULL) {
    close(this -> stdlog_d); 
    free(this -> stdlog_filename); 
    this -> stdlog_filename = NULL; 
  }; 
  this -> stdlog_d = -1; 
}; 
  
void pscript__stdout__unset(pscript_t * this) { 
  if (this -> stdout_filename != NULL) {
    close(this -> stdout_d); 
    free(this -> stdout_filename); 
    this -> stdout_filename = NULL; 
  }; 
  this -> stdout_d = -1; 
}; 
  
void pscript__stderr__unset(pscript_t * this) { 
  if (this -> stderr_filename != NULL) {
    close(this -> stderr_d); 
    free(this -> stderr_filename); 
    this -> stderr_filename = NULL; 
  }; 
  this -> stderr_d = -1; 
}; 
  


void pscript__stdlog__set__fdes(pscript_t * this, const int fdes) { 
  pscript__stdlog__unset(this); 
  this -> stdlog_d = fdes; 
}; 
  
void pscript__stdlog__set__file(pscript_t * this, const char * filename) { 
  if (filename == NULL) { 
      pscript__stdlog__unset(this); 
      return; 
  }; 
  if (this -> stdlog_filename != NULL) {
    if (0 == strcmp(this -> stdlog_filename, filename)) return; 
  };
  const int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC); 
  if (fd < 0) { 
    dputs_array(stderr_d, "I could not open the file '", filename, "\n"); 
    return; 
  }; 
  pscript__stdlog__unset(this); 
  this -> stdlog_d = fd; 
  this -> stdlog_filename = strcopy(filename); 
}; 
  
void pscript__stdout__set__fdes(pscript_t * this, const int fdes) { 
  pscript__stdout__unset(this); 
  this -> stdout_d = fdes; 
}; 
  
void pscript__stdout__set__file(pscript_t * this, const char * filename) { 
  if (filename == NULL) { 
      pscript__stdout__unset(this); 
      return; 
  }; 
  if (this -> stdout_filename != NULL) {
    if (0 == strcmp(this -> stdout_filename, filename)) return; 
  };
  const int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC); 
  if (fd < 0) {
    dputs_array(stderr_d, "I could not open the file '", filename, "\n"); 
    return; 
  }; 
  pscript__stdout__unset(this); 
  this -> stdout_d = fd; 
  this -> stdout_filename = strcopy(filename); 
}; 
  
void pscript__stderr__set__fdes(pscript_t * this, const int fdes, const int write_to_stderr_as_well_huh) { 
  pscript__stderr__unset(this); 
  this -> stderr_d = fdes; 
  this -> write_to_stderr_as_well_huh = write_to_stderr_as_well_huh; 
}; 
  
void pscript__stderr__set__file(pscript_t * this, const char * filename, const int write_to_stderr_as_well_huh) { 
  if (filename == NULL) { 
      pscript__stderr__unset(this); 
      return; 
  }; 
  if (this -> stderr_filename != NULL) {
    if (0 == strcmp(this -> stderr_filename, filename)) return; 
  };
  const int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC); 
  if (fd < 0) {
    dputs_array(stderr_d, "I could not open the file '", filename, "\n"); 
    return; 
  }; 
  pscript__stderr__unset(this); 
  this -> stderr_d = fd; 
  this -> stderr_filename = strcopy(filename); 
  this -> write_to_stderr_as_well_huh = write_to_stderr_as_well_huh; 
}; 
  

void pscript__WriteStdlog(pscript_t * this, const int argc, const char * argv[]) { 
  if (this -> stdlog_d < 0) { return; }; 
  if (this -> stdlog_prefix != NULL) dputs(this -> stdlog_d, this -> stdlog_prefix); 
  for (int i = 0; i < argc; i++) {
    dputs(this -> stdlog_d, argv[i]); 
  }; 
}; 
 
void pscript__WriteStdout(pscript_t * this, const int argc, const char * argv[]) { 
  if (this -> stdout_d < 0) { return; }; 
  if (this -> stdout_prefix != NULL) dputs(this -> stdout_d, this -> stdout_prefix); 
  for (int i = 0; i < argc; i++) {
    dputs(this -> stdout_d, argv[i]); 
  }; 
}; 
 
void pscript__WriteStderr(pscript_t * this, const int argc, const char * argv[]) { 
  pscript__WriteStdlog(this, argc, argv);
  if (this -> stderr_d < 0) { return; }; 
  if (this -> stderr_prefix != NULL) dputs(this -> stderr_d, this -> stderr_prefix); 
  for (int i = 0; i < argc; i++) {
    dputs(this -> stderr_d, argv[i]); 
  }; 
}; 
 






void pscript__writeln_set(pscript_t * this, void * writeln_data, int (* writeln_callback)(void * writeln_data, const char * cstr)) {
  this -> writeln_data = writeln_data; 
  this -> writeln_callback = writeln_callback; 
}; 

int pscript__writeln(pscript_t * this, const char * cstr) { 
  if (this -> writeln_callback != NULL) {
    return this -> writeln_callback(this -> writeln_data, cstr); 
  }; 
  this -> WriteStdout(this, 2, (const char * []){ cstr, "\n" } ); 
  return 0; 
}; 


void pscript__menu_set(pscript_t * this, void * menu_data, const int menu_async_huh, int  (* menu_callback)(void * menu_data, const char * menu_title, const int menu_item_nb, const char * item[])) { 
  this -> menu_data = menu_data; 
  this -> menu_async_huh = menu_async_huh; 
  this -> menu_callback = menu_callback; 
}; 
 
void pscript__menu__async_reverse_callback__push_return_value(pscript_t * this, const int choice) {
  assert(this -> menu_async_huh); 
  pscript_stack__push_value(this -> stack_env, pscript_type__get__integer(this -> type_env), sizeof(choice), (const char *) &choice); 
}; 


static int pscript__menu__default_callback(pscript_t * this, const char * menu_title, const int menu_item_nb, const char * item[]) { 
  //PScript_WriteStdout2(this, "The Z value of this TPoint3D object is ", field_value_cstr, ".", "\n"); 

  this -> WriteStdout(this, 2, (const char * []){ menu_title, "\n" } ); 
  for (int i = 0; i < menu_item_nb; i++) { 
    dputs_array(this -> stdout_d, "\t" "[", int_string__stack(i), "]" " ", item[i], "\n"); 
  }; 
  enum { buff_size = 32 }; 
  char buffer[buff_size]; 
  *buffer = '\0'; 
  //read(this -> stdin_d, buffer, buff_size); 
  read(stdin_d, buffer, buff_size); 
  const int choice = MAX(-1,MIN(menu_item_nb,atoi(buffer))); 
  return choice; 
}; 
 
int pscript__menu(pscript_t * this, const char * menu_title, const int menu_item_nb, const char * item[]) { 
  if (this -> menu_callback != NULL) {
    return this -> menu_callback(this -> menu_data, menu_title, menu_item_nb, item); 
  }; 
  return pscript__menu__default_callback(this, menu_title, menu_item_nb, item); 
}; 
 
int pscript__menu2(pscript_t * this, const int menu_i) { 
  const int menu_title_pstr = this -> menu_env -> get_title(this -> menu_env, menu_i);
  const char * menu_title = pscript_string__get_cstr(this -> string_env, menu_title_pstr); 
  const int menu_item_nb = this -> menu_env -> get_item_nb(this -> menu_env, menu_i); 
  const char * item[menu_item_nb]; 
  for (int i = 0; i < menu_item_nb; i++) {
    const int item_pstr = this -> menu_env -> get_item(this -> menu_env, menu_i, i); 
    const char * item_cstr = pscript_string__get_cstr(this -> string_env, item_pstr); 
    item[i] = item_cstr; 
  }; 
 
  return pscript__menu(this, menu_title, menu_item_nb, item); 
}; 
 






int pscript__ffiproc_callback__writeln(void * callback_data, const int arg_nb, const char arg_type[], const int arg_sizeof[], const int arg_offset[], const int arg_mem_size, const char arg_mem[], const int retval_type, const int retval_allocated_size, char * retval_value) { 
  pscript_t * this = callback_data; 
  const char * arg_string[arg_nb]; 
  
  for (int i = 0; i < arg_nb; i++) { 
    //extern const char * pscript_value__cstr  (pscript_type_env_t * type_env, pscript_string_env_t * string_env, const int type1, const int type1_sizeof, const char value1[]); 
    const int type1        = arg_type[i]; 
    const int type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
    const char * value1    = arg_mem + arg_offset[i]; 
    //arg_string[i] = pscript_value__get_cstr(this -> type_env, this -> string_env, this -> mem_env, type1, type1_sizeof, value1); 
    arg_string[i]          = pscript_value__get_cstr(this, type1, type1_sizeof, value1); 
    //arg_string[i]          = pscript_value__cstr_raw(this, type1, type1_sizeof, value1); 
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
    const char * to_be_printed = strconcat__stack_nb(arg_nb, arg_string); 
    //pmesserr("%s: " "%s" "\n", __func__, to_be_printed); 
    this -> writeln(this, to_be_printed); 
  }; 
#else 
  for (int i = 0; i < argc; i++) {
    dputs(stdout, arg_string[i]); 
  }; 
  dputs(stdout, "\n"); 
#endif 
  
  return 0; 
}; 





int pscript__ffiproc_callback__writeln__OLD(void * callback_data, const int argc, const char argv_core_type[], const int bool_nb, const char bool_stack[], const int integer_nb, const int integer_stack[], const int float_nb, const float float_stack[], const int char_nb, const char char_stack[], const int string_nb, const int string_stack[], const int pointer_nb, const int pointer_type[], const int pointer_stack[], const int proxy_nb, const int proxy_type[], const int proxy_stack[], const int array_nb, const int array_type[], const int array_offset[], const int array_mem_size, const char array_mem[], const int record_nb, const int record_type[], const int record_offset[], const int record_mem_size, const char record_mem[], const int udf_nb, const int udf_type[], const int udf_offset[], const int udf_mem_size, const char udf_mem[]) { 
  pscript_t * this = callback_data; 
  //dputs(stderr_d, "FFI WRITELN WAS CALLED!" "\n"); 
  
  int bool_i = 0; 
  int integer_i = 0; 
  int float_i = 0; 
  int char_i = 0; 
  int string_i = 0; 
  int pointer_i = 0; 
  int proxy_i = 0; 
  int array_i = 0; 
  int record_i = 0; 
  int udf_i = 0; 
  
  
  const char * arg_string[argc]; 
  
  for (int i = 0; i < argc; i++) {
    const char core_type = argv_core_type[i]; 
    
    switch (core_type) {
    default: assert(false); break; 
    case PSCRIPT_TYPE__BOOL:      arg_string[i] = bool_stack[bool_i] ? "TRUE" : "FALSE"; bool_i++; break;  
    case PSCRIPT_TYPE__INTEGER:   arg_string[i] =    int_string__stack(integer_stack[integer_i]); integer_i++; break;  
    case PSCRIPT_TYPE__INT_RANGE: arg_string[i] =    int_string__stack(integer_stack[integer_i]); integer_i++; break;  
    case PSCRIPT_TYPE__FLOAT:     arg_string[i] =  float_string__stack(  float_stack[  float_i]);   float_i++; break;  
    case PSCRIPT_TYPE__CHAR:      arg_string[i] =   char_string__stack(   char_stack[   char_i]);    char_i++; break;  
    case PSCRIPT_TYPE__STRING: {
      //dputs_array(stderr_d, "Writeln: arg_i = ", int_string__stack(i), " - " "string_stack[string_i (", int_string__stack(string_i), ")]: ", int_string__stack(string_stack[string_i]),"\n"); 
      arg_string[i] = pscript_string__get_cstr(this -> string_env, string_stack[string_i]); string_i++; 
    }; break;  
    case PSCRIPT_TYPE__NIL_LIST:  arg_string[i] = "nil"; pointer_i++; break;  
    case PSCRIPT_TYPE__NIL_PTR:   arg_string[i] = "NULL"; pointer_i++; break;  
    case PSCRIPT_TYPE__POINTER:   arg_string[i] =    int_string__stack(pointer_stack[pointer_i]); pointer_i++; break;  
    case PSCRIPT_TYPE__PROXY: do { 
	const int type_i = proxy_type[proxy_i]; 
	const int type_i_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
	const int value = proxy_stack[proxy_i]; 
	//arg_string[i] = pscript_value__cstr(this -> type_env, this -> string_env, this -> mem_env, type_i, type_i_sizeof, (const char *) &value); 
	arg_string[i] = pscript_value__cstr(this, type_i, type_i_sizeof, (const char *) &value); 
	proxy_i++; 
      } while (false); break;  
    case PSCRIPT_TYPE__ARRAY: {
#if 1
      arg_string[i] = "ARRAY"; 
#else 
      array_offset[array_i] = array_cumulated_offset; 
      array_cumulated_offset += arg_sizeof[i]; 
      pscript_value__get_value(arg_sizeof[i], arg_value, arg_sizeof[i], array_mem + array_offset[array_i]); 
#endif 
      array_i++; 
    }; break;  
    case PSCRIPT_TYPE__RECORD: { 
      const int type_i = record_type[record_i]; 
      const int type_i_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
      const int offset = record_offset[record_i]; 
      const char * value = record_mem + offset; 
      //arg_string[i] = pscript_value__cstr(this -> type_env, this -> string_env, this -> mem_env, type_i, type_i_sizeof, value); 
      arg_string[i] = pscript_value__cstr(this, type_i, type_i_sizeof, value); 
      record_i++; 
    }; break;  
    case PSCRIPT_TYPE__USER: { 
      const int type_i = udf_type[udf_i]; 
      const int type_i_sizeof = pscript_type__sizeof(this -> type_env, type_i); 
      const int offset = udf_offset[udf_i]; 
      const char * value = udf_mem + offset; 
      //arg_string[i] = pscript_value__cstr(this -> type_env, this -> string_env, this -> mem_env, type_i, type_i_sizeof, value); 
      arg_string[i] = pscript_value__cstr(this, type_i, type_i_sizeof, value); 
      udf_i++; 
    }; break;  
    };
  }; 

#if 1 
  {
    const char * to_be_printed = strconcat__stack_nb(argc, arg_string); 
    this -> writeln(this, to_be_printed); 
  }; 
#else 
  for (int i = 0; i < argc; i++) {
    dputs(stdout, arg_string[i]); 
  }; 
  dputs(stdout, "\n"); 
#endif 
  
  return 0; 
}; 
 
 























#if 0 
int pscript__ReadDescriptionFile_fd(pscript_t * this, const int yyin_fd, const char * filename) {
  this -> filename_pstr = pscript_string__push(this -> string_env, filename); 
  this -> filename = pscript_string__get_cstr(this -> string_env, this -> filename_pstr); 
  
  //pscript__stdlog__set__fdes(this, stdout); 
  
  FILE * yyin = fdopen(yyin_fd, "rb"); 
    
  yypscript_global_objet_being_filled_up_while_parsing = this; 
  yypscriptlexerinit(this -> filename_pstr); 
  yypscriptrestart(yyin); 
  //pscript_reset_parser(); 

#if 0 
  fprintf(yypscriptout == NULL ? stderr_FILE : yypscriptout, "================ ENV PRINT =========" "\n"); 
  pscript_env__print_all(this -> env, this -> string_env); 
#endif 
  const int res = yypscriptparse(); 
  
  fclose(yyin);
  
  //this -> token_env -> line_nb = pscript_get_nb_lignes();
  //this -> token_env -> char_nb = pscript_get_nb_chars_tot(); 
  
  switch (res) {
  case 0:  PScript_WriteStdlog(this, "Successful analysis." "\n"); return 0; break; 
  default: PScript_WriteStderr(this, "Failed analysis... (returned value: ", int_string__stack(res), ")" "\n"); return -3; break; 
  };
  
  return res; 
}; 

#else 
int pscript__ReadDescriptionFile_fd(pscript_t * this, const int yyin_fd, const char * filename) {
  assert(false); 
  return 0; 
}; 
#endif 



int pscript__ReadDescriptionFile(pscript_t * this, const char * filename) {
  if (filename == NULL) { PScript_WriteStderr(this, "Nothing to be read: the given filename is NULL." "\n"); return -1; }; 
  
  FILE * yyin = NULL; 
  yyin = fopen(filename, "rb"); 
  if (yyin == NULL) { 
    PScript_WriteStderr(this, "The file '%s' could not be opened." "\n", filename); 
    return -2; 
  }; 
  
  return pscript__ReadDescriptionFile_fd(this, fileno(yyin), filename); 
}; 












