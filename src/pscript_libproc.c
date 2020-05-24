#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_value.h" 
#include "pscript_libproc.h" 
#include "pvm_bytecode__type.ci" 


pscript_libproc_env_t * pscript_libproc_env__make(pscript_t * pscript_env) {
  MALLOC_BZERO(pscript_libproc_env_t,this); 
  this -> pscript_env = pscript_env; 
  this -> halt_offset = -1; 
  return this;
}; 

void pscript_libproc_env__delete(pscript_libproc_env_t * this) {
  free(this);
};   

int pscript_libproc__get_count(const pscript_libproc_env_t * this) {
  return this -> libproc_nb; 
}; 

int pscript_libproc__get_type(pscript_libproc_env_t * this, pscript_type_env_t * type_env, const int libproc_i) {
  assert(libproc_i >= 0); 
  assert(libproc_i < this -> libproc_nb); 
  const int index = libproc_i; 
  if (this -> libproc_type[libproc_i] < 0) {
    //this -> libproc_type[libproc_i] = pscript_type__make__libproc(this -> type_env, this -> arg_nb[index], this -> arg_name[index], this -> arg_byref[index], this -> arg_type[index], this -> retval_type[index]); 
    //this -> libproc_type[libproc_i] = pscript_type__make__libproc(type_env, this -> arg_nb[index], this -> arg_byref[index], this -> arg_type[index]); 
    this -> libproc_type[libproc_i] = pscript_type__make__libproc(type_env, this -> arg_nb[index], this -> arg_byref[index]); 
  }; 
  return this -> libproc_type[libproc_i]; 
}; 


//int pscript_libproc__push(pscript_libproc_env_t * this, const int name, const int arg_nb, const int arg_atleast, void * callback_data, pscript_libproc_callback_t * callback) {
int pscript_libproc__push(pscript_libproc_env_t * this, const int name, const int arg_nb, const int arg_atleast, pscript_libproc_callback_t * callback) {
  assert(this -> libproc_nb < pscript_libproc__size); 
  const int index = this -> libproc_nb; 
  this -> libproc_name[index] = name; 
  this -> libproc_type[index] =   -1; 
  //this -> libproc_callback_data[index] = callback_data; 
  this -> libproc_callback[index] = callback; 
  this -> arg_nb[index] = arg_nb; 
  this -> arg_atleast[index] = arg_atleast; 
  this -> libproc_nb ++; 
  return index; 
}; 


int pscript_libproc__get_name_pstr(const pscript_libproc_env_t * this, const int libproc_i) {
  return this -> libproc_name[libproc_i];
}; 

const char * pscript_libproc__get_name_cstr(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env, const int libproc_i) {
  return pscript_string__get_cstr(string_env, this -> libproc_name[libproc_i]); 
}; 

int pscript_libproc__get_arg_nb(const pscript_libproc_env_t * this, const int libproc_i) {
  return this -> arg_nb[libproc_i];
}; 

int pscript_libproc__get_arg_atleast(const pscript_libproc_env_t * this, const int libproc_i) {
  return this -> arg_atleast[libproc_i];
}; 

int pscript_libproc__get_arg_byref(const pscript_libproc_env_t * this, const int libproc_i, const int arg_i) { 
  return this -> arg_byref[libproc_i][arg_i];
}; 
 



int pscript_libproc__call(const pscript_libproc_env_t * this, const int libproc_i, const int on_stack_arg_nb) { 
  assert(libproc_i >= 0); 
  assert(libproc_i < this -> libproc_nb); 
  //return this -> libproc_callback[libproc_i](this -> libproc_callback_data[libproc_i]); 
  //return this -> libproc_callback[libproc_i](this -> pscript_env); 
  return this -> libproc_callback[libproc_i](this -> pscript_env, on_stack_arg_nb); 
}; 
 
 
 


#if 1 
static int pscript_libproc__map__one(pscript_t * this, const int call_address, const int data_type, const int data_sizeof, const char * data_value) { 
  const int8_t on_stack_arg_nb = 1; 
  const int ret_address = this -> libproc_env -> halt_offset; //this -> bytecode_env -> bytecode_nb - 1; 
  //const int base_mem_offset = pscript_mem__get_base_offset(this -> mem_env); 
  const int caller_ebp = pscript_vm__memory_frame__get(this -> vm); 
  //pscript_vm__call_stack__push(this -> vm, ret_address, arg_nb, base_mem_offset); 
  pscript_vm__call_stack__push(this -> vm, ret_address, on_stack_arg_nb, caller_ebp); 
  const int next_ipc0 = call_address; 
  
  pscript_stack__push_value(this -> stack_env, data_type, data_sizeof, data_value); 
  
#if 0 
  dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
  
  const int allright_huh = this -> vm -> run(this -> vm, next_ipc0); 
  if (allright_huh != PSCRIPT_VM__ERROR__HALT) { 
    this -> vm -> print_error(this -> vm, allright_huh); 
  }; 
  
  return allright_huh; 
}; 
#else 
static int pscript_libproc__map__one(pscript_t * this, const int call_address, const int data_type, const int data_sizeof, const char * data_value) { 
  const int8_t arg_nb = 1; 
  const int ret_address = this -> libproc_env -> halt_offset; //this -> bytecode_env -> bytecode_nb - 1; 
  const int base_mem_offset = pscript_mem__get_base_offset(this -> mem_env); 
  pscript_vm__call_stack__push(this -> vm, ret_address, arg_nb, base_mem_offset); 
  const int next_ipc0 = call_address; 
  
  pscript_stack__push_value(this -> stack_env, data_type, data_sizeof, data_value); 

#if 0 
	dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
	dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 

  const int allright_huh = this -> vm -> run(this -> vm, next_ipc0); 
  if (allright_huh != PSCRIPT_VM__ERROR__HALT) { 
    this -> vm -> print_error(this -> vm, allright_huh); 
  }; 
  
  return allright_huh; 
}; 
#endif 


