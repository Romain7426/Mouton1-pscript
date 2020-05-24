#include "pscript_global.h"
#include "pscript.h"
#include "pscript_vm.h"
#include "pscript_stack.h"
#include "pscript_value.h"
#include "pscript_type.h"
#include "pvm_bytecode__type.ci" 


pscript_vm_t * pscript_vm__make(pscript_t * pscript_env) {
  MALLOC_BZERO(pscript_vm_t,this); 
  
  ASSIGN_METHOD(pscript_vm,this,make); 
  ASSIGN_METHOD(pscript_vm,this,delete); 
  ASSIGN_METHOD(pscript_vm,this,run); 
  ASSIGN_METHOD(pscript_vm,this,step); 
  ASSIGN_METHOD(pscript_vm,this,print_error); 
  
  this -> pscript_env = pscript_env; 
  
  return this;
}; 

void pscript_vm__delete(pscript_vm_t * this) {
  free(this);
};   



#if 0 
//extern int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char arg_nb, const int mem_base_offset); 
static int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char arg_nb); 

static int pscript_vm__memory_frame__push(pscript_vm_t * this, const int ret_address, const int ebp); 
#endif 

//static int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char on_stack_arg_nb, const int mem_base_offset) { 
//static int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char on_stack_arg_nb) { 
//static int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char on_stack_arg_nb, const int caller_ebp) { 
int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char on_stack_arg_nb, const int caller_ebp) { 
  assert(this -> call_stack__nb < pscript_vm__call_stack__size); 
  const int index = this -> call_stack__nb; 

  this -> call_stack__ret   [index] = ret_address; 
  this -> call_stack__arg_nb[index] = on_stack_arg_nb; 
  //this -> call_stack__mem_base_offset[index] = mem_base_offset; 
  this -> call_stack__caller_ebp[index] = caller_ebp; 
  
  this -> call_stack__nb++; 
  return index; 
}; 

void pscript_vm__call_stack__drop(pscript_vm_t * this) { 
  assert(this -> call_stack__nb > 0); 
  this -> call_stack__nb--; 
}; 
 
#if 0 
// RL: This is idiotic garbage. 
static void pscript_vm__call_stack__caller_ebp__allocate_retval(pscript_vm_t * this, const int retval_sizeof) { 
  assert(this -> call_stack__nb  > 0); 
  const int index = this -> call_stack__nb - 1; 
  this -> call_stack__caller_ebp[index] += retval_sizeof; 
}; 
#endif 


#if 0 
static int pscript_vm__call_stack__get_mem_base_offset(pscript_vm_t * this) {
  assert(this -> call_stack__nb > 0); 
  const int index = this -> call_stack__nb - 1; 
  return this -> call_stack__mem_base_offset[index]; 
}; 
#endif 

static int pscript_vm__memory_frame__push_and_set(pscript_vm_t * this, const int ebp) { 
  assert(this -> memory_frame__nb < pscript_vm__memory_frame__size); 
  const int index = this -> memory_frame__nb; 
  this -> memory_frame__ebp[index] = ebp; 
  this -> memory_frame__nb++; 
  this -> memory_frame__active = ebp; 
  return index; 
}; 
 
//static int pscript_vm__memory_frame__get(const pscript_vm_t * this) { 
int pscript_vm__memory_frame__get(const pscript_vm_t * this) { 
#if 0 
  assert(this -> memory_frame__active > 0); 
  const int index = this -> memory_frame__active; 
  return this -> memory_frame__ebp[index]; 
#else 
  return this -> memory_frame__active; 
#endif 
}; 

static void pscript_vm__memory_frame__set(pscript_vm_t * this, const int ebp) { 
  this -> memory_frame__active = ebp; 
}; 

static int pscript_vm__memory_frame__is_active_on_top_huh(const pscript_vm_t * this) { 
  assert(this -> memory_frame__nb > 0); 
  const int top_index = this -> memory_frame__nb - 1; 
  const int top_ebp = this -> memory_frame__ebp[top_index]; 
  return (top_ebp == this -> memory_frame__active); 
}; 

static int pscript_vm__memory_frame__is_caller_on_top_huh(const pscript_vm_t * this) { 
  assert(this -> memory_frame__nb > 1); 
  assert(this -> call_stack__nb > 0); 
  const int caller_ebp = this -> call_stack__caller_ebp[this -> call_stack__nb - 1]; 
  //const int top_index = this -> memory_frame__nb - 1; 
  //const int top_ebp = this -> memory_frame__ebp[top_index]; 
  const int subtop_index = this -> memory_frame__nb - 2; 
  const int subtop_ebp = this -> memory_frame__ebp[subtop_index]; 
  return (subtop_ebp <= caller_ebp); 
}; 

static int pscript_vm__memory_frame__top(pscript_vm_t * this) { 
  assert(this -> memory_frame__nb > 0); 
  const int index = this -> memory_frame__nb; 
  return this -> memory_frame__ebp[index]; 
}; 

static void pscript_vm__memory_frame__drop(pscript_vm_t * this) { 
  assert(this -> memory_frame__nb > 0); 
  this -> memory_frame__nb --; 
}; 


// -------------------------------------------------------------------------------

// RL: Returns the number of objects stacked, or an error code. 
static int pscript_vm__step__call_ffi(pscript_vm_t * this, const int ffiproc_i, const int on_stack_arg_nb, const int ret_address) { 
  int next_ipc0 = 0; // RL: Number of objects stacked. 
  int retval_sizeof; 
  do { 
      //const int base_mem_offset = pscript_mem__get_base_offset(this -> pscript_env -> mem_env); 
      //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, base_mem_offset); 
      const int caller_ebp = pscript_vm__memory_frame__get(this); 
      pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
      
      const int atleast_arg_nb = pscript_ffiproc__get_atleast_arg_nb(this -> pscript_env -> ffiproc_env, ffiproc_i); 
      if (atleast_arg_nb > 0) { 
	if (on_stack_arg_nb < atleast_arg_nb) { 
#if 0 
	  this -> error_ipc0 = ipc0; 
	  this -> error_bytecode = bytecode; 
#endif 
	  this -> error_ffi_arg_nb = on_stack_arg_nb; 
	  this -> error_ffi_atleast_arg_nb = atleast_arg_nb; 
	  next_ipc0 = PSCRIPT_VM__ERROR__FFI_NOT_ENOUGH_ARG; 
	  break; 
	}; 
      }; 
      
      
      // FFI callback 
      //const char * arg_type = this -> pscript_env -> stack_env -> stack__type - arg_nb; // reverse 
      char arg_type  [on_stack_arg_nb]; 
      int  arg_sizeof[on_stack_arg_nb]; 
      int  arg_offset[on_stack_arg_nb]; 
      for (int i = 0; i < on_stack_arg_nb; i++) {
	arg_type[i] = this -> pscript_env -> stack_env -> stack__type[this -> pscript_env -> stack_env -> stack__nb - on_stack_arg_nb + i];
      }; 
      int  cumulated_sizeof = 0; 
      for (int i = 0; i < on_stack_arg_nb; i++) {
	arg_sizeof[i]     = pscript_type__sizeof(this -> pscript_env -> type_env, arg_type[i]); 
	cumulated_sizeof += arg_sizeof[i]; 
      }; 
      int cumulated_offset = 0; 
      for (int i = 0; i < on_stack_arg_nb; i++) {
	cumulated_offset += arg_sizeof[i]; 
	arg_offset[i]     = cumulated_sizeof - cumulated_offset; 
      }; 
      const char * arg_mem = pscript_stack__get_top__pointer(this -> pscript_env -> stack_env);   // this -> pscript_env -> stack_env -> mem__array +  - cumulated_offset; 
      // RL: The arguments are left on the stack during the FFI call. 
      //     Therefore we can't allocate retval on the stack right away. 
      // RL: We might change that as it is not intuitive for external FFI that there are things on the stack (even though that should be transparent). 
      const int retval_type            = pscript_ffiproc__get_retval_type(this -> pscript_env -> ffiproc_env, ffiproc_i); 
      const int retval_type_scalar_huh = pscript_type__scalar_huh(this -> pscript_env -> type_env, retval_type); //int_member_array(pscript_type__get_core_type(this -> type_env, retval_type), scalar_types); 
      //const int retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      char * retval_value_ptr = NULL; 
#if 1 
      if (0 == retval_sizeof) { 
	// RL: Do not do anything. 
      } 
      else { 
	if (retval_type_scalar_huh) { 
	  const int stack_offset = pscript_stack__push_uninitialized(this -> pscript_env -> stack_env, retval_type);  
	  retval_value_ptr = pscript_stack__stack_offset__get__pointer_writable(this -> pscript_env -> stack_env, stack_offset); 
	} 
	else { 
	  const int caller_on_top_huh = pscript_vm__memory_frame__is_caller_on_top_huh(this); 
	  int mem_offset; 
	  if (caller_on_top_huh) { 
	    mem_offset = pscript_mem__memory_frame__allocate_object(this -> pscript_env -> mem_env, caller_ebp, retval_sizeof); 
	  } 
	  else { 
	    mem_offset = pscript_mem__stack_heap__allocate(this -> pscript_env -> mem_env, caller_ebp, retval_sizeof); 
	  }; 
	  retval_value_ptr = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, mem_offset); 
	}; 
      }; 
#else 
      const int ref_ebp = retval_type_scalar_huh ? base_mem_offset : caller_ebp; 
      if (0 == retval_sizeof) { 
	// RL: Do not do anything. 
      } 
      else { 
	const int caller_on_top_huh = pscript_vm__memory_frame__is_caller_on_top_huh(this); 
	int mem_offset; 
	if (caller_on_top_huh) { 
	  mem_offset = pscript_mem__allocate(this -> pscript_env -> mem_env, retval_sizeof); 
	} 
	else { 
	  mem_offset = pscript_mem__stack_heap__allocate(this -> pscript_env -> mem_env, ref_ebp, retval_sizeof); 
	}; 
	retval_value = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, mem_offset); 
      }; 
#endif 
      
      const int allright = pscript_ffiproc__call(this -> pscript_env -> ffiproc_env, ffiproc_i, on_stack_arg_nb, arg_type, arg_sizeof, arg_offset, cumulated_offset, arg_mem, retval_type, retval_sizeof, retval_value_ptr); 
      
      if (retval_sizeof > 0 && retval_type_scalar_huh) { 
	char retval[retval_sizeof]; 
	pscript_stack__get_top__value(this -> pscript_env -> stack_env, /*value_sizeof_ref*/NULL, retval_sizeof, retval); 
	pscript_stack__drop(this -> pscript_env -> stack_env); 
	for (int i = 0; i < on_stack_arg_nb; i++) { 
	  pscript_stack__drop(this -> pscript_env -> stack_env); 
	}; 
	pscript_stack__push_value(this -> pscript_env -> stack_env, retval_type, retval_sizeof, retval); 
      } 
      else { 
	for (int i = 0; i < on_stack_arg_nb; i++) { 
	  pscript_stack__drop(this -> pscript_env -> stack_env); 
	}; 
	if (retval_sizeof > 0) { 
	  //dputs_array(stderr__fileno, "CALL_FFI - retval_sizeof = ", int_string__stack(retval_sizeof), "\n"); 
	  const int proxy_type   = pscript_type__make__proxy(this -> pscript_env -> type_env, retval_type); 
	  const int proxy_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, proxy_type); 
	  pscript_stack__push_value(this -> pscript_env -> stack_env, proxy_type, proxy_sizeof, retval_value_ptr); 
	}; 
      }; 
      
      pscript_vm__call_stack__drop(this); 
  } while (false); 
  
  
  if (next_ipc0 < 0) return next_ipc0; 
  if (retval_sizeof == 0) return 0; 
  return 1;  
}; 



static int pscript_vm__step__call_lambda(pscript_vm_t * this, const int type_i, const char * value, const int on_stack_arg_nb, const int ret_address) { 
  const int    type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type_i); 
  const char * lambda_object_ptr    = NULL; 
  if (PSCRIPT_TYPE__LAMBDA == type_core) { 
    lambda_object_ptr = value; 
  } 
  else { 
    int subtype_i = -1; 
    if (PSCRIPT_TYPE__POINTER == type_core) { 
      subtype_i = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type_i); 
    }
    else if (PSCRIPT_TYPE__PROXY == type_core) { 
      subtype_i = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type_i); 
    } 
    else { 
      assert(false); 
    }; 
    const int subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
    assert(PSCRIPT_TYPE__LAMBDA == subtype_core); 
    const int mem_offset = *(const int_pscript_mem_t *)(value); 
    lambda_object_ptr = pscript_mem__get_pointer(this -> pscript_env -> mem_env, mem_offset); 
  }; 
  
  const int lambda_bytecode_offset = *(const int *) (lambda_object_ptr); 
  const int caller_ebp = pscript_vm__memory_frame__get(this); 
  
  pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
  
  //next_ipc0 = lambda_bytecode_offset; 
  return lambda_bytecode_offset; 
}; 




// RL: TODO XXX FIXME: This is garbage. This should not exist. 
static void pscript_vm__memory_frame__recursive_refcounter_inc(pscript_mem_env_t * this, const int frame_ebp) { 
  const char *  mem_pointer              = pscript_mem__get_pointer(this, frame_ebp); 
  const int     ref_counter              = *(const int *) (mem_pointer - sizeof(int)); 
  if (0 >= ref_counter) { return; }; 
  const int     frame0_ref_counter_offset = pscript_mem__memory_frame__get_frame0_ref_counter_offset(this); 
  //char *        mem_pointer_w            = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, frame_ebp); 
	  
  //pscript_mem__memory_frame__print(stderr__fileno, this); 
  int         lex_env_nb = 0; 
  const int * lex_env__ptr = (const int *) (mem_pointer - sizeof(int) - sizeof(int)); 
  for (;;) { lex_env__ptr--; lex_env_nb++; if (*lex_env__ptr == frame0_ref_counter_offset) break; }; 
	  
  for (int i = 0; i < lex_env_nb-1; i++) { 
    const int refcounter_offset = lex_env__ptr[i]; 
    const int subframe_ebp      = refcounter_offset + sizeof(int); 
    pscript_mem__memory_frame__references_counter_inc(this, subframe_ebp); 
  }; 
}; 
 

