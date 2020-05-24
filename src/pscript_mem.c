#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_type.h" 
#include "pscript_mem.h" 

enum { INT_PSCRIPT_MEM_MAX = sizeof(int_pscript_mem_t) == 1 ? INT8_MAX : sizeof(int_pscript_mem_t) == 2 ? INT16_MAX : sizeof(int_pscript_mem_t) == 4 ? INT32_MAX : sizeof(int_pscript_mem_t) == 8 ? INT64_MAX : -1 }; 
const int_pscript_mem_t int_pscript_mem_max = INT_PSCRIPT_MEM_MAX; 

#define MEM_SIZE_BITSIZE 15 
#define TURING_BITSIZE 11 
enum { pscript__mem__size                            = (1LL << MEM_SIZE_BITSIZE) }; 
enum { pscript__mem__toplevel_compiletime__bss__size = (1LL <<               14) }; 
enum { pscript__mem__toplevel_compiletime__zss__size = (1LL <<               10) }; 
enum { pscript__mem__toplevel_compiletime__iss__size = (1LL <<               10) }; 
enum { pscript__mem__toplevel_runtime__size          = (1LL <<               10) }; 
enum { pscript__mem__memory_frame__size              = (1LL <<               13) }; 
enum { pscript__mem__stack_heap__size                = (1LL <<               11) }; 
enum { pscript__mem__turing_heap__size               = (1LL <<   TURING_BITSIZE) }; 
enum { pscript__mem__gap__size                       = pscript__mem__size - pscript__mem__toplevel_compiletime__bss__size - pscript__mem__toplevel_compiletime__zss__size - pscript__mem__toplevel_compiletime__iss__size - pscript__mem__toplevel_runtime__size - pscript__mem__memory_frame__size - pscript__mem__stack_heap__size - pscript__mem__turing_heap__size }; 
static void assert_compile_time__ds3d632sad4ss(void) { ASSERT_COMPILE(pscript__mem__toplevel_compiletime__bss__size >= (1 << 14)); }; 
static void assert_compile_time__ds3kkj9632sss(void) { ASSERT_COMPILE(pscript__mem__size - 1 <= INT_PSCRIPT_MEM_MAX); }; 
 

struct pscript_mem_env_t { 
  // RL: Might be useful in the future. I am thinking about switching from typing the variables to typing the memory. So far so good. So we'll see. 
  //     The point would be able to allow not to be typed when declared, but typed when first assigned. This is just late memory assignement. 
  //     We can't have a full untyped variable, because it's just does not make sense - we're a side-effect language, not a let-like language, so there's no semantic for that (it's impossible to describe a semantic that would make sense). 
  //     But for the purpore of delaying the typing of a variable, we just need to declare it as variant without subtype. As simple as that. 
  //     So might be useful, but likely it won't be. 
  pscript_type_env_t * type_env; 
  
  // --- WHOLE MEMORY --- 
  // RL: The full memory, with all the sub-memory spaces. 
  //     The idea is that, unlike C, every single part of the used memory is referenced by a variable. If no variable referenced this part of the memory, then it is not used. 
  //     And then a referenced part of a memory is a making-sense entity - it's not a bunch of random data. It's typed (the type is in the variable). 
  char mem__array[pscript__mem__size]; 
  
  // --- ORGANIZATION --- 
  // RL: The memory is subdivised in two major parts, and five minor parts: 
  //      A. Toplevel_Compiletime memory 
  //          1. The static part for global/top-level variables. 
  //          2. The stack  part for local variables. 
  //         The boundary between those two is not kept track, as we do not care: the static part is allocated at compile-time, and is like the level zero of the local stack memory. So it's basically the same thing. And then the local variables are stacked on top of these static variables. 
  //         NB 1: The expression stack is not here. This memory is for variables. For computing expression, please see 'pscript_stack_env_t'. 
  //         NB 2: The execution stack is not here (stack with return addresses). It's in the vm - cf. the 'call_stack' object. 
  //         NB 3: We will see that this division of the stack matters as we may want to keep the local variables allocated while keeping on computing (therefore using the expression-stack and the call-stack). We will see that this is very useful for a reverse-callback where a stack-context-switch is needed, and then we just need to make a local lambda. 
  //      B. Heap memory 
  //          3. The static part of the heap. 
  //          4. The stack  part of the heap. 
  //          5. The turing part of the heap. 
  //         NB 4: The heap is the part of the memory for objects whose lifetimes are perfectly synchronous with the execution, and therefore cannot be in the toplevel_compiletime part of the memory. 
  //         NB 5: The heap is divided into three sections depending on their uses. 
  //               Because, the main point of the heap is its management. While the management of the toplevel_compiletime memory is automatic, with minimal overhead, this is not the case of the heap where the dynamic objects have to be tracked. 
  //         NB 6: The static section of the heap is the one dedicated to objects having a program lifetime. The use-case is that, for any reason, we do not want to statically allocate a top-level variable, but we want to defer that allocation at run-time. For instance, because the size could dynamic, or allocated on first use, and so forth. As such, once allocated, these objects are not meant to be deallocated. They will be alive during the whole program, and will be deallocated on exit. As such, the management of this part is very easy: there's none. 
  //         NB 7: The stack section of the heap is the one dedicated to objects having a local lifetime. The scopes of such objects are known, and nothing wild gonna happen. They are under control. The use-case is that, for some reasons, they cannot be allocated on the stack (the toplevel_compiletime-memory stack); usually, it's because there's already another frame onto the frame where it should be allocated (because the allocation takes place in a sub-nested function, and the call to this function stacked its frame). As simple as that. In that case, we do not need a full-blown memory management - we just need to keep track to which stack frame it's related, and when that stack-frame is popped, so it is that heap-frame. 
  //               The drawback is that it is not a perferctly linear stack - it's a messy stack, where things could be not perfectly ordered: something belonging to an inner scope could have been allocated before something belonging to an outter scope. That's the point of that stack - would it have been perfectly linear, it would have been useless, and everything would have been allocated on the toplevel_compiletime stack. In this case, we just need to deallocate everything whose stack-frame is over or already over. 
  //               So objects are not individually followed in this section - scopes are. 
  //         NB 8: The turing section of the heap is the one dedicated to wild objects, whose lifetimes cannot be a-priori computed unless solving the Turing-machine halting problem. It is not constrained within a scope, and the object can travel nobody knows where. 
  //               That being said, if that's true for an un-typed language like Scheme and in which code and data are the same thing, it's less true in typed & structured languages: they have a bottom scope. 
  //               One use-case that I could not do but doing a malloc: simulating a delimited continuation for a reverse-callbak. I had basically two concurrent execution stacks, and I had to transfer data from one stack to the other one (the reverse-callback) - but I did not have nor fibers, nor coroutines, nor continuations. So I had to somehow save the current stack state until the call of the reverse-callback. To achieve that, I had to do a malloc, and saving that context into that newly allocated memory. It was kinda hugly because nothing made sense. And, true, there was a bottom scope, but it was even making sense to allocate that in that bottom scope, while the size of that context was not known a-priori. What could be done is, instead of a turing-malloc, doing a scope-malloc whose scope would have been that bottom scope. So it's not a perfect use-case of a turing-allocation. But, while coding, it is definitely is. Because, while writing code, we need first a solution to achieve something, so that the program can work - and only then one can think about doing something better. 
  //               So the use-case is the one above described: «I am writing code and I need a solution now.». 
  //               (I am still looking for a use-case which would not fit into the stack section of the heap. But cf. what being said of typed & structured languages.) 
  //               This case is the one needing a full-blown memory management, in which each object is individually followed. In C, the user has to keep manually manage its memory. In Java, OCaml, and other let-like & double-referenced languages, they implement garbage collection (which is something still obscure & unclear to me). In Python, they implement reference-counting mething. For some definitely unclear reasons, GC has been praised, and reference-counting disdained. In my experience, Java is bloated and stopped in the middle of nowhere because it's gc'ing... On the other hand, Python works quite well. And complexity considered, reference-counting is way simpler that garbage collection. Here, we implement reference-counting. Especially that the memory is not a bunch of random data, but structured & typed data, with all of them being referenced by a variable. So reference-counting definitely makes sense here. More than that, as the program is designed, the reference counter should be no more than one (the pointer type is here mostly empty, and assignement are by copy) (there could be cases where the counter is greater than one - to be thought about). 
  
  
  // --- TOPLEVEL_COMPILETIME --- 
  // RL: Toplevel_Compiletime memory management. 
  //     That variable indicates how much memory has been allocated so far. 
  //     Unlike a stack, this memory grows up. 
  //     And that's it for the toplevel_compiletime memory. 
  int toplevel_compiletime__bss__nb; // RL: Uninitialized 
  int toplevel_compiletime__zss__nb; // RL: Zero-initialized  
  int toplevel_compiletime__iss__nb; // RL: Initialized  
  
  // --- TOPLEVEL_RUNTIME --- 
  // RL: Idem. 
  // RL: This memory is to dynamically allocate memory which should not be deallocated. 
  int toplevel_runtime__nb; 
  
  // --- MEMORY_FRAME --- 
  // RL: Idem. 
  // RL: This memory is to store local variables & lexical environments. 
  // RL: With the use of lambda-objects, the dellocation of these objects might be deferred (need to access & to keep alive the nesting local environments). 
  // RL: The usage of each frame is kept using a references-counter. 
  int memory_frame__nb; 
  // RL: We need a 'top_ebp' variable as, after local allocations, we can't figure any structure in that memory. 
  //int memory_frame__top_ebp; 
  // RL: There's a second stack for EBPs which grows down from the top, so that we can still figure structures in that memory. 
  int memory_frame__ebp__nb; 
  
  // --- STACK_HEAP --- 
  // RL: Idem. 
  // RL: A memory frame could not have any room left to allocate local data 
  //     as another memory frame could have been stacked onto it (a lambda object was created, and the lexical env must be kept alive). 
  //     This is the purpose of that memory: to allocate local objects that should have been allocated into the memory frame but can't. 
  // RL: For instance, 'TPoint3D_make' should allocate the object on the memory frame, but if it can't, then it's allocated here. 
  // RL: Obviously, the deallocation may be deferred. 
  // RL: The usage of each object is linked to its parent memory frame. As long as its parent memory 
  //     frame is allocated, it should be allocated; when its parent memory frame is deallocated, then 
  //     it should be deallocated as well. 
  int stack_heap__nb; 
  
  // --- TURING_HEAP --- 
  // RL: This is the standard 'malloc' memory. 
  // RL: We do not use the weird method of linked list. 
  //     Because it's weird, and it's linear (with early stopping though). 
  //     We implement a sparse tree. It looks nicer. And it's logarithmic (if the tree is well-balanced). 
  //     The drawback of that method is the size of the largest block that could be allocated: it's halved at each allocation (roughly). That's why a more efficient implementation would first divide this memory space into parts: (i) small-sized allocations, (ii) medium-sized allocations, (iii) large-sized allocations. Well, let's keep it simple for the moment (and it won't be that simple, unfortunately). 
  //     - 
  //     That space contains allocated data which we gonna called «chunks». 
  //     These chunks are related to each others as a binary-tree. 
  //     Each chunks have meta-data related to the node that it is: 
  //      * size of the chunk (all including): int 
  //      * reference-counter ("nb_var_referencing"): int8_t 
  //      * height of the chunk (aka its number of descendents): char/int8_t/byte // Needed to balance the tree. 
  //      * size of the largest available free chunk on the left: int 
  //      * offset to that left descendent: int 
  //      * size of the largest available free chunk on the right: int 
  //      * offset to that right descendent: int 
  //      * TOTAL (to be updated): 5*sizeof(int) + sizeof(int16_t) + 2*sizeof(int8_t)  [ thus 164 bytes on a 32-int - so a lot of overhead for small allocations ] 
  //                                                                                   [ that could be reduced to int_log2(heap_size)_t ] 
  //     So here are the global data: 
  //      * root_offset: int // needed 
  //      * chunk_nb: int // not necessary but convenient 
  //      * largest_available_size: int // not necessary but convenient 
  //      * total_available: int // not necessary but convenient 
  int turing_heap__root_offset;       // RL: needed 
  int turing_heap__largest_available; // RL: half necessary in order to have continuity & consistency in algorithms - and convenient as well. 
  int turing_heap__chunk_nb;          // RL: not necessary but convenient 
  int turing_heap__total_available;   // RL: not necessary but convenient 
}; 