//static int pscript_libproc__map__aux(pscript_t * this, const int call_address, const int type2, const char * value2) { 
static int pscript_libproc__map__aux(pscript_t * this, const int call_address, const int type2, const char * value2, const int funproc_retval_type, const int funproc_retval_sizeof) { 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  const int type2_sizeof = pscript_type__sizeof(this -> type_env, type2); 

  //pmessage("pscript_libproc__map_aux: type2: [ %d ] %s " "\n", type2, pscript_type__get_cstr(this -> type_env, type2));
  //pmessage("pscript_libproc__map_aux: type2_core: [ %d ] %s " "\n", type2_core, pscript_type_core__cstr(type2_core));


  //int next_ipc0 = -1; 
  int allright_huh; 
  switch (type2_core) {
  default: assert(false); break; 
  case PSCRIPT_TYPE__INTEGER: 
  case PSCRIPT_TYPE__FLOAT: 
    allright_huh = pscript_libproc__map__one(this, call_address, type2, type2_sizeof, value2); 
    break; 
  case PSCRIPT_TYPE__RECORD: do {
      const int retval_type = type2; 
      const int retval_sizeof = type2_sizeof;						
      char retval_value[retval_sizeof];				
  
      const int field_nb = pscript_type__record__get_field_nb(this -> type_env, type2); //type_env -> cell__array[type_i].record_field_nb; 
      for (int i = 0; i < field_nb; i++) { 
	//const int field_name     = pscript_type__record__get_field_name  (type_env, type2, i); //type_env -> cell__array[type_i].record_field_name[i]; 
	const int field_type     = pscript_type__record__get_field_type  (this -> type_env, type2, i); //type_env -> cell__array[type_i].record_field_type[i]; 
	const int field_offset   = pscript_type__record__get_field_offset(this -> type_env, type2, i); //type_env -> cell__array[type_i].record_field_offset[i]; 
	const int field_sizeof   = pscript_type__sizeof(this -> type_env, field_type); 
	const char * field_value = value2 + field_offset; 

	allright_huh = pscript_libproc__map__one(this, call_address, field_type, field_sizeof, field_value); 
	
#if 0 
	dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
	dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 

	if (funproc_retval_sizeof > 0) { 
	  const int one_offset = pscript_stack__get_top__offset(this -> stack_env); 
	  const char * one_pointer = pscript_stack__get_top__pointer(this -> stack_env); 
	  const int one_type = pscript_stack__get_top__type(this -> stack_env); 
	  const int one_sizeof = pscript_type__sizeof(this -> type_env, one_type); 
	  if (field_type == one_type) { 
	    bcopy(this -> stack_env -> mem__array + one_offset, retval_value + field_offset, one_sizeof); 
	  } 
	  else { 
	    pscript_value__cast_generic(this, field_type, one_type, one_sizeof, one_pointer, field_sizeof, retval_value + field_offset); 
	  }; 
	  pscript_stack__drop(this -> stack_env);	      
	}; 
      };
      
      pscript_stack__push_value(this -> stack_env, retval_type, retval_sizeof, retval_value); 



#if 0 
    dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
    dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 



    } while (false); 
    break; 






  case PSCRIPT_TYPE__ARRAY: do {
      const int field2_i_min  = pscript_type__array__get_i_min  (this -> type_env, type2); 
      const int field2_i_max  = pscript_type__array__get_i_max  (this -> type_env, type2); 
      const int field2_nb     = field2_i_max - field2_i_min + 1; 
      const int field2_type   = pscript_type__array__get_subtype(this -> type_env, type2); 
      const int field2_sizeof = pscript_type__sizeof            (this -> type_env, field2_type); 
      
      const int retval_type   = pscript_type__make__array(this -> type_env, funproc_retval_type, field2_i_min, field2_i_max); 
      const int retval_sizeof = pscript_type__sizeof(this -> type_env, retval_type); 
      const int retval__stack_offset = (retval_sizeof > 0) ? pscript_stack__push_uninitialized(this -> stack_env, retval_type) : 0; 
      char *    retval_value  = (retval_sizeof > 0) ? pscript_stack__stack_offset__get__pointer_writable(this -> stack_env, retval__stack_offset) : NULL; 
      
      //int field2_offset = 0; 
      int retval_offset = 0; 
      const char * field2_value = value2; 
      for (int i = 0; i < field2_nb; i++) { 
	allright_huh = pscript_libproc__map__one(this, call_address, field2_type, field2_sizeof, field2_value); 
	
#if 0 
	dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
	dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
	
	//field2_offset += field2_sizeof; 
	field2_value  += field2_sizeof; 
	
	if (funproc_retval_sizeof > 0) { 
	  const char * one_pointer = pscript_stack__get_top__pointer(this -> stack_env); 
	  const int one_type = pscript_stack__get_top__type(this -> stack_env); 
	  const int one_sizeof = pscript_type__sizeof(this -> type_env, one_type); 
	  assert(funproc_retval_type == one_type); 
	  bcopy(one_pointer, retval_value + retval_offset, one_sizeof); 
	  pscript_stack__drop(this -> stack_env); 
	  retval_offset += funproc_retval_sizeof; 
	}; 
      };
      
      // RL: TODO XXX FIXME IMPROVE: The issue is that the type of the mapped function/procedure (the first argument) is not computed at compile-time (not yet). 
      //     And, as such, we can't foreknow whether an object gonna be returned. 
      //     So we suppose that methods always return something.  
      if (funproc_retval_sizeof == 0) { 
	pscript_stack__push_uninitialized(this -> stack_env, funproc_retval_type); 
      }; 
      
      
#if 0 
      dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
      dputs(stderr, "STACK MEM : "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
      
      
      
    } while (false); 
    break; 

    
    
    
    
    

  case PSCRIPT_TYPE__USER: do { 
      const int subtype2 = pscript_type__user_defined_type__get_subtype(this -> type_env, type2); 
      //const int subtype2_core = pscript_type__get_core_type(type_env, type2); 
      //pmessage("subtype2_core = [ %d ] %s " "\n", subtype2_core, pscript_type_core__cstr(subtype2_core));
      //allright_huh = pscript_libproc__map__one(this, call_address, subtype2, type2_sizeof, value2); 
      allright_huh = pscript_libproc__map__aux(this, call_address, subtype2, value2, funproc_retval_type, funproc_retval_sizeof); 
      //*retval_type_ref = type2; 
      //pscript_stack__push_value(this -> stack_env, retval_type, retval_sizeof, retval_value); 
      this -> stack_env -> stack__type[this -> stack_env -> stack__nb - 1] = type2; 
    } while (false); break; 
  }; 

  return allright_huh; 
  
}; 

static int pscript_libproc__map(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
    dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
    dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
  assert(this -> stack_env -> stack__nb >= 2);		
  const int type_index1 = this -> stack_env -> stack__nb - 2; 
  const int type_index2 = this -> stack_env -> stack__nb - 1; 
  const int type1 = this -> stack_env -> stack__type[type_index1]; 
  const int type2 = this -> stack_env -> stack__type[type_index2]; 
  const int type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
  const int type2_sizeof = pscript_type__sizeof(this -> type_env, type2); 
  char value1[type1_sizeof];						
  char value2[type2_sizeof];						
  const int stack_offset2 = pscript_stack__get_top__offset(this -> stack_env); 
  bcopy(this -> stack_env -> mem__array + stack_offset2, value2, type2_sizeof);	
  pscript_stack__drop(this -> stack_env);		
  const int stack_offset1 = pscript_stack__get_top__offset(this -> stack_env); 
  bcopy(this -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
  pscript_stack__drop(this -> stack_env);		
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  
  
  //assert(type1_core == PSCRIPT_TYPE__PROXY); 
  assert(type1_core == PSCRIPT_TYPE__PROXY_BYTECODE); 
  
  const int type1_subtype      = pscript_type__proxy_bytecode__get_subtype(this -> type_env, type1); 
  const int type1_subtype_core = pscript_type__get_core_type(this -> type_env, type1_subtype); 
  //pmessage("pscript_libproc__map: type1_subtype_core: [ %d ] %s " "\n", type1_subtype_core, pscript_type_core__cstr(type1_subtype_core));
  assert(type1_subtype_core == PSCRIPT_TYPE__FUNPROC); 
  //const int type1_funproc_i = 
  const int type1_retval        = pscript_type__funproc__get_retval_type(this -> type_env, type1_subtype); 
  const int type1_retval_sizeof = type1_retval >= 0 ? pscript_type__sizeof(this -> type_env, type1_retval) : 0; 
  
  //pmessage("pscript_libproc__map: type2_core: [ %d ] %s " "\n", type2_core, pscript_type_core__cstr(type2_core));
  
  const int call_address = *(int *) (value1); 
  
  
  
  int allright_huh;
  
  if (type2_core == PSCRIPT_TYPE__PROXY) { 
    const int type2_subtype = pscript_type__proxy__get_subtype(this -> type_env, type2); 
    //pmessage("pscript_libproc__map: type2_subtype: [ %d ] %s " "\n", type2_subtype, pscript_type__get_cstr(this -> type_env, type2_subtype));
    const int type2_subtype_offset = *(const int_pscript_mem_t *) value2; 
    const char * type2_subtype_pointer = pscript_mem__get_pointer(this -> mem_env, type2_subtype_offset); 
    allright_huh = pscript_libproc__map__aux(this, call_address, type2_subtype, type2_subtype_pointer, type1_retval, type1_retval_sizeof); 
  } 
  else { 
    allright_huh = pscript_libproc__map__aux(this, call_address, type2, value2, type1_retval, type1_retval_sizeof); 
  }; 

  
  return allright_huh; 
  //return this -> libproc_callback[libproc_i](this -> libproc_callback_data[libproc_i]); 
  //return this -> libproc_callback[libproc_i](this -> pscript_env); 
}; 
 
 
















static int pscript_libproc__variant_dispose(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
    dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
    dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
  assert(this -> stack_env -> stack__nb >= 1);		
  const int type_index1   = this -> stack_env -> stack__nb - 1; 
  const int type1         = this -> stack_env -> stack__type[type_index1]; 
  const int type1_sizeof  = pscript_type__sizeof(this -> type_env, type1); 
  //char value1[type1_sizeof]; 
  const int stack_offset1 = pscript_stack__get_top__offset(this -> stack_env); 
  //bcopy(this -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
  const char * value1 = this -> stack_env -> mem__array + stack_offset1; 
  //pscript_stack__drop(this -> stack_env); // RL: Something is expected to be on the stack. 
  const int type1_core    = pscript_type__get_core_type(this -> type_env, type1); 
  assert(type1_core == PSCRIPT_TYPE__PROXY);   
  const int type1_subtype      = pscript_type__proxy__get_subtype(this -> type_env, type1); 
  const int type1_subtype_core = pscript_type__get_core_type(this -> type_env, type1_subtype); 
  //pmessage("pscript_libproc__map: type1_subtype_core: [ %d ] %s " "\n", type1_subtype_core, pscript_type_core__cstr(type1_subtype_core));
  assert(type1_subtype_core == PSCRIPT_TYPE__VARIANT); 
  
  const int mem_offset             = *(const int_pscript_mem_t *)value1; 
  char *    mem_ptr_w              = pscript_mem__get_pointer_writable(this -> mem_env, mem_offset); 
  const int variant_sizeof         = pscript_type__sizeof(this -> type_env, pscript_type__get__variant(this -> type_env)); 
  const int variant_subtype        = *(const int_pscript_type_t *) (mem_ptr_w); 
  const int variant_subtype_sizeof = pscript_type__sizeof(this -> type_env, variant_subtype); 
  const int variant_mem_offset     = *(const int_pscript_mem_t *) (mem_ptr_w + sizeof(int_pscript_type_t)); 
  if (0 == variant_subtype) { return 0; }; 
  pscript_mem__dispose(this -> mem_env, variant_mem_offset, variant_subtype_sizeof); 
  bzero(mem_ptr_w, variant_sizeof); 
  return 0; 
}; 

static int pscript_libproc__typeof(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
    dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
    dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
  assert(this -> stack_env -> stack__nb >= 1);		
  const int type_index1   = this -> stack_env -> stack__nb - 1; 
  const int type1         = this -> stack_env -> stack__type[type_index1]; 
  const int type1_sizeof  = pscript_type__sizeof(this -> type_env, type1); 
  //char value1[type1_sizeof]; 
  const int stack_offset1 = pscript_stack__get_top__offset(this -> stack_env); 
  //bcopy(this -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
  const char * value1 = this -> stack_env -> mem__array + stack_offset1; 
  //pscript_stack__drop(this -> stack_env); // RL: Something is expected to be on the stack. 
  const int type1_core    = pscript_type__get_core_type(this -> type_env, type1); 
  const int integer_type_i = pscript_type__get__integer(this -> type_env); 
  if (type1_core != PSCRIPT_TYPE__PROXY) { 
    pscript_stack__drop(this -> stack_env); 
    //pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int_pscript_type_t), /*value*/(const char *)&type1); 
    pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int), /*value*/(const char *)&type1); 
    return 0; 
  }; 
  
  //assert(type1_core == PSCRIPT_TYPE__PROXY);   
  const int type1_subtype      = pscript_type__proxy__get_subtype(this -> type_env, type1); 
  pscript_stack__drop(this -> stack_env); 
  //pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int_pscript_type_t), /*value*/(const char *)&type1_subtype); 
  pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int), /*value*/(const char *)&type1_subtype); 
  return 0; 
}; 


