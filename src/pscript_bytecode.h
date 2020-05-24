#ifndef PSCRIPT_BYTECODE_H
#define PSCRIPT_BYTECODE_H

#ifndef PSCRIPT_BYTECODE_ENV_T 
#define PSCRIPT_BYTECODE_ENV_T
struct pscript_bytecode_env_t; 
typedef struct pscript_bytecode_env_t pscript_bytecode_env_t; 
#endif 

enum { pscript_bytecode__size = (1 << 15) }; 
struct pscript_bytecode_env_t {
  uint8_t bytecode_array[pscript_bytecode__size]; 
  int bytecode_nb; 
}; 

extern pscript_bytecode_env_t * pscript_bytecode_env__make(void); 
extern void pscript_bytecode_env__delete(pscript_bytecode_env_t * this); 


extern int pscript_bytecode__get_count(const pscript_bytecode_env_t * this); 
extern void bytecode_stack__print_nb(const int fd, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env, const int bytecode_nb); 
extern void bytecode_stack__print_all(const int fd, const pscript_bytecode_env_t * this, pscript_type_env_t * type_env); 

extern int pscript_bytecode__push_instruction(pscript_bytecode_env_t * this, const uint8_t bytecode); 
extern int pscript_bytecode__push_bool(pscript_bytecode_env_t * this, const int boolval); 
extern int pscript_bytecode__push_int(pscript_bytecode_env_t * this, const int intval); 
extern int pscript_bytecode__push_int8(pscript_bytecode_env_t * this, const int8_t intval); 
extern int pscript_bytecode__push_int16(pscript_bytecode_env_t * this, const int16_t intval); 
extern int pscript_bytecode__push_float(pscript_bytecode_env_t * this, const float floatval); 
extern int pscript_bytecode__push_string(pscript_bytecode_env_t * this, const int pstr); 
extern int pscript_bytecode__push_const(pscript_bytecode_env_t * this, const pscript_t * pscript_env, const int const_i); 
extern int pscript_bytecode__push_pointer_nil(pscript_bytecode_env_t * this); 
extern int pscript_bytecode__push_mem_offset(pscript_bytecode_env_t * this, const int mem_offset); 
extern int pscript_bytecode__push_bytecode_offset(pscript_bytecode_env_t * this, const int bytecode_offset); 
extern int pscript_bytecode__get_next_offset(const pscript_bytecode_env_t * this); 

#define pscript_bytecode__push_integer pscript_bytecode__push_int

extern int pscript_bytecode__rewrite_address(pscript_bytecode_env_t * this, const int address_bytecode_offset, const int current_bytecode_offset); 
extern int pscript_bytecode__rewrite_int8(pscript_bytecode_env_t * this, const int bytecode_offset, const int8_t value); 
extern int pscript_bytecode__rewrite_int (pscript_bytecode_env_t * this, const int bytecode_offset, const int value); 


#if 0 
extern void pscript_bytecode__push_bool(pscript_bytecode_env_t * this, const int bool_value); 
extern void pscript_bytecode__push_integer(pscript_bytecode_env_t * this, const int integer_value); 
extern void pscript_bytecode__push_float(pscript_bytecode_env_t * this, const float float_value); 
extern void pscript_bytecode__push_string(pscript_bytecode_env_t * this, const int pstr); 
extern void pscript_bytecode__push_const(pscript_bytecode_env_t * this, const int const_i); 
extern void pscript_bytecode__push_pointer_nil(pscript_bytecode_env_t * this); 

extern void pscript_bytecode__push_instruction(pscript_bytecode_env_t * this, const int instruction_value); 
#endif 


#endif /* PSCRIPT_BYTECODE_H */
