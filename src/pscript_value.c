#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_value.h" 


void pscript_value__print__raw_bytes(const int type1_sizeof, const char value1[]) { 
  for (int i = 0; i < type1_sizeof; i++) {
    if (i > 0) dputs(stderr_d, " "); 
    dputn(stderr_d, (unsigned char) value1[i]);
  }; 
}; 
 
int pscript_value__string(const pscript_t * this, const int type1, const int value1_sizeof, const char value1[]) {
  const char * retval_cstr = NULL; 
  int          retval_pstr = -1; 
  const char   type1_core  = pscript_type__get_core_type(this -> type_env, type1); 
  //pmessage("pscript_value__string: type1_core: [ %d ] %s " "\n", type1_core, pscript_type_core__cstr(type1_core));
  
  
    switch (type1_core) {
    default: assert(false); break; 
    //case PSCRIPT_TYPE__NULL:      retval_cstr =    int_string__stack(*(const int   *) value1); break; 
    case PSCRIPT_TYPE__BOOL:      retval_cstr = *value1 ? "TRUE" : "FALSE"; break; 
    case PSCRIPT_TYPE__INTEGER:   retval_cstr =    int_string__stack(*(const int   *) value1); break; 
    case PSCRIPT_TYPE__INT_RANGE: retval_cstr =    int_string__stack(*(const int   *) value1); break; 
    case PSCRIPT_TYPE__FLOAT:     retval_cstr =  float_string__stack(*(const float *) value1); break; 
    case PSCRIPT_TYPE__CHAR:      retval_cstr =   char_string__stack(*(const char  *) value1); break; 
    case PSCRIPT_TYPE__STRING: { 
      //dputs_array(stderr_d, "Writeln: arg_i = ", int_string__stack(i), " - " "string_stack[string_i (", int_string__stack(string_i), ")]: ", int_string__stack(string_stack[string_i]),"\n"); 
      retval_cstr = pscript_string__get_cstr(this -> string_env, *(const int_pscript_string_t *) value1); 
      retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
    }; break; 
    case PSCRIPT_TYPE__NIL_LIST:       retval_cstr = "nil"; break;  
    case PSCRIPT_TYPE__NIL_PTR:       retval_cstr = "NULL"; break;  
    case PSCRIPT_TYPE__POINTER:   retval_cstr =    int_string__stack(*(const int   *) value1); break;  
    case PSCRIPT_TYPE__PROXY: do {
	const int mem_offset = *(const int_pscript_mem_t *) value1; 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__PROXY: mem_offset = ", int_string__stack(mem_offset), "\n"); 
	if (this -> mem_env == NULL) { 
	  retval_cstr = int_string__stack(mem_offset); 
	  retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
	} 
	else { 
	  const int subtype        = pscript_type__proxy__get_subtype(this -> type_env, type1); 
	  const int subtype_sizeof = pscript_type__sizeof(this -> type_env, subtype); 
	  //dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__PROXY: subtype_sizeof = ", int_string__stack(subtype_sizeof), "\n"); 
	  char      subtype_value[subtype_sizeof]; 
	  pscript_mem__read(this -> mem_env, mem_offset, subtype_sizeof, subtype_value); 
#if 0 
	  dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__PROXY: subtype_value = "); 
	  pscript_value__print__raw_bytes(subtype_sizeof, subtype_value); 
	  dputs_array(stderr_d, "\n"); 
#endif 
	  const int subtype_pstr   = pscript_value__string(this, subtype, subtype_sizeof, subtype_value); 
	  retval_cstr = pscript_string__get_cstr(this -> string_env, subtype_pstr); 
	  retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
	}; 
      } while (false); break;  
    case PSCRIPT_TYPE__VARIANT: do {
	//if (type1 == pscript_type__get__variant(this -> type_env)) { return pscript_type__get_pstr(this -> type_env, type1); }; 
	const int subtype    = *(const int_pscript_type_t *) value1; 
	const int mem_offset = *(const int_pscript_mem_t *) (value1 + sizeof(int_pscript_type_t)); 
	//pscript_value__string(const pscript_t * this, const int type1, const int value1_sizeof, const char value1[]) {
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__VARIANT: mem_offset = ", int_string__stack(mem_offset), "\n"); 
	//const char * sub_type_cstr = pscript_type__get_cstr(this -> type_env, sub_type); 
	if (subtype == 0) { 
	  retval_cstr = "VARIANT/UNASSIGNED"; 
	  retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
	  break; 
	}; 
	if (this -> mem_env == NULL) { 
	  retval_cstr = int_string__stack(mem_offset); 
	  retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
	  break; 
	}; 
	const int subtype_sizeof = pscript_type__sizeof(this -> type_env, subtype); 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__VARIANT: subtype_sizeof = ", int_string__stack(subtype_sizeof), "\n"); 
	const char * mem_ptr = pscript_mem__get_pointer(this -> mem_env, mem_offset); 
	retval_pstr = pscript_value__string(this, subtype, subtype_sizeof, mem_ptr); 
	retval_cstr = pscript_string__get_cstr(this -> string_env, retval_pstr); 
      } while (false); break; 
    case PSCRIPT_TYPE__LIST: do { 
	int cumulated_offset = 0; 
	const int car_type  = *(const int_pscript_type_t *) (value1 + cumulated_offset); 
	cumulated_offset += sizeof(int_pscript_type_t); 
	const int car_value = *(const int *               ) (value1 + cumulated_offset); 
	cumulated_offset += sizeof(int); 
	const int cdr_type  = *(const int_pscript_type_t *) (value1 + cumulated_offset); 
	cumulated_offset += sizeof(int_pscript_type_t); 
	const int cdr_value = *(const int *               ) (value1 + cumulated_offset); 
	cumulated_offset += sizeof(int); 
	
	int car_pstr; 
	if (pscript_type__scalar_huh(this -> type_env, car_type)) { 
	  car_pstr = pscript_value__string(this, car_type, sizeof(int), (const char *) &car_value); 
	} 
	else { 
	  const char * car_ptr = pscript_mem__get_pointer(this -> mem_env, car_value); 
	  car_pstr = pscript_value__string(this, car_type, pscript_type__sizeof(this -> type_env, car_type), car_ptr); 
	}; 
	const char * car_cstr = pscript_string__get_cstr(this -> string_env, car_pstr); 
	
	int cdr_pstr; 
	if (pscript_type__scalar_huh(this -> type_env, cdr_type)) { 
	  cdr_pstr = pscript_value__string(this, cdr_type, sizeof(int), (const char *) &cdr_value); 
	} 
	else { 
	  const char * cdr_ptr = pscript_mem__get_pointer(this -> mem_env, cdr_value); 
	  cdr_pstr = pscript_value__string(this, cdr_type, pscript_type__sizeof(this -> type_env, cdr_type), cdr_ptr); 
	}; 
	const char * cdr_cstr = pscript_string__get_cstr(this -> string_env, cdr_pstr); 
	
	
	const char * list_cstr = strconcat__stack("(cons ", car_cstr, " ", cdr_cstr, ")"); 
	retval_cstr = list_cstr; 
	retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
	
      } while (false); break; 
    case PSCRIPT_TYPE__ARRAY: do { 
#if 1
	const int i_min = pscript_type__array__get_i_min(this -> type_env, type1); 
	const int i_max = pscript_type__array__get_i_max(this -> type_env, type1); 
	const int subtype = pscript_type__array__get_subtype(this -> type_env, type1); 
	const int subtype_sizeof = pscript_type__sizeof(this -> type_env, subtype); 
	const int field_nb = i_max - i_min + 1; 
	const char * subtype_string[field_nb]; 
	for (int i = 0; i < field_nb; i++) { 
	  const char * field_name_cstr = int_string__stack(i_min + i); 
	  const int field_offset = i * subtype_sizeof; 
	  const int field_value_string = pscript_value__string(this, subtype, subtype_sizeof, value1 + field_offset); 
	  const char * field_value_cstr = pscript_string__get_cstr(this -> string_env, field_value_string); 
	  subtype_string[i] = strconcat__stack(" ", field_name_cstr, " = ", field_value_cstr, ";"); 
	}; 
	const char * fields_cstr = strconcat__stack_nb(field_nb, subtype_string); 
	const char * array_cstr = strconcat__stack("ARRAY[", fields_cstr, " ]"); 
	retval_cstr = array_cstr; 
	retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
#else 
	array_offset[array_i] = array_cumulated_offset; 
	array_cumulated_offset += arg_sizeof[i]; 
	pscript_value__get_value(arg_sizeof[i], arg_value, arg_sizeof[i], array_mem + array_offset[array_i]); 
#endif 
      } while (false); break;  
    case PSCRIPT_TYPE__RECORD: do {
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: ", "\n"); 
#if 0
	dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: value1 = "); 
	pscript_value__print__raw_bytes(value1_sizeof, value1); 
	dputs_array(stderr_d, "\n"); 
#endif 
	const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type1); //this -> type_env -> cell__array[type_i].record_field_nb; 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: field_nb = ", int_string__stack(field_nb), "\n"); 
	const char * subtype_string[field_nb]; 
	for (int i = 0; i < field_nb; i++) {
	  const int    field_name         = pscript_type__record__get_field_name(this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	  const char * field_name_cstr    = pscript_string__get_cstr(this -> string_env, field_name); 
	  const int    field_type         = pscript_type__record__get_field_type  (this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	  const int    field_offset       = pscript_type__record__get_field_offset(this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	  const int    field_sizeof       = pscript_type__sizeof(this -> type_env, field_type); 
	  const int    field_value_string = pscript_value__string(this, field_type, field_sizeof, value1 + field_offset); 
	  const char * field_value_cstr   = pscript_string__get_cstr(this -> string_env, field_value_string); 
	  subtype_string[i] = strconcat__stack(" ", field_name_cstr, " = ", field_value_cstr, ";"); 
	  //dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: subtype_string[i] = ", subtype_string[i], "\n"); 
	}; 
	const char * fields_cstr = strconcat__stack_nb(field_nb, subtype_string); 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: fields_cstr = ", fields_cstr, "\n"); 
	const char * record_cstr = strconcat__stack("RECORD[", fields_cstr, " ]"); 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: record_cstr = ", record_cstr, "\n"); 
	retval_cstr = record_cstr; 
	//dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
	
	retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
      } while (false); break;  

    case PSCRIPT_TYPE__USER: do {
	const char * udf_name_cstr = pscript_type__user_defined_type__get_name_cstr(this -> type_env, type1); 
	const int    subtype       = pscript_type__user_defined_type__get_subtype(this -> type_env, type1); 
	const char * subtype_cstr  = pscript_value__cstr(this, subtype, value1_sizeof, value1); 
	retval_cstr = strconcat__stack(udf_name_cstr, ": " , subtype_cstr); 
	retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
      } while (false); break;  

    case PSCRIPT_TYPE__LAMBDA: do { 
	// RL: LAMBDA OBJECT 
	//     _____________ 
	//      - int: bytecode @ 
	//      - lex-env-stack (short version) 
	// 
	// RL: LEX-ENV-STACK 
	//     _____________ 
	//      - nb 
	//      - [0]: top-level (whose @ is NULL) 
	//      - ... 
	//      - [nb-1]: EBP 
	const int lambda_offset     = *(const int    *) (value1); 
	const int lex_env_stack__nb = *(const int8_t *) (value1 + sizeof(int)); 
	int lex_env_stack__array[lex_env_stack__nb]; 
	int cumulated_offset = sizeof(int) + sizeof(int8_t); 
	for (int i = 0; i < lex_env_stack__nb; i++) { 
	  lex_env_stack__array[i] = *(const int8_t *) (value1 + cumulated_offset); 
	  cumulated_offset += sizeof(int); 
	}; 

	const char * lambda_offset__cstr     = int_string__stack(lambda_offset); 
	const char * lex_env_stack__nb__cstr = int_string__stack(lex_env_stack__nb); 
	const char * lex_env_stack__array__cstr[lex_env_stack__nb]; 
	for (int i = 0; i < lex_env_stack__nb; i++) { 
	  //lex_env_stack__array__cstr[i] = int_string__stack(lex_env_stack__array[i]); 
	  lex_env_stack__array__cstr[i] = strconcat__stack("", int_string__stack(i), "@ ", int_string__stack(lex_env_stack__array[i]), "; "); 
	}; 
	const char * lex_env_cstr = strconcat__stack_nb(lex_env_stack__nb, lex_env_stack__array__cstr); 
	
	
	//retval_cstr = "<lambda>"; 
	retval_cstr = strconcat__stack("<lambda @", lambda_offset__cstr, " lex_env<", lex_env_stack__nb__cstr, ">: [", lex_env_cstr, "]>"); 
	retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
      } while (false); break;  
    }; 
    
    
    // RL: CLang is here doing an alloca-optimization: 
    //     alloca are not function-wide, but scope-wide... That's terrible. 
    //     Therefore, I have to copy that data to the heap as soon as possible. 
    //     Otherwise CLang optimizes them away. 
    //     The "do { ... } while" especially seems to trigger that weird behavior. 
    if (retval_pstr < 0) { 
      retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
    }; 

    return retval_pstr; 

    
#if 0 

    //pmessage("pscript_value__string: retval_cstr = %s " "\n", retval_cstr); 
    
    if (type1_core == PSCRIPT_TYPE__RECORD) { 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr = ", retval_cstr, "\n"); 
      //dputs_array(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = ", int_string__stack((int)retval_cstr), "\n"); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
      dprintf(stderr__fileno, "Writeln: PSCRIPT_TYPE__RECORD: retval_cstr@ = %p\n", retval_cstr); 
    };
    
    const int retval_string = pscript_string__push(this -> string_env, retval_cstr); 

    //pmessage("pscript_value__string: retval_string = %s " "\n", pscript_string__get_cstr(this -> string_env, retval_string)); 
    if (type1_core == PSCRIPT_TYPE__RECORD) { 
      dputs_array(stderr_d, "Writeln: PSCRIPT_TYPE__RECORD: retval_string = ", pscript_string__get_cstr(this -> string_env, retval_string), "\n"); 
    }; 

    return retval_string; 
#endif 
}; 
 
const char * pscript_value__cstr(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]) {
  const int value_pstr = pscript_value__string(this, type1, type1_sizeof, value1); 
  return pscript_string__get_cstr(this -> string_env, value_pstr); 
}; 




char pscript_value__get_bool(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 1); 
  return *value1; 
}; 

