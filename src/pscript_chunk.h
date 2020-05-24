#ifndef PSCRIPT_CHUNK_H
#define PSCRIPT_CHUNK_H


// *** CHUNK ENV *** 

#ifndef PSCRIPT_CHUNK_ENV_T 
#define PSCRIPT_CHUNK_ENV_T 
struct pscript_chunk_env_t; 
typedef struct pscript_chunk_env_t pscript_chunk_env_t; 
#endif 
 
enum { pscript_chunk__memory__array_size__default = (1 << 13) }; 
struct pscript_chunk_env_t { 
  int8_t  bytesize;           // RL: If the structure unit is 8 bits, 16 bits, 32 bits, or 64 bits. Currently, it's statically 16, but we could do something more dynamic. 
  int8_t  malloced_huh; 
  int16_t memory__array_size; // RL: Allocated size of the array. 
  int16_t memory__array_nb;   // RL: Top of the array. 
  char    memory__array[]; 
}; 

// RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that an chunk will fit into a 'int_pscript_chunk_t'. 
typedef       int16_t             int_pscript_chunk_t; 
extern  const int_pscript_chunk_t int_pscript_chunk__max; 

static uint16_t pscript_chunk_env__sizeof(pscript_chunk_env_t * this) { 
  return sizeof(*this) + this -> memory__array_size; 
}; 

static uint16_t pscript_chunk_env__sizeof2(const int memory__array_size) { 
  return sizeof(pscript_chunk_env_t) + memory__array_size; 
}; 

static pscript_chunk_env_t * pscript_chunk_env__make(const int data_memory_size) { 
  pscript_chunk_env_t * this; 
  const int memory__array_size = data_memory_size > 0 ? data_memory_size : pscript_chunk__memory__array_size__default; 
  const int this_sizeof = pscript_chunk_env__sizeof2(memory__array_size); 
  this = malloc(this_sizeof); 
  bzero(this, this_sizeof); 
  this -> bytesize = 16; 
  this -> malloced_huh = true; 
  this -> memory__array_size = memory__array_size; 
  this -> memory__array_nb = 0; 
  return this; 
}; 

static void pscript_chunk_env__free(pscript_chunk_env_t * this) { 
  if (this -> malloced_huh) { 
    free(this); 
  }; 
}; 

static pscript_chunk_env_t * pscript_chunk_env__make_b(const int data_memory_size, const int buffer_size, char * buffer, int * buffer_used_ref) { 
  pscript_chunk_env_t * this; 
  const int memory__array_size = data_memory_size > 0 ? data_memory_size : pscript_chunk__memory__array_size__default; 
  const int this_sizeof = pscript_chunk_env__sizeof2(memory__array_size); 
  if (buffer_size < this_sizeof) { return NULL; }; // RL: TODO XXX FIXME: Throw an exception 
  this = (pscript_chunk_env_t *) buffer; 
  bzero(this, this_sizeof); 
  this -> bytesize = 16; 
  this -> malloced_huh = false; 
  this -> memory__array_size = memory__array_size; 
  this -> memory__array_nb = 0; 
  if (buffer_used_ref != NULL) { *buffer_used_ref = this_sizeof; }; 
  return this; 
}; 




#endif /* PSCRIPT_CHUNK_H */