static void pscript_vm__memory_frame__recursive_refcounter_dec(pscript_mem_env_t * this, const int frame_ebp) { 
  //dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "frame_ebp = ", int_string__stack(frame_ebp), "\n");
  const char *  mem_pointer              = pscript_mem__get_pointer(this, frame_ebp); 
  const int     ref_counter              = *(const int *) (mem_pointer - sizeof(int)); 
  if (0 < ref_counter) { return; }; 
  const int     frame0_ref_counter_offset = pscript_mem__memory_frame__get_frame0_ref_counter_offset(this); 
  //char *        mem_pointer_w            = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, frame_ebp); 
  
  //pscript_mem__memory_frame__print(stderr__fileno, this); 

  assert(frame_ebp != frame0_ref_counter_offset + sizeof(int)); 

  int         lex_env_nb = 0; 
  const int * lex_env__ptr = (const int *) (mem_pointer - sizeof(int) - sizeof(int)); 
  for (;;) { lex_env__ptr--; lex_env_nb++; if (*lex_env__ptr == frame0_ref_counter_offset) break; }; 
	  
  for (int i = 0; i < lex_env_nb-1; i++) { 
    const int refcounter_offset = lex_env__ptr[i]; 
    const int subframe_ebp      = refcounter_offset + sizeof(int); 
    pscript_mem__memory_frame__references_counter_dec(this, subframe_ebp); 
  }; 
  
  for (int i = 0; i < lex_env_nb-1; i++) { 
    const int index             = lex_env_nb-1 - i - 1; 
    const int refcounter_offset = lex_env__ptr[index]; 
    const int subframe_ebp      = refcounter_offset + sizeof(int); 
    pscript_vm__memory_frame__recursive_refcounter_dec(this, subframe_ebp); 
  };   
}; 
 




#define BYTECODE_READ_ARG(type) *(type *) (bytecode_env -> bytecode_array + (next_ipc0 += sizeof(type)) - sizeof(type)) 

int pscript_vm__step(pscript_vm_t * this, const int ipc0) {
#if 0 
  dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
#elif 0 
  dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
  dputs_array(stderr__fileno, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
  dputs_array(stderr__fileno, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#elif 0 
  pmessage("ipc0 = %d" "\n", ipc0); 
  // 2143, 2157 
#endif
#if 0 
  const int out_stream = stderr; 
  dputs(out_stream, "vm_run__step: "); 
  dputs(out_stream, "ipc0 = "); 
  dputn(out_stream, ipc0); 
  dputs(out_stream, " ; stack_nb = "); 
  dputn(out_stream, vm_stack_nb); 
  dputs(out_stream, " ; bytecode raw = "); 
  dputn(out_stream, vm_execflow[ipc0]); 
  dputs(out_stream, " ; bytecode = "); 
  {  const char * bytecode_str = (ipc0 < vm_execflow_nb) ? bytecode_type__get_cstr(vm_execflow[ipc0]) : "ERROR"; 
    dputs(out_stream, bytecode_str); 
    dput_spaces(out_stream, 24 - strlen(bytecode_str)); }; 
  dputs(out_stream, " - "); 
  //dputc(out_stream, '\n'); 
  vm_stack__print(stderr); 
#endif 
  pscript_bytecode_env_t * bytecode_env = this -> pscript_env -> bytecode_env; 
  
  if (ipc0 >= bytecode_env -> bytecode_nb) { return PSCRIPT_VM__ERROR__BYTEADDRESS_OUT_OF_RANGE; }; 
  const int bytecode = bytecode_env -> bytecode_array[ipc0]; 

#if 0 
      dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 
  
  int next_ipc0 = ipc0 + 1; 
  
  switch (bytecode) {
  default: next_ipc0 = PSCRIPT_VM__ERROR__UNKNOWN_BYTECODE; this -> error_ipc0 = ipc0; this -> error_bytecode = bytecode; break; 

  case PVM_BYTECODE__NULL:  
    this -> error_ipc0 = ipc0; 
    this -> error_bytecode = bytecode; 
    next_ipc0 = PSCRIPT_VM__ERROR__NULL_INSTRUCTION; break;   
  case PVM_BYTECODE__ABORT: 
    this -> error_ipc0 = ipc0; 
    this -> error_bytecode = bytecode; 
    next_ipc0 = PSCRIPT_VM__ERROR__ABORT; break;   
  case PVM_BYTECODE__HALT:  next_ipc0 = PSCRIPT_VM__ERROR__HALT; break;   
  case PVM_BYTECODE__NOP: break;  

  case PVM_BYTECODE__PUSH: { 
    //const int8_t value_sizeof = bytecode_env -> bytecode_array[ipc0 + 1]; 
    const int8_t type_i = bytecode_env -> bytecode_array[ipc0 + 1]; 
    const int8_t value_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
    next_ipc0 = ipc0 + 2 + value_sizeof; 
    pscript_stack__push_value(this -> pscript_env -> stack_env, type_i, value_sizeof, bytecode_env -> bytecode_array + ipc0 + 2);  
   }; break; 

  case PVM_BYTECODE__CALL: do { 
#if 0 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
      dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
    const int8_t on_stack_arg_nb = BYTECODE_READ_ARG(int8_t); //bytecode_env -> bytecode_array[ipc0 + 1]; 
    const int call_address       = BYTECODE_READ_ARG(int); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 2); 
    const int ret_address        = next_ipc0; //ipc0 + 6; 
    //const int base_mem_offset = pscript_mem__get_base_offset(this -> pscript_env -> mem_env); 
    const int caller_ebp         = pscript_vm__memory_frame__get(this); 
    //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, base_mem_offset); 
    pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
    next_ipc0 = call_address; 
  } while (false); break; 

  case PVM_BYTECODE__MEM_ALLOCATE: { // size is in the bytecode 
    // RL: TODO XXX FIXME: The current local memory may not belong to the caller, therefore the allocation should then be done on the stack-heap. 
    assert(false); 
    const int size_to_be_allocated = *(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    //const int mem_offset = pscript_mem__allocate(this -> pscript_env -> mem_env, size_to_be_allocated);  
    next_ipc0 = ipc0 + 1 + 4; 
  }; break; 
    
  case PVM_BYTECODE__CHECK_ARG_NB: {  // ARG_NB is in the bytecode 
    //pmessage("PVM_BYTECODE__CHECK_ARG_NB TODO XXX: to be implemented." "\n"); 
    const int8_t expected_arg_nb   = BYTECODE_READ_ARG(int8_t); //bytecode_env -> bytecode_array[ipc0 + 1]; 
    const int    call_stack__index = this -> call_stack__nb - 1; 
    const int    on_stack_arg_nb   = this -> call_stack__arg_nb[call_stack__index]; 
    if (on_stack_arg_nb != expected_arg_nb) { 
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error__expected_arg_nb = expected_arg_nb; 
      this -> error__on_stack_arg_nb = on_stack_arg_nb; 
      next_ipc0 = PSCRIPT_VM__ERROR__ARG_NB; 
      break; 
    }; 
    //next_ipc0 = ipc0 + 2; 
  }; break; 
    
  case PVM_BYTECODE__MEM_STORE_LOCAL: do { // type then @ are in the bytecode, value on the stack  
#if 0 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
      dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      const int type1               = BYTECODE_READ_ARG(int8_t); //*(int8_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int type1_sizeof        = pscript_type__sizeof(this -> pscript_env -> type_env, type1); 
      const int value1_local_offset = BYTECODE_READ_ARG(int_pscript_mem_t); //*(int_pscript_mem_t *) (bytecode_env -> bytecode_array + ipc0 + 2); 
      //const int base_offset  = pscript_vm__call_stack__get_mem_base_offset(this); 
      const int base_offset       = pscript_vm__memory_frame__get(this); 
      const int value1_abs_offset = base_offset + value1_local_offset; 
      const char * value2_pointer = pscript_stack__get_top__pointer(this -> pscript_env -> stack_env); 
      const int type2 = pscript_stack__get_top__type(this -> pscript_env -> stack_env); 
      if (type1 == type2) { 
	pscript_mem__write(this -> pscript_env -> mem_env, value1_abs_offset, type1_sizeof, value2_pointer); 
	//pscript_stack__drop(this -> pscript_env -> stack_env, local_size); 
	break; 
      }; 
      
      const int core_type2 = pscript_type__get_core_type(this -> pscript_env -> type_env, type2); 
      const int type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type2); 
      const int lval_type1 = type1; 
      
      if (core_type2 != PSCRIPT_TYPE__PROXY) { 
	const int cast_type2 = lval_type1; 
	const int cast_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, cast_type2); 
	char cast_value2[cast_type2_sizeof]; 
	pscript_value__cast_generic(this -> pscript_env, lval_type1, type2, type2_sizeof, value2_pointer, cast_type2_sizeof, cast_value2); 
	pscript_mem__write(this -> pscript_env -> mem_env, value1_abs_offset, cast_type2_sizeof, cast_value2); 
	break; 
      } 
      else { 
	const int proxy_type2 = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type2); 
	const int proxy_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, proxy_type2); 
	if (proxy_type2 == lval_type1) { 
	  //pmesslog("VM: MEM_STORE_LOCAL: proxy_type2 == lval_type1" "\n"); 
	  //pmesslog("VM: MEM_STORE_LOCAL: proxy_type2_sizeof = %d " "\n", proxy_type2_sizeof); 
	  const int proxy2_offset = *(const int_pscript_mem_t *) value2_pointer; 
#if 0
	  {
	    char val1[proxy_type2_sizeof]; 
	    char val2[proxy_type2_sizeof]; 
	    pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset1, proxy_type2_sizeof, val1); 
	    pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset2, proxy_type2_sizeof, val2); 
	    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val1); dputs(stderr_d, "\n"); 
	    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val2); dputs(stderr_d, "\n"); 
	  }; 
#endif 
	  pscript_mem__copy(this -> pscript_env -> mem_env, proxy2_offset, value1_abs_offset, proxy_type2_sizeof); 
#if 0
	  {
	    char val1[proxy_type2_sizeof]; 
	    char val2[proxy_type2_sizeof]; 
	    pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset1, proxy_type2_sizeof, val1); 
	    pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset2, proxy_type2_sizeof, val2); 
	    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val1); dputs(stderr_d, "\n"); 
	    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val2); dputs(stderr_d, "\n"); 
	  }; 
#endif 
	}
	else {
	  const int cast_type2 = lval_type1; 
	  const int cast_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, cast_type2); 
	  const int proxy2_offset = *(const int_pscript_mem_t *) value2_pointer; 
	  char cast_value2[cast_type2_sizeof]; 
	  const char * proxy_value2 = pscript_mem__get_pointer(this -> pscript_env -> mem_env, proxy2_offset); 
	  pscript_value__cast_generic(this -> pscript_env, lval_type1, proxy_type2, proxy_type2_sizeof, proxy_value2, cast_type2_sizeof, cast_value2); 
	  pscript_mem__write(this -> pscript_env -> mem_env, value1_abs_offset, cast_type2_sizeof, cast_value2); 
	}; 
	break; 
      }; 
    

    } while (false); 
    pscript_stack__drop(this -> pscript_env -> stack_env); 
    //next_ipc0 = ipc0 + 6; 
    break; 

    
    
  case PVM_BYTECODE__MEM_LOAD_LOCAL: { // type then @ are in the bytecode 
#if 1
    const int data_type = BYTECODE_READ_ARG(int8_t); //*(int8_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int data_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, data_type); 
    const int data_local_offset = BYTECODE_READ_ARG(int_pscript_mem_t); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 2); 
    //const int base_offset  = pscript_vm__call_stack__get_mem_base_offset(this); 
    const int base_offset  = pscript_vm__memory_frame__get(this); 
    const int abs_offset   = base_offset + data_local_offset; 
    const char * mem_pointer = pscript_mem__get_pointer(this -> pscript_env -> mem_env, abs_offset); 
    pscript_stack__push_value(this -> pscript_env -> stack_env, data_type, data_sizeof, mem_pointer); 
    //next_ipc0 = ipc0 + 6; 
#else 
    const int local_offset = *(int_pscript_mem_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int local_size   = *(int *) (bytecode_env -> bytecode_array + ipc0 + 5); 
    const int base_offset  = pscript_vm__call_stack__get_mem_base_offset(this); 
    const int abs_offset   = base_offset + local_offset; 
    const char * stack_pointer = pscript_stack__get_top__pointer(this -> pscript_env -> stack_env); 
    pscript_mem__write(this -> pscript_env -> mem_env, abs_offset, local_size, stack_pointer); 
    //pscript_stack__drop(this -> pscript_env -> stack_env, local_size); 
    pscript_stack__drop(this -> pscript_env -> stack_env); 
    next_ipc0 = ipc0 + 9; 
#endif 
  }; break; 
    




  case PVM_BYTECODE__MEM_LOAD: do { // type then @ are in the bytecode 
      const int data_type = BYTECODE_READ_ARG(int8_t); //*(int8_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int data_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, data_type); 
      const int data_offset = BYTECODE_READ_ARG(int_pscript_mem_t); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 2); 
      const int abs_offset  = data_offset; 
      const char * mem_pointer = pscript_mem__get_pointer(this -> pscript_env -> mem_env, abs_offset); 
      pscript_stack__push_value(this -> pscript_env -> stack_env, data_type, data_sizeof, mem_pointer); 
    } while (false); 
    //next_ipc0 = ipc0 + 6; 
#if 0 
    dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "STACK MEM: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> mem__nb * sizeof(*(this -> pscript_env -> stack_env -> mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
    break; 

    