int pscript_value__get_integer(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 4); 
  return *(const int *)value1; 
}; 

float pscript_value__get_float(const int type1_sizeof, const char value1[]) {
  assert((size_t)type1_sizeof >= sizeof(float)); 
  return *(const float *)value1; 
}; 

char pscript_value__get_char(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 1); 
  return *value1; 
}; 

int pscript_value__get_string(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 4); 
  return *(const int_pscript_string_t *)value1; 
}; 

int pscript_value__get_pointer(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 4); 
  return *(const int_pscript_mem_t *)value1; 
}; 

int pscript_value__get_proxy(const int type1_sizeof, const char value1[]) {
  assert(type1_sizeof >= 4); 
  return *(const int_pscript_mem_t *)value1; 
}; 

void pscript_value__get_value(const int type1_sizeof, const char value1[], const int retval_allocated_sizeof, char retval_value[]) {
  assert(retval_allocated_sizeof >= type1_sizeof); 
  bcopy(value1, retval_value, type1_sizeof); 
}; 





char pscript_value__cast_to_bool__scalar(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]) {
  char bval; 
  assert(type1_sizeof >= 1); 
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); bval = false; break; 
  case PSCRIPT_TYPE__BOOL: { bval = *(const char *) value1; }; break; 
#if 0 
  case PSCRIPT_TYPE__INTEGER: break; 
  case PSCRIPT_TYPE__INT_RANGE: break; 
  case PSCRIPT_TYPE__FLOAT: break; 
  case PSCRIPT_TYPE__CHAR: break; 
  case PSCRIPT_TYPE__STRING: break; 
  case PSCRIPT_TYPE__NIL: break; 
  case PSCRIPT_TYPE__POINTER: break; 
  case PSCRIPT_TYPE__ARRAY: break; 
  case PSCRIPT_TYPE__RECORD: break; 