// ********* STUFFS ********* 
 
#if   MEM_SIZE_BITSIZE <  8 
#error TODO 
#elif MEM_SIZE_BITSIZE < 16 
typedef int16_t pscript_mem__offset_int_t; 
#elif MEM_SIZE_BITSIZE < 24 
#error TODO 
#elif MEM_SIZE_BITSIZE < 32 
typedef int32_t pscript_mem__offset_int_t; 
//#elif MEM_SIZE_BITSIZE < 48 
//#error TODO 
#elif MEM_SIZE_BITSIZE < 64 
typedef int64_t pscript_mem__offset_int_t; 
#else 
#error MEM_SIZE_BITSIZE too large. 
#endif 

typedef int8_t pscript_mem__refcounter_int_t; 

enum { pscript__mem__toplevel_compiletime__bss__base = 0 }; 
enum { pscript__mem__toplevel_compiletime__bss__top  = pscript__mem__toplevel_compiletime__bss__base + pscript__mem__toplevel_compiletime__bss__size }; 
enum { pscript__mem__toplevel_compiletime__zss__base = pscript__mem__toplevel_compiletime__bss__top }; 
enum { pscript__mem__toplevel_compiletime__zss__top  = pscript__mem__toplevel_compiletime__zss__base + pscript__mem__toplevel_compiletime__zss__size }; 
enum { pscript__mem__toplevel_compiletime__iss__base = pscript__mem__toplevel_compiletime__zss__top }; 
enum { pscript__mem__toplevel_compiletime__iss__top  = pscript__mem__toplevel_compiletime__iss__base + pscript__mem__toplevel_compiletime__iss__size }; 
enum { pscript__mem__toplevel_runtime__base          = pscript__mem__toplevel_compiletime__iss__top }; 
enum { pscript__mem__toplevel_runtime__top           = pscript__mem__toplevel_runtime__base     + pscript__mem__toplevel_runtime__size     }; 
enum { pscript__mem__memory_frame__base              = pscript__mem__toplevel_runtime__top     }; 
enum { pscript__mem__memory_frame__top               = pscript__mem__memory_frame__base         + pscript__mem__memory_frame__size         }; 
enum { pscript__mem__stack_heap__base                = pscript__mem__memory_frame__top }; 
enum { pscript__mem__stack_heap__top                 = pscript__mem__stack_heap__base           + pscript__mem__stack_heap__size           }; 
enum { pscript__mem__turing_heap__base               = pscript__mem__stack_heap__top  }; 
enum { pscript__mem__turing_heap__top                = pscript__mem__turing_heap__base          + pscript__mem__turing_heap__size          }; 
enum { pscript__mem__gap__base               = pscript__mem__turing_heap__top  }; 
enum { pscript__mem__gap__top                = pscript__mem__gap__base          + pscript__mem__gap__size          }; 

static void assert_compile_time__dd63t4ad4ss(void) { ASSERT_COMPILE(pscript__mem__gap__top == pscript__mem__size); }; 

int pscript_mem__to_which_memory_part_that_variable_belongs(const pscript_mem_env_t * this, const int mem_offset) { 
  if (mem_offset < pscript__mem__toplevel_compiletime__bss__base) { return -1; }; 
  if (mem_offset < pscript__mem__toplevel_compiletime__bss__top)  { return PSCRIPT_MEM__TOPLEVEL_COMPILETIME; }; 
  if (mem_offset < pscript__mem__toplevel_compiletime__zss__top)  { return PSCRIPT_MEM__TOPLEVEL_COMPILETIME; }; 
  if (mem_offset < pscript__mem__toplevel_compiletime__iss__top)  { return PSCRIPT_MEM__TOPLEVEL_COMPILETIME; }; 
  if (mem_offset < pscript__mem__toplevel_runtime__top)      { return PSCRIPT_MEM__TOPLEVEL_RUNTIME; }; 
  if (mem_offset < pscript__mem__memory_frame__top)          { return PSCRIPT_MEM__MEMORY_FRAME; }; 
  if (mem_offset < pscript__mem__stack_heap__top)            { return PSCRIPT_MEM__STACK_HEAP;  }; 
  if (mem_offset < pscript__mem__turing_heap__top)           { return PSCRIPT_MEM__TURING_HEAP; }; 
  return -1; 
}; 

 
int pscript_mem__to_which_memory_frame_that_variable_belongs(const pscript_mem_env_t * this, const int mem_offset) { 
  const int mem_part = pscript_mem__to_which_memory_part_that_variable_belongs(this, mem_offset); 
  if (mem_part < 0) { return mem_part; }; 
  switch (mem_part) { 
  default: return ~mem_part; break; 
  case PSCRIPT_MEM__TOPLEVEL_COMPILETIME: return 0; break; 
  case PSCRIPT_MEM__TOPLEVEL_RUNTIME:     return 0; break; 
  case PSCRIPT_MEM__MEMORY_FRAME:         return pscript_mem__memory_frame__belongs_to(this, mem_offset); break; 
  case PSCRIPT_MEM__STACK_HEAP:           return /*RL: Returns the memory_frame_ref.*/pscript_mem__stack_heap__belongs_to(this, mem_offset); break; 
  case PSCRIPT_MEM__TURING_HEAP:          return /*RL: Does not make sense.*/-1; break; 
  }; 
  assert(false); 
}; 
 
 
 
// ********* MEM_ENV ********* 

static void pscript_mem__turing_heap__init(pscript_mem_env_t * this); 
static void pscript_mem__memory_frame__init(pscript_mem_env_t * this); 

pscript_mem_env_t * pscript_mem_env__make_r(pscript_mem_env_t * this, pscript_type_env_t * type_env) { 
  BZERO_THIS(this); 
  this -> type_env = type_env; 
  
#if 1 
  { 
    // RL: Address zero is not available. 
    *((int *) (this -> mem__array)) = 0; 
    this -> toplevel_compiletime__bss__nb++; 
#if 0
    // RL: Bottom frame is not actually allocated and used as a reference counter to the global memory frame. 
    *((int *) (this -> mem__array + pscript__mem__memory_frame__base                  )) = pscript__mem__memory_frame__base; // RL: former ebp  
    *((int *) (this -> mem__array + pscript__mem__memory_frame__base +     sizeof(int))) = 2 * sizeof(int); // RL: frame size 
    *((int *) (this -> mem__array + pscript__mem__memory_frame__base + 2 * sizeof(int))) = 1; // RL: Ref counter (should never be deallocated) 
    this -> memory_frame__nb += 3 * sizeof(int); 
    this -> memory_frame__top_ebp = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
#endif 
  } 
#elif 0 
  { 
    //(int *) (this -> mem__array) = 0; // RL: Refused by the compiler. So two steps. 
    int * null_ptr = (int *)this -> mem__array; 
    *null_ptr = 0; 
    this -> toplevel_compiletime__nb += sizeof(int); 
  } 
#else 
  this -> mem__array[0] = '\0'; 
  this -> toplevel_compiletime__nb ++; 
#endif 
  
  pscript_mem__memory_frame__init(this); 
  pscript_mem__turing_heap__init(this); 
  
  return this; 
}; 

pscript_mem_env_t * pscript_mem_env__make(pscript_type_env_t * type_env) { 
  MALLOC_THIS(pscript_mem_env_t,this); 
  return pscript_mem_env__make_r(this, type_env); 
}; 

void pscript_mem_env__delete_r(pscript_mem_env_t * this) { 
  // NA 
}; 

void pscript_mem_env__delete(pscript_mem_env_t * this) { 
  pscript_mem_env__delete_r(this); 
  free(this);
}; 


// ********* COMMON OPERATORS ********* 

#if 0 
int pscript_mem__get_base_offset(const pscript_mem_env_t * this) { 
  // This is the EBP pointer. 
  return this -> toplevel_compiletime__nb; 
}; 
#endif 
 
const int8_t * pscript_mem__get_pointer(const pscript_mem_env_t * this, const int mem_offset) { 
  assert(0 < mem_offset); 
  assert(pscript__mem__size > mem_offset);   
  return this -> mem__array + mem_offset; 
}; 

int8_t * pscript_mem__get_pointer_writable(pscript_mem_env_t * this, const int mem_offset) { 
  return this -> mem__array + mem_offset; 
}; 
 
void pscript_mem__write(pscript_mem_env_t * this, const int offset, const int value_sizeof, const char value[]) { 
  //assert((size_t)offset + value_sizeof < sizeof(this -> mem__array)); 
  //assert(offset > 0 && offset + value_sizeof <= pscript__mem__toplevel_compiletime__size); 
  assert(offset > 0 && offset + value_sizeof <= pscript__mem__size); 
  bcopy(value, this -> mem__array + offset, value_sizeof); 
}; 
 
void pscript_mem__read(const pscript_mem_env_t * this, const int offset, const int value_sizeof, char value[]) { 
  assert(offset > 0); 
  //assert((size_t)offset + value_sizeof < sizeof(this -> mem__array)); 
  //assert(offset + value_sizeof <= pscript__mem__toplevel_compiletime__size); 
  assert(offset + value_sizeof <= pscript__mem__size); 
  bcopy(this -> mem__array + offset, value, value_sizeof); 
}; 
  
void pscript_mem__copy(pscript_mem_env_t * this, const int src_offset, const int dest_offset, const int bytesize) { 
  assert( src_offset > 0); 
  assert(dest_offset > 0); 
  assert( src_offset + bytesize <= pscript__mem__size); 
  assert(dest_offset + bytesize <= pscript__mem__size); 
  bcopy(this -> mem__array + src_offset, this -> mem__array + dest_offset, bytesize); 
}; 
 
 
// ********* TOPLEVEL_COMPILETIME_BSS ********* 

int pscript_mem__toplevel_compiletime__bss__allocate(pscript_mem_env_t * this, const int type_sizeof) { 
  //assert((size_t)this -> toplevel_compiletime__nb + type_sizeof < sizeof(this -> mem__array)); 
  assert(this -> toplevel_compiletime__bss__nb + type_sizeof <= pscript__mem__toplevel_compiletime__bss__size); 
  const int mem_offset = pscript__mem__toplevel_compiletime__bss__base + this -> toplevel_compiletime__bss__nb; 
  this -> toplevel_compiletime__bss__nb += type_sizeof; 
  return mem_offset; 
}; 
 
void pscript_mem__toplevel_compiletime__bss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated) { 
  // RL: Should not be called. Anyways. 
  assert(size_to_be_deallocated < this -> toplevel_compiletime__bss__nb); 
  this -> toplevel_compiletime__bss__nb -= size_to_be_deallocated; 
}; 