#define STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL			\
    assert(this -> pscript_env -> stack_env -> stack__nb >= 2);		\
    const int type_index1 = this -> pscript_env -> stack_env -> stack__nb - 2; \
    const int type_index2 = this -> pscript_env -> stack_env -> stack__nb - 1; \
    const int type1 = this -> pscript_env -> stack_env -> stack__type[type_index1]; \
    const int type2 = this -> pscript_env -> stack_env -> stack__type[type_index2]; \
    const int type1_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1); \
    const int type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type2); \
    char value1[type1_sizeof];						\
    char value2[type2_sizeof];						\
    const int stack_offset2 = pscript_stack__get_top__offset(this -> pscript_env -> stack_env); \
    bcopy(this -> pscript_env -> stack_env -> mem__array + stack_offset2, value2, type2_sizeof); \
    pscript_stack__drop(this -> pscript_env -> stack_env);		\
    const int stack_offset1 = pscript_stack__get_top__offset(this -> pscript_env -> stack_env); \
    bcopy(this -> pscript_env -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof); \
    pscript_stack__drop(this -> pscript_env -> stack_env);		

#define STACK__BINARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT		\
    const int type1__proxy_transparent = pscript_type__transparent_proxy__get_type(this -> pscript_env -> type_env, type1); \
    const int type2__proxy_transparent = pscript_type__transparent_proxy__get_type(this -> pscript_env -> type_env, type2); \
    const int type1__proxy_transparent__sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1__proxy_transparent); \
    const int type2__proxy_transparent__sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type2__proxy_transparent); \
    const char * type1__proxy_transparent__value = pscript_type__transparent_proxy__get_value(this -> pscript_env, type1, type1_sizeof, value1); \
    const char * type2__proxy_transparent__value = pscript_type__transparent_proxy__get_value(this -> pscript_env, type2, type2_sizeof, value2); 
    
#define STACK__RETVAL(retval_size_to_alloc)				\
    const int retval_sizeof_alloc = retval_size_to_alloc;		\
    char retval_value[retval_sizeof_alloc];				\
    int retval_sizeof = -1;						\
    int retval_type = -1; 
    
#define STACK__BINARY_OPERATION_PREAMBLE				\
    STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL;			\
    STACK__RETVAL(MAX3((int)sizeof(int),type1_sizeof,type2_sizeof)); 
    
#define STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL__NO_DROP		\
    assert(this -> pscript_env -> stack_env -> stack__nb >= 1);		\
    const int type_index1   = this -> pscript_env -> stack_env -> stack__nb - 1; \
    const int type1         = this -> pscript_env -> stack_env -> stack__type[type_index1]; \
    const int type1_core    = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); \
    const int type1_sizeof  = pscript_type__sizeof(this -> pscript_env -> type_env, type1); \
    char value1[type1_sizeof];						\
    const int stack_offset1 = pscript_stack__get_top__offset(this -> pscript_env -> stack_env); \
    bcopy(this -> pscript_env -> stack_env -> mem__array + stack_offset1, value1, type1_sizeof);	
    
#define STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL			\
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL__NO_DROP;		\
    pscript_stack__drop(this -> pscript_env -> stack_env);		
    
#define STACK__UNARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT		\
    const int type1__proxy_transparent           = pscript_type__transparent_proxy__get_type(this -> pscript_env -> type_env, type1); \
    const int type1__proxy_transparent__sizeof   = pscript_type__sizeof(this -> pscript_env -> type_env, type1__proxy_transparent); \
    const char * type1__proxy_transparent__value = pscript_type__transparent_proxy__get_value(this -> pscript_env, type1, type1_sizeof, value1); 
    
#define STACK__UNARY_OPERATION_PREAMBLE__TRANSPARENT		\
    const int    type1__transparent         = type1_core == PSCRIPT_TYPE__PROXY ? pscript_type__transparent_proxy__get_type(this -> pscript_env -> type_env, type1) : type1_core == PSCRIPT_TYPE__VARIANT ? *(const int_pscript_type_t *)value1 : type1; \
    const int    type1__transparent__core   = pscript_type__get_core_type(this -> pscript_env -> type_env, type1__transparent); \
    const int    type1__transparent__sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type1__transparent); \
    const char * type1__transparent__value  = type1_core == PSCRIPT_TYPE__PROXY ? pscript_type__transparent_proxy__get_value(this -> pscript_env, type1, type1_sizeof, value1) : type1_core == PSCRIPT_TYPE__VARIANT ? (const char *)(pscript_mem__get_pointer(this -> pscript_env -> mem_env, *(const int_pscript_mem_t *)(value1 + sizeof(int_pscript_type_t)))) : value1; 
    
#define STACK__UNARY_OPERATION_PREAMBLE					\
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL;				\
    STACK__RETVAL(MAX((int)sizeof(int),type1_sizeof)); 
		  
