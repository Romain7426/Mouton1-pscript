#ifndef PSCRIPT_CONST_H
#define PSCRIPT_CONST_H

#ifndef PSCRIPT_CONST_ENV_T 
#define PSCRIPT_CONST_ENV_T 
struct pscript_const_env_t; 
typedef struct pscript_const_env_t pscript_const_env_t; 
#endif 



enum { pscript_const__mem__size        = (1 << 8) }; 
enum { pscript_const__stack__size      = (1 << 5) }; 
enum { pscript_const__stack__mem__size = (1 << 7) }; 
enum { pscript_const__env__size        = (1 << 7) }; 
struct pscript_const_env_t { 
  pscript_t * pscript_env; 
  //pscript_type_env_t * type_env; 
  
  char mem[pscript_const__mem__size]; 
  int  mem_nb; 
  
  char stack__mem[pscript_const__stack__mem__size]; 
  int  stack__mem__nb; 
  int  stack__type[pscript_const__stack__size]; 
  //int  stack__offset[pscript_const__stack__size]; 
  int  stack__nb; 
#if 0
  int stack__value_bool[pscript_const__stack__size]; 
  int stack__value_int[pscript_const__stack__size]; 
  int stack__value_float[pscript_const__stack__size]; 
  int stack__value_char[pscript_const__stack__size]; 
  int stack__value_str[pscript_const__stack__size]; 
  int stack__value_nil[pscript_const__stack__size]; 
  int stack__value_pointer[pscript_const__stack__size]; 
  int stack__value_array[pscript_const__stack__size]; // offset dans la const_mem 
  int stack__value_record[pscript_const__stack__size]; // offset dans la const_mem 
  // proc & fun cannot be 'const' per se: this const is a compile-time const, while proc&fun are known only after relocation time (wihch means right before being run). 
  // user does not make sense, as it is built bottom-up. 
#endif 
  
  int env__name[pscript_const__env__size]; 
  int env__type[pscript_const__env__size]; 
  int env__mem_offset[pscript_const__env__size]; 
  int env__nb; 
}; 

//extern pscript_const_env_t * pscript_const_env__make(pscript_type_env_t * type_env); 
extern pscript_const_env_t * pscript_const_env__make(pscript_t * pscript_env); 
extern void pscript_const_env__delete(pscript_const_env_t * this); 

extern int pscript_const__get_count(const pscript_const_env_t * this); 

extern int pscript_const__stack__push_value(pscript_const_env_t * this, const int type1, const int type1_sizeof, const char value1[]); 

extern int pscript_const__stack__push_bool  (pscript_const_env_t * this, const int bool_value); 
extern int pscript_const__stack__push_int   (pscript_const_env_t * this, const int int_value); 
extern int pscript_const__stack__push_float (pscript_const_env_t * this, const float float_value); 
extern int pscript_const__stack__push_string(pscript_const_env_t * this, const int pstr); 
extern int pscript_const__stack__push_nil   (pscript_const_env_t * this); 

extern int pscript_const__stack__push_pointer(pscript_const_env_t * this); 
extern int pscript_const__stack__push_array  (pscript_const_env_t * this); 
extern int pscript_const__stack__push_record (pscript_const_env_t * this); 

//extern void pscript_const__stack__pick(pscript_const_env_t * this, const int const_i);
extern int pscript_const__stack__push_const_value(pscript_const_env_t * this, const int const_i);

 
extern int pscript_const__stack__pop(pscript_const_env_t * this); 
extern int pscript_const__stack__top_get_type(const pscript_const_env_t * this); 
extern int pscript_const__stack__top_get_value(const pscript_const_env_t * this); 
extern int pscript_const__stack__top_get_value2(const pscript_const_env_t * this, const int value_allocated_sizeof, char * value); 


extern int pscript_const__env__name__push_nolookup(pscript_const_env_t * this, const int name, const int type_i); 
extern int pscript_const__env__name__lookup(const pscript_const_env_t * this, const int name); 
#define pscript_const__env__name__push pscript_const__env__name__push_lookup

extern int pscript_const__env__name__push_from_stack__nolookup(pscript_const_env_t * this, const int name); 
extern void pscript_const__stack__drop(pscript_const_env_t * this); 


#define pscript_const__push_bool   pscript_const__stack__push_bool
#define pscript_const__push_int    pscript_const__stack__push_int
#define pscript_const__push_float  pscript_const__stack__push_float
#define pscript_const__push_string pscript_const__stack__push_string
#define pscript_const__push_nil    pscript_const__stack__push_nil

#if 0 
extern int   pscript_const__top_indice(void); 

#endif 
extern int   pscript_const__get_type(const pscript_const_env_t * this, const int const_i); 
extern int   pscript_const__get_value_bool(const pscript_const_env_t * this, const int const_i); 
extern int   pscript_const__get_value_int(const pscript_const_env_t * this, const int const_i); 
extern float pscript_const__get_value_float(const pscript_const_env_t * this, const int const_i); 
extern char  pscript_const__get_value_char(const pscript_const_env_t * this, const int const_i); 
extern int   pscript_const__get_value_str(const pscript_const_env_t * this, const int const_i); 
extern int   pscript_const__get_value_nil(const pscript_const_env_t * this, const int const_i); 



//extern void pscript_const__stack__push_int(pscript_const_env_t * this, const int intval);
extern void pscript_const__stack__and(pscript_const_env_t * this);
extern void pscript_const__stack__or(pscript_const_env_t * this);
extern void pscript_const__stack__equal_huh(pscript_const_env_t * this);
extern void pscript_const__stack__diff_huh(pscript_const_env_t * this);
extern void pscript_const__stack__inf_huh(pscript_const_env_t * this);
extern void pscript_const__stack__sup_huh(pscript_const_env_t * this);
extern void pscript_const__stack__infeq_huh(pscript_const_env_t * this);
extern void pscript_const__stack__supeq_huh(pscript_const_env_t * this);
extern void pscript_const__stack__add(pscript_const_env_t * this);
extern void pscript_const__stack__sub(pscript_const_env_t * this);
extern void pscript_const__stack__mul(pscript_const_env_t * this);
extern void pscript_const__stack__idiv(pscript_const_env_t * this);
extern void pscript_const__stack__imod(pscript_const_env_t * this);
extern void pscript_const__stack__rdiv(pscript_const_env_t * this);
extern void pscript_const__stack__not(pscript_const_env_t * this);
extern void pscript_const__stack__opposite(pscript_const_env_t * this);

#endif /* PSCRIPT_CONST_H */
