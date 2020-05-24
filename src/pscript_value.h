#ifndef PSCRIPT_VALUE_H
#define PSCRIPT_VALUE_H

extern void pscript_value__print__raw_bytes(const int type1_sizeof, const char value1[]); 
extern const char * pscript_value__cstr  (const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]); 
extern int          pscript_value__string(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]); 
#define pscript_value__pstr pscript_value__string
#define pscript_value__get_cstr pscript_value__cstr
#define pscript_value__get_pstr pscript_value__pstr

extern char  pscript_value__get_bool   (const int type1_sizeof, const char value1[]); 
extern int   pscript_value__get_integer(const int type1_sizeof, const char value1[]); 
extern float pscript_value__get_float  (const int type1_sizeof, const char value1[]); 
extern char  pscript_value__get_char   (const int type1_sizeof, const char value1[]); 
extern int   pscript_value__get_string (const int type1_sizeof, const char value1[]); 
extern int   pscript_value__get_pointer(const int type1_sizeof, const char value1[]); 
extern int   pscript_value__get_proxy  (const int type1_sizeof, const char value1[]); 
extern void  pscript_value__get_value  (const int type1_sizeof, const char value1[], const int retval_allocated_sizeof, char retval_value[]); 


extern char  pscript_value__cast_to_bool__scalar   (const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]); 
extern int   pscript_value__cast_to_integer__scalar(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]); 
extern float pscript_value__cast_to_float__scalar  (const pscript_t * this, const int type1, const int type1_sizeof, const char value1[]); 

extern void  pscript_value__cast_to_float(const pscript_t * this, const int type2, const int value2_sizeof, const char value2[], const int cast_value2_sizeof, char cast_value2[]); 
extern void  pscript_value__cast_generic(pscript_t * this, const int type1, const int type2, const int value2_sizeof, const char value2[], const int cast_value2_sizeof, char cast_value2[]); 


extern void pscript_value__operation__bool_and(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
void pscript_value__operation__bool_or(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__bool_xor(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__bool_not(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 

extern void pscript_value__operation__equal_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__diff_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__inf_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__sup_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__infeq_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__supeq_huh(const pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 

extern void pscript_value__operation__neg(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__add(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__sub(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__mul(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__idiv(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__imod(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__opposite(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 
extern void pscript_value__operation__rdiv(pscript_t * this, const int type1, const int type1_sizeof, const char value1[], const int type2, const int type2_sizeof, const char value2[], int * retval_type_ref, int * retval_sizeof_ref, const int retval_sizeof_alloc, char retval_value[]); 

extern const char * pscript_type__transparent_proxy__get_value(pscript_t * this, const int type2, const int type2_sizeof, const char value2[]); 




#endif /* PSCRIPT_VALUE_H */ 