static int pscript_libproc__sizeof(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
    dputs(stderr, "STACK TYPE: "); pscript_value__print__raw(this -> stack_env -> stack__nb * sizeof(*(this -> stack_env -> stack__type)), this -> stack_env -> stack__type); dputs(stderr, "\n"); 
    dputs(stderr, "STACK MEM: "); pscript_value__print__raw(this -> stack_env -> mem__nb * sizeof(*(this -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> stack_env)); dputs(stderr, "\n"); 
#endif 
  assert(this -> stack_env -> stack__nb >= 1);		
  const int type_index1   = this -> stack_env -> stack__nb - 1; 
  const int type1         = this -> stack_env -> stack__type[type_index1]; 
  const int type1_sizeof  = pscript_type__sizeof(this -> type_env, type1); 
  //char value1[type1_sizeof]; 
  const int stack_offset1 = pscript_stack__get_top__offset(this -> stack_env); 
  //bcopy(this -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
  const char * value1 = this -> stack_env -> mem__array + stack_offset1; 
  //pscript_stack__drop(this -> stack_env); // RL: Something is expected to be on the stack. 
  const int type1_core    = pscript_type__get_core_type(this -> type_env, type1); 
  const int integer_type_i = pscript_type__get__integer(this -> type_env); 
  if (type1_core != PSCRIPT_TYPE__PROXY) { 
    pscript_stack__drop(this -> stack_env); 
    pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int), /*value*/(const char *)&type1_sizeof); 
    return 0; 
  }; 
  
  //assert(type1_core == PSCRIPT_TYPE__PROXY);   
  const int type1_subtype        = pscript_type__proxy__get_subtype(this -> type_env, type1); 
  const int type1_subtype_sizeof = pscript_type__sizeof(this -> type_env, type1_subtype); 
  pscript_stack__drop(this -> stack_env); 
  pscript_stack__push_value(this -> stack_env, /*type_i*/integer_type_i, /*value_sizeof*/sizeof(int), /*value*/(const char *)&type1_subtype_sizeof); 
  return 0; 
}; 