#define STACK__OPERATION__POSTAMBLE					\
    pscript_stack__push_value(this -> pscript_env -> stack_env, retval_type, retval_sizeof, retval_value); 
    
 
    
  case PVM_BYTECODE__CALL_STACK_OBJECT: do { 
      // RL: The object to be called is on the top of the stack. 
      //dputs_array(stderr__fileno, "PVM_BYTECODE__CALL_STACK_OBJECT", "\n");
      const int8_t on_stack_arg_nb = BYTECODE_READ_ARG(int8_t); //bytecode_env -> bytecode_array[ipc0 + 1]; 
      const int    ret_address     = next_ipc0; 
#if 0 
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL__NO_DROP; 
      //STACK__UNARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT; 
      STACK__UNARY_OPERATION_PREAMBLE__TRANSPARENT; 
      
      
      //dputs_array(stderr__fileno, "PSCRIPT_TYPE__FFIPROC: ", "on_stack_arg_nb: ", int_string__stack(on_stack_arg_nb), "\n");
      //dputs_array(stderr__fileno, "PSCRIPT_TYPE__FFIPROC: ", "type1: ", pscript_type__get_cstr(this -> pscript_env -> type_env, type1), "\n");
    
      //const int type1_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type1__proxy_transparent); 
      
      if (type1__transparent__core == PSCRIPT_TYPE__FUNPROC) { 
	pscript_stack__drop(this -> pscript_env -> stack_env);		
	//const int32_t funproc_boffset = *((const int32_t *) value1); 
	const int32_t funproc_boffset = *((const int32_t *) type1__transparent__value); 
	//const int32_t funproc_boffset = *((const int32_t *) type1__proxy_transparent__value); 
	//dputs_array(stderr__fileno, "PSCRIPT_TYPE__FUNPROC: ", "funproc_boffset: ", int_string__stack(funproc_boffset), "\n"); 
	const int call_address        = funproc_boffset; 
	const int caller_ebp          = pscript_vm__memory_frame__get(this); 
	const int retval_type         = pscript_type__funproc__get_retval_type(this -> pscript_env -> type_env, type1__transparent); 
	const int retval_sizeof       = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
	const int ret_address         = retval_sizeof == 0 ? next_ipc0 + 1 : next_ipc0; // RL: sizeof(DROP) 
	pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
	next_ipc0 = call_address; 
	break; 
      }; 
      
      if (type1__transparent__core == PSCRIPT_TYPE__FFIPROC) { 
	pscript_stack__drop(this -> pscript_env -> stack_env);		
	//const int16_t ffiproc_i = *((const int16_t *) value1); 
	const int16_t ffiproc_i = *((const int16_t *) type1__transparent__value); 
	//const int16_t ffiproc_i = *((const int16_t *) type1__proxy_transparent__value); 
	//dputs_array(stderr__fileno, "PSCRIPT_TYPE__FFIPROC: ", "ffiproc_i: ", int_string__stack(ffiproc_i), "\n"); 
	const int alright_huh = pscript_vm__step__call_ffi(this, ffiproc_i, on_stack_arg_nb, ret_address); 
	if (alright_huh >= 0) { 
	  next_ipc0 = ret_address; 
	  const int retval_nb = alright_huh; 
	  //dputs_array(stderr__fileno, "CALL_FFI - retval_nb = ", int_string__stack(retval_nb), "\n"); 
	  //dputs_array(stderr__fileno, "CALL_FFI - ret_address = ", int_string__stack(ret_address), "\n"); 
	  if (0 == retval_nb) { 
	    // RL: The type of the object is not statically computed, 
	    //     we always expect something on the stack, 
	    //     therefore we must push an object on the stack. 
	    STACK__RETVAL(0); 
	    retval_type   = pscript_type__get__void(this -> pscript_env -> type_env); 
	    retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
	    STACK__OPERATION__POSTAMBLE; 
#if 0 
  dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
  dputs_array(stderr__fileno, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
  dputs_array(stderr__fileno, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
	  }; 
	} 
	else { 
	  this -> error_ipc0     =     ipc0; 
	  this -> error_bytecode = bytecode; 
	  next_ipc0              = alright_huh; 
	}; 
	
	break; 
      }; 
      
      if (type1__transparent__core == PSCRIPT_TYPE__LIBPROC) { 
	this -> error_ipc0     =     ipc0; 
	this -> error_bytecode = bytecode; 
	next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
	break; 
      }; 
      
      if (type1__transparent__core == PSCRIPT_TYPE__LAMBDA) { 
	//next_ipc0 = pscript_vm__step__call_lambda(this, type1, value1, on_stack_arg_nb, ret_address); 
#if 0 
	if (4202 != *(const int_pscript_mem_t *)type1__transparent__value) { 
	  assert(false); 
	}; 
#endif 
	next_ipc0 = pscript_vm__step__call_lambda(this, type1__transparent, type1__transparent__value, on_stack_arg_nb, ret_address); 
	break; 
      }; 
      

      assert(false); 
      this -> error_ipc0     =     ipc0; 
      this -> error_bytecode = bytecode; 
      next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
      
    } while (false); break; 


  case PVM_BYTECODE__EQUAL_HUH: do { 
      STACK__BINARY_OPERATION_PREAMBLE; 
      
#if 0 
      dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
      
      pscript_value__operation__equal_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
      STACK__OPERATION__POSTAMBLE;
      //next_ipc0 = ipc0 + 1; 
  } while (false); break; 


  case PVM_BYTECODE__INFEQ_HUH: { 
    STACK__BINARY_OPERATION_PREAMBLE; 

#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 

    pscript_value__operation__infeq_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
  }; break; 


  case PVM_BYTECODE__SUPEQ_HUH: { 
    STACK__BINARY_OPERATION_PREAMBLE; 

#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 

    pscript_value__operation__supeq_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
  }; break; 


  case PVM_BYTECODE__SUP_HUH: { 
    STACK__BINARY_OPERATION_PREAMBLE; 

#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 

    pscript_value__operation__sup_huh(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
  }; break; 


  case PVM_BYTECODE__JFALSE: { 
    const int jump_address = *(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int type_i = pscript_stack__get_top__type(this -> pscript_env -> stack_env); 
    const int type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type_i); 
    if (type_core != PSCRIPT_TYPE__BOOL) {
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error_value_type = type_i; 
      next_ipc0 = PSCRIPT_VM__ERROR__BOOL_EXPECTED; 
      break; 
    }; 
    
#if 1 
    char bool_value; 
    int value_sizeof; 
    pscript_stack__get_top__value(this -> pscript_env -> stack_env, &value_sizeof, 1, &bool_value); 
#else 
    const int type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type_i); 
    const char value[type_sizeof]; 
    int value_sizeof; 
    pscript_stack__get_top__value(this -> pscript_env -> stack_env, &value_sizeof, type_sizeof, value); 
#endif 

    pscript_stack__drop(this -> pscript_env -> stack_env); 

    if (bool_value) {
      next_ipc0 = ipc0 + 5; 
    }
    else {
      next_ipc0 = jump_address; 
    }; 
    
  }; break; 
    
    



  case PVM_BYTECODE__JTRUE: { 
    const int jump_address = *(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int type_i = pscript_stack__get_top__type(this -> pscript_env -> stack_env); 
    const int type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type_i); 
    if (type_core != PSCRIPT_TYPE__BOOL) {
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error_value_type = type_i; 
      next_ipc0 = PSCRIPT_VM__ERROR__BOOL_EXPECTED; 
      break; 
    }; 
    
    char bool_value; 
    int value_sizeof; 
    pscript_stack__get_top__value(this -> pscript_env -> stack_env, &value_sizeof, 1, &bool_value); 
    pscript_stack__drop(this -> pscript_env -> stack_env); 

    if (bool_value) {
      next_ipc0 = jump_address; 
    }
    else {
      next_ipc0 = ipc0 + 5; 
    }; 
    
  }; break; 
    
    


  case PVM_BYTECODE__LEA_LOCAL: { 
    const int type_i       = BYTECODE_READ_ARG(int8_t); //*(char *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int local_offset = BYTECODE_READ_ARG(int_pscript_mem_t); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 2); 
    const int type_core    = pscript_type__get_core_type(this -> pscript_env -> type_env, type_i); 
    if ((type_core != PSCRIPT_TYPE__POINTER) && (type_core != PSCRIPT_TYPE__PROXY)) {
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error_core_type__got = type_core; 
      this -> error_core_type__expected = PSCRIPT_TYPE__POINTER; 
      next_ipc0 = PSCRIPT_VM__ERROR__TYPE_MISMATCHED; 
      break; 
    }; 
    
    //const int base_offset  = pscript_vm__call_stack__get_mem_base_offset(this); 
    const int base_offset  = pscript_vm__memory_frame__get(this); 
    const int abs_offset   = base_offset + local_offset; 
    //pscript_stack__push_value(this -> pscript_env -> stack_env, type_i, sizeof(abs_offset), (const char *) &abs_offset); 
    pscript_stack__push_value(this -> pscript_env -> stack_env, type_i, sizeof(int_pscript_mem_t), (const char *) &abs_offset); 
    
    //next_ipc0 = ipc0 + 6; 
  }; break; 


  case PVM_BYTECODE__DROP: { 
    pscript_stack__drop(this -> pscript_env -> stack_env); 
  }; break; 
    
  case PVM_BYTECODE__DUP: {
    STACK__UNARY_OPERATION_PREAMBLE;
    pscript_stack__push_value(this -> pscript_env -> stack_env, type1, type1_sizeof, value1); 
    pscript_stack__push_value(this -> pscript_env -> stack_env, type1, type1_sizeof, value1); 

#if 0
    dputs(stderr_d, "DUP - VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
#endif 

    //next_ipc0 = ipc0 + 1; 
  }; break; 
    
  case PVM_BYTECODE__RECORD_GET_FIELD_VALUE_BY_NAME: {
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL;
    //const int type1_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
    assert(PSCRIPT_TYPE__POINTER == type1_core || PSCRIPT_TYPE__PROXY == type1_core || PSCRIPT_TYPE__RECORD == type1_core || PSCRIPT_TYPE__USER == type1_core); 
    int subtype_i;
    if (PSCRIPT_TYPE__RECORD == type1_core) { 
      subtype_i = type1; 
    }
    else if (PSCRIPT_TYPE__POINTER == type1_core) { 
      subtype_i = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
    }
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      subtype_i = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
    } 
    else if (PSCRIPT_TYPE__USER == type1_core) { 
      subtype_i = pscript_type__user_defined_type__get_subtype(this -> pscript_env -> type_env, type1); 
    } 
    else { 
      assert(false);
    }; 
    int subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
    while (PSCRIPT_TYPE__USER == subtype_core) { 
      subtype_i = pscript_type__user_defined_type__get_subtype(this -> pscript_env -> type_env, subtype_i); 
      subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
    }; 
    const int ident_i = *(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int field_i = pscript_type__record__get_field_index(this -> pscript_env -> type_env, subtype_i, ident_i);  
    const int field_type   = pscript_type__record__get_field_type  (this -> pscript_env -> type_env, subtype_i, field_i);  
    const int field_offset = pscript_type__record__get_field_offset(this -> pscript_env -> type_env, subtype_i, field_i); 
    const int field_sizeof = pscript_type__record__get_field_sizeof(this -> pscript_env -> type_env, subtype_i, field_i); 
    char field_value[field_sizeof]; 
    const int value_is_on_the_stack_huh = !(PSCRIPT_TYPE__POINTER == type1_core || PSCRIPT_TYPE__PROXY == type1_core); 
    if (value_is_on_the_stack_huh) { 
      bcopy(value1 + field_offset, field_value, field_sizeof); 
    }
    else { 
      const int record_mem_offset = *(int_pscript_mem_t *) value1; 
      pscript_mem__read(this -> pscript_env -> mem_env, record_mem_offset + field_offset, field_sizeof, field_value); 
    }; 
    pscript_stack__push_value(this -> pscript_env -> stack_env, field_type, field_sizeof, field_value); 
    next_ipc0 = ipc0 + 3; 
  }; break; 

  case PVM_BYTECODE__RECORD_GET_FIELD_ADDRESS_BY_NAME: {
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL; 
    //dputs_array(stderr__fileno, __FILE__, ":", STRINGIFY(__LINE__), ":<", __func__, ">: ", "PVM_BYTECODE__RECORD_GET_FIELD_ADDRESS_BY_NAME: type1 = ", pscript_type__get_cstr(this -> pscript_env -> type_env, type1), "\n");
    //const int type1_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
    assert(PSCRIPT_TYPE__POINTER == type1_core || PSCRIPT_TYPE__PROXY == type1_core); 
    int subtype_i;
    if (PSCRIPT_TYPE__POINTER == type1_core) { 
      subtype_i = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
    }
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      subtype_i = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
    } 
    else { 
      assert(false);
    }; 
    int subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
    while (PSCRIPT_TYPE__USER == subtype_core) { 
      subtype_i = pscript_type__user_defined_type__get_subtype(this -> pscript_env -> type_env, subtype_i); 
      subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
    }; 
    const int ident_i = *(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int field_i = pscript_type__record__get_field_index(this -> pscript_env -> type_env, subtype_i, ident_i);  
    const int field_type   = pscript_type__record__get_field_type  (this -> pscript_env -> type_env, subtype_i, field_i);  
    const int field_offset = pscript_type__record__get_field_offset(this -> pscript_env -> type_env, subtype_i, field_i); 
    //const int field_sizeof = pscript_type__record__get_field_sizeof(this -> pscript_env -> type_env, subtype_i, field_i); 
    const int field_ptr_type_i = pscript_type__make__pointer(this -> pscript_env -> type_env, field_type); 
    const int field_ptr_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, field_ptr_type_i); 
    const int field_ptr_value = *(int_pscript_mem_t *) value1 + field_offset; 
    pscript_stack__push_value(this -> pscript_env -> stack_env, field_ptr_type_i, field_ptr_sizeof, (const char *) &field_ptr_value); 
    next_ipc0 = ipc0 + 3; 
  }; break; 

  case PVM_BYTECODE__MEM_LOAD__STACK: {
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL;
    const int core_type1 = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
    int subtype1; 
    if (core_type1 == PSCRIPT_TYPE__POINTER) { 
      subtype1 = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
    }
    else if (core_type1 == PSCRIPT_TYPE__PROXY) { 
      subtype1 = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
    }
    else { 
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error_core_type__got = core_type1; 
      this -> error_core_type__expected = PSCRIPT_TYPE__POINTER; 
      next_ipc0 = PSCRIPT_VM__ERROR__TYPE_MISMATCHED; 
      break; 
    }; 
    
    const int subtype1_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, subtype1); 
    char subtype1_value[subtype1_sizeof]; 
    
    const int abs_mem_offset = *(int_pscript_mem_t *)value1; 
    pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset, subtype1_sizeof, subtype1_value); 
    
    pscript_stack__push_value(this -> pscript_env -> stack_env, subtype1, subtype1_sizeof, subtype1_value); 
    
    //next_ipc0 = ipc0 + 1; 
  }; break; 
    
  case PVM_BYTECODE__MEM_STORE__STACK: do { // POSTFIX: [-1] is @:  [0] is value (the size must be taken into account) 
#if 0 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
      dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      STACK__BINARY_OPERATION_PREAMBLE; 
#if 0 
      dputs(stderr_d, "VALUE1: [ "); dputn(stderr_d, type1); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "VALUE2: [ "); dputn(stderr_d, type2); dputs(stderr_d, " ] "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#elif 0 
      dputs_array(stderr__fileno, "VALUE1: [ ", pscript_type__get_cstr(this -> pscript_env -> type_env, type1), " ] "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
      dputs_array(stderr__fileno, "VALUE2: [ ", pscript_type__get_cstr(this -> pscript_env -> type_env, type2), " ] "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
#if 0 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
      dputs_array(stderr__fileno, "ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      const int core_type1 = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
      if (core_type1 != PSCRIPT_TYPE__POINTER && core_type1 != PSCRIPT_TYPE__PROXY && core_type1 != PSCRIPT_TYPE__VARIANT) { 
	this -> error_ipc0 = ipc0; 
	this -> error_bytecode = bytecode; 
	this -> error_core_type__got = core_type1; 
	this -> error_core_type__expected = PSCRIPT_TYPE__POINTER; 
	next_ipc0 = PSCRIPT_VM__ERROR__TYPE_MISMATCHED; 
	break; 
      }; 

    const int core_type2 = pscript_type__get_core_type(this -> pscript_env -> type_env, type2); 
    
    int lval_type1; 
    //int abs_mem_offset1; 
    if (core_type1 == PSCRIPT_TYPE__POINTER) { 
      lval_type1 = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
      //abs_mem_offset1 = *(const int *)value1; 
    } 
    else if (core_type1 == PSCRIPT_TYPE__PROXY) { 
      lval_type1 = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
      //abs_mem_offset1 = *(const int *)value1; 
    } 
    else if (core_type1 == PSCRIPT_TYPE__VARIANT) { 
      //lval_type1 = pscript_type__variant__get_subtype(this -> pscript_env -> type_env, type1); 
      lval_type1 = *(const int_pscript_type_t *) value1; 
      //abs_mem_offset1 = *(const int *) (value1 + sizeof(int_pscript_type_t)); 
    } 
    else {
      //assert(false); 
      this -> error_ipc0     =     ipc0; 
      this -> error_bytecode = bytecode; 
      next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
      break; 
    }; 

    //dputs_array(stderr__fileno, "SUB_TYPE1: [ ", pscript_type__get_cstr(this -> pscript_env -> type_env, lval_type1), " ] "); dputs(stderr_d, "\n"); 
    
    //const int abs_mem_offset1 = *(const int *)value1; 
    const int abs_mem_offset1 = *(const int_pscript_mem_t *)value1; 
    
    if (pscript_type__get_core_type(this -> pscript_env -> type_env, lval_type1) == PSCRIPT_TYPE__VARIANT) { 
      // RL: This means that this is the first assignement of the variable. 
      //     Therefore, memory has to be allocated for the assignement, 
      //     and the variant variable has to be adjusted accordingly. 
      //if (-1 != pscript_type__variant__get_subtype(this -> pscript_env -> type_env, lval_type1)) { 
      //const int type1_subtype = *(const int *) pscript_mem__get_pointer(this ->  pscript_env -> mem_env, abs_mem_offset1); 
      const int type1_subtype = *(const int_pscript_type_t *) pscript_mem__get_pointer(this ->  pscript_env -> mem_env, abs_mem_offset1); 
      
#if 0 
      if (-1 != type1_subtype) { 
	assert(false); 
	this -> error_ipc0     =     ipc0; 
	this -> error_bytecode = bytecode; 
	next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
	break; 
      }; 
      // RL: First, we have to determine to which memory frame that variable belongs to. 
      const int memory_part_i  = pscript_mem__to_which_memory_part_that_variable_belongs (this -> pscript_env -> mem_env, /*mem_offset*/abs_mem_offset1); 
      const int memory_frame_i = pscript_mem__to_which_memory_frame_that_variable_belongs(this -> pscript_env -> mem_env, /*mem_offset*/abs_mem_offset1); 
      const int frames_nb      = pscript_mem__stack_heap__count(this -> pscript_env -> mem_env); 
      dputs_array(stderr__fileno, "MEMPART:  ", int_string__stack(memory_part_i), " - MEMFRAME: ", int_string__stack(memory_frame_i), " - FRAMES NB: ", int_string__stack(frames_nb), "\n"); 
      const int mem_toplevel_compiletime_bss_used = pscript_mem__toplevel_compiletime__bss__used(this -> pscript_env -> mem_env); 
      const int mem_toplevel_compiletime_zss_used = pscript_mem__toplevel_compiletime__zss__used(this -> pscript_env -> mem_env); 
      const int mem_toplevel_compiletime_iss_used = pscript_mem__toplevel_compiletime__iss__used(this -> pscript_env -> mem_env); 
      const int mem_toplevel_runtime_used     = pscript_mem__toplevel_runtime__used    (this -> pscript_env -> mem_env); 
      const int mem_memory_frame_used         = pscript_mem__memory_frame__used        (this -> pscript_env -> mem_env); 
      const int mem_stack_heap_used           = pscript_mem__stack_heap__used          (this -> pscript_env -> mem_env); 
      const int mem_turing_heap_used           = pscript_mem__turing_heap__used        (this -> pscript_env -> mem_env); 
      dputs_array(stderr__fileno, "MEM_TOPLEVEL_COMPILETIME_BSS_USED: ", int_string__stack(mem_toplevel_compiletime_bss_used), "\n"); 
      dputs_array(stderr__fileno, "MEM_TOPLEVEL_COMPILETIME_ZSS_USED: ", int_string__stack(mem_toplevel_compiletime_zss_used), "\n"); 
      dputs_array(stderr__fileno, "MEM_TOPLEVEL_COMPILETIME_ISS_USED: ", int_string__stack(mem_toplevel_compiletime_iss_used), "\n"); 
      dputs_array(stderr__fileno, "MEM_TOPLEVEL_RUNTIME_USED    : ", int_string__stack(mem_toplevel_runtime_used    ), "\n"); 
      dputs_array(stderr__fileno, "MEM_MEMORY_FRAME_USED        : ", int_string__stack(mem_memory_frame_used        ), "\n"); 
      dputs_array(stderr__fileno, "MEM_STACK_HEAP_USED          : ", int_string__stack(mem_stack_heap_used          ), "\n"); 
      dputs_array(stderr__fileno, "MEM_TURING_HEAP_USED         : ", int_string__stack(mem_turing_heap_used         ), "\n"); 
      const int mem_memory_frame_count = pscript_mem__memory_frame__count(this -> pscript_env -> mem_env); 
      const int mem_stack_heap_count   = pscript_mem__stack_heap__count  (this -> pscript_env -> mem_env); 
      const int mem_turing_heap_count  = pscript_mem__turing_heap__count (this -> pscript_env -> mem_env); 
      dputs_array(stderr__fileno, "MEM_MEMORY_FRAME_COUNT: ", int_string__stack(mem_memory_frame_count), "\n"); 
      dputs_array(stderr__fileno, "MEM_STACK_HEAP_COUNT  : ", int_string__stack(mem_stack_heap_count  ), "\n"); 
      dputs_array(stderr__fileno, "MEM_TURING_HEAP_COUNT : ", int_string__stack(mem_turing_heap_count ), "\n"); 
      const int mem_turing_heap_height = pscript_mem__turing_heap__height(this -> pscript_env -> mem_env); 
      dputs_array(stderr__fileno, "MEM_TURING_HEAP_HEIGHT: ", int_string__stack(mem_turing_heap_height), "\n"); 

      if (memory_frame_i < 0) { 
	this -> error_ipc0     =     ipc0; 
	this -> error_bytecode = bytecode; 
	next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
	break; 
      }; 
#endif 
      

      int type2_actual; 
      const char * type2_actual_value; 
      if (core_type2 == PSCRIPT_TYPE__PROXY) { 
	type2_actual = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type2); 
	const int type2_actual_mem_offset = *(int_pscript_mem_t *)value2; 
	type2_actual_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, type2_actual_mem_offset); 
      } 
      else if (core_type2 == PSCRIPT_TYPE__VARIANT) { 
	//type2_actual = pscript_type__variant__get_subtype(this -> pscript_env -> type_env, type2); 
	type2_actual = *(const int_pscript_type_t *) value2; 
	const int type2_actual_mem_offset = *(const int_pscript_mem_t *) (value2 + sizeof(int_pscript_type_t)); 
	type2_actual_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, type2_actual_mem_offset); 
      } 
      else { 
	type2_actual = type2; 
	type2_actual_value = (char *)value2; 
      }; 
      
      
      
      const int type2_actual_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, type2_actual); 
      
      const int type1_variant_mem_offset = pscript_mem__runtime__allocate_same_memory_frame(this -> pscript_env -> mem_env, abs_mem_offset1, type2_actual_sizeof); 

      if (0 == type1_subtype) { 
	pscript_mem__write(this -> pscript_env -> mem_env, abs_mem_offset1, sizeof(int_pscript_type_t), (const char *)&type2_actual); 
	pscript_mem__write(this -> pscript_env -> mem_env, abs_mem_offset1 + sizeof(int_pscript_type_t), sizeof(int_pscript_mem_t), (const char *)&type1_variant_mem_offset); 
	
	pscript_mem__write(this -> pscript_env -> mem_env, type1_variant_mem_offset, type2_actual_sizeof, type2_actual_value); 
	break;
      };  
      
      if (type1_subtype == type2_actual) { 
	pscript_mem__write(this -> pscript_env -> mem_env, type1_variant_mem_offset, type2_actual_sizeof, type2_actual_value); 
	break; 
      }; 
      
      // RL: ELSE: Casting 
      { 
	const int cast_type2        = type1_subtype; 
	const int cast_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, cast_type2); 
	char cast_value2[cast_type2_sizeof]; 
	
	pscript_value__cast_generic(this -> pscript_env, cast_type2, type2, type2_sizeof, value2, cast_type2_sizeof, cast_value2); 
	
	pscript_mem__write(this -> pscript_env -> mem_env, type1_variant_mem_offset, cast_type2_sizeof, cast_value2); 
	break; 
      }; 
      
      
      assert(false); 
      break; 
    }; 

    if (core_type2 != PSCRIPT_TYPE__PROXY) {
      
      const int cast_type2        = lval_type1; 
      const int cast_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, cast_type2); 
      char cast_value2[cast_type2_sizeof]; 
      
      pscript_value__cast_generic(this -> pscript_env, lval_type1, type2, type2_sizeof, value2, cast_type2_sizeof, cast_value2); 
      
      const int abs_mem_offset1 = *(int_pscript_mem_t *)value1; 
      pscript_mem__write(this -> pscript_env -> mem_env, abs_mem_offset1, cast_type2_sizeof, cast_value2); 
      break; 
    } 
    else { 
      const int proxy_type2 = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type2); 
      const int proxy_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, proxy_type2); 
      if (proxy_type2 == lval_type1) { 
#if 0
	pmessage("VM: MEM_STORE__STACK: proxy_type2 == lval_type1" "\n"); 
	pmessage("VM: MEM_STORE__STACK: proxy_type2_sizeof = %d " "\n", proxy_type2_sizeof); 
#endif 
	const int abs_mem_offset1 = *(int_pscript_mem_t *)value1; 
	const int abs_mem_offset2 = *(int_pscript_mem_t *)value2; 
#if 0
	{
	  char val1[proxy_type2_sizeof]; 
	  char val2[proxy_type2_sizeof]; 
	  pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset1, proxy_type2_sizeof, val1); 
	  pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset2, proxy_type2_sizeof, val2); 
	  dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val1); dputs(stderr_d, "\n"); 
	  dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val2); dputs(stderr_d, "\n"); 
	}; 