int pscript_mem__toplevel_compiletime__bss__used(const pscript_mem_env_t * this) { 
  return this -> toplevel_compiletime__bss__nb; 
}; 

 


 

// ********* TOPLEVEL_COMPILETIME_ZSS ********* 

int pscript_mem__toplevel_compiletime__zss__allocate(pscript_mem_env_t * this, const int type_sizeof) { 
  //assert((size_t)this -> toplevel_compiletime__nb + type_sizeof < sizeof(this -> mem__array)); 
  assert(this -> toplevel_compiletime__zss__nb + type_sizeof <= pscript__mem__toplevel_compiletime__zss__size); 
  const int mem_offset = pscript__mem__toplevel_compiletime__zss__base + this -> toplevel_compiletime__zss__nb; 
  this -> toplevel_compiletime__zss__nb += type_sizeof; 
  return mem_offset; 
}; 
 
void pscript_mem__toplevel_compiletime__zss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated) { 
  // RL: Should not be called. Anyways. 
  assert(size_to_be_deallocated < this -> toplevel_compiletime__zss__nb); 
  this -> toplevel_compiletime__zss__nb -= size_to_be_deallocated; 
}; 

int pscript_mem__toplevel_compiletime__zss__used(const pscript_mem_env_t * this) { 
  return this -> toplevel_compiletime__zss__nb; 
}; 

 


 

// ********* TOPLEVEL_COMPILETIME_ISS ********* 

int pscript_mem__toplevel_compiletime__iss__allocate(pscript_mem_env_t * this, const int type_sizeof) { 
  //assert((size_t)this -> toplevel_compiletime__nb + type_sizeof < sizeof(this -> mem__array)); 
  assert(this -> toplevel_compiletime__iss__nb + type_sizeof <= pscript__mem__toplevel_compiletime__iss__size); 
  const int mem_offset = pscript__mem__toplevel_compiletime__iss__base + this -> toplevel_compiletime__iss__nb; 
  this -> toplevel_compiletime__iss__nb += type_sizeof; 
  return mem_offset; 
}; 
 
void pscript_mem__toplevel_compiletime__iss__deallocate(pscript_mem_env_t * this, const int size_to_be_deallocated) { 
  // RL: Should not be called. Anyways. 
  assert(size_to_be_deallocated < this -> toplevel_compiletime__iss__nb); 
  this -> toplevel_compiletime__iss__nb -= size_to_be_deallocated; 
}; 

int pscript_mem__toplevel_compiletime__iss__used(const pscript_mem_env_t * this) { 
  return this -> toplevel_compiletime__iss__nb; 
}; 

 


 

// ********* TOPLEVEL_RUNTIME ********* 

int pscript_mem__toplevel_runtime__allocate(pscript_mem_env_t * this, const int type_sizeof) { 
  assert(this -> toplevel_runtime__nb + type_sizeof <= pscript__mem__toplevel_runtime__size); 
  const int mem_offset = pscript__mem__toplevel_runtime__base + this -> toplevel_runtime__nb; 
  this -> toplevel_runtime__nb += type_sizeof; 
  return mem_offset; 
}; 
 
int pscript_mem__toplevel_runtime__dispose(pscript_mem_env_t * this, const int mem_offset, const int object_sizeof) { 
  assert(object_sizeof <= this -> toplevel_runtime__nb); 
  if (mem_offset + object_sizeof != pscript__mem__toplevel_runtime__base + this -> toplevel_runtime__nb) { 
    return -1; 
  }; 
  this -> toplevel_runtime__nb -= object_sizeof; 
  return object_sizeof; 
}; 
 
void pscript_mem__toplevel_runtime__deallocate(pscript_mem_env_t * this, const int type_sizeof) { 
  // RL: Should not be called. Anyways. 
  assert(type_sizeof <= this -> toplevel_runtime__nb); 
  this -> toplevel_runtime__nb -= type_sizeof;  
}; 

int pscript_mem__toplevel_runtime__used(const pscript_mem_env_t * this) { 
  return this -> toplevel_runtime__nb; 
}; 
 
 
// ********* MEMORY_FRAME ********* 

      // RL: CALL-MEMORY-FRAME 
      //     _________________ 
      //      - retval 
      //      - arg 
      //      - localvar 
      //      - stack of references to lexical env [ pointing to reference counters ] 
      //      - frame-size 
      //      - reference counter 
      //      - [EBP points here] 

void pscript_mem__memory_frame__init(pscript_mem_env_t * this) { 
  // RL: Bottom frame is not actually allocated and used as a reference counter to the global memory frame. 
#if 1 
  *((int *) (this -> mem__array + pscript__mem__memory_frame__base              )) = 2 * sizeof(int); // RL: frame size 
  *((int *) (this -> mem__array + pscript__mem__memory_frame__base + sizeof(int))) = 1; // RL: Ref counter (this bottom frame should never be deallocated) 
  this -> memory_frame__nb += 2 * sizeof(int); 
  //this -> memory_frame__top_ebp = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
  //this -> memory_frame__ebp__nb = pscript__mem__memory_frame__top - sizeof(pscript_mem__offset_int_t); 
  this -> memory_frame__ebp__nb = sizeof(pscript_mem__offset_int_t); 
  *((pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)) = pscript__mem__memory_frame__base + this -> memory_frame__nb; // RL: current ebp  
#else 
  *((int *) (this -> mem__array + pscript__mem__memory_frame__base                  )) = pscript__mem__memory_frame__base; // RL: former ebp  
  *((int *) (this -> mem__array + pscript__mem__memory_frame__base +     sizeof(int))) = 2 * sizeof(int); // RL: frame size 
  *((int *) (this -> mem__array + pscript__mem__memory_frame__base + 2 * sizeof(int))) = 1; // RL: Ref counter (should never be deallocated) 
  this -> memory_frame__nb += 3 * sizeof(int); 
  this -> memory_frame__top_ebp = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
#endif 
}; 

int pscript_mem__memory_frame__allocate_frame(pscript_mem_env_t * this, const int frame_size) { 
  assert(frame_size >= 2*(int)sizeof(int)); 
  assert(this -> memory_frame__nb + frame_size <= pscript__mem__memory_frame__size - this -> memory_frame__ebp__nb - (int)sizeof(pscript_mem__offset_int_t)); 
  char * p; 
  const int frame_offset = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
  this -> memory_frame__nb += frame_size; 
  const int frame_ebp    = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
  p = this -> mem__array + frame_ebp; 
  *(int *) (p - 2*sizeof(int)) = frame_size; // Ref: frame size 
  *(int *) (p -   sizeof(int)) = 1; // RL: Ref counter 
  
  //this -> memory_frame__top_ebp = frame_ebp; 
  this -> memory_frame__ebp__nb += sizeof(pscript_mem__offset_int_t); 
  *((pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)) = frame_ebp; // RL: current ebp  
  
  return frame_ebp; 
}; 

int pscript_mem__memory_frame__allocate_object__force(pscript_mem_env_t * this, const int frame_ebp, const int object_size) { 
  const int top_frame_ebp = *((const pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)); 
  assert(top_frame_ebp == frame_ebp); 
  assert(this -> memory_frame__nb + object_size <= pscript__mem__memory_frame__size - this -> memory_frame__ebp__nb); 
  const int mem_offset = pscript__mem__memory_frame__base + this -> memory_frame__nb; 
  this -> memory_frame__nb += object_size; 
  return mem_offset; 
}; 

int pscript_mem__memory_frame__allocate_object(pscript_mem_env_t * this, const int frame_ebp , const int object_size) {  
  const int top_frame_ebp = *((const pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)); 
  if (top_frame_ebp == frame_ebp) { 
    return pscript_mem__memory_frame__allocate_object__force(this, frame_ebp, object_size); 
  }; 
  return pscript_mem__stack_heap__allocate(this, frame_ebp, object_size); 
}; 

int pscript_mem__memory_frame__dispose_object(pscript_mem_env_t * this, const int frame_ebp , const int mem_offset, const int object_sizeof) {  
  const int top_frame_ebp = *((const pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)); 
  if (top_frame_ebp != frame_ebp) { 
    return -1; 
  }; 
  if (mem_offset + object_sizeof != pscript__mem__memory_frame__base + this -> memory_frame__nb) { 
    return -2; 
  }; 
  this -> memory_frame__nb -= object_sizeof; 
  return object_sizeof; 
}; 



// RL: EBP is an absolute offset as it's used to determine absolute 
//     address in memory of local variables. 
int pscript_mem__memory_frame__get_ebp(const pscript_mem_env_t * this) { 
  //return this -> memory_frame__top_ebp; 
  return *((const pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - this -> memory_frame__ebp__nb)); 
}; 

int pscript_mem__memory_frame__used(const pscript_mem_env_t * this) { 
  return this -> memory_frame__nb + this -> memory_frame__ebp__nb; 
}; 
 
int pscript_mem__memory_frame__count(const pscript_mem_env_t * this) { 
#if 1 
  int count = 0; 
  const char * top = this -> mem__array + pscript__mem__memory_frame__top; 
  const char * p   = top - this -> memory_frame__ebp__nb; 
  while (p < top) { p += sizeof(pscript_mem__offset_int_t); count++; }; 
  assert(p == top); 
  return count; 
#else
  int count = 0; 
  const char * base = this -> mem__array + pscript__mem__memory_frame__base; 
  const char * p    = base + this -> memory_frame__top_ebp; 
  while (p > base) { 
    const int frame_size = *((const int *) (p - 2*sizeof(int))); 
    const int former_ebp = *((const int *) (p - frame_size - sizeof(int))); 
    p = this -> mem__array + former_ebp; 
    count ++; 
  }; 
  assert(p == base); 
  return count; 
#endif 
}; 

int pscript_mem__memory_frame__belongs_to(const pscript_mem_env_t * this, const int mem_offset) { 
#if 1 
  const char * ebp_top    = this -> mem__array + pscript__mem__memory_frame__top; 
  const char * ebp_p      = ebp_top - this -> memory_frame__ebp__nb; 
  const int    frame_base = pscript__mem__memory_frame__base; 
  const int    frame_top  = frame_base + this -> memory_frame__nb; 
  if (mem_offset <   frame_base) { return -1; }; 
  if (mem_offset >=  frame_top ) { return -1; }; 
  while (ebp_p < ebp_top) { 
    const int ebp        = *((const pscript_mem__offset_int_t *) ebp_p); 
    const int frame_size = *((const int *) (this -> mem__array + ebp - 2*sizeof(int))); 
    if (mem_offset >= (ebp - frame_size)) { return ebp; }; 
    ebp_p += sizeof(pscript_mem__offset_int_t); 
  }; 
  assert(ebp_p == ebp_top); 
  return -1; 
#else 
  const char * base = this -> mem__array + pscript__mem__memory_frame__base; 
  const char * top  = base + this -> memory_frame__nb; 
  const char * ebp  = this -> mem__array + this -> memory_frame__top_ebp; 
  const char * q    = this -> mem__array + mem_offset; 
  if (q <  base) { return -1; }; 
  if (q >=  top) { return -1; }; 
  while (ebp > base) { 
    const int frame_size = *((const int *) (ebp - 2*sizeof(int))); 
    if (q >= (ebp - frame_size)) { return ebp - this -> mem__array; }; 
    const int former_ebp = *((const int *) (ebp - frame_size - sizeof(int))); 
    ebp = this -> mem__array + former_ebp; 
  }; 
  assert(false); 
  return -1; 
#endif 
}; 