static int pscript_libproc__cons(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
  dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs(stderr, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr, "\n"); 
  dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 

  assert(this -> stack_env -> stack__nb >= 2);		
  const int type_index1 = this -> stack_env -> stack__nb - 2; 
  const int type_index2 = this -> stack_env -> stack__nb - 1; 
  const int type1 = this -> stack_env -> stack__type[type_index1]; 
  const int type2 = this -> stack_env -> stack__type[type_index2]; 
  const int type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
  const int type2_sizeof = pscript_type__sizeof(this -> type_env, type2); 
#if 1 
  const char * value1 = pscript_stack__get__pointer(this -> stack_env, /*depth*/1); 
  const char * value2 = pscript_stack__get__pointer(this -> stack_env, /*depth*/0); 
#else 
  char value1[type1_sizeof];						
  char value2[type2_sizeof];						
  const int stack_offset2 = pscript_stack__get_top__offset(this -> stack_env); 
  bcopy(this -> stack_env -> mem__array + stack_offset2, value2, type2_sizeof);	
  pscript_stack__drop(this -> stack_env); 
  const int stack_offset1 = pscript_stack__get_top__offset(this -> stack_env); 
  bcopy(this -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
  pscript_stack__drop(this -> stack_env); 
#endif 
  const int type1_core = pscript_type__get_core_type(this -> type_env, type1); 
  const int type2_core = pscript_type__get_core_type(this -> type_env, type2); 
  
  int type1_actual; 
  const char * type1_actual_value; 
  if (type1_core == PSCRIPT_TYPE__PROXY) { 
    type1_actual = pscript_type__proxy__get_subtype(this -> type_env, type1); 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *)value1; 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else if (type1_core == PSCRIPT_TYPE__VARIANT) { 
    //type1_actual = pscript_type__variant__get_subtype(this -> type_env, type1); 
    type1_actual = *(const int_pscript_type_t *) value1; 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *) (value1 + sizeof(int_pscript_type_t)); 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else { 
    type1_actual = type1; 
    type1_actual_value = (const char *)value1; 
  }; 
  
  int type2_actual; 
  const char * type2_actual_value; 
  if (type2_core == PSCRIPT_TYPE__PROXY) { 
    type2_actual = pscript_type__proxy__get_subtype(this -> type_env, type2); 
    const int type2_actual_mem_offset = *(const int_pscript_mem_t *)value2; 
    type2_actual_value = pscript_mem__get_pointer(this -> mem_env, type2_actual_mem_offset); 
  } 
  else if (type2_core == PSCRIPT_TYPE__VARIANT) { 
    //type2_actual = pscript_type__variant__get_subtype(this -> type_env, type2); 
    type2_actual = *(const int_pscript_type_t *) value2; 
    const int type2_actual_mem_offset = *(const int_pscript_mem_t *) (value2 + sizeof(int_pscript_type_t)); 
    type2_actual_value = pscript_mem__get_pointer(this -> mem_env, type2_actual_mem_offset); 
  } 
  else { 
    type2_actual = type2; 
    type2_actual_value = (const char *)value2; 
  }; 
  

  const int caller_ebp = this -> vm -> call_stack__nb > 0 ? this -> vm -> call_stack__caller_ebp[this -> vm -> call_stack__nb - 1] : 0; 
  
  const int type1_actual_sizeof = pscript_type__sizeof(this -> type_env, type1_actual); 
  char *    type1_actual_scalar_value; 
  int       type1_actual_mem_offset; 
  if (pscript_type__scalar_huh(this -> type_env, type1_actual)) { 
    type1_actual_scalar_value = alloca(type1_actual_sizeof); 
    bcopy(type1_actual_value, type1_actual_scalar_value, type1_actual_sizeof); 
    type1_actual_mem_offset = -1; 
    //dputs_array(stderr__fileno, "type1_actual_value = ", int_string__stack(type1_actual_value[0]), " ", int_string__stack(type1_actual_value[1]), "\n"); 
    //dputs_array(stderr__fileno, "type1_actual_scalar_value = ", int_string__stack(type1_actual_scalar_value[0]), " ", int_string__stack(type1_actual_scalar_value[1]), "\n"); 
  } 
  else { 
    type1_actual_scalar_value = NULL; 
    if (caller_ebp > 0) { 
      type1_actual_mem_offset = pscript_mem__memory_frame__allocate_object(this -> mem_env, caller_ebp , type1_actual_sizeof); 
    } 
    else { 
      type1_actual_mem_offset = pscript_mem__toplevel_runtime__allocate(this -> mem_env, type1_actual_sizeof); 
    }; 
    pscript_mem__write(this -> mem_env, type1_actual_mem_offset, type1_actual_sizeof, type1_actual_value); 
  }; 
  
  const int type2_actual_sizeof = pscript_type__sizeof(this -> type_env, type2_actual); 
  char *    type2_actual_scalar_value; 
  int       type2_actual_mem_offset; 
  if (pscript_type__scalar_huh(this -> type_env, type2_actual)) { 
    type2_actual_scalar_value = alloca(type2_actual_sizeof); 
    bcopy(type2_actual_value, type2_actual_scalar_value, type2_actual_sizeof); 
    type2_actual_mem_offset = -1; 
  } 
  else { 
    type2_actual_scalar_value = NULL; 
    if (caller_ebp > 0) { 
      type2_actual_mem_offset = pscript_mem__memory_frame__allocate_object(this -> mem_env, caller_ebp , type2_actual_sizeof); 
    } 
    else { 
      type2_actual_mem_offset = pscript_mem__toplevel_runtime__allocate(this -> mem_env, type2_actual_sizeof); 
    }; 
    pscript_mem__write(this -> mem_env, type2_actual_mem_offset, type2_actual_sizeof, type2_actual_value); 
  }; 
  
  pscript_stack__drop(this -> stack_env); 
  pscript_stack__drop(this -> stack_env); 
  
  const int    retval_type         = pscript_type__get__list(this -> type_env); 
  const int    retval_stack_offset = pscript_stack__push_uninitialized(this -> stack_env, retval_type);  
  //const char * retval_stack_ptr    = pscript_stack__get__pointer(this -> stack_env, /*depth*/0); 
  char * retval_stack_wptr         = pscript_stack__stack_offset__get__pointer_writable(this -> stack_env, retval_stack_offset); 
  
  int cumulated_offset = 0; 
  *(int_pscript_type_t *)(retval_stack_wptr + cumulated_offset) = type1_actual; 
  cumulated_offset += sizeof(int_pscript_type_t); 
  if (pscript_type__scalar_huh(this -> type_env, type1_actual)) { 
    *(int *)(retval_stack_wptr + cumulated_offset) = *(int *)type1_actual_scalar_value; 
  }
  else { 
    *(int *)(retval_stack_wptr + cumulated_offset) = type1_actual_mem_offset; 
  }; 
  cumulated_offset += sizeof(int); 

  *(int_pscript_type_t *)(retval_stack_wptr + cumulated_offset) = type2_actual; 
  cumulated_offset += sizeof(int_pscript_type_t); 
  if (pscript_type__scalar_huh(this -> type_env, type2_actual)) { 
    *(int *)(retval_stack_wptr + cumulated_offset) = *(int *)type2_actual_scalar_value; 
  }
  else { 
    *(int *)(retval_stack_wptr + cumulated_offset) = type2_actual_mem_offset; 
  }; 
  cumulated_offset += sizeof(int); 
  
  return 0; 
}; 



static int pscript_libproc__car(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
  dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs(stderr, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr, "\n"); 
  dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 

  assert(this -> stack_env -> stack__nb >= 1); 
  const int    type1_depth  = 0; 
  const int    type1_index  = this -> stack_env -> stack__nb - 1 - type1_depth; 
  const int    type1        = this -> stack_env -> stack__type[type1_index]; 
  const int    type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
  const char * value1       = pscript_stack__get__pointer(this -> stack_env, /*depth*/type1_depth); 
  const int    type1_core   = pscript_type__get_core_type(this -> type_env, type1); 
  
  int type1_actual; 
  const char * type1_actual_value; 
  if (type1_core == PSCRIPT_TYPE__PROXY) { 
    type1_actual = pscript_type__proxy__get_subtype(this -> type_env, type1); 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *)value1; 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else if (type1_core == PSCRIPT_TYPE__VARIANT) { 
    //type1_actual = pscript_type__variant__get_subtype(this -> type_env, type1); 
    type1_actual = *(const int_pscript_type_t *) value1; 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *) (value1 + sizeof(int_pscript_type_t)); 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else { 
    type1_actual = type1; 
    type1_actual_value = (const char *)value1; 
  }; 
  
  assert(type1_actual == pscript_type__get__list(this -> type_env)); 
  
  const int car_type  = *(const int_pscript_type_t *)(type1_actual_value); 
  const int car_value = *(const int *)(type1_actual_value + sizeof(int_pscript_type_t)); 
  
  pscript_stack__drop(this -> stack_env); 

  if (pscript_type__scalar_huh(this -> type_env, car_type)) { 
    pscript_stack__push_value(this -> stack_env, car_type, pscript_type__sizeof(this -> type_env, car_type), (const char *)&car_value); 
  } 
  else { 
    const int car_type_proxy = pscript_type__make__proxy(this -> type_env, car_type); 
    pscript_stack__push_value(this -> stack_env, car_type_proxy, pscript_type__sizeof(this -> type_env, car_type_proxy), (const char *)&car_value); 
  }; 
  return 0; 
}; 



static int pscript_libproc__cdr(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
  dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs(stderr, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr, "\n"); 
  dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 
  
  assert(this -> stack_env -> stack__nb >= 1); 
  const int    type1_depth  = 0; 
  const int    type1_index  = this -> stack_env -> stack__nb - 1 - type1_depth; 
  const int    type1        = this -> stack_env -> stack__type[type1_index]; 
  const int    type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
  const char * value1       = pscript_stack__get__pointer(this -> stack_env, /*depth*/type1_depth); 
  const int    type1_core   = pscript_type__get_core_type(this -> type_env, type1); 
  
  int type1_actual; 
  const char * type1_actual_value; 
  if (type1_core == PSCRIPT_TYPE__PROXY) { 
    type1_actual = pscript_type__proxy__get_subtype(this -> type_env, type1); 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *)value1; 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else if (type1_core == PSCRIPT_TYPE__VARIANT) { 
    //type1_actual = pscript_type__variant__get_subtype(this -> type_env, type1); 
    type1_actual = *(const int_pscript_type_t *) value1; 
    const int type1_actual_mem_offset = *(const int_pscript_mem_t *) (value1 + sizeof(int_pscript_type_t)); 
    type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
  } 
  else { 
    type1_actual = type1; 
    type1_actual_value = (const char *)value1; 
  }; 
  
  assert(type1_actual == pscript_type__get__list(this -> type_env)); 
  
  const int cdr_type  = *(const int_pscript_type_t *)(type1_actual_value + sizeof(int_pscript_type_t) + sizeof(int)); 
  const int cdr_value = *(const int *)(type1_actual_value + sizeof(int_pscript_type_t) + sizeof(int) + sizeof(int_pscript_type_t)); 
  
  pscript_stack__drop(this -> stack_env); 

  if (pscript_type__scalar_huh(this -> type_env, cdr_type)) { 
    pscript_stack__push_value(this -> stack_env, cdr_type, pscript_type__sizeof(this -> type_env, cdr_type), (const char *)&cdr_value); 
  } 
  else { 
    const int cdr_type_proxy = pscript_type__make__proxy(this -> type_env, cdr_type); 
    pscript_stack__push_value(this -> stack_env, cdr_type_proxy, pscript_type__sizeof(this -> type_env, cdr_type_proxy), (const char *)&cdr_value); 
  }; 
  return 0; 
}; 



static int pscript_libproc__list(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
  dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs(stderr, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr, "\n"); 
  dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 

  assert(this -> stack_env -> stack__nb >= on_stack_arg_nb); 

  const int nil_list_type = pscript_type__get__nil_list(this -> type_env); 
  
  pscript_stack__push_value(this -> stack_env, nil_list_type, pscript_type__sizeof(this -> type_env, nil_list_type), (const char *)&nil_list_type); 
  
  for (int i = 0; i < on_stack_arg_nb; i++) { 
    pscript_libproc__cons(this, 2); 
  }; 

  return 0; 
}; 



static int pscript_libproc__array_make(pscript_t * this, const int on_stack_arg_nb) { 
#if 0 
  dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs(stderr, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr, "\n"); 
  dputs(stderr, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr, "\n"); 
  dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 

  assert(this -> stack_env -> stack__nb >= on_stack_arg_nb); 
  
  const int variant_standard_type = pscript_type__get__variant(this -> type_env); 
  //dputs_array(stderr__fileno, "variant_standard_type = ", int_string__stack(variant_standard_type), "\n"); 
  const int variant_sizeof        = pscript_type__sizeof(this -> type_env, variant_standard_type); 
  //dputs_array(stderr__fileno, "variant_sizeof = ", int_string__stack(variant_sizeof), "\n"); 
  
  const int retval_type_i      = pscript_type__make__array(this -> type_env, variant_standard_type, 0, on_stack_arg_nb - 1); 
  const int retval_type_sizeof = pscript_type__sizeof(this -> type_env, retval_type_i); 
  
  const int caller_ebp = this -> vm -> call_stack__nb > 0 ? this -> vm -> call_stack__caller_ebp[this -> vm -> call_stack__nb - 1] : 0; 
  int retval_mem_offset; 
  if (caller_ebp > 0) { 
    retval_mem_offset = pscript_mem__memory_frame__allocate_object(this -> mem_env, caller_ebp, retval_type_sizeof); 
  } 
  else { 
    retval_mem_offset = pscript_mem__toplevel_runtime__allocate(this -> mem_env, retval_type_sizeof); 
  }; 
  
  char *    retval_mem_wptr    = pscript_stack__stack_offset__get__pointer_writable(this -> stack_env, retval_mem_offset); 
  
  //int cumulated_offset = on_stack_arg_nb * variant_sizeof; 
  int cumulated_offset = retval_type_sizeof; 
  for (int i = 0; i < on_stack_arg_nb; i++) { 
    cumulated_offset -= variant_sizeof; 
    
    const int    type1_depth  = 0; 
    const int    type1_index  = this -> stack_env -> stack__nb - 1 - type1_depth; 
    const int    type1        = this -> stack_env -> stack__type[type1_index]; 
    const int    type1_sizeof = pscript_type__sizeof(this -> type_env, type1); 
    const char * value1       = pscript_stack__get__pointer(this -> stack_env, /*depth*/type1_depth); 
    const int    type1_core   = pscript_type__get_core_type(this -> type_env, type1); 
    
    int type1_actual; 
    const char * type1_actual_value; 
    int array_cell_variant_mem_offset; 
    if (type1_core == PSCRIPT_TYPE__PROXY) { 
      type1_actual = pscript_type__proxy__get_subtype(this -> type_env, type1); 
      const int type1_actual_mem_offset = *(const int_pscript_mem_t *)value1; 
      type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
      array_cell_variant_mem_offset = type1_actual_mem_offset; 
    } 
    else if (type1_core == PSCRIPT_TYPE__VARIANT) { 
      //type1_actual = pscript_type__variant__get_subtype(this -> type_env, type1); 
      type1_actual = *(const int_pscript_type_t *) value1; 
      const int type1_actual_mem_offset = *(const int_pscript_mem_t *) (value1 + sizeof(int_pscript_type_t)); 
      type1_actual_value = pscript_mem__get_pointer(this -> mem_env, type1_actual_mem_offset); 
      array_cell_variant_mem_offset = type1_actual_mem_offset; 
    } 
    else { 
      type1_actual = type1; 
      type1_actual_value = (const char *)value1; 
      const int type1_actual_sizeof = pscript_type__sizeof(this -> type_env, type1_actual); 
      array_cell_variant_mem_offset = pscript_mem__runtime__allocate_same_memory_frame(this -> mem_env, caller_ebp, type1_actual_sizeof); 
      pscript_mem__write(this -> mem_env, array_cell_variant_mem_offset, type1_actual_sizeof, type1_actual_value); 
    }; 
    
    // RL: It's a variant, so we have to move each argument into memory. 
    // RL: If the item is already a variant or a proxy, we do not need to allocate memory. 
    
    //const int array_cell_variant_mem_offset = pscript_mem__runtime__allocate_same_memory_frame(this -> mem_env, caller_ebp, type1_actual_sizeof); 
    //pscript_mem__write(this -> mem_env, array_cell_variant_mem_offset, type1_actual_sizeof, type1_actual_value); 
    
    //dputs_array(stderr__fileno, "cumulated_offset = ", int_string__stack(cumulated_offset), "\n"); 
    pscript_mem__write(this -> mem_env, retval_mem_offset + cumulated_offset                             , sizeof(int_pscript_type_t), (const char *)&type1_actual); 
    //cumulated_offset += sizeof(int_pscript_type_t); 
    pscript_mem__write(this -> mem_env, retval_mem_offset + cumulated_offset + sizeof(int_pscript_type_t), sizeof(int_pscript_mem_t ), (const char *)&array_cell_variant_mem_offset); 
    //cumulated_offset += sizeof(int_pscript_mem_t); 
    
    pscript_stack__drop(this -> stack_env); 
  }; 
  
  
  const int retval_type_proxy = pscript_type__make__proxy(this -> type_env, retval_type_i); 
  pscript_stack__push_value(this -> stack_env, retval_type_proxy, pscript_type__sizeof(this -> type_env, retval_type_proxy), (const char *)&retval_mem_offset); 
  
  return 0; 
}; 



static int pscript_libproc__halt_offset__get(pscript_libproc_env_t * this) { 
  return this -> halt_offset; 
}; 

static int pscript_libproc__halt_offset__set(pscript_libproc_env_t * this) { 
  if (this -> halt_offset >= 0) return this -> halt_offset; 
  this -> halt_offset = pscript_bytecode__push_instruction(this -> pscript_env -> bytecode_env, PVM_BYTECODE__HALT); 
  return this -> halt_offset; 
}; 






int pscript_libproc__push_standard_lib(pscript_libproc_env_t * this) { 
  pscript_libproc__halt_offset__set(this); 
  
  {
    const char * ident_cstr = "map"; 
    const int    ident_i    = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i  = pscript_libproc__push(this, ident_i, 2, 2, pscript_libproc__map); 
    const int    env_i      = pscript_env__push(this -> pscript_env -> env, ident_i, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };  
  
  {
    const char * ident_cstr     = "variant_dispose"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/1, /*arg_atleast*/-1, pscript_libproc__variant_dispose); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
    this -> arg_byref[libproc_i][0] = true;
  };
  
  {
    const char * ident_cstr     = "typeof"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/1, /*arg_atleast*/-1, pscript_libproc__typeof); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "sizeof"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/1, /*arg_atleast*/-1, pscript_libproc__sizeof); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "cons"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/2, /*arg_atleast*/-1, pscript_libproc__cons); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "car"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/1, /*arg_atleast*/-1, pscript_libproc__car); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "cdr"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/1, /*arg_atleast*/-1, pscript_libproc__cdr); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "list"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/-1, /*arg_atleast*/-1, pscript_libproc__list); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  {
    const char * ident_cstr     = "array_make"; 
    const int    ident_pstr     = pscript_string__push(this -> pscript_env -> string_env, ident_cstr); 
    const int    libproc_i      = pscript_libproc__push(this, ident_pstr, /*arg_nb*/-1, /*arg_atleast*/-1, pscript_libproc__array_make); 
    const int    toplevel_env_i = pscript_env__push(this -> pscript_env -> env, ident_pstr, PSCRIPT_ENV__LIBPROC, libproc_i); 
  };
  
  return 0; 
}; 

















