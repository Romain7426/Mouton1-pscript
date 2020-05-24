#ifndef PSCRIPT_MEM_H
#define PSCRIPT_MEM_H

#ifndef PSCRIPT_MEM_ENV_T 
#define PSCRIPT_MEM_ENV_T 
struct pscript_mem_env_t; 
typedef struct pscript_mem_env_t pscript_mem_env_t; 
#endif 
extern  const int32_t              pscript_mem_env__sizeof; 

// RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that a mem will fit into a 'int_pscript_mem_t'. 
typedef       int16_t            int_pscript_mem_t; 
extern  const int_pscript_mem_t  int_pscript_mem_max; 

enum { 
  PSCRIPT_MEM__TOPLEVEL_COMPILETIME, 
  PSCRIPT_MEM__TOPLEVEL_RUNTIME, 
  PSCRIPT_MEM__MEMORY_FRAME, 
  PSCRIPT_MEM__STACK_HEAP, 
  PSCRIPT_MEM__TURING_HEAP, 
  PSCRIPT_MEM__COUNT 
}; 

extern pscript_mem_env_t * pscript_mem_env__make(pscript_type_env_t * type_env); 
extern void pscript_mem_env__delete(pscript_mem_env_t * this);  

#if 0 
extern int pscript_mem__allocate(pscript_mem_env_t * this, const int type_sizeof);  
#define pscript_mem__alloc pscript_mem__allocate

extern void pscript_mem__deallocate(pscript_mem_env_t * this, const int mem_size); 
//extern void pscript_mem__deallocate_abs(pscript_mem_env_t * this, const int mem_offset); 
//extern void pscript_mem__deallocate_frame(pscript_mem_env_t * this, const int ebp, const int frame_size); 

extern int pscript_mem__get_base_offset(const pscript_mem_env_t * this); 
#endif 

extern const int8_t * pscript_mem__get_pointer(const pscript_mem_env_t * this, const int mem_offset); 
extern       int8_t * pscript_mem__get_pointer_writable(pscript_mem_env_t * this, const int mem_offset); 

extern void pscript_mem__write(pscript_mem_env_t * this, const int offset, const int value_sizeof, const char value[]); 
extern void pscript_mem__read(const pscript_mem_env_t * this, const int offset, const int value_sizeof, char value[]); 
extern void pscript_mem__copy(pscript_mem_env_t * this, const int src_offset, const int dest_offset, const int bytesize); // The function is safe if mem-spaces overlap. 


extern int pscript_mem__to_which_memory_part_that_variable_belongs(const pscript_mem_env_t * this, const int mem_offset); 
extern int pscript_mem__to_which_memory_frame_that_variable_belongs(const pscript_mem_env_t * this, const int mem_offset); 


extern pscript_mem_env_t * pscript_mem_env__make_r(pscript_mem_env_t * this, pscript_type_env_t * type_env); 
extern pscript_mem_env_t * pscript_mem_env__make(pscript_type_env_t * type_env); 
extern void pscript_mem_env__delete_r(pscript_mem_env_t * this); 
extern void pscript_mem_env__delete(pscript_mem_env_t * this); 


// RL: EXPRESSION STACK 
// RL: If the object is on the stack, then we move it to the memory. (A method acts on an object by 
//     side-effects. And, in the currently memory model, the stack is not part of the memory, therefore 
//     it cannot referenced [no variable ever points to the stack].) 
// RL: On the long run, referencing the stack should not be a problem: on the stack could be any data, but their lifetime is an expression. And they can't be referenced before or after the expression. So doing method calls on on-stack objects should be doable. 


extern int  pscript_mem__runtime__allocate_same_memory_frame(pscript_mem_env_t * this, const int mem_offset, const int type_sizeof); 
extern int  pscript_mem__dispose(pscript_mem_env_t * this, const int mem_offset, const int object_sizeof); 


extern int  pscript_mem__toplevel_compiletime__bss__allocate  (pscript_mem_env_t * this, const int type_sizeof); 
extern void pscript_mem__toplevel_compiletime__bss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated); 
extern int  pscript_mem__toplevel_compiletime__bss__used(const pscript_mem_env_t * this); 