int pscript_mem__memory_frame__get_frame0_ref_counter_offset(const pscript_mem_env_t * this) { 
  const int ebp0 = *((const pscript_mem__offset_int_t *) (this -> mem__array + pscript__mem__memory_frame__top - sizeof(pscript_mem__offset_int_t))); 
  return ebp0 - sizeof(int); 
  //return pscript__mem__memory_frame__base + 2 * sizeof(int); 
}; 

int pscript_mem__memory_frame__get_frame_size(const pscript_mem_env_t * this, const int frame_ebp) { 
  return *((const int *) (this -> mem__array + frame_ebp - 2*sizeof(int))); 
}; 

int pscript_mem__memory_frame__get_frame_offset(const pscript_mem_env_t * this, const int frame_ebp) { 
  const int frame_size = *((const int *) (this -> mem__array + frame_ebp - 2*sizeof(int))); 
  return frame_ebp - frame_size; 
}; 

int pscript_mem__memory_frame__references_counter_get(const pscript_mem_env_t * this, const int frame_ebp) { 
  return *((const int *) (this -> mem__array + frame_ebp - sizeof(int))); 
}; 

int pscript_mem__memory_frame__references_counter_inc(pscript_mem_env_t * this, const int frame_ebp) { 
  return (*((int *) (this -> mem__array + frame_ebp - sizeof(int))) += 1); 
}; 

int pscript_mem__memory_frame__references_counter_dec(pscript_mem_env_t * this, const int frame_ebp) { 
  return (*((int *) (this -> mem__array + frame_ebp - sizeof(int))) -= 1); 
}; 


static int pscript_mem__memory_frame__deallocate__force_one(pscript_mem_env_t * this, const int frame_ebp) { 
  const int frame_size           = *((const int *) (this -> mem__array + frame_ebp - 2*sizeof(int))); 
  this -> memory_frame__nb      -= frame_size; 
  this -> memory_frame__ebp__nb -= sizeof(pscript_mem__offset_int_t); 
  return frame_size + sizeof(pscript_mem__offset_int_t); 
}; 
 
// RL: Should not be called as it should be automatic through reference-counter decreasing. 
// RL: Returns the actual number of bytes deallocated? 
int pscript_mem__memory_frame__deallocate(pscript_mem_env_t * this, const int frame_ebp) { 
  assert(false); 
  return -1; 
#if 0 
  const char * p = this -> mem__array + pscript__mem__memory_frame__base + this -> memory_frame__nb; 
  while (this -> memory_frame__nb > 0) { 
    p -= sizeof(pscript_mem__offset_int_t); 
    const pscript_mem__offset_int_t chunk_size = *((const pscript_mem__offset_int_t *) p); 
    p -= sizeof(int); 
    const int stack_frame_ref = *((const int *) p); 
    if (stack_frame_ref < disposed_stack_frame_ref) { return; }; 
    assert(chunk_size <= this -> memory_frame__nb); 
    this -> memory_frame__nb -= chunk_size; 
  }; 
#endif 
}; 

// RL: Removes all the top frames whose ref-counters are zero. 
int pscript_mem__memory_frame__gc(pscript_mem_env_t * this) { 
  const char * ebp_top    = this -> mem__array + pscript__mem__memory_frame__top; 
  const char * ebp_p      = ebp_top - this -> memory_frame__ebp__nb; 
  while (ebp_p < ebp_top) { 
    const int frame_ebp         = *((const pscript_mem__offset_int_t *) ebp_p); 
    const int frame_ref_counter = *((const int *) (this -> mem__array + frame_ebp -   sizeof(int))); 
    if (frame_ref_counter > 0) { return 0; }; 
    const int frame_size        = *((const int *) (this -> mem__array + frame_ebp - 2*sizeof(int))); 
    this -> memory_frame__nb      -= frame_size; 
    this -> memory_frame__ebp__nb -= sizeof(pscript_mem__offset_int_t); 
    ebp_p += sizeof(pscript_mem__offset_int_t); 
  }; 
  assert(false); // RL: Should never be reached as the bottom frame should never be deallocated. 
  assert(ebp_p == ebp_top); 
  return 0; 
}; 

void pscript_mem__memory_frame__print(const int fd, const pscript_mem_env_t * this) { 
  const char * ebp_top    = this -> mem__array + pscript__mem__memory_frame__top; 
  const char * ebp_p      = ebp_top - this -> memory_frame__ebp__nb; 
  while (ebp_p < ebp_top) { 
    const int frame_ebp         = *((const pscript_mem__offset_int_t *) ebp_p); 
    const int frame_ref_counter = *((const int *) (this -> mem__array + frame_ebp -   sizeof(int))); 
    const int frame_size        = *((const int *) (this -> mem__array + frame_ebp - 2*sizeof(int))); 
    dputs_array(fd, "MEMORY_FRAME: EBP = ", int_string__stack(frame_ebp), " - REF_COUNTER = ", int_string__stack(frame_ref_counter), " - SIZE: ", int_string__stack(frame_size), "\n");
    //abort(); 
    ebp_p += sizeof(pscript_mem__offset_int_t); 
  }; 
  assert(ebp_p == ebp_top); 
}; 


 
 
// ********* STACK_HEAP ********* 

// RL: CHUNK STRUCTURE 
//      - user data 
//      - frame ref 
//      - chunk size 
int pscript_mem__stack_heap__allocate(pscript_mem_env_t * this, const int stack_frame_ref, const int type_sizeof) { 
  const pscript_mem__offset_int_t chunk_size = type_sizeof + sizeof(pscript_mem__offset_int_t) + sizeof(pscript_mem__offset_int_t);  
  assert(this -> stack_heap__nb + chunk_size <= pscript__mem__stack_heap__size); 
  const int mem_offset = pscript__mem__stack_heap__base + this -> stack_heap__nb; 
  this -> stack_heap__nb += chunk_size; 
  char * p = this -> mem__array + mem_offset + type_sizeof; 
  *((pscript_mem__offset_int_t *) p) = stack_frame_ref; 
  p += sizeof(pscript_mem__offset_int_t); 
  *((pscript_mem__offset_int_t *) p) = chunk_size; 
  return mem_offset; 
}; 

int pscript_mem__stack_heap__dispose(pscript_mem_env_t * this, const int frame_ebp, const int mem_offset, const int type_sizeof) { 
  const pscript_mem__offset_int_t chunk_size = type_sizeof + sizeof(pscript_mem__offset_int_t) + sizeof(pscript_mem__offset_int_t);  
  assert(this -> stack_heap__nb + chunk_size <= pscript__mem__stack_heap__size); 

  if (mem_offset + chunk_size == pscript__mem__stack_heap__base + this -> stack_heap__nb) { 
    this -> stack_heap__nb -= chunk_size; 
    return chunk_size; 
  }; 
  
  char * p = this -> mem__array + mem_offset; 
  *((pscript_mem__offset_int_t *) (p + chunk_size - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))) = 0; 
  
  return 0; 
}; 

int pscript_mem__stack_heap__used(const pscript_mem_env_t * this) { 
  return this -> stack_heap__nb; 
}; 

int pscript_mem__stack_heap__count(const pscript_mem_env_t * this) { 
  int count = 0; 
  const char * base = this -> mem__array + pscript__mem__stack_heap__base; 
  const char * p    = base + this -> stack_heap__nb; 
  while (p > base) { 
    const int chunk_size = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t))); 
    p -= chunk_size; 
    count ++; 
  }; 
  assert(p == base); 
  return count; 
}; 

int pscript_mem__stack_heap__belongs_to(const pscript_mem_env_t * this, const int mem_offset) { 
  const char * base = this -> mem__array + pscript__mem__stack_heap__base; 
  const char * p    = base + this -> stack_heap__nb; 
  const char * q    = this -> mem__array + mem_offset; 
  if (q <  base) { return -1; }; 
  if (q >=    p) { return -1; }; 
  while (p > base) { 
    const int chunk_size = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t))); 
    p -= chunk_size; 
    if (q >= p) { 
      const int frame_ref = *((const pscript_mem__offset_int_t *) (p + chunk_size - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))); 
      return frame_ref; 
    }; 
  }; 
  assert(false); 
  return -1; 
}; 
 
void pscript_mem__stack_heap__deallocate(pscript_mem_env_t * this, const int disposed_stack_frame_ref) { 
  assert(false); // RL: This function does not have any meaning. 
#if 0 
  const char * p = this -> mem__array + pscript__mem__stack_heap__base + this -> stack_heap__nb; 
  while (this -> stack_heap__nb > 0) { 
    p -= sizeof(pscript_mem__offset_int_t); 
    const int chunk_size = *((const pscript_mem__offset_int_t *) p); 
    p -= sizeof(int); 
    const int stack_frame_ref = *((const int *) p); 
    if (stack_frame_ref < disposed_stack_frame_ref) { return; }; 
    assert(chunk_size <= this -> stack_heap__nb); 
    this -> stack_heap__nb -= chunk_size; 
  }; 
#endif 
}; 

// RL: Removes all the top chunks which were blanked. 
void pscript_mem__stack_heap__gc(pscript_mem_env_t * this) { 
  const char * base = this -> mem__array + pscript__mem__stack_heap__base; 
  const char * p    = base + this -> stack_heap__nb; 
  while (p > base) { 
    const int chunk_size = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t))); 
    const int frame_ebp  = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))); 
    if (frame_ebp > 0) { return; }; 
    p -= chunk_size; 
    this -> stack_heap__nb -= chunk_size; 
  }; 
  assert(p == base); 
}; 



// RL: All chunks which are referecing to unused frames will be blanked. 
void pscript_mem__stack_heap__update(pscript_mem_env_t * this) { 
  const char * base = this -> mem__array + pscript__mem__stack_heap__base; 
  char * p          = this -> mem__array + pscript__mem__stack_heap__base + this -> stack_heap__nb; 
  while (p > base) { 
    const int chunk_size = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t))); 
    const int frame_ebp  = *((const pscript_mem__offset_int_t *) (p - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))); 
    p -= chunk_size; 
    if (frame_ebp == 0) { continue; }; 
    if (frame_ebp >= this -> memory_frame__nb) { 
      *((pscript_mem__offset_int_t *) (p + chunk_size - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))) = 0; 
      continue; 
    }; 
    if (0 >= pscript_mem__memory_frame__references_counter_get(this, frame_ebp)) { 
      *((pscript_mem__offset_int_t *) (p + chunk_size - sizeof(pscript_mem__offset_int_t) - sizeof(pscript_mem__offset_int_t))) = 0; 
      continue; 
    }; 
  }; 
  assert(p == base); 
}; 
 

 
 
// ********* TURING_HEAP ********* 

//enum { pscript__mem__turing_heap__size__log2 = LOG2(pscript__mem__turing_heap__size) }; 