#if 0 



//int pscript_libproc__env__name__push_nolookup(pscript_libproc_env_t * this, const int name, const int type_i) {
int pscript_libproc__push_full(pscript_libproc_env_t * this, const int name, const int retval_type, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int localvar_nb, const int localvar_name[], const int localvar_type[], const int lexical_env, const int src_code, const int bytecode_offset, const int bytecode_size) {
  assert(this -> libproc_nb < pscript_libproc__size); 
  const int index = this -> libproc_nb; 
  
  this -> retval_type[index] = retval_type; 

  assert(arg_nb < pscript_libproc__arg__size); 
  this -> arg_nb[index] = arg_nb; 
  for (int i = 0; i < arg_nb; i++) { 
    this -> arg_name[index][i] = arg_name[i]; 
    this -> arg_byref[index][i] = arg_byref[i]; 
    this -> arg_type[index][i] = arg_type[i]; 
  }; 

  assert(localvar_nb < pscript_libproc__localvar__size); 
  this -> localvar_nb[index] = localvar_nb; 
  for (int i = 0; i < localvar_nb; i++) { 
    this -> localvar_name[index][i] = localvar_name[i]; 
    this -> localvar_type[index][i] = localvar_type[i]; 
  }; 

  this -> libproc_name[index] = name; 
  this -> libproc_lexical_env[index] = lexical_env; 
  this -> libproc_src_code[index] = src_code; 
  this -> libproc_bytecode_offset[index] = bytecode_offset; 
  this -> libproc_bytecode_size[index] = bytecode_size; 

  this -> libproc_nb ++; 
  return index; 
}; 