#endif 
	pscript_mem__copy(this -> pscript_env -> mem_env, abs_mem_offset2, abs_mem_offset1, proxy_type2_sizeof); 
#if 0
	{
	  char val1[proxy_type2_sizeof]; 
	  char val2[proxy_type2_sizeof]; 
	  pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset1, proxy_type2_sizeof, val1); 
	  pscript_mem__read(this -> pscript_env -> mem_env, abs_mem_offset2, proxy_type2_sizeof, val2); 
	  dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val1); dputs(stderr_d, "\n"); 
	  dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(proxy_type2_sizeof, val2); dputs(stderr_d, "\n"); 
	}; 
#endif 
      }
      else {
	const int cast_type2 = lval_type1; 
	const int cast_type2_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, cast_type2); 
	const int abs_mem_offset2 = *(int_pscript_mem_t *)value2; 
	char cast_value2[cast_type2_sizeof]; 
	const char * proxy_value2 = pscript_mem__get_pointer(this -> pscript_env -> mem_env, abs_mem_offset2); 
	pscript_value__cast_generic(this -> pscript_env, lval_type1, proxy_type2, proxy_type2_sizeof, proxy_value2, cast_type2_sizeof, cast_value2); 
	
	const int abs_mem_offset1 = *(int_pscript_mem_t *)value1; 
	pscript_mem__write(this -> pscript_env -> mem_env, abs_mem_offset1, cast_type2_sizeof, cast_value2); 
      }; 
      break; 
    }; 
    

    
    
    
    //pscript_stack__drop(this -> pscript_env -> stack_env); // No: already dropped in the preamble. 
    //pscript_stack__drop(this -> pscript_env -> stack_env); // No: already dropped in the preamble. 
    
  } while (false); ; 
    //next_ipc0 = ipc0 + 1; 
    break; 
    

  case PVM_BYTECODE__JUMP: { 
    const int jump_address = *(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    next_ipc0 = jump_address; 
  }; break; 
    
  case PVM_BYTECODE__MEM_DEALLOCATE: { // size is in the bytecode 
    // RL: TODO XXX FIXME: The current local memory may not belong to the caller, therefore the allocation should then be done on the stack-heap. 
    assert(false); 
    const int to_be_dropped_size = *(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    //pscript_mem__deallocate(this -> pscript_env -> mem_env, to_be_dropped_size); 
    next_ipc0 = ipc0 + 5; 
  }; break; 

  case PVM_BYTECODE__RET: { 
    assert(this -> call_stack__nb >= 1); 
    const int index = this -> call_stack__nb - 1; 
    const int ret_address = this -> call_stack__ret[index]; 
    const int caller_ebp  = this -> call_stack__caller_ebp[index]; 
    this -> call_stack__nb --; 
    next_ipc0 = ret_address; 
    pscript_vm__memory_frame__set(this, caller_ebp); 
  }; break; 



  case PVM_BYTECODE__NEG: { 
    //STACK__UNARY_OPERATION_PREAMBLE; 
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL; 
    STACK__UNARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT; 
    STACK__RETVAL(type1__proxy_transparent__sizeof); 
    
#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
#endif 
    
    //pscript_value__operation__opposite(this -> pscript_env, type1, type1_sizeof, value1, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    pscript_value__operation__opposite(this -> pscript_env, type1__proxy_transparent, type1__proxy_transparent__sizeof, type1__proxy_transparent__value, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value);     STACK__OPERATION__POSTAMBLE; 
    //next_ipc0 = ipc0 + 1; 
  }; break; 
    
    
    
  case PVM_BYTECODE__ADD: { 
    //STACK__BINARY_OPERATION_PREAMBLE; 
    STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL; 
    STACK__BINARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT; 
    STACK__RETVAL(MAX(type1__proxy_transparent__sizeof,type2__proxy_transparent)); 
    
#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
    
    //pscript_value__operation__add(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    pscript_value__operation__add(this -> pscript_env, type1__proxy_transparent, type1__proxy_transparent__sizeof, type1__proxy_transparent__value, type2__proxy_transparent, type2__proxy_transparent__sizeof, type2__proxy_transparent__value, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    
    STACK__OPERATION__POSTAMBLE; 
    //next_ipc0 = ipc0 + 1; 
  }; break; 
    
    
    
  case PVM_BYTECODE__SUB: { 
    //STACK__BINARY_OPERATION_PREAMBLE; 
    STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL; 
    STACK__BINARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT; 
    STACK__RETVAL(MAX(type1__proxy_transparent__sizeof,type2__proxy_transparent)); 

#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 

    //pscript_value__operation__sub(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    pscript_value__operation__sub(this -> pscript_env, type1__proxy_transparent, type1__proxy_transparent__sizeof, type1__proxy_transparent__value, type2__proxy_transparent, type2__proxy_transparent__sizeof, type2__proxy_transparent__value, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 

    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
  }; break; 




  case PVM_BYTECODE__MULT: { 
    //STACK__BINARY_OPERATION_PREAMBLE; 
    // RL: Here, we have an issue: type2 could be a proxy of a record. Therefore, the 'retval_sizeof_alloc' would be one of a pointer, instead of a record. 
    STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL; 
    STACK__BINARY_OPERATION_PREAMBLE__PROXY_TRANSPARENT; 
    STACK__RETVAL(MAX(type1__proxy_transparent__sizeof,type2__proxy_transparent)); 

#if 0  
    { 
      dputs_array(stderr__fileno, "type2 = (", int_string__stack(type2), ")", pscript_type__get_cstr(this -> pscript_env -> type_env, type2), "\n"); 
      dputs_array(stderr__fileno, "type2__proxy_transparent = (", int_string__stack(type2__proxy_transparent), ")", pscript_type__get_cstr(this -> pscript_env -> type_env, type2__proxy_transparent), "\n"); 
    }; 
#endif 
    
#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
    
    //pscript_value__operation__mul(this -> pscript_env, type1, type1_sizeof, value1, type2, type2_sizeof, value2, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    pscript_value__operation__mul(this -> pscript_env, type1__proxy_transparent, type1__proxy_transparent__sizeof, type1__proxy_transparent__value, type2__proxy_transparent, type2__proxy_transparent__sizeof, type2__proxy_transparent__value, &retval_type, &retval_sizeof, retval_sizeof_alloc, retval_value); 
    STACK__OPERATION__POSTAMBLE; 
    //next_ipc0 = ipc0 + 1; 
  }; break; 





  case PVM_BYTECODE__CALL_METHOD: do { 
    dputs(stderr_d, "STACK NB  : "); dputn(stderr_d, this -> pscript_env -> stack_env -> stack__nb); dput_eol(stderr_d);
#if 1 
    const int    on_stack_arg_nb      = BYTECODE_READ_ARG(int8_t);  //*(int8_t  *) (bytecode_env -> bytecode_array + ipc0 + 1); 
    const int    method_ident_pstr    = BYTECODE_READ_ARG(int16_t); //*(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 2); 
    const int8_t method_arg_nb        = on_stack_arg_nb - 1; 
    const int    supposed_ret_address = next_ipc0; 
    const int    caller_ebp           = pscript_vm__memory_frame__get(this); 
#else 
    const int8_t on_stack_arg_nb      = bytecode_env -> bytecode_array[ipc0 + 1]; 
    const int8_t method_arg_nb        = on_stack_arg_nb - 1; 
    const int    method_ident_pstr    = *(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 2); 
    const int    supposed_ret_address = ipc0 + 4; 
    //const int    base_mem_offset      = pscript_mem__get_base_offset(this -> pscript_env -> mem_env); 
    const int    caller_ebp           = pscript_vm__memory_frame__get(this); 
#endif 
#if 0 
    // RL: Issue here: the object type is not computed at compile-time, 
    //     therefore the return type of the method was not known at compile-time,  
    //     therefore an object was always supposed to be returned. 
    // RL: Moreover, the code could be wrongly typed and expect an object to be returned. 
    //     Therefore we can't just skip 'drop' which might not exist. 
    const int    ret_address       = retval_sizeof == 0 ? supposed_ret_address + 1 : supposed_ret_address; // RL: sizeof(DROP) 
    //const int    ret_address       = ipc0 + 4; 
    //const int    base_mem_offset   = pscript_mem__get_base_offset(this -> pscript_env -> mem_env); 
    const int    caller_ebp        = pscript_vm__memory_frame__get(this); 
    //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, base_mem_offset); 
    //pscript_vm__call_stack__push(this, ret_address, method_arg_nb, caller_ebp); 
    pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
#endif 
    
    // RL: Getting 'this' type 
    assert(this -> pscript_env -> stack_env -> stack__nb >= method_arg_nb + 1); 
    const int this_type      = this -> pscript_env -> stack_env -> stack__type[this -> pscript_env -> stack_env -> stack__nb - method_arg_nb - 1]; 
    const int this_type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, this_type); 
    
    // RL: Looking up for the method // RL: TODO XXX FIXME: For UDF, do a recursive lookup? (Like inheritance?) 
    int object_type = this_type; 
    if (this_type_core == PSCRIPT_TYPE__PROXY) { 
      object_type = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, this_type); 
    }; 
    const int method_i = pscript_type__methode__lookup(this -> pscript_env -> type_env, object_type, method_ident_pstr); 
    if (method_i < 0) { 
      this -> error_ipc0 = ipc0; 
      this -> error_bytecode = bytecode; 
      this -> error_ffi_arg_nb = method_arg_nb; 
      this -> error_object_type = object_type; 
      this -> error_method_ident_pstr = method_ident_pstr; 
      next_ipc0 = PSCRIPT_VM__ERROR__OBJECT_DOES_NOT_HAVE_THAT_METHOD; 
      break; 
    }; 

#if 0
    // ------------------------------------------------------------------- 
    // RL: UGLY HACK 
    // 
   // RL: TODO XXX FIXME: Object should be allocated in the memory_frame and not on the stack. 
   // RL: Problem: The resulting type of 'this * this' is not computed at compile-time. 
   //     Therefore, at compile-time, we do not know the size of the resulting value. 
   //     ... 
   //     Well, nothing is great here. 
   //     If the type was compile-time computed, then 'pscript_value' would not have to allocate any memory. 
   //     If the type is only known when 'pscript_value' happens, then it could make sense that 'pscript_value' allocates the needed memory, even though I do not like that much. 
   //     ... 
   //     Whyt not. 
   //     ... 
   //     In the meantime, we'll do a hack: when the method will be called, if the object is on the stack, it moves it in the local memory frame (a cast issue...! casting an object to a proxy to that objet, casting issueswhich requires to the ability to allocate memory) 
   //     ... 
   //     Nothing is great. Ugly hack in the meantime.  
   //     ... 
#elif 0 
    // RL: If the object is on the stack, then it will be move to the memory by the method. 
    //     As a method acts on an object by side-effects, a method should get a reference. 
    // RL: Also, the object reference is dropped from the stack at the start of the method call! 
    //     Therefore, if an object is fully pushed on the stack, it will be then fully dropped by the method call. 
    //     And there won't be any reference to it anymore. It won't exist anymore after the method call. 
    // RL: What about the case of a method which returns 'this'? We'll see. Because we want it to return a 
    //     reference to that object, but that object might have been on the stack. We'll see. 
#elif 1 
    // RL: If the object is on the stack, then we move it to the memory. (A method acts on an object by 
    //     side-effects. And, in the current memory model, the stack is not part of the memory, therefore 
    //     it cannot referenced [no variable ever points to the stack].) 
    // RL: On the long run, referencing the stack should not be a problem: on the stack could be any data, but their lifetime is an expression. And they can't be referenced before or after the expression. So doing method calls on on-stack objects should be doable. 
    if (this_type_core != PSCRIPT_TYPE__PROXY) { 
      //dputs_array(stderr__fileno, "HERE - ", __FILE__, " ", __func__, " ", int_string__stack(__LINE__), "\n"); 
#if 0 
      dputs(stderr_d, "STACK NB  : "); dputn(stderr_d, this -> pscript_env -> stack_env -> stack__nb); dput_eol(stderr_d);
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      // RL: First, the 
      // RL: First, can we allocate it on the local_memory or do we have to allocate it on the local_heap? 
      //const int active_on_top_huh = pscript_vm__memory_frame__is_active_on_top_huh(this); 
      const int caller_on_top_huh              = pscript_vm__memory_frame__is_caller_on_top_huh(this); 
      const int allocate_on_regular_memory_huh = caller_on_top_huh; //active_on_top_huh; 
      const int this_type_sizeof               = pscript_type__sizeof(this -> pscript_env -> type_env, this_type); 
      int this_mem_offset; 
      if (allocate_on_regular_memory_huh) {
	this_mem_offset = pscript_mem__memory_frame__allocate_frame(this -> pscript_env -> mem_env, this_type_sizeof); 
      } 
      else { 
	const int ref_ebp = caller_ebp; 
	this_mem_offset = pscript_mem__stack_heap__allocate(this -> pscript_env -> mem_env, caller_ebp, this_type_sizeof); 
      }; 
      char * this_mem_ptr = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, this_mem_offset); 
#if 1 
      pscript_stack__get__value(this -> pscript_env -> stack_env, /*depth*/method_arg_nb, /*value_sizeof_ref*/NULL, /*value_allocated_sizeof*/this_type_sizeof, /*value*/this_mem_ptr); 
      const int proxy_type_i      = pscript_type__make__proxy(this -> pscript_env -> type_env, this_type); 
      const int proxy_type_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, proxy_type_i); 
      // RL: TODO XXX FIXME: This is not great. 
      //     On the long run, instead of shifting around the data in the stack, the stack should be in memory and then addressable. 
      //pscript_stack__substitute(this -> pscript_env -> stack_env, /*depth*/method_arg_nb, /*substituting_type_i*/proxy_type_i, /*substituting_value_sizeof*/sizeof(int), /*substituting_value*/(const char *)&this_mem_offset); 
      pscript_stack__substitute(this -> pscript_env -> stack_env, /*depth*/method_arg_nb, /*substituting_type_i*/proxy_type_i, /*substituting_value_sizeof*/proxy_type_sizeof, /*substituting_value*/(const char *)&this_mem_offset); 