#endif 
  }; 
  return bval; 
}; 

int pscript_value__cast_to_integer__scalar(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]) {
  int intval; 
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); intval = false; break; 
  case PSCRIPT_TYPE__INTEGER:  { intval = *(const int *) value1; }; break; 
#if 0 
  case PSCRIPT_TYPE__BOOL: break; 
  case PSCRIPT_TYPE__INT_RANGE: break; 
  case PSCRIPT_TYPE__FLOAT: break; 
  case PSCRIPT_TYPE__CHAR: break; 
  case PSCRIPT_TYPE__STRING: break; 
  case PSCRIPT_TYPE__NIL: break; 
  case PSCRIPT_TYPE__POINTER: break; 
  case PSCRIPT_TYPE__ARRAY: break; 
  case PSCRIPT_TYPE__RECORD: break; 
#endif 
  }; 
  return intval; 
}; 

float pscript_value__cast_to_float__scalar(const pscript_t * this, const int type1, const int value1_sizeof, const char value1[]) {
  float floatval; 
  pscript_value__cast_to_float(this, type1, value1_sizeof, value1, sizeof(floatval), (char *) &floatval); 
  return floatval; 
}; 








void pscript_value__cast_to_integer(const pscript_t * this, const int type2, const int value2_sizeof, const char value2[], const int cast_value2_sizeof, char cast_value2[]) {
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  int castval; 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: { const int   intval1   = *(const int   *) value2; castval = intval1;   }; break; 
  case PSCRIPT_TYPE__FLOAT:   { const float floatval1 = *(const float *) value2; castval = floatval1; }; break; 
#if 0 
  case PSCRIPT_TYPE__BOOL: break; 
  case PSCRIPT_TYPE__INTEGER: break; 
  case PSCRIPT_TYPE__INT_RANGE: break; 
  case PSCRIPT_TYPE__CHAR: break; 
  case PSCRIPT_TYPE__STRING: break; 
  case PSCRIPT_TYPE__NIL: break; 
  case PSCRIPT_TYPE__POINTER: break; 
  case PSCRIPT_TYPE__ARRAY: break; 
  case PSCRIPT_TYPE__RECORD: break; 
#endif 
  }; 
  *((int *) cast_value2) = castval; 
  //pmessage("pscript_value__cast_to_integer: castval: %d " "\n", castval); 
  return; 
}; 

 
void pscript_value__cast_to_float(const pscript_t * this, const int type2, const int value2_sizeof, const char value2[], const int cast_value2_sizeof, char cast_value2[]) {
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  float floatval1; 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: { const int intval1 = *(const int *) value2; floatval1 = (float) intval1; }; break; 
  case PSCRIPT_TYPE__FLOAT: { floatval1 = *(const float *) value2; }; break; 
#if 0 
  case PSCRIPT_TYPE__BOOL: break; 
  case PSCRIPT_TYPE__INTEGER: break; 
  case PSCRIPT_TYPE__INT_RANGE: break; 
  case PSCRIPT_TYPE__CHAR: break; 
  case PSCRIPT_TYPE__STRING: break; 
  case PSCRIPT_TYPE__NIL: break; 
  case PSCRIPT_TYPE__POINTER: break; 
  case PSCRIPT_TYPE__ARRAY: break; 
  case PSCRIPT_TYPE__RECORD: break; 
#endif 
  }; 
  *((float *) cast_value2) = floatval1;
  return; 
}; 

 
void pscript_value__cast_generic(pscript_t * this, const int type1, const int type2, const int value2_sizeof, const char value2[], const int cast_value2_sizeof, char cast_value2[]) {
  const int type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
  assert(type1_sizeof <= cast_value2_sizeof); 
  
  if (type1 == type2) {
    bcopy(value2, cast_value2, type1_sizeof); 
    return; 
  }; 

  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  //pmessage("pscript_value__cast_generic: type1_core: [ %d ] %s " "\n", type1_core, pscript_type_core__cstr(type1_core));

  const char type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  //pmessage("pscript_value__cast_generic: type2_core: [ %d ] %s " "\n", type2_core, pscript_type_core__cstr(type2_core));

  //pmessage("%s: " " type1: [ %d ] %s " "\n", __func__, type1, pscript_type__get_cstr(this -> type_env, type1)); 


  switch (type1_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__cast_to_integer(this, type2, value2_sizeof, value2, cast_value2_sizeof, cast_value2); break; 
  case PSCRIPT_TYPE__FLOAT:   pscript_value__cast_to_float  (this, type2, value2_sizeof, value2, cast_value2_sizeof, cast_value2); break; 
  case PSCRIPT_TYPE__STRING:  do {
      const int value_pstr = pscript_value__string(this, type2, value2_sizeof, value2); 
      *(int_pscript_string_t *) cast_value2 = value_pstr; 
    } while (false); break; 
  case PSCRIPT_TYPE__INT_RANGE: do { 
      pscript_value__cast_to_integer(this, type2, value2_sizeof, value2, cast_value2_sizeof, cast_value2); 
      const int casted_int_value = *(int *) cast_value2; 
      const int i_min = pscript_type__int_range__get_i_min(this -> type_env, type1); 
      const int i_max = pscript_type__int_range__get_i_max(this -> type_env, type1); 
      ///pmessage("%s: " "i_min = %d - i_max = %d " "\n", __func__, i_min, i_max);
      if (casted_int_value < i_min) { 
	pmesserr("%s: " "The integer value %d is too small to fit in the range %d..%d - Flooring it to %d." "\n", __func__, casted_int_value, i_min, i_max, i_min); 
	*(int *) cast_value2 = i_min; 
      }; 
      if (casted_int_value > i_max) { 
	//dputs_array(stderr_d, "The integer value ", int_string__stack(casted_int_value), " is too large to fit in the range ", int_string__stack(i_min), "..", int_string__stack(i_max), " - Ceiling it to ", int_string__stack(i_max), "." "\n"); 
	pmesserr("%s: " "The integer value %d is too large to fit in the range %d..%d - Ceiling it to %d." "\n", __func__, casted_int_value, i_min, i_max, i_max); 
	*(int *) cast_value2 = i_max; 
      }; 
    } while (false); break; 
  }; 
  
}; 
