int pscript_libproc__lookup(const pscript_libproc_env_t * this, const int name) {
  for (int i = 0; i < this -> libproc_nb; i++) {
    if (this -> libproc_name[i] == name) return i; 
  }; 
  return -1; 
}; 




int pscript_libproc__push_empty(pscript_libproc_env_t * this) {
  assert(this -> libproc_nb < pscript_libproc__size); 
  const int index = this -> libproc_nb; 
  if (0 != this -> libproc_nb) { 
    if (-1 == this -> retval_type[this -> libproc_nb-1]) {
      return this -> libproc_nb - 1; 
    };
  };
  
  pmessage("Pushing a new libproc (empty) - libproc_i: %d" "\n", index); 
  
  this -> retval_type[index] = -1; 
  this -> arg_nb[index] = 0; 
  this -> localvar_nb[index] = 0; 
  this -> libproc_name[index] = -1; 
  this -> libproc_lexical_env[index] = -1; 
  this -> libproc_src_code[index] = -1; 
  this -> libproc_bytecode_offset[index] = -1; 
  this -> libproc_bytecode_size[index] = -1; 

  this -> libproc_nb ++; 
  return index; 
}; 









int pscript_libproc__top(pscript_libproc_env_t * this) { 
  assert(this -> libproc_nb > 0); 
  return this -> libproc_nb - 1; 
}; 
 