#if   TURING_BITSIZE < ( 8 - 1) 
typedef int8_t  turing_heap_offset_int_t; 
#elif TURING_BITSIZE < (16 - 1) 
typedef int16_t turing_heap_offset_int_t; 
#elif TURING_BITSIZE < (24 - 1) 
#if 1 
typedef int32_t turing_heap_offset_int_t; 
// RL: TODO XXX FIXME: I have to find a way to have a type that behaves close to int. 
//     Especially, I would like to be able to do 
//            *((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = pscript__mem__turing_heap__size;  
//      - 
//     That thing works: 
//            *((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = (turing_heap_offset_int_t) { pscript__mem__turing_heap__size }; 
//      - 
//     So we should be able to do things with some macros dedicated to conversion.  
//      - 
//     NB: Packing does not work at sub-byte level. So only multiples of 8 make sense. 
#error "FIXME: 24 bits" 
#elif 0 
typedef int turing_heap_offset_int_t:24; 
#elif 0 
typedef char turing_heap_offset_int_t[3]; 
#elif 0 
typedef struct { char n[3]; } turing_heap_offset_int_t ; 
#else 
#pragma pack(push, 1) 
typedef struct { int n:24; } turing_heap_offset_int_t ; 
#pragma pack(pop) 
#endif 
#elif TURING_BITSIZE < (32 - 1) 
typedef int32_t turing_heap_offset_int_t; 
#elif TURING_BITSIZE < (48 - 1) 
#error TODO 
#elif TURING_BITSIZE < (64 - 1) 
#error TODO 
#else 
#error TURING_BITSIZE too large. 
#endif 

enum { chunk__size__size             = sizeof(turing_heap_offset_int_t) }; 
enum { chunk__refcounter__size       = sizeof(int8_t) }; 
enum { chunk__height__size           = sizeof(int8_t) }; 
enum { chunk__left_avail_size__size  = sizeof(turing_heap_offset_int_t) }; 
enum { chunk__left_offset__size      = sizeof(turing_heap_offset_int_t) }; 
enum { chunk__right_avail_size__size = sizeof(turing_heap_offset_int_t) }; 
enum { chunk__right_offset__size     = sizeof(turing_heap_offset_int_t) }; 

enum { chunk__size__offset             = 0 }; 
enum { chunk__refcounter__offset       = chunk__size__offset             + chunk__size__size             }; 
enum { chunk__height__offset           = chunk__refcounter__offset       + chunk__refcounter__size       }; 
enum { chunk__left_avail_size__offset  = chunk__height__offset           + chunk__height__size           }; 
enum { chunk__left_offset__offset      = chunk__left_avail_size__offset  + chunk__left_avail_size__size  }; 
enum { chunk__right_avail_size__offset = chunk__left_offset__offset      + chunk__left_offset__size      }; 
enum { chunk__right_offset__offset     = chunk__right_avail_size__offset + chunk__right_avail_size__size }; 

enum { pscript__mem__turing_heap__overhead = chunk__right_offset__offset + chunk__right_offset__size }; 
 
static void assert_compile_time__ds3d632s3ss(void) { ASSERT_COMPILE(pscript__mem__turing_heap__overhead < pscript__mem__turing_heap__size); }; 
 