void pscript_value__operation__bool_and(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {

  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  const char bval1 = pscript_value__cast_to_bool__scalar(this, type1, type1_sizeof, value1); 
  if (!bval1) {       
    *retval_value = false; 
    return; 
  }; 

  const char bval2 = pscript_value__cast_to_bool__scalar(this, type2, type2_sizeof, value2); 
  *retval_value = bval2; 
}; 

void pscript_value__operation__bool_or(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {

  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  const char bval1 = pscript_value__cast_to_bool__scalar(this, type1, type1_sizeof, value1); 
  if (bval1) {       
    *retval_value = true; 
    return; 
  }; 
  
  const char bval2 = pscript_value__cast_to_bool__scalar(this, type2, type2_sizeof, value2); 
  *retval_value = bval2; 
}; 

void pscript_value__operation__bool_xor(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {

  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  const char bval1 = pscript_value__cast_to_bool__scalar(this, type1, type1_sizeof, value1); 
  const char bval2 = pscript_value__cast_to_bool__scalar(this, type2, type2_sizeof, value2); 
  //*retval_value = bval1 ^^ bval2; 
  if (bval1) *retval_value = !bval2; 
  else *retval_value = bval2; 
}; 

void pscript_value__operation__bool_not(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  const char bval1 = pscript_value__cast_to_bool__scalar(this, type1, type1_sizeof, value1); 
  *retval_value = !bval1; 
}; 
 
void pscript_value__operation__equal_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) { 
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  
  if (type1 != type2) { 
    *retval_value = false; 
    return; 
  }; 
  
  for (int i = 0; i < type1_sizeof; i++) { 
    if (value1[i] != value2[i]) { 
      *retval_value = false; 
      return; 
    }; 
  }; 
  
  *retval_value = true; 
}; 
 
void pscript_value__operation__diff_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 

  if (type1 != type2) { 
    *retval_value = true; 
    return; 
  }; 
  
  for (int i = 0; i < type1_sizeof; i++) {
    if (value1[i] != value2[i]) { 
      *retval_value = true; 
      return; 
    }; 
  }; 
  
  *retval_value = false; 
}; 

void pscript_value__operation__inf_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 

  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  *retval_value = (intval1 < intval2); 
}; 

void pscript_value__operation__sup_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 

  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  *retval_value = (intval1 > intval2); 
}; 

void pscript_value__operation__infeq_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) { 
#if 0 
  dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
  dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 

  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  
#if 0 
  pmessage("INFEQ_HUH: %d <= %d" "\n", intval1, intval2); 
#endif 
  
  *retval_value = (intval1 <= intval2); 
}; 