// RL: ZSS is necessary for variant type, as it must be initialized. 
extern int  pscript_mem__toplevel_compiletime__zss__allocate  (pscript_mem_env_t * this, const int type_sizeof); 
extern void pscript_mem__toplevel_compiletime__zss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated); 
extern int  pscript_mem__toplevel_compiletime__zss__used(const pscript_mem_env_t * this); 
 
// RL: So far, ISS is un-necessary. 
extern int  pscript_mem__toplevel_compiletime__iss__allocate  (pscript_mem_env_t * this, const int type_sizeof); 
extern void pscript_mem__toplevel_compiletime__iss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated); 
extern int  pscript_mem__toplevel_compiletime__iss__used(const pscript_mem_env_t * this); 
 
extern int  pscript_mem__toplevel_runtime__allocate  (pscript_mem_env_t * this, const int type_sizeof); 
extern void pscript_mem__toplevel_runtime__deallocate(pscript_mem_env_t * this, const int type_sizeof); 
extern int  pscript_mem__toplevel_runtime__used(const pscript_mem_env_t * this); 
 
extern int  pscript_mem__memory_frame__allocate_frame  (      pscript_mem_env_t * this, const int frame_size); 
extern int  pscript_mem__memory_frame__allocate_object__force(    pscript_mem_env_t * this, const int frame_ebp , const int object_size); 
extern int  pscript_mem__memory_frame__allocate_object(    pscript_mem_env_t * this, const int frame_ebp , const int object_size); 
extern int  pscript_mem__memory_frame__deallocate_frame(      pscript_mem_env_t * this, const int frame_ebp ); 
extern int  pscript_mem__memory_frame__gc        (      pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__count     (const pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__used      (const pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__belongs_to(const pscript_mem_env_t * this, const int mem_offset); 
extern int  pscript_mem__memory_frame__get_ebp   (const pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__get_frame0_ebp(const pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__get_frame0_ref_counter_offset(const pscript_mem_env_t * this); 
extern int  pscript_mem__memory_frame__references_counter_get(const pscript_mem_env_t * this, const int frame_ebp); 
extern int  pscript_mem__memory_frame__references_counter_inc(pscript_mem_env_t * this, const int frame_ebp); 
extern int  pscript_mem__memory_frame__references_counter_dec(pscript_mem_env_t * this, const int frame_ebp); 
extern int  pscript_mem__memory_frame__get_frame_size      (const pscript_mem_env_t * this, const int frame_ebp); 
extern int  pscript_mem__memory_frame__get_frame_offset      (const pscript_mem_env_t * this, const int frame_ebp); 
#define pscript_mem__memory_frame__get_references_count pscript_mem__memory_frame__references_counter_get
extern void pscript_mem__memory_frame__print(const int fd, const pscript_mem_env_t * this); 
 
extern int  pscript_mem__stack_heap__allocate  (      pscript_mem_env_t * this, const int memory_frame_ref, const int type_sizeof); 
extern void pscript_mem__stack_heap__deallocate(      pscript_mem_env_t * this, const int disposed_memory_frame_ref); 
extern void pscript_mem__stack_heap__gc        (      pscript_mem_env_t * this); 
extern int  pscript_mem__stack_heap__count     (const pscript_mem_env_t * this); 
extern int  pscript_mem__stack_heap__used      (const pscript_mem_env_t * this); 
extern int  pscript_mem__stack_heap__belongs_to(const pscript_mem_env_t * this, const int mem_offset); 
extern void pscript_mem__stack_heap__update(          pscript_mem_env_t * this); 
 
extern int  pscript_mem__turing_heap__allocate  (  pscript_mem_env_t * this, const int type_sizeof); 
extern int  pscript_mem__turing_heap__deallocate(  pscript_mem_env_t * this, const int  mem_offset); 
extern int  pscript_mem__turing_heap__count (const pscript_mem_env_t * this); 
extern int  pscript_mem__turing_heap__used  (const pscript_mem_env_t * this); 
extern int  pscript_mem__turing_heap__height(const pscript_mem_env_t * this); 




#endif /* PSCRIPT_MEM_H */