#if 0 
#define OFFSET__ALLOCATED_HUH(o)           ((o) < 0) 
#define OFFSET__GET_VALUE(o)               ((o) < 0 ? ~(o) : (o)) 
#define OFFSET__SET_VALUE(o,allocated_huh) ((allocated_huh) ? (o) : ~(o)) 
#else 
enum { pscript_mem__offset_int__upmost_bit = 1 << ((sizeof(turing_heap_offset_int_t) << 3) - 1) }; 
enum { pscript_mem__offset_int__mask       = ~pscript_mem__offset_int__upmost_bit }; 
#define OFFSET__ALLOCATED_HUH(o)           ((o) & pscript_mem__offset_int__upmost_bit) 
#define OFFSET__GET_VALUE(o)               ((o) | pscript_mem__offset_int__mask) 
#define OFFSET__SET_VALUE(o,allocated_huh) ((allocated_huh) ? (o) : (o) | pscript_mem__offset_int__upmost_bit) 
#endif 
  
 
static void pscript_mem__turing_heap__init(pscript_mem_env_t * this) { 
  this -> turing_heap__root_offset       = 0; 
  this -> turing_heap__chunk_nb          = 0; 
  this -> turing_heap__largest_available = pscript__mem__turing_heap__size; 
  this -> turing_heap__total_available   = pscript__mem__turing_heap__size; 
  { 
    char * p = this -> mem__array + pscript__mem__turing_heap__base; 
    //*((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = *((turing_heap_offset_int_t *) &pscript__mem__turing_heap__size); 
    //*((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = (turing_heap_offset_int_t) (int) pscript__mem__turing_heap__size; 
    //*((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = (turing_heap_offset_int_t) pscript__mem__turing_heap__size; 
    *((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = pscript__mem__turing_heap__size; 
    //*((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = (turing_heap_offset_int_t) { pscript__mem__turing_heap__size }; 
    *((int8_t                   *) (p + chunk__height__offset)) = -1; 
  }; 
}; 

#if 0 
  int turing_heap__root_offset;       // RL: needed 
  int turing_heap__largest_available; // RL: half necessary in order to have continuity & consistency in algorithms - and convenient as well. 
  int turing_heap__chunk_nb;          // RL: not necessary but convenient 
  int turing_heap__total_available;   // RL: not necessary but convenient 
#endif  

static int pscript_mem__turing_heap__used__aux(const pscript_mem_env_t * this, const int chunk_offset) { 
  if (!OFFSET__ALLOCATED_HUH(chunk_offset)) { return 0; }; 
  
  const char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
  
  const int chunk_size       = *((const turing_heap_offset_int_t      *) (p + chunk__size__offset            )); 
  const int refcounter       = *((const pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset      )); 
  const int height           = *((const int8_t                        *) (p + chunk__height__offset          )); 
  const int left_avail_size  = *((const turing_heap_offset_int_t      *) (p + chunk__left_avail_size__offset )); 
  const int left_offset      = *((const turing_heap_offset_int_t      *) (p + chunk__left_offset__offset     )); 
  const int right_avail_size = *((const turing_heap_offset_int_t      *) (p + chunk__right_avail_size__offset)); 
  const int right_offset     = *((const turing_heap_offset_int_t      *) (p + chunk__right_offset__offset    )); 
  
#if 1 
  const int  left_used = pscript_mem__turing_heap__used__aux(this,  left_offset); 
  const int right_used = pscript_mem__turing_heap__used__aux(this, right_offset); 
#else 
  const int  left_allocated_huh = OFFSET__ALLOCATED_HUH( left_offset); //( left_offset >= 0); 
  const int right_allocated_huh = OFFSET__ALLOCATED_HUH(right_offset); //(right_offset >= 0); 
  
  const int  left_used =  left_allocated_huh ? pscript_mem__turing_heap__used__aux(this,  left_offset) : 0; 
  const int right_used = right_allocated_huh ? pscript_mem__turing_heap__used__aux(this, right_offset) : 0; 
#endif 
  
  return chunk_size + left_used + right_used; 
}; 

int pscript_mem__turing_heap__used(const pscript_mem_env_t * this) { 
  const int mem_used  = pscript_mem__turing_heap__used__aux(this, this -> turing_heap__root_offset); 
  const int mem_avail = pscript__mem__turing_heap__size - mem_used; 
  assert(mem_avail == this -> turing_heap__total_available); 
  return mem_used; 
}; 

 
static int pscript_mem__turing_heap__count__aux(const pscript_mem_env_t * this, const int chunk_offset) { 
  if (!OFFSET__ALLOCATED_HUH(chunk_offset)) { return 0; }; 
  
  const char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
  
  const int chunk_size       = *((const turing_heap_offset_int_t      *) (p + chunk__size__offset            )); 
  const int refcounter       = *((const pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset      )); 
  const int height           = *((const int8_t                        *) (p + chunk__height__offset          )); 
  const int left_avail_size  = *((const turing_heap_offset_int_t      *) (p + chunk__left_avail_size__offset )); 
  const int left_offset      = *((const turing_heap_offset_int_t      *) (p + chunk__left_offset__offset     )); 
  const int right_avail_size = *((const turing_heap_offset_int_t      *) (p + chunk__right_avail_size__offset)); 
  const int right_offset     = *((const turing_heap_offset_int_t      *) (p + chunk__right_offset__offset    )); 
  
  const int  left_count = pscript_mem__turing_heap__count__aux(this,  left_offset); 
  const int right_count = pscript_mem__turing_heap__count__aux(this, right_offset); 
  
  return 1 + left_count + right_count; 
}; 

int pscript_mem__turing_heap__count(const pscript_mem_env_t * this) { 
  const int chunk_count  = pscript_mem__turing_heap__count__aux(this, this -> turing_heap__root_offset); 
  assert(chunk_count == this -> turing_heap__chunk_nb); 
  return chunk_count; 
}; 

 
static int pscript_mem__turing_heap__height__aux(const pscript_mem_env_t * this, const int chunk_offset) { 
  if (!OFFSET__ALLOCATED_HUH(chunk_offset)) { return 0; }; 
  
  const char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
  
  const int chunk_size       = *((const turing_heap_offset_int_t      *) (p + chunk__size__offset            )); 
  const int refcounter       = *((const pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset      )); 
  const int height           = *((const int8_t                        *) (p + chunk__height__offset          )); 
  const int left_avail_size  = *((const turing_heap_offset_int_t      *) (p + chunk__left_avail_size__offset )); 
  const int left_offset      = *((const turing_heap_offset_int_t      *) (p + chunk__left_offset__offset     )); 
  const int right_avail_size = *((const turing_heap_offset_int_t      *) (p + chunk__right_avail_size__offset)); 
  const int right_offset     = *((const turing_heap_offset_int_t      *) (p + chunk__right_offset__offset    )); 
  
  const int  left_height = pscript_mem__turing_heap__height__aux(this,  left_offset); 
  const int right_height = pscript_mem__turing_heap__height__aux(this, right_offset); 
  
  return 1 + left_height + right_height; 
}; 

int pscript_mem__turing_heap__height(const pscript_mem_env_t * this) { 
  const int height = pscript_mem__turing_heap__height__aux(this, this -> turing_heap__root_offset); 
  return height; 
}; 

 

static void pscript_mem__turing_heap__update_height__aux(pscript_mem_env_t * this, const int ascendent_chunk_offset, const int updated_chunk_offset) { 
  if (this -> turing_heap__root_offset == updated_chunk_offset) { return; }; 
  
  char * p = this -> mem__array + pscript__mem__turing_heap__base + ascendent_chunk_offset; 
  const int  left_offset = *((turing_heap_offset_int_t     *) (p + chunk__left_offset__offset )); 
  const int right_offset = *((turing_heap_offset_int_t     *) (p + chunk__right_offset__offset)); 
  
  if (updated_chunk_offset < ascendent_chunk_offset) { 
    pscript_mem__turing_heap__update_height__aux(this, left_offset, updated_chunk_offset); 
  } 
  else { 
    pscript_mem__turing_heap__update_height__aux(this, right_offset, updated_chunk_offset); 
  }; 
  
  const int  left_allocated_huh = OFFSET__ALLOCATED_HUH( left_offset); //( left_offset >= 0); 
  const int right_allocated_huh = OFFSET__ALLOCATED_HUH(right_offset); //(right_offset >= 0); 

  char * q_left  = this -> mem__array + pscript__mem__turing_heap__base +  left_offset; 
  char * q_right = this -> mem__array + pscript__mem__turing_heap__base + right_offset; 
  const int  left_height =  left_allocated_huh ? *((int8_t *) (q_left  + chunk__height__offset)) : -1; 
  const int right_height = right_allocated_huh ? *((int8_t *) (q_right + chunk__height__offset)) : -1; 
  const int ascendent_height = 1 + MAX(left_height,right_height); 
  *((int8_t *) (p + chunk__height__offset)) = ascendent_height; 
}; 
 
static void pscript_mem__turing_heap__update_height(pscript_mem_env_t * this, const int updated_chunk_offset) { 
  pscript_mem__turing_heap__update_height__aux(this, this -> turing_heap__root_offset, updated_chunk_offset); 
}; 

 



static int pscript_mem__turing_heap__update_available_mem__aux(pscript_mem_env_t * this, const int current_chunk_offset, const int updated_chunk_offset) { 
  // RL: 'height' should already have been updated. 
  char * p = this -> mem__array + pscript__mem__turing_heap__base + current_chunk_offset; 
  const int current_height     = *((int8_t                    *) (p  + chunk__height__offset)); 
  const int current_chunk_size = *((turing_heap_offset_int_t *) (p  + chunk__size__offset  )); 
  if (current_chunk_offset == updated_chunk_offset) { 
    if (current_height < 0) { 
      return current_chunk_size; 
    } 
    else { 
      const int  left_avail_size = *((turing_heap_offset_int_t *) (p + chunk__left_avail_size__offset )); 
      const int right_avail_size = *((turing_heap_offset_int_t *) (p + chunk__right_avail_size__offset)); 
      return MAX(left_avail_size,right_avail_size); 
    }; 
    assert(false); 
  }; 
  
  const int  left_offset = *((turing_heap_offset_int_t *) (p + chunk__left_offset__offset )); 
  const int right_offset = *((turing_heap_offset_int_t *) (p + chunk__right_offset__offset)); 
  int    left_avail_size = *((turing_heap_offset_int_t *) (p + chunk__left_avail_size__offset )); 
  int   right_avail_size = *((turing_heap_offset_int_t *) (p + chunk__right_avail_size__offset)); 
  if (updated_chunk_offset < current_chunk_offset) { 
    left_avail_size = pscript_mem__turing_heap__update_available_mem__aux(this, left_offset, updated_chunk_offset); 
    *((turing_heap_offset_int_t *) (p + chunk__left_avail_size__offset )) = left_avail_size; 
  } 
  else { 
    right_avail_size = pscript_mem__turing_heap__update_available_mem__aux(this, right_offset, updated_chunk_offset); 
    *((turing_heap_offset_int_t *) (p + chunk__right_avail_size__offset)) = right_avail_size; 
  }; 
  const int current_avail_size = MAX(left_avail_size,right_avail_size); 
  return current_avail_size; 
}; 
 
static void pscript_mem__turing_heap__update_available_mem(pscript_mem_env_t * this, const int updated_chunk_offset) { 
  this -> turing_heap__largest_available = pscript_mem__turing_heap__update_available_mem__aux(this, this -> turing_heap__root_offset, updated_chunk_offset); 
}; 
 
 
static void pscript_mem__turing_heap__update_height_and_available_mem(pscript_mem_env_t * this, const int updated_chunk_offset) { 
  pscript_mem__turing_heap__update_height(this, updated_chunk_offset); 
  pscript_mem__turing_heap__update_available_mem(this, updated_chunk_offset); 
}; 





 
static int pscript_mem__turing_heap__allocate__aux__write_chunk(pscript_mem_env_t * this, const int chunk_base_offset, const int chunk_base_size, const int type_sizeof, turing_heap_offset_int_t * ascendant_to_descendant_ref) { 
  const int chunk_size   = type_sizeof + pscript__mem__turing_heap__overhead; 
  const int chunk_offset = chunk_base_offset + (chunk_base_size - chunk_size) / 2; 
  const int mem_offset   = pscript__mem__turing_heap__base + chunk_offset + pscript__mem__turing_heap__overhead; 
  
  char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
  
  const int left_chunk_offset  = chunk_base_offset; 
  const int left_chunk_size    = chunk_offset - left_chunk_offset; 
  const int left_avail_size    = left_chunk_size; 
  const int right_chunk_offset = chunk_offset + chunk_size; 
  const int right_chunk_size   = chunk_base_size - chunk_size - left_chunk_size; 
  const int right_avail_size   = right_chunk_size; 
  
  const int  left_allocated_huh =  left_chunk_size > pscript__mem__turing_heap__overhead; 
  const int right_allocated_huh = right_chunk_size > pscript__mem__turing_heap__overhead; 
  
  *((turing_heap_offset_int_t     *) (p + chunk__size__offset            )) = chunk_size; 
  *((pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset      )) = 1; 
  *((int8_t                        *) (p + chunk__height__offset          )) = 0; 
  *((turing_heap_offset_int_t     *) (p + chunk__left_avail_size__offset )) = left_avail_size; 
  *((turing_heap_offset_int_t     *) (p + chunk__left_offset__offset     )) = OFFSET__SET_VALUE(left_chunk_offset,left_allocated_huh); 
  *((turing_heap_offset_int_t     *) (p + chunk__right_avail_size__offset)) = right_avail_size; 
  *((turing_heap_offset_int_t     *) (p + chunk__right_offset__offset    )) = OFFSET__SET_VALUE(right_chunk_offset,right_allocated_huh); 
  
  if (left_allocated_huh) { 
    char * q = this -> mem__array + pscript__mem__turing_heap__base + left_chunk_offset; 
    //*((char (*)[chunk__height__size]) (q + chunk__height__offset)) = -1; 
    *((turing_heap_offset_int_t *) (q + chunk__size__offset  )) = left_avail_size; 
    *((int8_t                    *) (q + chunk__height__offset)) = -1; 
  }; 
  
  if (right_allocated_huh) { 
    char * q = this -> mem__array + pscript__mem__turing_heap__base + right_chunk_offset; 
    //*((char (*)[chunk__height__size]) (q + chunk__height__offset)) = -1; 
    *((turing_heap_offset_int_t *) (q + chunk__size__offset  )) = right_avail_size; 
    *((int8_t                    *) (q + chunk__height__offset)) = -1; 
  }; 
  
  *ascendant_to_descendant_ref = chunk_offset; 
  this -> turing_heap__chunk_nb ++; 
  this -> turing_heap__total_available -= chunk_size; 
  pscript_mem__turing_heap__update_height_and_available_mem(this, chunk_offset); 
  
  return mem_offset; 
}; 
 
 
int pscript_mem__turing_heap__allocate(pscript_mem_env_t * this, const int type_sizeof) { 
  const int chunk_size = type_sizeof + pscript__mem__turing_heap__overhead; 
  if (chunk_size > this -> turing_heap__largest_available) { return -1; }; // RL: TODO XXX FIXME: should be an exception - but no exception in C. :-( 
  
  // ALGORITHM: it is an available-chunk looking-up algorithm. 
  
  int chunk_offset = this -> turing_heap__root_offset; 
  turing_heap_offset_int_t * ascendant_to_descendant_ref = (turing_heap_offset_int_t *) &this -> turing_heap__root_offset; // RL: Proof that little endian is the right way to go. // RL: FIXME: That code is wrong on non-little-endian system. 
  int chunk_base_size = pscript__mem__turing_heap__size; 
  do { 
    char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
    const int height = *((int8_t *) (p + chunk__height__offset)); 
    if (height < 0) { 
      // RL: This chunk is available. 
      const int mem_offset = pscript_mem__turing_heap__allocate__aux__write_chunk(this, chunk_offset, chunk_base_size, type_sizeof, ascendant_to_descendant_ref); 
      return mem_offset; 
    }; 
    
    // RL: Going left or right? 
    const int going_left  = 1; 
    const int going_right = 2; 
    int going_where_huh = 0; 
    const int  left_avail_size = *((turing_heap_offset_int_t *) (p + chunk__left_avail_size__offset )); 
    const int right_avail_size = *((turing_heap_offset_int_t *) (p + chunk__right_avail_size__offset)); 
    if (chunk_size > left_avail_size) { 
      if (chunk_size > right_avail_size) { 
	return -1; // RL: Well, we should not be here, as it should have been detected earlier. 
      } 
      else { 
	going_where_huh = going_right; 
      }
    }
    else { 
      if (chunk_size > right_avail_size) { 
	going_where_huh = going_left; 
      }
      else { 
	const int  left_chunk_offset = *((turing_heap_offset_int_t *) (p + chunk__left_offset__offset)); 
	const int right_chunk_offset = *((turing_heap_offset_int_t *) (p + chunk__right_offset__offset)); 
	char * q_left  = this -> mem__array + pscript__mem__turing_heap__base +  left_chunk_offset; 
	char * q_right = this -> mem__array + pscript__mem__turing_heap__base + right_chunk_offset; 
#if 0 
	const int  left_allocated_huh = OFFSET__ALLOCATED_HUH( left_offset); //( left_offset >= 0); 
	const int right_allocated_huh = OFFSET__ALLOCATED_HUH(right_offset); //(right_offset >= 0); 
	const int  left_height =  left_allocated_huh ? *((int8_t *) (q_left  + chunk__height__offset)) : -1; 
	const int right_height = right_allocated_huh ? *((int8_t *) (q_right + chunk__height__offset)) : -1; 
#else 
	// RL: If left or right were too small to be allocated (to contain a chunk-overhead), we would have already been discarded and not be here. 
	const int  left_height = *((int8_t *) (q_left  + chunk__height__offset)); 
	const int right_height = *((int8_t *) (q_right + chunk__height__offset)); 
#endif 
	if (left_height < right_height) { 
	  going_where_huh = going_left; 
	} 
	else if (left_height > right_height) { 
	  going_where_huh = going_right; 
	} 
	else { 
	  const int alea = rand() & RAND_MAX_1FIELD; 
	  if (alea <= RAND_MAX_1FIELD_HALF) { 
	    going_where_huh = going_left; 
	  } 
	  else { 
	    going_where_huh = going_right; 
	  }; 
	}; 
      }; 
    }; 
    
    // RL: Going there. 
    if (going_where_huh == going_left) { 
      const int left_chunk_offset = *((turing_heap_offset_int_t *) (p + chunk__left_offset__offset)); 
      chunk_offset = left_chunk_offset; 
      ascendant_to_descendant_ref = (turing_heap_offset_int_t *) (p + chunk__left_offset__offset); 
      chunk_base_size = left_avail_size; 
      continue; 
    } 
    else if (going_where_huh == going_right) {  
      const int right_chunk_offset = *((turing_heap_offset_int_t *) (p + chunk__right_offset__offset)); 
      chunk_offset = right_chunk_offset; 
      ascendant_to_descendant_ref = (turing_heap_offset_int_t *) (p + chunk__right_offset__offset); 
      chunk_base_size = right_avail_size; 
      continue; 
    } 
    else { 
      assert(false);  
      continue; 
    }; 
    assert(false); 
  } while (false); 
  assert(false); 
  return -1; 
}; 
 

static void pscript_mem__turing_heap__deallocate__aux__update_global(pscript_mem_env_t * this, const int chunk_offset, const int chunk_size) { //const int mem_offset, const int base_offset, const int base_size) { 
  this -> turing_heap__chunk_nb --; 
  this -> turing_heap__total_available += chunk_size; 
  pscript_mem__turing_heap__update_height_and_available_mem(this, chunk_offset); 
}; 

static void pscript_mem__turing_heap__deallocate__aux__leaf__update_ascendant(pscript_mem_env_t * this, const int being_deallocated_chunk_offset, const int base_offset, const int base_size) { 
  assert(this -> turing_heap__chunk_nb > 0); 
  
  { 
    char * p = this -> mem__array + pscript__mem__turing_heap__base + base_offset; 
    *((turing_heap_offset_int_t *) (p + chunk__size__offset  )) = base_size; 
    *((int8_t                    *) (p + chunk__height__offset)) = -1; 
  }; 
  

  if (1 == this -> turing_heap__chunk_nb) { 
    // RL: The being deallocated chunk is the only one left. 
    assert(this -> turing_heap__root_offset == being_deallocated_chunk_offset); 
    assert(base_offset == 0); 
    assert(base_size   == pscript__mem__turing_heap__size); 
    this -> turing_heap__root_offset = 0; 
    this -> turing_heap__largest_available = pscript__mem__turing_heap__size; // RL: Should be done in 'update_global'. 
    return; 
  }; 
  
  assert(being_deallocated_chunk_offset != this -> turing_heap__root_offset); // RL: We're supposedly deallocating a leaf. 
  
  int ascendent_chunk_offset = this -> turing_heap__root_offset; 
  do { 
    char * p = this -> mem__array + pscript__mem__turing_heap__base + ascendent_chunk_offset; 
    const int  left_offset = *((turing_heap_offset_int_t     *) (p + chunk__left_offset__offset )); 
    const int right_offset = *((turing_heap_offset_int_t     *) (p + chunk__right_offset__offset)); 
    if (being_deallocated_chunk_offset < ascendent_chunk_offset) { 
      if (being_deallocated_chunk_offset != left_offset) { 
	ascendent_chunk_offset = left_offset; 
	continue; 
      }; 
      *((turing_heap_offset_int_t     *) (p + chunk__left_avail_size__offset)) = base_size; 
      *((turing_heap_offset_int_t     *) (p + chunk__left_offset__offset    )) = base_offset; 
      return; 
      break; 
    }
    else { 
      if (being_deallocated_chunk_offset != right_offset) { 
	ascendent_chunk_offset = right_offset; 
	continue; 
      }; 
      *((turing_heap_offset_int_t     *) (p + chunk__right_avail_size__offset)) = base_size; 
      *((turing_heap_offset_int_t     *) (p + chunk__right_offset__offset    )) = base_offset; 
      return; 
      break; 
    }; 
    assert(false); 
  } while (false); 
  assert(false); 
}; 
 
 
static void pscript_mem__turing_heap__substitute(pscript_mem_env_t * this, const int old_chunk_offset, const int new_chunk_offset) { 
  if (this -> turing_heap__root_offset == old_chunk_offset) { 
    this -> turing_heap__root_offset = new_chunk_offset; 
    return;
  }; 
  
  int current_chunk_offset = this -> turing_heap__root_offset; 
  for (;;) { 
    char * p = this -> mem__array + pscript__mem__turing_heap__base + current_chunk_offset; 
    const int  left_offset = *((turing_heap_offset_int_t *) (p + chunk__left_offset__offset )); 
    const int right_offset = *((turing_heap_offset_int_t *) (p + chunk__right_offset__offset)); 
    
    if (old_chunk_offset < current_chunk_offset) {
      if (old_chunk_offset != left_offset) { 
	current_chunk_offset = left_offset; 
	continue;
      }; 
      *((turing_heap_offset_int_t *) (p + chunk__left_offset__offset )) = new_chunk_offset; 
      return; 
    };
    
    if (old_chunk_offset > current_chunk_offset) {
      if (old_chunk_offset != right_offset) { 
	current_chunk_offset = right_offset; 
	continue;
      }; 
      *((turing_heap_offset_int_t *) (p + chunk__right_offset__offset )) = new_chunk_offset; 
      return; 
    };

    assert(false); 
  }; 
  assert(false);   
}; 

// RL: We have to do something like AVL to keep the tree balanced. 
//     However, our invariant is not as strong as the AVL - we just need to be locally ordered. 
//     Thus, we can take up the rightmost leaf on the left or the leftmost leaf on the right. 
//     We will choose the deepest one, thus keeping the tree balanced. 
//     Yep, that's gonna do the job. 
// RL: The size of the freed memory. 
int pscript_mem__turing_heap__deallocate(pscript_mem_env_t * this, const int mem_offset) { 
  assert(mem_offset >= pscript__mem__turing_heap__base); 
  assert(mem_offset <  pscript__mem__turing_heap__top ); 
  const int chunk_offset = mem_offset - pscript__mem__turing_heap__base - pscript__mem__turing_heap__overhead; 
  assert(chunk_offset >= 0); 
  
  // RL: Reading the chunk. 
  char * p = this -> mem__array + pscript__mem__turing_heap__base + chunk_offset; 
  const int chunk_size       = *((turing_heap_offset_int_t     *) (p + chunk__size__offset            )); 
  const int refcounter       = *((pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset      )); 
  const int height           = *((int8_t                        *) (p + chunk__height__offset          )); 
  const int left_avail_size  = *((turing_heap_offset_int_t     *) (p + chunk__left_avail_size__offset )); 
  const int left_offset      = *((turing_heap_offset_int_t     *) (p + chunk__left_offset__offset     )); 
  const int right_avail_size = *((turing_heap_offset_int_t     *) (p + chunk__right_avail_size__offset)); 
  const int right_offset     = *((turing_heap_offset_int_t     *) (p + chunk__right_offset__offset    )); 
  
  assert(height >= 0); 
  assert(refcounter > 0); 
  
  //*((pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset  )) --; // RL: Refused by the compiler. 
  *((pscript_mem__refcounter_int_t *) (p + chunk__refcounter__offset  )) -= 1; // RL: Works. 
  
  if (refcounter > 1) { return 0; }; 
  
  // RL: We have to deallocate that chunk. 
  //     First, do we have descendants? 
  { 
    char * q_left  = this -> mem__array + pscript__mem__turing_heap__base +  left_offset; 
    char * q_right = this -> mem__array + pscript__mem__turing_heap__base + right_offset; 
#if 1 
    const int  left_allocated_huh = OFFSET__ALLOCATED_HUH( left_offset); //( left_offset >= 0); 
    const int right_allocated_huh = OFFSET__ALLOCATED_HUH(right_offset); //(right_offset >= 0); 
    const int  left_height =  left_allocated_huh ? *((int8_t *) (q_left  + chunk__height__offset)) : -1; 
    const int right_height = right_allocated_huh ? *((int8_t *) (q_right + chunk__height__offset)) : -1; 
#else 
    const int  left_height = *((int8_t *) (q_left  + chunk__height__offset)); 
    const int right_height = *((int8_t *) (q_right + chunk__height__offset)); 
#endif 
    
    if (-1 == left_height && -1 == right_height) { 
      // RL: We are a leaf. 
      const int base_size = left_avail_size + chunk_size + right_avail_size; 
      const int base_offset = left_offset; 
      pscript_mem__turing_heap__deallocate__aux__leaf__update_ascendant(this, chunk_offset, base_offset, base_size); 
      pscript_mem__turing_heap__deallocate__aux__update_global(this, /* already_updated_chunk */base_offset, /* deallocated size */chunk_size); 
      return 0; 
    }; 
    
    if (-1 == left_height) { 
      // RL: Let's move up the right descendant. 
      //     But we have to add the LHS empty space to its leftmost descendant. 
      // RL: Looking up for the leftmost. 
      int leftmost_descendant__chunk_offset = right_offset; 
      char * ld = NULL; 
      int  ld__left_offset = -1; 
      for (;;) { 
	ld = this -> mem__array + pscript__mem__turing_heap__base + leftmost_descendant__chunk_offset; 
	ld__left_offset = *((turing_heap_offset_int_t *) (ld + chunk__left_offset__offset)); 
	char * ld__q_left = this -> mem__array + pscript__mem__turing_heap__base + ld__left_offset; 
#if 1 
	const int ld__left_allocated_huh = OFFSET__ALLOCATED_HUH(ld__left_offset); //(ld__left_offset >= 0); 
	const int ld__left_height = ld__left_allocated_huh ? *((int8_t *) (ld__q_left + chunk__height__offset)) : -1; 
#else 
	const int ld__left_height = *((int8_t *) (ld__q_left + chunk__height__offset)); 
#endif 
	if (ld__left_height != -1) { 
	  leftmost_descendant__chunk_offset = ld__left_offset; 
	  continue; 
	}; 
	// RL: Found. 
	break; 
      }; 
      // RL: Updating leftmost. 
      int ld__left_avail = *((turing_heap_offset_int_t *) (ld + chunk__left_avail_size__offset)); 
      ld__left_offset = OFFSET__GET_VALUE(left_offset); //ABS(left_offset); // RL: We do not know whether the LHS has been allocated. 
      ld__left_avail += left_avail_size + chunk_size; 
      *((turing_heap_offset_int_t *) (ld         + chunk__left_offset__offset    )) = ld__left_offset; 
      *((turing_heap_offset_int_t *) (ld         + chunk__left_avail_size__offset)) = ld__left_avail; 
      char * ld__q_left = this -> mem__array + pscript__mem__turing_heap__base + ld__left_offset; 
      *((turing_heap_offset_int_t *) (ld__q_left + chunk__size__offset           )) = ld__left_avail; 
      *((int8_t                    *) (ld__q_left + chunk__height__offset         )) = -1; // RL: We might not already be be set if the previous size was too small to contain a chunk-header.  
      // RL: Moving up. 
      pscript_mem__turing_heap__substitute(this, chunk_offset, right_offset); 
      // RL: Updating tree data. 
      pscript_mem__turing_heap__deallocate__aux__update_global(this, /* already_updated_chunk */ld__left_offset, /* deallocated size */chunk_size); 
      return 0; 
    }; 
    
    if (-1 == right_height) { 
      // RL: Let's move up the left descendant. 
      //     But we have to add the RHS empty space to its rightmost descendant. 
      // RL: Looking up for the rightmost. 
      int rightmost_descendant__chunk_offset = left_offset; 
      char * rd = NULL; 
      int  rd__right_offset = -1; 
      for (;;) { 
	rd = this -> mem__array + pscript__mem__turing_heap__base + rightmost_descendant__chunk_offset; 
	rd__right_offset = *((turing_heap_offset_int_t *) (rd + chunk__right_offset__offset)); 
	char *    rd__q_right      = this -> mem__array + pscript__mem__turing_heap__base + rd__right_offset; 
	const int rd__right_allocated_huh = OFFSET__ALLOCATED_HUH(rd__right_offset); 
	const int rd__right_height = rd__right_allocated_huh ? *((int8_t *) (rd__q_right + chunk__height__offset)) : -1; 
	if (rd__right_height != -1) { 
	  rightmost_descendant__chunk_offset = rd__right_offset; 
	  continue; 
	}; 
	// RL: Found. 
	break; 
      }; 
      // RL: Updating rightmost. 
      int rd__right_avail = *((turing_heap_offset_int_t *) (rd + chunk__right_avail_size__offset)); 
      rd__right_offset = OFFSET__GET_VALUE(right_offset); // RL: We do not know whether the RHS has been allocated. 
      rd__right_avail += chunk_size + right_avail_size; 
      *((turing_heap_offset_int_t *) (rd          + chunk__right_offset__offset    )) = rd__right_offset; 
      *((turing_heap_offset_int_t *) (rd          + chunk__right_avail_size__offset)) = rd__right_avail; 
      char *    rd__q_right      = this -> mem__array + pscript__mem__turing_heap__base + rd__right_offset; 
      *((turing_heap_offset_int_t *) (rd__q_right + chunk__size__offset            )) = rd__right_avail; 
      *((int8_t                    *) (rd__q_right + chunk__height__offset          )) = -1; // RL: We might not already be be set if the previous size was too small to contain a chunk-header.  
      // RL: Moving up. 
      pscript_mem__turing_heap__substitute(this, chunk_offset, left_offset); 
      // RL: Updating tree data. 
      pscript_mem__turing_heap__deallocate__aux__update_global(this, /* already_updated_chunk */rd__right_offset, /* deallocated size */chunk_size); 
      return 0; 
    }; 
    
    
    assert(-1 != left_height && -1 != right_height); { 
      const int  left_choosen = 1; 
      const int right_choosen = 2; 
      int left_or_right_huh = 0; 
      if (left_height < right_height) { 
	left_or_right_huh = left_choosen; 
      } 
      else if (left_height > right_height) { 
	left_or_right_huh = right_choosen; 
      } 
      else {
	const int random_draw = rand() & RAND_MAX_1FIELD; 
	left_or_right_huh = (random_draw <= RAND_MAX_1FIELD_HALF) ? left_choosen : right_choosen; 
      }; 
      
      int rightmost_descendant__chunk_offset = left_offset; 
      for (;;) { 
	char *    rd = this -> mem__array + pscript__mem__turing_heap__base + rightmost_descendant__chunk_offset; 
	int       rd__right_offset = *((turing_heap_offset_int_t *) (rd + chunk__right_offset__offset)); 
	char *    rd__q_right      = this -> mem__array + pscript__mem__turing_heap__base + rd__right_offset; 
	const int rd__right_allocated_huh = OFFSET__ALLOCATED_HUH(rd__right_offset); 
	const int rd__right_height = rd__right_allocated_huh ? *((int8_t *) (rd__q_right + chunk__height__offset)) : -1; 
	if (rd__right_height != -1) { 
	  rightmost_descendant__chunk_offset = rd__right_offset; 
	  continue; 
	}; 
	break; 
      }; 
      
      int leftmost_descendant__chunk_offset = right_offset; 
      for (;;) { 
	char *    ld = this -> mem__array + pscript__mem__turing_heap__base + leftmost_descendant__chunk_offset; 
	int       ld__left_offset = *((turing_heap_offset_int_t *) (ld + chunk__left_offset__offset)); 
	char *    ld__q_left      = this -> mem__array + pscript__mem__turing_heap__base + ld__left_offset; 
	const int ld__left_allocated_huh = OFFSET__ALLOCATED_HUH(ld__left_offset); //(ld__left_offset >= 0); 
	const int ld__left_height = ld__left_allocated_huh ? *((int8_t *) (ld__q_left + chunk__height__offset)) : -1; 
	if (ld__left_height != -1) { 
	  leftmost_descendant__chunk_offset = ld__left_offset; 
	  continue; 
	}; 
	break; 
      }; 
      
      int target_chunk_offset = -1; 
      if (left_or_right_huh == left_choosen) { 
	target_chunk_offset = rightmost_descendant__chunk_offset; 
      }
      else if (left_or_right_huh == right_choosen) { 
	target_chunk_offset = leftmost_descendant__chunk_offset; 
      } 
      else { 
	assert(false); 
      }; 

      
      // RL: Adding the empty space to the other side. 
      if (left_or_right_huh == left_choosen) { 
	// RL: The freed space has to be added to the right-hand-side tree, id the leftmost chunk. 
	char * ld = this -> mem__array + pscript__mem__turing_heap__base +  leftmost_descendant__chunk_offset; 
	char * rd = this -> mem__array + pscript__mem__turing_heap__base + rightmost_descendant__chunk_offset; 
	const int rd__right_avail  = *((turing_heap_offset_int_t *) (ld + chunk__right_avail_size__offset)); 
	const int rd__right_offset = *((turing_heap_offset_int_t *) (ld + chunk__right_offset__offset    )); 
	// RL: Updating the ascendent. 
	const int ld__left_avail   = rd__right_avail + chunk_size + (*((turing_heap_offset_int_t *) (ld + chunk__left_avail_size__offset))); 
	const int ld__left_offset  = OFFSET__GET_VALUE(rd__right_offset); 
	*((turing_heap_offset_int_t *) (ld + chunk__left_offset__offset))  = ld__left_offset; 
	*((turing_heap_offset_int_t *) (ld + chunk__size__offset       ))  = ld__left_avail; 
	// RL: Updating the empty space. 
	char *    ld__q_left      = this -> mem__array + pscript__mem__turing_heap__base + ld__left_offset; 
	*((turing_heap_offset_int_t *) (ld__q_left + chunk__size__offset  )) = ld__left_avail; 
	*((int8_t                    *) (ld__q_left + chunk__height__offset)) = -1; // RL: Might not be already set if the space was previously too small to contain a chunk-header. 
      }
      else if (left_or_right_huh == right_choosen) { 
	// RL: The freed space has to be added to the left-hand-side tree, id the rightmost chunk. 
	char * rd = this -> mem__array + pscript__mem__turing_heap__base + rightmost_descendant__chunk_offset; 
	char * ld = this -> mem__array + pscript__mem__turing_heap__base +  leftmost_descendant__chunk_offset; 
	const int ld__left_avail   = *((turing_heap_offset_int_t *) (rd + chunk__left_avail_size__offset)); 
	const int ld__left_offset  = *((turing_heap_offset_int_t *) (rd + chunk__left_offset__offset    )); 
	// RL: Updating the ascendent. 
	const int rd__right_avail  = (*((turing_heap_offset_int_t *) (rd + chunk__right_avail_size__offset))) + chunk_size + ld__left_avail; 
	const int rd__right_offset__field =  *((turing_heap_offset_int_t *) (rd + chunk__right_offset__offset)); 
	const int rd__right_offset = OFFSET__GET_VALUE(rd__right_offset__field); 
	*((turing_heap_offset_int_t *) (rd + chunk__right_offset__offset)) = rd__right_offset; // RL: Could have been negative before hand. 
	*((turing_heap_offset_int_t *) (rd + chunk__size__offset        )) = rd__right_avail; 
	// RL: Updating the empty space. 
	char *    rd__q_right      = this -> mem__array + pscript__mem__turing_heap__base + rd__right_offset; 
	*((turing_heap_offset_int_t *) (rd__q_right + chunk__size__offset  )) = rd__right_avail; 
	*((int8_t                    *) (rd__q_right + chunk__height__offset)) = -1; // RL: Might have been too small beforehand. 
      } 
      else { 
	assert(false); 
      }; 
      
      // RL: Removing the target chunk from its ascendent. 
      int new_offset = -1; 
      if (left_or_right_huh == left_choosen) { 
	char * rd = this -> mem__array + pscript__mem__turing_heap__base + rightmost_descendant__chunk_offset; 
	new_offset = *((turing_heap_offset_int_t *) (rd + chunk__left_offset__offset)); 
	rightmost_descendant__chunk_offset = new_offset; 
      }
      else if (left_or_right_huh == right_choosen) { 
	char * ld = this -> mem__array + pscript__mem__turing_heap__base + leftmost_descendant__chunk_offset; 
	new_offset = *((turing_heap_offset_int_t *) (ld + chunk__right_offset__offset)); 
	leftmost_descendant__chunk_offset = new_offset; 
      } 
      else { 
	assert(false); 
      };
      pscript_mem__turing_heap__substitute(this, target_chunk_offset, new_offset); 
      
      // RL: Adding the two new descendants to the target chunk. 
      {
	char * tp = this -> mem__array + pscript__mem__turing_heap__base + target_chunk_offset; 
	if (target_chunk_offset != left_offset) 
	  *((turing_heap_offset_int_t *) (tp + chunk__left_offset__offset )) = left_offset; 
	if (target_chunk_offset != right_offset) 
	  *((turing_heap_offset_int_t *) (tp + chunk__right_offset__offset)) = right_offset; 
      }; 
      
      // RL: Moving up the target chunk. 
      pscript_mem__turing_heap__substitute(this, chunk_offset, target_chunk_offset); 
      
      // RL: Updating tree data. 
      pscript_mem__turing_heap__update_height(this,  leftmost_descendant__chunk_offset); 
      pscript_mem__turing_heap__update_height(this, rightmost_descendant__chunk_offset); 
      pscript_mem__turing_heap__update_available_mem(this,  leftmost_descendant__chunk_offset); 
      pscript_mem__turing_heap__update_available_mem(this, rightmost_descendant__chunk_offset); 
      
      // RL: Done. 
      return 0; 
    }; 
    
    assert(false); 
  }; 
  assert(false); 
  return -1; 
}; 







// ********* OTHER OPERATIONS ********* 

int pscript_mem__runtime__allocate_same_memory_frame(pscript_mem_env_t * this, const int mem_offset, const int type_sizeof) { 
  const int mem_part = pscript_mem__to_which_memory_part_that_variable_belongs(this, mem_offset); 
  if (mem_part < 0) { return mem_part; }; 
  switch (mem_part) { 
  default: return ~mem_part; break; 
  case PSCRIPT_MEM__TOPLEVEL_COMPILETIME: 
  case PSCRIPT_MEM__TOPLEVEL_RUNTIME:     return pscript_mem__toplevel_runtime__allocate(this, type_sizeof); break; 
  case PSCRIPT_MEM__MEMORY_FRAME: { 
    const int owning_ebp = pscript_mem__memory_frame__belongs_to(this, mem_offset); 
    const int top_ebp    = pscript_mem__memory_frame__get_ebp(this); 
    if (top_ebp == owning_ebp) { 
      return pscript_mem__memory_frame__allocate_object(this, owning_ebp, type_sizeof); 
    }; 
    return pscript_mem__stack_heap__allocate(this, owning_ebp, type_sizeof); 
  }; break; 
  case PSCRIPT_MEM__STACK_HEAP: { 
    const int owning_ebp = pscript_mem__memory_frame__belongs_to(this, mem_offset); 
    return pscript_mem__stack_heap__allocate(this, owning_ebp, type_sizeof); 
    break; 
  }; 
  case PSCRIPT_MEM__TURING_HEAP:          return pscript_mem__turing_heap__allocate(this, type_sizeof); break; 
  }; 
  assert(false); 
}; 

int pscript_mem__dispose(pscript_mem_env_t * this, const int mem_offset, const int object_sizeof) { 
  const int mem_part = pscript_mem__to_which_memory_part_that_variable_belongs(this, mem_offset); 
  if (mem_part < 0) { return mem_part; }; 
  switch (mem_part) { 
  default: return ~mem_part; break; 
  case PSCRIPT_MEM__TOPLEVEL_COMPILETIME: assert(false); break; 
  case PSCRIPT_MEM__TOPLEVEL_RUNTIME:     return pscript_mem__toplevel_runtime__dispose(this, mem_offset, object_sizeof); break; 
  case PSCRIPT_MEM__MEMORY_FRAME: { 
    const int owning_ebp = pscript_mem__memory_frame__belongs_to(this, mem_offset); 
    const int top_ebp    = pscript_mem__memory_frame__get_ebp(this); 
    if (top_ebp == owning_ebp) { 
      return pscript_mem__memory_frame__dispose_object(this, owning_ebp, mem_offset, object_sizeof); 
    }; 
    return -1; 
  }; break; 
  case PSCRIPT_MEM__STACK_HEAP: { 
    const int owning_ebp = pscript_mem__memory_frame__belongs_to(this, mem_offset); 
    return pscript_mem__stack_heap__dispose(this, owning_ebp, mem_offset, object_sizeof); 
    break; 
  }; 
  case PSCRIPT_MEM__TURING_HEAP: return pscript_mem__turing_heap__deallocate(this, mem_offset); break; 
  }; 
  assert(false); 
}; 



 
 
 
 
 
 
 
 
 
 