#else 
      pscript_stack__get_top__value(this -> pscript_env -> stack_env, /*value_sizeof_ref*/NULL, /*value_allocated_sizeof*/this_type_sizeof, /*value*/this_mem_ptr); 
      pscript_stack__drop(this -> pscript_env -> stack_env); 
      const int proxy_type_i = pscript_type__make__proxy(this -> pscript_env -> type_env, this_type); 
      pscript_stack__push_value(this -> pscript_env -> stack_env, proxy_type_i, sizeof(this_mem_offset), &this_mem_offset); 
#endif 
#if 0 
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 

    }; 
#endif     
    
    // RL: Now, in order to run it, we have to filter whether the method is a regular function, or a ffi, etc. 
    const int method_type = pscript_type__methode__get_type(this -> pscript_env -> type_env, object_type, method_i); 
    if (PSCRIPT_METHODE_TYPE__FUNPROC == method_type) { 
      const int funproc_i      = pscript_type__methode__get_funproc(this -> pscript_env -> type_env, object_type, method_i); 
      const int funproc_offset = pscript_funproc__get_bytecode_offset(this -> pscript_env -> funproc_env, funproc_i); 
      const int retval_type    = pscript_funproc__get_retval_type(this -> pscript_env -> funproc_env, funproc_i); 
      const int retval_sizeof  = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
#if 1 
      // RL: Issue here: the object type is not computed at compile-time, 
      //     therefore the return type of the method was not known at compile-time,  
      //     therefore an object was always supposed to be returned. 
      // RL: Moreover, the code could be wrongly typed and expect an object to be returned. 
      //     Therefore we can't just skip 'drop' which might not exist. 
      //const int next_ope_is_drop_huh = PVM_BYTECODE__DROP == bytecode_env -> bytecode_array[supposed_ret_address]; 
      //const int    ret_address       = (retval_sizeof == 0 && next_ope_is_drop_huh) ? supposed_ret_address + 1 : supposed_ret_address; // RL: sizeof(DROP) 
      const int    ret_address       = supposed_ret_address; 
      //const int    ret_address       = ipc0 + 4; 
      //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, base_mem_offset); 
      //pscript_vm__call_stack__push(this, ret_address, method_arg_nb, caller_ebp); 
      pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
#endif 

      const int call_address   = funproc_offset; 
      next_ipc0 = call_address; 
      break; 
    }; // RL: if (PSCRIPT_METHODE_TYPE__FUNPROC == method_type) 
    






#if 1 
      const int    ret_address       = supposed_ret_address; 
      //const int    ret_address       = ipc0 + 4; 
      //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, base_mem_offset); 
      //pscript_vm__call_stack__push(this, ret_address, method_arg_nb, caller_ebp); 
      //pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); // RL: Done in 'pscript_vm__step__call_ffi'. 