int pscript_libproc__set_name(pscript_libproc_env_t * this, const int libproc_i, const int name_pstr) { 
  this -> libproc_name[libproc_i] = name_pstr; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_lexical_env(pscript_libproc_env_t * this, const int libproc_i, const int lexical_env) { 
  this -> libproc_lexical_env[libproc_i] = lexical_env; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_retval_type(pscript_libproc_env_t * this, const int libproc_i, const int retval_type_i) { 
  this -> retval_type[libproc_i] = retval_type_i; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_src_code(pscript_libproc_env_t * this, const int libproc_i, const int src_code) { 
  this -> libproc_src_code[libproc_i] = src_code; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_bytecode_offset(pscript_libproc_env_t * this, const int libproc_i, const int bytecode_offset) { 
  this -> libproc_bytecode_offset[libproc_i] = bytecode_offset; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_bytecode_size(pscript_libproc_env_t * this, const int libproc_i, const int bytecode_size) { 
  this -> libproc_bytecode_size[libproc_i] = bytecode_size; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_arg_push(pscript_libproc_env_t * this, const int libproc_i, const int arg_name, const int byref_huh, const int arg_type) { 
  assert(this -> arg_nb[libproc_i] < pscript_libproc__arg__size); 
  const int arg_i = this -> arg_nb[libproc_i]; 
  
  this -> arg_name[libproc_i][arg_i] = arg_name; 
  this -> arg_byref[libproc_i][arg_i] = byref_huh; 
  this -> arg_type[libproc_i][arg_i] = arg_type; 
  
  this -> arg_nb[libproc_i]++; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_localvar_push(pscript_libproc_env_t * this, const int libproc_i, const int localvar_name, const int localvar_type) { 
  assert(this -> localvar_nb[libproc_i] < pscript_libproc__localvar__size); 
  const int localvar_i = this -> localvar_nb[libproc_i]; 
  
  this -> localvar_name[libproc_i][localvar_i] = localvar_name; 
  this -> localvar_type[libproc_i][localvar_i] = localvar_type; 
  
  this -> localvar_nb[libproc_i]++; 
  return libproc_i; 
}; 
 
int pscript_libproc__set_localvar_type_nb(pscript_libproc_env_t * this, const int libproc_i, const int ident_nb, const int type_i) { 
  assert(this -> localvar_nb[libproc_i] > ident_nb); 
  for (int i = 0; i < ident_nb; i++) { 
    const int localvar_i = this -> localvar_nb[libproc_i] - ident_nb + i; 
    this -> localvar_type[libproc_i][i] = type_i; 
  }; 
  return libproc_i; 
}; 
 




int pscript_libproc__get_retval_type(const pscript_libproc_env_t * this, const int libproc_i) { 
  return this -> retval_type[libproc_i]; 
}; 
 
int pscript_libproc__get_lexical_env(const pscript_libproc_env_t * this, const int libproc_i) { 
  return this -> libproc_lexical_env[libproc_i]; 
}; 
 


int pscript_libproc__get_arg_all_sizeof(const pscript_libproc_env_t * this, const int libproc_i) { 
  int arg_all_sizeof = 0; 
  for (int i = 0; i < this -> arg_nb[libproc_i]; i++) { 
    arg_all_sizeof += pscript_type__sizeof(this -> type_env, this -> arg_type[libproc_i][i]); 
  }; 
  return arg_all_sizeof; 
}; 
 
int pscript_libproc__get_localvar_all_sizeof(const pscript_libproc_env_t * this, const int libproc_i) { 
  int localvar_all_sizeof = 0; 
  for (int i = 0; i < this -> localvar_nb[libproc_i]; i++) { 
    localvar_all_sizeof += pscript_type__sizeof(this -> type_env, this -> localvar_type[libproc_i][i]); 
  }; 
  return localvar_all_sizeof; 
}; 

int pscript_libproc__get_bytecode_offset(const pscript_libproc_env_t * this, const int libproc_i) { 
  return this -> libproc_bytecode_offset[libproc_i]; 
}; 
 
int pscript_libproc__get_arg_nb(const pscript_libproc_env_t * this, const int libproc_i) { 
  return this -> arg_nb[libproc_i];
}; 
 
int pscript_libproc__get_arg_name(const pscript_libproc_env_t * this, const int libproc_i, const int arg_i) { 
  return this -> arg_name[libproc_i][arg_i];
}; 
 
int pscript_libproc__get_arg_byref(const pscript_libproc_env_t * this, const int libproc_i, const int arg_i) { 
  return this -> arg_byref[libproc_i][arg_i];
}; 
 
int pscript_libproc__get_arg_sizeof(const pscript_libproc_env_t * this, const int libproc_i, const int arg_i) { 
  return pscript_type__sizeof(this -> type_env, this -> arg_type[libproc_i][arg_i]);
}; 
 
int pscript_libproc__get_localvar_nb(const pscript_libproc_env_t * this, const int libproc_i) { 
  return this -> localvar_nb[libproc_i];
}; 
 
int pscript_libproc__get_localvar_name(const pscript_libproc_env_t * this, const int libproc_i, const int localvar_i) { 
  return this -> localvar_name[libproc_i][localvar_i];
}; 
 
int pscript_libproc__get_localvar_sizeof(const pscript_libproc_env_t * this, const int libproc_i, const int localvar_i) { 
  return pscript_type__sizeof(this -> type_env, this -> localvar_type[libproc_i][localvar_i]);
}; 
 


#if 1 

#if 0 
  int libproc_name[pscript_libproc__size]; 
  int libproc_lexical_env[pscript_libproc__size]; 
  int libproc_src_code[pscript_libproc__size]; 
  int libproc_bytecode_offset[pscript_libproc__size]; 
  int libproc_bytecode_size[pscript_libproc__size]; 
  int libproc_nb; 
#endif 

static void libproc_stack__print_one_libproc(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env, const int libproc_i) {
  const int filedes = fileno(yypascalout); 
  
  const char * libproc_i__str = int_string__stack(libproc_i); 
  const int libproc_i__str__spaces_nb = MAX(0, 4 - strlen(libproc_i__str)); 
  dputs(filedes, "IDX "); 
  dput_spaces(filedes, libproc_i__str__spaces_nb); 
  dputs(filedes, libproc_i__str); 
  dputs(filedes, ": "); 
  
  const int libproc_name_pstr = this -> libproc_name[libproc_i]; 
  const char * libproc_name_cstr = pscript_string__get_cstr(string_env, libproc_name_pstr); 
  const int libproc_name_cstr__nb_spaces = MAX(0, 17 - strlen(libproc_name_cstr)); 
  dputs(filedes, libproc_name_cstr); 
  dput_spaces(filedes, libproc_name_cstr__nb_spaces); 
  dputs(filedes, ": "); 

  const char * libproc_lenv__str = int_string__stack(this -> libproc_lexical_env[libproc_i]); 
  const int libproc_lenv__str__spaces_nb = MAX(0, 4 - strlen(libproc_lenv__str)); 
  dputs(filedes, "lexical env: "); 
  dput_spaces(filedes, libproc_lenv__str__spaces_nb); 
  dputs(filedes, libproc_lenv__str); 
  dputs(filedes, ": "); 
  
  const char * libproc_boffset__str = int_string__stack(this -> libproc_bytecode_offset[libproc_i]); 
  const int libproc_boffset__str__spaces_nb = MAX(0, 4 - strlen(libproc_boffset__str)); 
  dputs(filedes, "Bytecode offset: "); 
  dput_spaces(filedes, libproc_boffset__str__spaces_nb); 
  dputs(filedes, libproc_boffset__str); 
  dputs(filedes, ": "); 
  
  const char * libproc_bsize__str = int_string__stack(this -> libproc_bytecode_size[libproc_i]); 
  const int libproc_bsize__str__spaces_nb = MAX(0, 4 - strlen(libproc_bsize__str)); 
  dputs(filedes, "Bytecode size: "); 
  dput_spaces(filedes, libproc_bsize__str__spaces_nb); 
  dputs(filedes, libproc_bsize__str); 
  dputs(filedes, ": "); 
  

#if 0 
  const int srcval_len = pscript_string__get_len(this -> string_env, libproc_stack__get_srcval(this, libproc_i)); 
  const char * libproc_len_str = int_string__stack(srcval_len); 
  const int libproc_len_str_len = strlen(libproc_len_str); 
  const int libproc_len_str_len_spaces_nb = MAX(0, 6 - libproc_len_str_len); 
  dputs(filedes, libproc_type_str); 
  dput_spaces(filedes, nb_spaces); 
  dputs(filedes, " [len =  "); 
  dput_spaces(filedes, libproc_len_str_len_spaces_nb); 
  dputs(filedes, libproc_len_str); 
  dputs(filedes, " ] : "); 
  dputs(filedes, libproc_stack__get_srcval_cstr(this, libproc_i)); 
#if 0
  const char * str = cstring__concat__zarray__stack(((const char *[]) {
     "" , ": ", 
	libproc_stack__get_srcval_cstr(this, libproc_i), 
	NULL})); 
  dputs(filedes, str); 
#endif 
#endif 
}; 

static void libproc_stack__print_stats__base(const pscript_libproc_env_t * this, const int base) {
  const int filedes = fileno(yypascalout); 
  dputs(filedes, "LIBPROC_STACK("); 
  dputn(filedes, this -> libproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, base); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> libproc_nb)) / ((double) base))); 
  dputs(filedes, "% ) \n"); 
}; 

static void libproc_stack__print_stats(const pscript_libproc_env_t * this) {
  libproc_stack__print_stats__base(this, pscript_libproc__size); 
}; 

void libproc_stack__print_nb(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env, const int libproc_nb) {
  const int filedes = fileno(yypascalout); 
  dputs(filedes, "LIBPROC_STACK("); 
  dputn(filedes, this -> libproc_nb); 
  dputs(filedes, " / "); 
  dputn(filedes, pscript_libproc__size); 
  dputs(filedes, " = "); 
  dputn(filedes, (int) (((double) (100 * this -> libproc_nb)) / ((double) pscript_libproc__size))); 
  dputs(filedes, "% ) = { \n"); 
  const int nb = MIN(this -> libproc_nb,libproc_nb); 
  for (int i = 0; i < nb; i++) {
    dputs(filedes, "  "); 
    libproc_stack__print_one_libproc(this, string_env, i);
    dputs(filedes, "\n"); 
  }; 
  dputs(filedes, "} \n"); 
}; 
 
void libproc_stack__print_all(const pscript_libproc_env_t * this, const pscript_string_env_t * string_env) {
  libproc_stack__print_nb(this, string_env, this -> libproc_nb); 
}; 
#endif 
 
 
#endif 