void pscript_value__operation__supeq_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
#if 0
  dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
  dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
  
  *retval_type_ref = pscript_type__get__bool(this -> type_env); 
  *retval_sizeof_ref = 1; 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 

  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  *retval_value = (intval1 >= intval2); 
}; 

void pscript_value__operation__neg(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: { 
    *retval_type_ref = pscript_type__get__integer(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const int intval1 = *(const int *) value1; 
    *((int *) retval_value) = -intval1; 
  }; break; 
  case PSCRIPT_TYPE__FLOAT: { 
    *retval_type_ref = pscript_type__get__float(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const float floatval1 = *(const float *) value1; 
    *((float *) retval_value) = - floatval1;
  }; break; 
  }; 
}; 

void pscript_value__operation__idiv(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__integer(this -> type_env); 
  *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  
  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  *((int *) retval_value) = intval1 / intval2; 
}; 

void pscript_value__operation__imod(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__integer(this -> type_env); 
  *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  
  const int intval1 = pscript_value__cast_to_integer__scalar(this, type1, type1_sizeof, value1); 
  const int intval2 = pscript_value__cast_to_integer__scalar(this, type2, type1_sizeof, value2); 
  *((int *) retval_value) = intval1 % intval2; 
}; 

void pscript_value__operation__rdiv(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  
  *retval_type_ref = pscript_type__get__float(this -> type_env); 
  *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  
  const float floatval1 = pscript_value__cast_to_float__scalar(this, type1, type1_sizeof, value1); 
  const float floatval2 = pscript_value__cast_to_float__scalar(this, type2, type1_sizeof, value2); 
  *((float *) retval_value) = floatval1 / floatval2; 
}; 













static void pscript_value__operation__mul__integer(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  assert(type1 == pscript_type__get__integer(this -> type_env)); 

  const int intval1 = *(const int *) value1; 

  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 

  case PSCRIPT_TYPE__PROXY: do { 
      const int subtype2          = pscript_type__proxy__get_subtype(this -> type_env, type2); 
      const int subtype2_offset   = *(const int_pscript_mem_t *) value2; 
      const int subtype2_sizeof   = pscript_type__sizeof(this -> type_env, subtype2); 
      const char * subtype2_value = pscript_mem__get_pointer(this -> mem_env, subtype2_offset); 
      pscript_value__operation__mul__integer(this, type1, type1_sizeof, value1, subtype2, /*sizeof*/subtype2_sizeof, /*value*/subtype2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
    } while (false); break; 

  case PSCRIPT_TYPE__INTEGER: do { 
      *retval_type_ref = pscript_type__get__integer(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const int intval2 = *(const int *) value2; 
      *((int *) retval_value) = intval1 * intval2; 
    } while (false); break; 

  case PSCRIPT_TYPE__FLOAT: do { 
      *retval_type_ref = pscript_type__get__float(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const float floatval1 = (float) intval1; 
      const float floatval2 = *(const float *) value2; 
      *((float *) retval_value) = floatval1 * floatval2; 
    } while (false); break; 

  case PSCRIPT_TYPE__RECORD: do { 
      assert(type2_sizeof <= retval_sizeof_alloc); 
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type2); //this -> type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) {
	const int field_name     = pscript_type__record__get_field_name  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value2 + field_offset; 
#if 0 
      dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	pscript_value__operation__mul__integer(this, type1, type1_sizeof, value1, field_type, field_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
      }; 
      *retval_type_ref = type2; 
      *retval_sizeof_ref = type2_sizeof; 
    } while (false); break; 

  case PSCRIPT_TYPE__USER: do { 
      const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__mul__integer(this, type1, type1_sizeof, value1, subtype, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break; 
    
  }; 
}; 

static void pscript_value__operation__mul__float(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  assert(type1 == pscript_type__get__float(this -> type_env)); 

  const float floatval1 = *(const float *) value1; 
  //pmessage("%s: " "\n", __func__, 

  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
    
  case PSCRIPT_TYPE__PROXY: do { 
      const int subtype2          = pscript_type__proxy__get_subtype(this -> type_env, type2); 
      const int subtype2_offset   = *(const int_pscript_mem_t *) value2; 
      const int subtype2_sizeof   = pscript_type__sizeof(this -> type_env, subtype2); 
      const char * subtype2_value = pscript_mem__get_pointer(this -> mem_env, subtype2_offset); 
      pscript_value__operation__mul__float(this, type1, type1_sizeof, value1, subtype2, /*sizeof*/subtype2_sizeof, /*value*/subtype2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
    } while (false); break; 

  case PSCRIPT_TYPE__INTEGER: do { 
    *retval_type_ref = pscript_type__get__float(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const int intval2 = *(const int *) value2; 
    const float floatval2 = (float) intval2; 
    *((float *) retval_value) = floatval1 * floatval2; 
  } while (false); break; 
    
  case PSCRIPT_TYPE__FLOAT: do { 
    *retval_type_ref = pscript_type__get__float(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const float floatval2 = *(const float *) value2; 
    *((float *) retval_value) = floatval1 * floatval2; 
  } while (false); break; 
    
  case PSCRIPT_TYPE__RECORD: do { 
      assert(type2_sizeof <= retval_sizeof_alloc); 
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type2); //this -> type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) {
	const int field_name     = pscript_type__record__get_field_name  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value2 + field_offset; 
#if 0 
      dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	pscript_value__operation__mul__float(this, type1, type1_sizeof, value1, field_type, field_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
      }; 
      *retval_type_ref = type2; 
      *retval_sizeof_ref = type2_sizeof; 
    } while (false); break; 
    
  case PSCRIPT_TYPE__USER: do { 
      const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__mul__float(this, type1, type1_sizeof, value1, subtype, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break; 
    
  }; 
}; 



static void pscript_value__operation__mul__record_record(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__RECORD); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  //pmessage("type2_core = [ %d ] %s " "\n", type2_core, pscript_type_core__cstr(type2_core));
  assert(type2_core == PSCRIPT_TYPE__RECORD); 


  do {
      assert(type1_sizeof <= retval_sizeof_alloc); 
      const int field1_nb = pscript_type__record__get_field_nb(this -> type_env, type1); 
      const int field2_nb = pscript_type__record__get_field_nb(this -> type_env, type2); 
      const int field_nb = MIN(field1_nb, field2_nb); 
      for (int i = 0; i < field_nb; i++) { 
	const int field1_type     = pscript_type__record__get_field_type  (this -> type_env, type1, i); 
	const int field1_offset   = pscript_type__record__get_field_offset(this -> type_env, type1, i); 
	const int field1_sizeof   = pscript_type__sizeof(this -> type_env, field1_type); 
	const char * field1_value = value1 + field1_offset; 
	const int field2_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); 
	const int field2_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); 
	const int field2_sizeof   = pscript_type__sizeof(this -> type_env, field2_type); 
	const char * field2_value = value2 + field2_offset; 
#if 1
	pscript_value__operation__mul(this, field1_type, field1_sizeof, field1_value, field2_type, field2_sizeof, field2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field1_offset, retval_value + field1_offset); 
#else
	binary_operation * op = pscript_value__operation__mul__methods[field1_type]; 
	assert(op != NULL); 
	
#if 0 
	dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	op(this, field1_type, field1_sizeof, field1_value, field2_type, field2_sizeof, field2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field1_offset, retval_value + field1_offset); 
#endif 
      }; 
      *retval_type_ref = type1; 
      *retval_sizeof_ref = type1_sizeof; 
    } while (false);  

    



}; 




static void pscript_value__operation__mul__record(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__RECORD); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__mul__integer(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT: pscript_value__operation__mul__float(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD: pscript_value__operation__mul__record_record(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      const int subtype2_core = pscript_type__get_core_type(this -> type_env, type2); 
      //pmessage("subtype2_core = [ %d ] %s " "\n", subtype2_core, pscript_type_core__cstr(subtype2_core));
      pscript_value__operation__mul__record(this, type1, type1_sizeof, value1, subtype2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break; 
  }; 
}; 


static void pscript_value__operation__mul__udf(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__USER); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) { 
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__mul__integer(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT: pscript_value__operation__mul__float(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD: pscript_value__operation__mul__record(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__mul__udf(this, type1, type1_sizeof, value1, subtype2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
    } while (false); break; 
  }; 
}; 


void pscript_value__operation__mul(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__mul__integer(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT: pscript_value__operation__mul__float(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD: pscript_value__operation__mul__record(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: pscript_value__operation__mul__udf(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  }; 
}; 





static void pscript_value__operation__add__integer(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  assert(type1 == pscript_type__get__integer(this -> type_env)); 

  const int intval1 = *(const int *) value1; 

  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 

  case PSCRIPT_TYPE__INTEGER: do { 
      *retval_type_ref = pscript_type__get__integer(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const int intval2 = *(const int *) value2; 
      *((int *) retval_value) = intval1 + intval2; 
    } while (false); break; 

  case PSCRIPT_TYPE__FLOAT: do { 
      *retval_type_ref = pscript_type__get__float(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const float floatval1 = (float) intval1; 
      const float floatval2 = *(const float *) value2; 
      *((float *) retval_value) = floatval1 + floatval2; 
    } while (false); break; 

  case PSCRIPT_TYPE__RECORD: do { 
      assert(type2_sizeof <= retval_sizeof_alloc); 
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type2); //this -> type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) {
	const int field_name     = pscript_type__record__get_field_name  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value2 + field_offset; 
#if 0 
      dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	pscript_value__operation__add__integer(this, type1, type1_sizeof, value1, field_type, field_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
      }; 
      *retval_type_ref = type2; 
      *retval_sizeof_ref = type2_sizeof; 
    } while (false); break; 

    case PSCRIPT_TYPE__ARRAY: do {
      assert(type2_sizeof <= retval_sizeof_alloc); 
	const int i_min          = pscript_type__array__get_i_min  (this -> type_env, type2); 
	const int i_max          = pscript_type__array__get_i_max  (this -> type_env, type2); 
	const int subtype        = pscript_type__array__get_subtype(this -> type_env, type2); 
	const int subtype_sizeof = pscript_type__sizeof(this -> type_env, subtype); 
	const int field_nb       = i_max - i_min + 1; 
	for (int i = 0; i < field_nb; i++) { 
	  const int field_offset   = i * subtype_sizeof; 
	  const char * field_value = value1 + field_offset; 
	  pscript_value__operation__add__integer(this, type1, type1_sizeof, value1, subtype, subtype_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
	}; 
	*retval_type_ref = type2; 
	*retval_sizeof_ref = type2_sizeof; 
      } while (false); break; 
    
  case PSCRIPT_TYPE__USER: do { 
      const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__add__integer(this, type1, type1_sizeof, value1, subtype, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break; 
    
  }; 
}; 

static void pscript_value__operation__add__float(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  assert(type1 == pscript_type__get__float(this -> type_env)); 

  const float floatval1 = *(const float *) value1; 
  //pmessage("%s: " "\n", __func__, 

  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
    
  case PSCRIPT_TYPE__INTEGER: do { 
    *retval_type_ref = pscript_type__get__float(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const int intval2 = *(const int *) value2; 
    const float floatval2 = (float) intval2; 
    *((float *) retval_value) = floatval1 + floatval2; 
  } while (false); break; 
    
  case PSCRIPT_TYPE__FLOAT: do { 
    *retval_type_ref = pscript_type__get__float(this -> type_env); 
    *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
    assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
    const float floatval2 = *(const float *) value2; 
    *((float *) retval_value) = floatval1 + floatval2; 
  } while (false); break; 
    
  case PSCRIPT_TYPE__RECORD: do { 
      assert(type2_sizeof <= retval_sizeof_alloc); 
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type2); //this -> type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) {
	const int field_name     = pscript_type__record__get_field_name  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value2 + field_offset; 
#if 0 
      dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	pscript_value__operation__add__float(this, type1, type1_sizeof, value1, field_type, field_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
      }; 
      *retval_type_ref = type2; 
      *retval_sizeof_ref = type2_sizeof; 
    } while (false); break; 
    
  case PSCRIPT_TYPE__USER: do { 
      const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__add__float(this, type1, type1_sizeof, value1, subtype, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break; 
    
  }; 
}; 

static void pscript_value__operation__add__string(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) { 
  assert(type1 == pscript_type__get__string(this -> type_env)); 
  
  //const int type1_pstr = *(const int *) value1; 
  
  const int type1_pstr = pscript_value__string(this, /*type1*/type1, /*value1_sizeof*/type1_sizeof, /*value1*/value1); 
  const int type2_pstr = pscript_value__string(this, /*type1*/type2, /*value1_sizeof*/type2_sizeof, /*value1*/value2); 
  
  const char * type1_cstr = pscript_string__get_cstr(this -> string_env, type1_pstr); 
  const char * type2_cstr = pscript_string__get_cstr(this -> string_env, type2_pstr); 
  
  const char * retval_cstr = strconcat__stack(type1_cstr, type2_cstr); 
  
  const int retval_pstr = pscript_string__push(this -> string_env, retval_cstr); 
  
  *retval_type_ref       = pscript_type__get__string(this -> type_env); 
  *retval_sizeof_ref     = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
  assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
  *((int *) retval_value) = retval_pstr;   
}; 







typedef void binary_operation(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]);  

static binary_operation * pscript_value__operation__add__methods[] = { 
 NULL, //PSCRIPT_TYPE__NULL, 
 NULL, //PSCRIPT_TYPE__VOID, 
 NULL, //PSCRIPT_TYPE__ANY, 
 NULL, //PSCRIPT_TYPE__BOOL, 
 pscript_value__operation__add__integer, //PSCRIPT_TYPE__INTEGER, 
 pscript_value__operation__add__integer, //PSCRIPT_TYPE__INT_RANGE, 
 pscript_value__operation__add__integer, //PSCRIPT_TYPE__INT_RANGE__DYN, 
 pscript_value__operation__add__float, //PSCRIPT_TYPE__FLOAT, 
 NULL, //PSCRIPT_TYPE__CHAR, 
 NULL, //PSCRIPT_TYPE__STRING, 
 NULL, //PSCRIPT_TYPE__VARIANT, 
 NULL, //PSCRIPT_TYPE__NIL_LIST, 
 NULL, //PSCRIPT_TYPE__NIL_PTR, 
 NULL, //PSCRIPT_TYPE__POINTER, // RL: A pointer is never the owner of an object. It does not have to do any memory management. 
 NULL, //PSCRIPT_TYPE__PROXY, 
 NULL, //PSCRIPT_TYPE__PROXY_BYTECODE, 
 NULL, //PSCRIPT_TYPE__LIST, 
 NULL, //PSCRIPT_TYPE__ARRAY, 
 NULL, //PSCRIPT_TYPE__ARRAY__DYN, 
 NULL, //PSCRIPT_TYPE__RECORD, 
 NULL, //PSCRIPT_TYPE__FUNPROC, 
 NULL, //PSCRIPT_TYPE__FFIPROC, 
 NULL, //PSCRIPT_TYPE__LIBPROC, 
 NULL, //PSCRIPT_TYPE__LAMBDA_META, 
 NULL, //PSCRIPT_TYPE__LAMBDA, 
 NULL, //PSCRIPT_TYPE__FUNSIGNATURE, 
 NULL, //PSCRIPT_TYPE__USER, 
  NULL 
}; 

enum { pscript_value__operation__add__methods__size = ARRAY_SIZE(pscript_value__operation__add__methods) }; 

static void assert_compile_time__d632sad4gdss(void) {
  ASSERT_COMPILE(pscript_value__operation__add__methods__size == PSCRIPT_TYPE__COUNT + 1); 
}; 
 

static void pscript_value__operation__add__record_record(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__RECORD); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  assert(type2_core == PSCRIPT_TYPE__RECORD); 


  do {
      assert(type1_sizeof <= retval_sizeof_alloc); 
      const int field1_nb = pscript_type__record__get_field_nb(this -> type_env, type1); 
      const int field2_nb = pscript_type__record__get_field_nb(this -> type_env, type2); 
      const int field_nb = MIN(field1_nb, field2_nb); 
      for (int i = 0; i < field_nb; i++) { 
	const int field1_type     = pscript_type__record__get_field_type  (this -> type_env, type1, i); 
	const int field1_offset   = pscript_type__record__get_field_offset(this -> type_env, type1, i); 
	const int field1_sizeof   = pscript_type__sizeof(this -> type_env, field1_type); 
	const char * field1_value = value1 + field1_offset; 
	const int field2_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); 
	const int field2_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); 
	const int field2_sizeof   = pscript_type__sizeof(this -> type_env, field2_type); 
	const char * field2_value = value2 + field2_offset; 
#if 1
	pscript_value__operation__add(this, field1_type, field1_sizeof, field1_value, field2_type, field2_sizeof, field2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field1_offset, retval_value + field1_offset); 
#else
	binary_operation * op = pscript_value__operation__add__methods[field1_type]; 
	assert(op != NULL); 
	
#if 0 
	dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	op(this, field1_type, field1_sizeof, field1_value, field2_type, field2_sizeof, field2_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field1_offset, retval_value + field1_offset); 
#endif 
      }; 
      *retval_type_ref = type1; 
      *retval_sizeof_ref = type1_sizeof; 
    } while (false);  

    



}; 








static void pscript_value__operation__add__array_array(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__ARRAY); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  assert(type2_core == PSCRIPT_TYPE__ARRAY); 

  do {
    const int field1_i_min          = pscript_type__array__get_i_min  (this -> type_env, type1); 
    const int field1_i_max          = pscript_type__array__get_i_max  (this -> type_env, type1); 
    const int field1_nb = field1_i_max - field1_i_min + 1; 
    const int field2_i_min          = pscript_type__array__get_i_min  (this -> type_env, type2); 
    const int field2_i_max          = pscript_type__array__get_i_max  (this -> type_env, type2); 
    const int field2_nb = field2_i_max - field2_i_min + 1; 
    const int field_nb = MIN(field1_nb, field2_nb); 
    const int field1_type        = pscript_type__array__get_subtype(this -> type_env, type1); 
    const int field2_type        = pscript_type__array__get_subtype(this -> type_env, type2); 
    const int field1_sizeof   = pscript_type__sizeof(this -> type_env, field1_type); 
    const int field2_sizeof   = pscript_type__sizeof(this -> type_env, field2_type); 
    const int field_max_sizeof = MAX(field1_sizeof, field2_sizeof); 
    assert(field_nb * field_max_sizeof <= retval_sizeof_alloc); 
    int field_type = -1; 
    int field_sizeof; 
    int cumulated_offset = 0; 
    for (int i = 0; i < field_nb; i++) { 
      const int    field1_offset = field1_sizeof * i; 
      const char * field1_value  = value1 + field1_offset; 
      const int    field2_offset = field2_sizeof * i; 
      const char * field2_value  = value2 + field2_offset; 
      pscript_value__operation__add(this, field1_type, field1_sizeof, field1_value, field2_type, field2_sizeof, field2_value, &field_type, &field_sizeof, retval_sizeof_alloc - cumulated_offset, retval_value + cumulated_offset); 
      cumulated_offset += field_sizeof; 
    }; 
    const int field_i_min = field1_i_min == field2_i_min ? field1_i_min : 0; 
    const int field_i_max = field_i_min + field_nb - 1; 
    *retval_type_ref   = pscript_type__make__array(this -> type_env, field_type, field_i_min, field_i_max); 
    *retval_sizeof_ref = cumulated_offset; 
  } while (false);  
  
}; 




static void pscript_value__operation__add__record(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__RECORD); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__add__integer      (this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT:   pscript_value__operation__add__float        (this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD:  pscript_value__operation__add__record_record(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__add__record(this, type1, type1_sizeof, value1, subtype2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break;     
  }; 
}; 


static void pscript_value__operation__add__array(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__ARRAY); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__add__integer     (this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT:   pscript_value__operation__add__float       (this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  //case PSCRIPT_TYPE__RECORD:  pscript_value__operation__add__array_record(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__ARRAY:   pscript_value__operation__add__array_array (this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__add__record(this, type1, type1_sizeof, value1, subtype2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type2; 
    } while (false); break;     
  }; 
}; 


static void pscript_value__operation__add__udf(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__USER); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__add__integer(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT: pscript_value__operation__add__float(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD: pscript_value__operation__add__record(this, type2, type2_sizeof, value2, type1, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      pscript_value__operation__add__udf(this, type1, type1_sizeof, value1, subtype2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
    } while (false); break; 
  }; 
}; 


void pscript_value__operation__add(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: pscript_value__operation__add__integer(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__FLOAT:   pscript_value__operation__add__float  (this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__STRING:  pscript_value__operation__add__string (this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__RECORD:  pscript_value__operation__add__record (this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__ARRAY:   pscript_value__operation__add__array  (this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  case PSCRIPT_TYPE__USER:    pscript_value__operation__add__udf    (this, type1, type1_sizeof, value1, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); break; 
  }; 
}; 







void pscript_value__operation__opposite(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) {
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  switch (type1_core) {
  default: assert(false); break; 

  case PSCRIPT_TYPE__INTEGER: do { 
      *retval_type_ref = pscript_type__get__integer(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const int intval1 = *(const int *) value1; 
      *((int *) retval_value) = - intval1; 
    } while (false); break; 

  case PSCRIPT_TYPE__FLOAT: do { 
      *retval_type_ref = pscript_type__get__float(this -> type_env); 
      *retval_sizeof_ref = pscript_type__sizeof(this -> type_env, *retval_type_ref); 
      assert(retval_sizeof_alloc >= *retval_sizeof_ref); 
      const float floatval1 = *(const float *) value1; 
      *((float *) retval_value) = - floatval1; 
    } while (false); break; 

  case PSCRIPT_TYPE__RECORD: do { 
      assert(type1_sizeof <= retval_sizeof_alloc); 
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type1); //this -> type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) {
	const int field_name     = pscript_type__record__get_field_name  (this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type1, i); //this -> type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value1 + field_offset; 
#if 0 
      dputs(stderr_d, "FIELD_VALUE: [ "); dputn(stderr_d, field_type); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(field_sizeof, field_value); dputs(stderr_d, "\n"); 
#endif 
	pscript_value__operation__opposite(this, field_type, field_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
      }; 
      *retval_type_ref = type1; 
      *retval_sizeof_ref = type1_sizeof; 
    } while (false); break; 

    case PSCRIPT_TYPE__ARRAY: do {
      assert(type1_sizeof <= retval_sizeof_alloc); 
	const int i_min = pscript_type__array__get_i_min(this -> type_env, type1); 
	const int i_max = pscript_type__array__get_i_max(this -> type_env, type1); 
	const int subtype = pscript_type__array__get_subtype(this -> type_env, type1); 
	const int subtype_sizeof = pscript_type__sizeof(this -> type_env, subtype); 
	const int field_nb = i_max - i_min + 1; 
	for (int i = 0; i < field_nb; i++) { 
	  const int field_offset = i * subtype_sizeof; 
	  const char * field_value = value1 + field_offset; 
	  pscript_value__operation__opposite(this, subtype, subtype_sizeof, field_value, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc - field_offset, retval_value + field_offset); 
	}; 
	*retval_type_ref = type1; 
	*retval_sizeof_ref = type1_sizeof; 
      } while (false); break;  
    
  case PSCRIPT_TYPE__USER: do { 
      const int subtype = pscript_type__user_defined_type__get_subtype(this -> type_env, type1); 
      pscript_value__operation__opposite(this, subtype, type1_sizeof, value1, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
      *retval_type_ref = type1; 
    } while (false); break; 
    
  }; 
}; 





void pscript_value__operation__sub(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]) { 
  char value2_opposite[type2_sizeof]; 
  pscript_value__operation__opposite(this, type2, type2_sizeof, value2, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, value2_opposite); 
  pscript_value__operation__add(this, type1, type1_sizeof, value1, type2, type2_sizeof, value2_opposite, retval_type_ref, retval_sizeof_ref, retval_sizeof_alloc, retval_value); 
}; 




const char * pscript_type__transparent_proxy__get_value(pscript_t * this, const int type2, const int type2_sizeof, const char value2[]) { 
  int          subtype2        = type2; 
  int          subtype2_offset; 
  const char * subtype2_value  = value2; 
  for (;;) { 
    const int subtype2_core = pscript_type__get_core_type(this -> type_env, subtype2); 
    if (PSCRIPT_TYPE__PROXY != subtype2_core) { return subtype2_value; }; 
    subtype2        = pscript_type__proxy__get_subtype(this -> type_env, subtype2); 
    subtype2_offset = *(const int_pscript_mem_t *) subtype2_value; 
    subtype2_value  = pscript_mem__get_pointer(this -> mem_env, subtype2_offset); 
  };
}; 