#endif 
    
    if (PSCRIPT_METHODE_TYPE__FFIPROC == method_type) { 
      const int ffiproc_i      = pscript_type__methode__get_ffiproc(this -> pscript_env -> type_env, object_type, method_i); 
#if 0 
      // RL: Done in 'pscript_vm__step__call_ffi'. 
      const int ffiproc_arg_nb = on_stack_arg_nb; //method_arg_nb + 1; 
      const int atleast_arg_nb = pscript_ffiproc__get_atleast_arg_nb(this -> pscript_env -> ffiproc_env, ffiproc_i); 
      if (ffiproc_arg_nb < atleast_arg_nb) { 
	this -> error_ipc0 = ipc0; 
	this -> error_bytecode = bytecode; 
	this -> error_ffi_arg_nb = ffiproc_arg_nb; 
	this -> error_ffi_atleast_arg_nb = atleast_arg_nb; 
	next_ipc0 = PSCRIPT_VM__ERROR__FFI_NOT_ENOUGH_ARG; 
	break; 
      }; 
#endif 
      
      
      // RL: Here, we should pay attention to 'this' object which might not be a proxy, but could be fully on the stack. 
#if 0 
      dputs(stderr_d, "STACK NB  : "); dputn(stderr_d, this -> pscript_env -> stack_env -> stack__nb); dput_eol(stderr_d);
#endif 
      const int nb_objects_stacked = pscript_vm__step__call_ffi(this, ffiproc_i, on_stack_arg_nb, ret_address); 
#if 0 
      dputs(stderr_d, "STACK NB  : "); dputn(stderr_d, this -> pscript_env -> stack_env -> stack__nb); dput_eol(stderr_d);
      dputs(stderr_d, "nb_objects_stacked  : "); dputn(stderr_d, nb_objects_stacked); dput_eol(stderr_d);
#endif 
      if (nb_objects_stacked < 0) { 
	this -> error_ipc0     =     ipc0; 
	this -> error_bytecode = bytecode; 
	next_ipc0 = nb_objects_stacked; 
	break; 
      }; 
      
      // RL: We have to stack one, because, at compile-time, types are not computed. 
      if (0 == nb_objects_stacked) { 
	const int retval_type = pscript_ffiproc__get_retval_type(this -> pscript_env -> ffiproc_env, ffiproc_i); 
	pscript_stack__push_uninitialized(this -> pscript_env -> stack_env, retval_type);  
      }; 
      
#if 0 
      dputs(stderr_d, "STACK NB  : "); dputn(stderr_d, this -> pscript_env -> stack_env -> stack__nb); dput_eol(stderr_d);
#endif 
      
      next_ipc0 = ret_address; 
      break; 
    }; // RL: PSCRIPT_METHODE_TYPE__FFIPROC == method_type 
    
    
    assert(false); 
  } while (false); break; 
    
    


  case PVM_BYTECODE__CALL_FFI: do { // @ is in the bytecode  
      const int on_stack_arg_nb = *(int8_t  *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int ffiproc_i       = *(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 2); 
      const int ret_address     = ipc0 + 4; 
      const int alright_huh = pscript_vm__step__call_ffi(this, ffiproc_i, on_stack_arg_nb, ret_address); 
      if (alright_huh >= 0) { 
	next_ipc0 = ret_address; 
      } 
      else { 
	this -> error_ipc0     =     ipc0; 
	this -> error_bytecode = bytecode; 
	next_ipc0 = alright_huh; 
      }; 
    } while (false); 
    break; 
    
    
    
  case PVM_BYTECODE__CALL_LIB: do { // @ is in the bytecode  
#if 0 
      dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 

      const int on_stack_arg_nb = BYTECODE_READ_ARG(int8_t); //*(int8_t  *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int libproc_i       = BYTECODE_READ_ARG(int16_t); //*(int16_t *) (bytecode_env -> bytecode_array + ipc0 + 2); 
      const int ret_address     = next_ipc0; //ipc0 + 4; 
      //const int base_mem_offset = pscript_mem__get_base_offset(this -> pscript_env -> mem_env); 
      //pscript_vm__call_stack__push(this, ret_address, arg_nb, base_mem_offset); 
      const int caller_ebp = pscript_vm__memory_frame__get(this); 
      pscript_vm__call_stack__push(this, ret_address, on_stack_arg_nb, caller_ebp); 
      
      const int atleast_arg_nb = pscript_libproc__get_arg_atleast(this -> pscript_env -> libproc_env, libproc_i); 
      if (atleast_arg_nb >= 0) { 
	if (on_stack_arg_nb < atleast_arg_nb) { 
	  this -> error_ipc0 = ipc0; 
	  this -> error_bytecode = bytecode; 
	  this -> error_ffi_arg_nb = on_stack_arg_nb; 
	  this -> error_ffi_atleast_arg_nb = atleast_arg_nb; 
	  next_ipc0 = PSCRIPT_VM__ERROR__LIB_NOT_ENOUGH_ARG; 
	  break; 
	}; 
      }; 
      
      const int expected_arg_nb = pscript_libproc__get_arg_nb(this -> pscript_env -> libproc_env, libproc_i); 
      if (expected_arg_nb >= 0) { 
	if (on_stack_arg_nb != expected_arg_nb) { 
	  this -> error_ipc0 = ipc0; 
	  this -> error_bytecode = bytecode; 
	  this -> error_ffi_arg_nb = on_stack_arg_nb; 
	  this -> error_ffi_atleast_arg_nb = atleast_arg_nb; 
	  next_ipc0 = PSCRIPT_VM__ERROR__LIB_NOT_ENOUGH_ARG; 
	  break; 
	}; 
      }; 
      
      // LIB callback 
      const int allright = pscript_libproc__call(this -> pscript_env -> libproc_env, libproc_i, on_stack_arg_nb); 
      
#if 0 
      dputs_array(stderr__fileno, "--> ipc0 = ", int_string__stack(ipc0), "\n"); 
      dputs_array(stderr__fileno, "STACK_NB = ", int_string__stack(this -> pscript_env -> stack_env -> stack__nb), "\n"); 
      dputs(stderr_d, "STACK_TYPE(<-): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK_MEM (->): "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
      dputs_array(stderr__fileno, "bytecode = (", int_string__stack(bytecode),")", pvm_bytecode__cstr(bytecode)," \n"); 
#endif 
      
      this -> call_stack__nb --; 
      
      next_ipc0 = ret_address; 
    } while (false); 
    break; 
    
    
    
    
    
    
  case PVM_BYTECODE__ARRAY_GET_ELT0: do { // POSTFIX: array @ stack[0] 
    STACK__UNARY_OPERATION_PREAMBLE__NO_RETVAL; 
    
#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
#endif 
    
    //const int type1_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
    int array_type; 
    int array_mem_offset; 
    const char * array_value; 
    if (PSCRIPT_TYPE__ARRAY == type1_core) { 
      array_type = type1; 
      array_value = value1; 
      array_mem_offset = -1; 
    } 
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      array_type = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
      array_mem_offset = *(const int_pscript_mem_t *)value1; 
      array_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, array_mem_offset); 
    } 
    else if (PSCRIPT_TYPE__POINTER == type1_core) { 
      array_type = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
      array_mem_offset = *(const int_pscript_mem_t *)value1; 
      array_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, array_mem_offset); 
    } 
    else { 
      pmesserr("%s: " "We need an array on the left part of a subscript operation - We got an object of type [ %d ] %s." "\n", __func__, type1, pscript_type__get_cstr(this -> pscript_env -> type_env, type1)); 
      assert(false); 
      array_type = -1; 
      array_mem_offset = -1; 
    }; 
    const int array_type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, array_type); 
    assert(PSCRIPT_TYPE__ARRAY == array_type_core); 
    
    const int array_subtype_i = pscript_type__array__get_subtype(this -> pscript_env -> type_env, array_type); 
    const int i_min           = pscript_type__array__get_i_min  (this -> pscript_env -> type_env, array_type); 
    const int i_max           = pscript_type__array__get_i_max  (this -> pscript_env -> type_env, array_type); 
    int subscript_value = i_min; 
    
    const int array_subtype_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, array_subtype_i); 
    const int array_subtype_offset = (subscript_value - i_min) * array_subtype_sizeof; 
    const char * element_value = array_value + array_subtype_offset; 
    const int subscript_mem_offset = array_mem_offset + array_subtype_offset; 
    
    STACK__RETVAL(MAX(sizeof(subscript_mem_offset),(size_t)array_subtype_sizeof)); 
    if (PSCRIPT_TYPE__ARRAY == type1_core) { 
      retval_type = array_subtype_i; 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy(element_value, retval_value, retval_sizeof); 
    } 
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      retval_type = pscript_type__make__proxy(this -> pscript_env -> type_env, array_subtype_i); 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy((const char *) &subscript_mem_offset, retval_value, retval_sizeof); 
    } 
    else if (PSCRIPT_TYPE__POINTER == type1_core) { 
      retval_type = pscript_type__make__pointer(this -> pscript_env -> type_env, array_subtype_i); 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy((const char *) &subscript_mem_offset, retval_value, retval_sizeof); 
    } 
    else { 
      assert(false); 
    }; 
    
    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
      } while (false); break; 
    
  case PVM_BYTECODE__ARRAY_GET_ELT_I: do { // POSTFIX: array @ stack[-1], index @ stack[0] 
    STACK__BINARY_OPERATION_PREAMBLE__NO_RETVAL; 
    
#if 0 
    dputs(stderr_d, "VALUE1: "); pscript_value__print__raw_bytes(type1_sizeof, value1); dputs(stderr_d, "\n"); 
    dputs(stderr_d, "VALUE2: "); pscript_value__print__raw_bytes(type2_sizeof, value2); dputs(stderr_d, "\n"); 
#endif 
    
    const int type1_core = pscript_type__get_core_type(this -> pscript_env -> type_env, type1); 
    int array_type; 
    int array_mem_offset; 
    const char * array_value; 
    if (PSCRIPT_TYPE__ARRAY == type1_core) { 
      array_type = type1; 
      array_value = value1; 
      array_mem_offset = -1; 
    } 
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      array_type = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, type1); 
      array_mem_offset = *(const int_pscript_mem_t *)value1; 
      array_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, array_mem_offset); 
    } 
    else if (PSCRIPT_TYPE__POINTER == type1_core) { 
      array_type = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, type1); 
      array_mem_offset = *(const int_pscript_mem_t *)value1; 
      array_value = pscript_mem__get_pointer(this -> pscript_env -> mem_env, array_mem_offset); 
    } 
    else { 
      pmesserr("%s: " "We need an array on the left part of a subscript operation - We got an object of type [ %d ] %s." "\n", __func__, type1, pscript_type__get_cstr(this -> pscript_env -> type_env, type1)); 
      assert(false); 
      array_type = -1; 
      array_mem_offset = -1; 
    }; 
    const int array_type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, array_type); 
    assert(PSCRIPT_TYPE__ARRAY == array_type_core); 
    
    const int array_subtype_i = pscript_type__array__get_subtype(this -> pscript_env -> type_env, array_type); 
    const int i_min           = pscript_type__array__get_i_min  (this -> pscript_env -> type_env, array_type); 
    const int i_max           = pscript_type__array__get_i_max  (this -> pscript_env -> type_env, array_type); 
    int subscript_value = *(int *) value2; 
    if (subscript_value < i_min) { 
      pmesserr("%s: " "The subscript value %d is lower than the lower bound of the array (%d) - Therefore flooring it to %d." "\n", __func__, subscript_value, i_min, i_min); 
      subscript_value = i_min; 
    }; 
    if (subscript_value > i_max) { 
      pmesserr("%s: " "The subscript value %d is greater than the upper bound of the array (%d) - Therefore ceiling it to %d." "\n", __func__, subscript_value, i_max, i_max); 
      subscript_value = i_max; 
    }; 
    
    const int array_subtype_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, array_subtype_i); 
    const int array_subtype_offset = (subscript_value - i_min) * array_subtype_sizeof; 
    const char * element_value = array_value + array_subtype_offset; 
    const int subscript_mem_offset = array_mem_offset + array_subtype_offset; 

    STACK__RETVAL(MAX(sizeof(subscript_mem_offset),(size_t)array_subtype_sizeof)); 
    if (PSCRIPT_TYPE__ARRAY == type1_core) { 
      retval_type = array_subtype_i; 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy(element_value, retval_value, retval_sizeof); 
    } 
    else if (PSCRIPT_TYPE__PROXY == type1_core) { 
      retval_type = pscript_type__make__proxy(this -> pscript_env -> type_env, array_subtype_i); 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy((const char *) &subscript_mem_offset, retval_value, retval_sizeof); 
    } 
    else if (PSCRIPT_TYPE__POINTER == type1_core) { 
      retval_type = pscript_type__make__pointer(this -> pscript_env -> type_env, array_subtype_i); 
      retval_sizeof = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
      bcopy((const char *) &subscript_mem_offset, retval_value, retval_sizeof); 
    } 
    else { 
      assert(false); 
    }; 
    
    STACK__OPERATION__POSTAMBLE;
    //next_ipc0 = ipc0 + 1; 
      } while (false); break; 
    


  case PVM_BYTECODE__MENU: do { 
      const int8_t menu_i = bytecode_env -> bytecode_array[ipc0 + 1]; 
      // If the call is async, then the returned value will be known later on, 
      // and will be pushed by the external program (using the function 'menu__async_reverse_callback__push_return_value'); 
      const int choice = pscript__menu2(this -> pscript_env, menu_i); 
      if (not(this -> pscript_env -> menu_async_huh)) { 
	pscript_stack__push_value(this -> pscript_env -> stack_env, pscript_type__get__integer(this -> pscript_env -> type_env), sizeof(choice), (const char *) &choice); 
      }; 
      next_ipc0 = ipc0 + 2; 
    } while (false); break; 
 




  case PVM_BYTECODE__CALL_LAMBDA: do { 
      // stack[0]: lambda object or reference to a lambda object 
      const int8_t on_stack_arg_nb = BYTECODE_READ_ARG(int8_t); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int   ret_address = next_ipc0; 
#if 0 
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      const int    top_object_type_i    = pscript_stack__get_top__type(   this -> pscript_env -> stack_env); 
      const char * top_object_value     = pscript_stack__get_top__pointer(this -> pscript_env -> stack_env); 

      next_ipc0 = pscript_vm__step__call_lambda(this, top_object_type_i, top_object_value, on_stack_arg_nb, ret_address); 

    } while (false); break; 
    
    
  case PVM_BYTECODE__MAKE_LAMBDA: do { 
      // RL: CALL-MEMORY-FRAME 
      //     _________________ 
      //      - retval 
      //      - arg 
      //      - localvar 
      //      - stack of references to lexical env [ pointing to reference counters ] 
      //      - frame-size 
      //      - reference counter 
      //      - [EBP points here] 
      // 
      // RL: The target is to make a lambda object. 
      //     So we need to traverse the call-memory-frame, increment the reference counters, and then load it onto the expression-stack. 
      const int    lex_env_stack__local_offset = BYTECODE_READ_ARG(int);    //*(int    *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int8_t lambda_type_i               = BYTECODE_READ_ARG(int8_t); //*(int8_t *) (bytecode_env -> bytecode_array + ipc0 + 5); 
      const int8_t nesting_level               = BYTECODE_READ_ARG(int8_t); //*(int8_t *) (bytecode_env -> bytecode_array + ipc0 + 6); 
      const int    lambda_bytecode_offset      = BYTECODE_READ_ARG(int);    //*(int    *) (bytecode_env -> bytecode_array + ipc0 + 7); 
      //const int base_offset = pscript_vm__call_stack__get_mem_base_offset(this); 
      const int base_offset = pscript_vm__memory_frame__get(this); 
      const int lex_env_stack__offset = base_offset + lex_env_stack__local_offset; 
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
      // 
      //     That way, from the reference counter, we can look up for the NULL address to get to the top. Not sure that gonna be useful though. 
      // RL: Object that gonna be pushed onto the expression-stack. 
      const int lambda_sizeof = sizeof(int) + sizeof(char) + nesting_level * sizeof(int); 
      char      lambda_value[lambda_sizeof]; 
      *(int    *) (lambda_value + 0) = lambda_bytecode_offset; 
      *(int8_t *) (lambda_value + 4) = nesting_level; 
      if (nesting_level <= 0) { 
	// RL: INC reference counters 
	char * mem_pointer = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, 0); 
	*((int *) (mem_pointer)) += 1; 
      }
      else { 
	char * mem_pointer = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, lex_env_stack__offset); 
	const char lex_env_stack__nb = *(const int8_t *) (mem_pointer); 
	const char lex_env_stack__nb__expected = nesting_level + 1; 
	if (lex_env_stack__nb__expected != lex_env_stack__nb) { 
	  this -> error_ipc0 = ipc0; 
	  this -> error_bytecode = bytecode; 
	  this -> error__expected_frame_size = lex_env_stack__nb__expected; 
	  this -> error__actual_frame_size = lex_env_stack__nb; 
	  next_ipc0 = PSCRIPT_VM__ERROR__NESTING_AND_FRAME_SIZES_MISMATCHED; 
	  break; 
	}; 
	// RL: INC reference counters 
	for (int i = 0; i < lex_env_stack__nb; i++) { 
	  *(((int *) (mem_pointer + 1)) + i) += 1; 
	}; 
	bcopy(mem_pointer + 1 + sizeof(int), lambda_value + 5, (lex_env_stack__nb - 1) * sizeof(int)); 
      }; 
      pscript_stack__push_value(this -> pscript_env -> stack_env, lambda_type_i, lambda_sizeof, lambda_value); 
      //next_ipc0 = ipc0 + 3 + 2*sizeof(int); 
    } while (false); break; 
    
    
    
      // RL: CALL-MEMORY-FRAME 
      //     _________________ 
      //      - retval 
      //      - arg 
      //      - localvar 
      //      - stack of references to lexical env [ pointing to reference counters ] 
      //      - frame-size 
      //      - reference counter 
      //      - [EBP points here] 
  case PVM_BYTECODE__MEMORY_FRAME__ALLOC_AND_INC: do { 
      // bytecode[0]: frame_size 
      const int frame_size = BYTECODE_READ_ARG(int); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      const int frame_ebp  = pscript_mem__memory_frame__allocate_frame(this -> pscript_env -> mem_env, frame_size); 
      pscript_vm__memory_frame__push_and_set(this, frame_ebp); 
    } while (false); break; 

  case PVM_BYTECODE__MEMORY_FRAME__LEX_ENV__STATIC_INIT: do { 
      const int frame_ebp                 = pscript_vm__memory_frame__get(this); 
      const int frame0_ref_counter_offset = pscript_mem__memory_frame__get_frame0_ref_counter_offset(this -> pscript_env -> mem_env); 
      char *    mem_pointer               = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, frame_ebp); 
      *(char *) (mem_pointer - sizeof(char) - sizeof(int) - sizeof(int) - sizeof(int) - sizeof(int)) = 2; // RL: Size of the array 
      *(int  *) (mem_pointer                - sizeof(int) - sizeof(int) - sizeof(int) - sizeof(int)) = frame0_ref_counter_offset; // RL: refcounter0 
      *(int  *) (mem_pointer                              - sizeof(int) - sizeof(int) - sizeof(int)) = frame_ebp - sizeof(int); // RL: refcounter[n-1] 
      //next_ipc0 = ipc0 + 1; 
      //pscript_mem__memory_frame__print(stderr__fileno, this -> pscript_env -> mem_env); 
      pscript_vm__memory_frame__recursive_refcounter_inc(this -> pscript_env -> mem_env, frame_ebp); 
    } while (false); break; 
    
  case PVM_BYTECODE__MEMORY_FRAME__LEX_ENV__LAMBDA_INIT: do { 
      // stack[0]: lambda object or ref to a lambda object 
#if 0 
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
      const int    top_object_type_i    = pscript_stack__get_top__type(   this -> pscript_env -> stack_env); 
      const char * top_object_value     = pscript_stack__get_top__pointer(this -> pscript_env -> stack_env);     
      const int    top_object_type_core = pscript_type__get_core_type(this -> pscript_env -> type_env, top_object_type_i); 
      const char * lambda_object_ptr = NULL; 
      if (PSCRIPT_TYPE__LAMBDA == top_object_type_core) { 
	lambda_object_ptr = top_object_value; 
      } 
      else { 
	int subtype_i; 
	int mem_offset; 
	if (PSCRIPT_TYPE__POINTER == top_object_type_core) { 
	  subtype_i  = pscript_type__pointer__get_subtype(this -> pscript_env -> type_env, top_object_type_i); 
	  mem_offset = *(const int_pscript_mem_t *)(top_object_value); 
	}
	else if (PSCRIPT_TYPE__PROXY == top_object_type_core) { 
	  subtype_i  = pscript_type__proxy__get_subtype(this -> pscript_env -> type_env, top_object_type_i); 
	  mem_offset = *(const int_pscript_mem_t *)(top_object_value); 
	} 
	else if (PSCRIPT_TYPE__VARIANT == top_object_type_core) { 
	  subtype_i  = *(const int_pscript_type_t *) (top_object_value); 
	  mem_offset = *(const int_pscript_mem_t *)(top_object_value + sizeof(int_pscript_type_t)); 
	} 
	else { 
	  subtype_i  = -1; 
	  mem_offset = -1; 
	  assert(false); 
	}; 
	const int subtype_core = pscript_type__get_core_type(this -> pscript_env -> type_env, subtype_i); 
	//assert(PSCRIPT_TYPE__LAMBDA == top_object_type_core); 
	assert(PSCRIPT_TYPE__LAMBDA == subtype_core); 
	//const int mem_offset = *(const int *)(top_object_value); 
	lambda_object_ptr = pscript_mem__get_pointer(this -> pscript_env -> mem_env, mem_offset); 
      }; 
      
      const char  lex_env_nb  = *(lambda_object_ptr + sizeof(int)); 
      const int * lex_env_ptr = (const int *)(lambda_object_ptr + sizeof(int) + sizeof(char)); 
      
      const int frame_ebp                 = pscript_vm__memory_frame__get(this); 
      { 
	const int frame0_ref_counter_offset = pscript_mem__memory_frame__get_frame0_ref_counter_offset(this -> pscript_env -> mem_env); 
	char *    mem_pointer               = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, frame_ebp); 
	const int offset                    = (4 + lex_env_nb)*sizeof(int); 
	int *     lex_env_ptr               = (int *)(mem_pointer - offset); 
	*(char *) (mem_pointer - sizeof(char) - offset                   ) = 2 + lex_env_nb; // RL: Size of the array 
	*(int  *) (mem_pointer                - offset                   ) = frame0_ref_counter_offset; // RL: refcounter[0]
	*(int  *) (mem_pointer - sizeof(int)  - sizeof(int) - sizeof(int)) = frame_ebp - sizeof(int);  // RL: refcounter[n-1]
	bcopy(lex_env_ptr, mem_pointer - offset + sizeof(int), lex_env_nb*sizeof(int)); 
      };
      
      pscript_vm__memory_frame__recursive_refcounter_inc(this -> pscript_env -> mem_env, frame_ebp); 
    } while (false); break; 
    
  case PVM_BYTECODE__MEMORY_FRAME__DEC_AND_CONDITIONALLY_DEALLOC: do { 
      // bytecode[0]: retval_type 
      const int8_t retval_type = BYTECODE_READ_ARG(int8_t); //*(int *) (bytecode_env -> bytecode_array + ipc0 + 1); 
      // RL: CALL-MEMORY-FRAME 
      //     _________________ 
      //      - retval 
      //      - arg 
      //      - localvar 
      //      - stack of references to lexical env [ pointing to reference counters ] 
      //      - frame-size 
      //      - reference counter 
      //      - [EBP points here] 
      
      const int frame_ebp = pscript_vm__memory_frame__get(this); 
      // RL: First, pushing 'retval' onto the stack. 
      // RL: That thing is managed here because we do not know EBP at compile-time, and we want the 'retval' to stay in place as much as possible. 
      { 
	const char * mem_pointer            = pscript_mem__get_pointer(this -> pscript_env -> mem_env, frame_ebp); 
	const int    frame_size             = *((const int *)(mem_pointer - sizeof(int) - sizeof(int))); 
	const int    retval_offset          = - frame_size; //pscript_funproc__get_retval_offset(this -> funproc_env, funproc_i); 
	const int    retval_abs             = frame_ebp - frame_size; 
	const int    retval_type_scalar_huh = pscript_type__scalar_huh(this -> pscript_env -> type_env, retval_type); 
	const int    retval_type_sizeof     = pscript_type__sizeof(this -> pscript_env -> type_env, retval_type); 
	const char * retval_value           = (mem_pointer - frame_size); 
	
	if (0 == retval_type_sizeof) { 
	  // RL: Do not push anything. 
	} 
	else if (retval_type_scalar_huh) { 
	  pscript_stack__push_value(this -> pscript_env -> stack_env, retval_type, retval_type_sizeof, retval_value); 
	} 
	else { 
	  const int subtype           = retval_type; 
	  const int lea_type_i        = pscript_type__make__proxy(this -> pscript_env -> type_env, subtype); 
	  const int caller_on_top_huh = pscript_vm__memory_frame__is_caller_on_top_huh(this); 
	  if (caller_on_top_huh) { 
	    //pscript_stack__push_value(this -> pscript_env -> stack_env, lea_type_i, sizeof(retval_abs), (const char *)&retval_abs); 
	    pscript_stack__push_value(this -> pscript_env -> stack_env, lea_type_i, sizeof(int_pscript_mem_t), (const char *)&retval_abs); 
	    char * mem_pointer = pscript_mem__get_pointer_writable(this -> pscript_env -> mem_env, frame_ebp); 
	    *(int *)(mem_pointer - sizeof(int) - sizeof(int)) -= retval_type_sizeof; // RL: Deallocating 'retval' from the current memory-frame. 
	    //pscript_vm__call_stack__caller_ebp__allocate_retval(this, retval_type_sizeof); // RL: Allocating 'retval' to the caller memory-frame. 
	  } 
	  else { 
	    const int local_heap__mem_offset = pscript_mem__stack_heap__allocate(this -> pscript_env -> mem_env, frame_ebp, retval_type_sizeof); 
	    pscript_mem__copy(this -> pscript_env -> mem_env, retval_abs, local_heap__mem_offset, retval_type_sizeof); 
	    //pscript_stack__push_value(this -> pscript_env -> stack_env, lea_type_i, sizeof(local_heap__mem_offset), (const char *) &local_heap__mem_offset); 
	    pscript_stack__push_value(this -> pscript_env -> stack_env, lea_type_i, sizeof(int_pscript_mem_t), (const char *) &local_heap__mem_offset); 
	  }; 
	}; 
      }; 
      
      // RL: Second, decrementing the references-counter. 
      if (pscript_mem__memory_frame__references_counter_dec(this -> pscript_env -> mem_env, frame_ebp) > 0) { break; }; 
      
      // RL: Third, if there is any more references to this call-memory-frame, 
      //     then, from now onward, this frame will be considered deallocated for all intents and purposes. 
      //     As such, we have to decrease all the pointers that it was referencing to. 
      pscript_vm__memory_frame__recursive_refcounter_dec(this -> pscript_env -> mem_env, frame_ebp); 
      
      // RL: Fourth, updating the stack heap. (Expensive.) 
      // RL: This is expensive as the whole structure is traversed. 
      //     Because, the problem is that we can't know for sure that this memory space is ordered. 
      //     That's the point of its existence: things are allocated here when we can't allocate them anywhere else. 
      //     So this function should be called as little as possible. 
      // RL: (An optimization would be to implement it with an avl-like tree structure over it.) 
      // RL: However, this should not be an issue unless lambda-objects are intensively used. 
      pscript_mem__stack_heap__update(this -> pscript_env -> mem_env); 
      
      // RL: Fifth, garbage-collecting the memory-frame. (Cheap.) 
      pscript_mem__memory_frame__gc(this -> pscript_env -> mem_env); 
      
      // RL: Sixth, garbage-collecting the stack-heap. (Cheap.) 
      pscript_mem__stack_heap__gc(this -> pscript_env -> mem_env); 
      
    } while (false); break; 
    
    
    
  case PVM_BYTECODE__VMCALL:  
  case PVM_BYTECODE__SYSCALL:  
  case PVM_BYTECODE__CALL_PTR:  // @ is on the stack. 
  case PVM_BYTECODE__CALL_CLIENT:  
  case PVM_BYTECODE__CALL_CLIENT_METHOD:  
  case PVM_BYTECODE__ENSURE_TYPE:  // ARG_NB is in the bytecode 
  case PVM_BYTECODE__MEM_STORE:  // @ is in the bytecode:  [0] is value (the size must be taken into account) 
  case PVM_BYTECODE__BIT_NOT:  
  case PVM_BYTECODE__BIT_AND:  
  case PVM_BYTECODE__BIT_OR:  
  case PVM_BYTECODE__BIT_XOR:  
  case PVM_BYTECODE__BOOL_NOT:  
  case PVM_BYTECODE__BOOL_AND:  
  case PVM_BYTECODE__BOOL_OR:  
  case PVM_BYTECODE__BOOL_XOR:  
  case PVM_BYTECODE__INF_HUH:  
  case PVM_BYTECODE__IFF: 
  case PVM_BYTECODE__ZERO:  
  case PVM_BYTECODE__INC1:  
  case PVM_BYTECODE__DEC1:  
  case PVM_BYTECODE__EUCLIDIAN: 
  case PVM_BYTECODE__RDIV:  
  case PVM_BYTECODE__IDIV:  
  case PVM_BYTECODE__IMOD: 
    this -> error_ipc0 = ipc0; 
    this -> error_bytecode = bytecode; 
    next_ipc0 = PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED; 
    break;  

  }; 
  
  return next_ipc0; 
}; 




