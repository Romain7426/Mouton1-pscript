#ifndef PSCRIPT_VM_H
#define PSCRIPT_VM_H 

#ifndef PSCRIPT_VM_T 
#define PSCRIPT_VM_T 
struct pscript_vm_t; 
typedef struct pscript_vm_t pscript_vm_t; 
#endif 

enum { 
 PSCRIPT_VM__ERROR__COUNT_BASE = INT8_MIN, 
 PSCRIPT_VM__ERROR__HALT, 
 PSCRIPT_VM__ERROR__ABORT, 
 PSCRIPT_VM__ERROR__BYTEADDRESS_OUT_OF_RANGE, 
 PSCRIPT_VM__ERROR__UNKNOWN_BYTECODE, 
 PSCRIPT_VM__ERROR__NULL_INSTRUCTION, 
 PSCRIPT_VM__ERROR__INSTRUCTION_NOT_YET_IMPLEMENTED, 
 PSCRIPT_VM__ERROR__DIVISION_BY_ZERO, 
 PSCRIPT_VM__ERROR__STACK_OVERFLOW, 
 PSCRIPT_VM__ERROR__STACK_OUT_OF_RANGE, 
 PSCRIPT_VM__ERROR__MEM_OVERFLOW, 
 PSCRIPT_VM__ERROR__MEM_OUT_OF_RANGE, 
 PSCRIPT_VM__ERROR__BOOL_EXPECTED, 
 PSCRIPT_VM__ERROR__TYPE_MISMATCHED, 
 PSCRIPT_VM__ERROR__FFI_NOT_ENOUGH_ARG, 
 PSCRIPT_VM__ERROR__LIB_NOT_ENOUGH_ARG, 
 PSCRIPT_VM__ERROR__OBJECT_DOES_NOT_HAVE_THAT_METHOD, 
 PSCRIPT_VM__ERROR__ARG_NB, 
 PSCRIPT_VM__ERROR__NESTING_AND_FRAME_SIZES_MISMATCHED, 
 PSCRIPT_VM__ERROR__COUNT_SHIFTED 
}; 
enum { PSCRIPT_VM__ERROR__COUNT = PSCRIPT_VM__ERROR__COUNT_SHIFTED - PSCRIPT_VM__ERROR__COUNT_BASE - 1 }; 

enum { pscript_vm__call_stack__size = (1 << 9) }; 
enum { pscript_vm__memory_frame__size = pscript_vm__call_stack__size }; 
struct pscript_vm_t {
  pscript_t * pscript_env; 
  
  int error_ipc0; 
  int error_bytecode; 
  int error_value_type; 
  int error_core_type__got; 
  int error_core_type__expected;   
  int error_ffi_arg_nb; 
  int error_ffi_atleast_arg_nb; 
  int error_method_ident_pstr; 
  int error_object_type; 
  int error__expected_arg_nb; 
  int error__on_stack_arg_nb; 
  int error__expected_frame_size; 
  int error__actual_frame_size; 


  int  call_stack__ret[pscript_vm__call_stack__size]; 
  char call_stack__arg_nb[pscript_vm__call_stack__size]; 
  //int  call_stack__mem_base_offset[pscript_vm__call_stack__size]; 
  int  call_stack__caller_ebp[pscript_vm__call_stack__size]; 
  int  call_stack__nb; 
  
  int memory_frame__ebp[pscript_vm__memory_frame__size]; 
  int memory_frame__nb; 
  int memory_frame__active; // RL: mem_offset - not an index in the ebp array 

  pscript_vm_t * (* make)(pscript_t * pscript_env); 
  void (* delete)(pscript_vm_t * this); 
  int (* run)(pscript_vm_t * this, const int this_ipc0); 
  int (* step)(pscript_vm_t * this, const int this_ipc0); 
  void (* print_error)(const pscript_vm_t * this, const int retcode); 
}; 

extern pscript_vm_t * pscript_vm__make(pscript_t * pscript_env); 
extern void pscript_vm__delete(pscript_vm_t * this); 

extern int pscript_vm__run(pscript_vm_t * this, const int this_ipc0); 
extern int pscript_vm__step(pscript_vm_t * this, const int ipc0); 

extern void pscript_vm__print_error(const pscript_vm_t * this, const int retcode); 
extern const char * pscript_vm__error__get_cstr(const int retcode);
#define pscript_vm__error__cstr pscript_vm__error__get_cstr

// RL: 2020-03-02: Needed by LIBPROC. 
extern int pscript_vm__call_stack__push(pscript_vm_t * this, const int ret_address, const char on_stack_arg_nb, const int caller_ebp); 
extern int pscript_vm__memory_frame__get(const pscript_vm_t * this); 



#endif /* PSCRIPT_VM_H */ 