int pscript_vm__run(pscript_vm_t * this, const int this_ipc0) {
  int next_ipc0; 
  
  next_ipc0 = this_ipc0; 
  do { 
    next_ipc0 = pscript_vm__step(this, next_ipc0); 
  } while (0 <= next_ipc0); 
  
  return next_ipc0; 
}; 




static const char * pscript_vm__error__cstr_array[] = { 
 "HALT", 
 "ABORT", 
 "BYTEADDRESS_OUT_OF_RANGE", 
 "UNKNOWN_BYTECODE", 
 "NULL_INSTRUCTION", 
 "INSTRUCTION_NOT_YET_IMPLEMENTED", 
 "DIVISION_BY_ZERO", 
 "STACK_OVERFLOW", 
 "STACK_OUT_OF_RANGE", 
 "MEM_OVERFLOW", 
 "MEM_OUT_OF_RANGE", 
 "BOOL_EXPECTED", 
 "TYPE_MISMATCHED", 
 "FFI_NOT_ENOUGH_ARG", 
 "LIB_NOT_ENOUGH_ARG", 
 "OBJECT_DOES_NOT_HAVE_THAT_METHOD", 
 "ARG_NB", 
 "NESTING_AND_FRAME_SIZES_MISMATCHED", 
 NULL
}; 
 
enum { pscript_vm__error__cstr_size = ARRAY_SIZE(pscript_vm__error__cstr_array) }; 

static void assert_compile_time__ds3d632sad4ss(void) {
  ASSERT_COMPILE(pscript_vm__error__cstr_size == PSCRIPT_VM__ERROR__COUNT + 1); 
}; 
 


const char * pscript_vm__error__get_cstr(const int retcode) {
  const int array_index = retcode - PSCRIPT_VM__ERROR__COUNT_BASE - 1; 
  return pscript_vm__error__cstr_array[array_index]; 
}; 


void pscript_vm__print_error(const pscript_vm_t * this, const int retcode) {
  if (retcode == PSCRIPT_VM__ERROR__HALT) { return; }; 
  
  const char * retcode_cstr = pscript_vm__error__get_cstr(retcode); 
  const char * bytecode_str = bytecode_type__get_cstr(this -> error_bytecode); 
  switch (retcode) { 
  default: do { 
      pmesserr("VM Error [ Bytecode address: %d - Bytecode: (%d)%s ] : (%d)%s " "\n", this -> error_ipc0, this -> error_bytecode, bytecode_str, retcode, retcode_cstr); 
#if 1 
      dputs(stderr_d, "STACK TYPE: "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env -> stack__nb * sizeof(*(this -> pscript_env -> stack_env -> stack__type)), this -> pscript_env -> stack_env -> stack__type); dputs(stderr_d, "\n"); 
      dputs(stderr_d, "STACK MEM : "); pscript_value__print__raw_bytes(this -> pscript_env -> stack_env ->   mem__nb * sizeof(*(this -> pscript_env -> stack_env ->  mem__array)), pscript_stack__get_top__pointer(this -> pscript_env -> stack_env)); dputs(stderr_d, "\n"); 
#endif 
    } while (false); break; 
  case PSCRIPT_VM__ERROR__OBJECT_DOES_NOT_HAVE_THAT_METHOD: do { 
      const char * method_ident_cstr = pscript_string__get_cstr(this -> pscript_env -> string_env, this -> error_method_ident_pstr); 
      const char * object_type_cstr = pscript_type__get_cstr(this -> pscript_env -> type_env, this -> error_object_type); 
      pmesserr("VM Error [ Bytecode address: %d - Bytecode: (%d)%s ] : (%d)%s: '%s' - Object type: '%s' " "\n", this -> error_ipc0, this -> error_bytecode, bytecode_str, retcode, retcode_cstr, method_ident_cstr, object_type_cstr); 
    } while (false); break; 
  case PSCRIPT_VM__ERROR__ARG_NB: do { 
      const int expected_arg_nb = this -> error__expected_arg_nb; 
      const int on_stack_arg_nb = this -> error__on_stack_arg_nb; 
      pmesserr("VM Error [ Bytecode address: %d - Bytecode: (%d)%s ] : (%d)%s - Expected_arg_nb: %d - On_stack_arg_nb: %d " "\n", this -> error_ipc0, this -> error_bytecode, bytecode_str, retcode, retcode_cstr, expected_arg_nb, on_stack_arg_nb); 
    } while (false); break; 
  }; 
  
  // RL: TODO XXX FIXME: Print the call-stack. 
  
}; 



























#if 0 
  switch (next_ipc0) {
  default: pmesserr("Something got wrong (next bytecode address: %d)" "\n", next_ipc0); break; 
  case PSCRIPT_VM__ERROR__HALT: return true; break;  
  case PSCRIPT_VM__ERROR__ABORT: 
}; break;  
  case PSCRIPT_VM__ERROR__BYTEADDRESS_OUT_OF_RANGE: break;  
  case PSCRIPT_VM__ERROR__UNKNOWN_BYTECODE: break;  
  case PSCRIPT_VM__ERROR__NULL_INSTRUCTION: break;  
  case PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED: break;  
  case PSCRIPT_VM__ERROR__DIVISION_BY_ZERO: break;  
  case PSCRIPT_VM__ERROR__STACK_OVERFLOW: break;  
  case PSCRIPT_VM__ERROR__STACK_OUT_OF_RANGE: break;  
  case PSCRIPT_VM__ERROR__MEM_OVERFLOW: break;  
  case PSCRIPT_VM__ERROR__MEM_OUT_OF_RANGE: break;  

  case -2: error_str = cstr2str("The end of the execution inflow was reached whereas it was still running."); break; 
  case -3: error_str = cstr2str("Unknown bytecode encountered."); break; 
  case -4: error_str = cstr2str("NULL instruction encountered."); break; 
  case -5: error_str = cstr2str("Program requested aborting."); break; 
  case -6: {
    const char * bytecode_str = bytecode_type__get_cstr(error_state -> code2); 
    error_str = string__concat__carray(((const char * []) { "Instruction ", bytecode_str, " not yet implemented." } ));
    break;
  }; 
  case -8: error_str = cstr2str("Division by zero."); break; 
  }; 
  error_state -> str1 = error_str; 
  
  return (-1 == next_ipc0); 
#endif 


