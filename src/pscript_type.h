#ifndef PSCRIPT_TYPE_H
#define PSCRIPT_TYPE_H

#ifndef PSCRIPT_TYPE_ENV_T 
#define PSCRIPT_TYPE_ENV_T 
struct pscript_type_env_t; 
typedef struct pscript_type_env_t pscript_type_env_t; 
#endif 
extern  const int32_t              pscript_type_env__sizeof; 
// RL: Use for storage, where data should be packed. Anywhere else, 'int' is good. The property ensures that a type will fit into a 'int_pscript_type_t'. 
typedef       int16_t              int_pscript_type_t; 
extern  const int_pscript_type_t   int_pscript_type_max; 

enum { 
 PSCRIPT_TYPE__NULL, // RL: That type should not be used. 
 PSCRIPT_TYPE__VOID, 
 PSCRIPT_TYPE__ANY, 
 PSCRIPT_TYPE__BOOL, 
 PSCRIPT_TYPE__INTEGER, 
 PSCRIPT_TYPE__INT_RANGE, 
 PSCRIPT_TYPE__INT_RANGE__DYN, 
 PSCRIPT_TYPE__FLOAT, 
 PSCRIPT_TYPE__CHAR, 
 PSCRIPT_TYPE__STRING, 
 PSCRIPT_TYPE__NIL_LIST, // RL: List 
 PSCRIPT_TYPE__NIL_PTR, // RL: Pointer 
 PSCRIPT_TYPE__VARIANT, // RL: With the current implementation, the subtype cannot be in the type; 
                        //     it must be in the value. The issue being that the sub-type is unknown 
                        //     until the first assignment. But then the type must be changed. And that 
                        //     cannot happen for a function parameter: the arg type of a function cannot 
                        //     be changed. 
                        //     Therefore, the sub-type must be in the value. 
                        // RL: Second issue: As the fact that the value has not been assignement yet cannot 
                        //     be put in the type, it must be written in the value, and therefore the value 
                        //     must be initialized...! 
                        //     The value cannot be initialized at compile-time (no VM, no memory). 
                        //     I do not have any initializing procedures, and I do not want any. 
                        //     But I still need a solution. 
                        //     One solution is to have an initialized memory at compile-time (zero is good enough). 
                        //     So I need a second kind of compile-time memory. Great. 
 PSCRIPT_TYPE__POINTER, // RL: A pointer is never the owner of an object. It does not have to do any memory management. 
 PSCRIPT_TYPE__PROXY, 
 PSCRIPT_TYPE__PROXY_BYTECODE, 
 PSCRIPT_TYPE__LIST, // RL: Two variants. 
 PSCRIPT_TYPE__ARRAY, 
 PSCRIPT_TYPE__ARRAY__DYN, 
 PSCRIPT_TYPE__RECORD, 
 PSCRIPT_TYPE__FUNPROC, 
 PSCRIPT_TYPE__FFIPROC, 
 PSCRIPT_TYPE__LIBPROC, 
 PSCRIPT_TYPE__LAMBDA_META, // RL: What's that, huh? A lambda object is a purely run-time object, as it needs to know references to memory frames to bind names. 
                            //     However, we still need to manipulate representation of such objects at compile-time. Here are lambda_meta. 
                            //     Especially, lambda objects are variable-sized objects, while lambda-meta objects are constant-sized objects. 
 PSCRIPT_TYPE__LAMBDA, 
 PSCRIPT_TYPE__FUNSIGNATURE, 
 PSCRIPT_TYPE__USER, 
 PSCRIPT_TYPE__COUNT 
}; 

// RL: The basic difference between FUNPROC & LAMBDA is that FUNPROC is a scalar and LAMBDA is an object. 
//      - FUNPROC must be a top-level function. There are only run-time environments: the top-level, and the fun-level. So the function address is enough to describe it. 
//        Such a variable is bound by "function fact(n) begin ... end;".  
//        On that matter, this is not a variable, but a constant. 
//      - FUNSIGNATURE is a type of a variable, not a constant. Such a variable can be assigned functions or lambdas. 
//        It cannot be the type of an actual object in memory. 
//        Its sizeof is the size of a pointer. However, the way it should be interpreted depends on the object actually hold in it. 
//        The first bit of that pointer is a flag: 0 means it is a regular FUNPROC; 1 means it's a lambda. 
//        That way a standard funproc can be easily converted into a funsignature. 
//      - LAMBDA is a lambda object. It is too large to be a scalar. It's still a function though. 
//        It has a subtype: a FUNPROC. 
 
enum { 
 PSCRIPT_METHODE_TYPE__FUNPROC, 
 PSCRIPT_METHODE_TYPE__FFIPROC, 
 // RL: A method cannot be a lambda. A method is 100% static. Does not make sense otherwise. 
 PSCRIPT_METHODE_TYPE__COUNT 
}; 


extern const int scalar_types[]; 
#define SCALAR_TYPES__LIST PSCRIPT_TYPE__VOID, PSCRIPT_TYPE__NULL, PSCRIPT_TYPE__BOOL, PSCRIPT_TYPE__INTEGER, PSCRIPT_TYPE__INT_RANGE, PSCRIPT_TYPE__INT_RANGE__DYN, PSCRIPT_TYPE__FLOAT, PSCRIPT_TYPE__CHAR, PSCRIPT_TYPE__STRING, PSCRIPT_TYPE__NIL_LIST, PSCRIPT_TYPE__NIL_PTR, PSCRIPT_TYPE__VARIANT, PSCRIPT_TYPE__POINTER, PSCRIPT_TYPE__FUNPROC, PSCRIPT_TYPE__FFIPROC, PSCRIPT_TYPE__LIBPROC 
//, PSCRIPT_TYPE__FUN 
extern int pscript_type__scalar_huh__core(const int pscript_type_core); 
extern int pscript_type__scalar_huh(const pscript_type_env_t * this, const int pscript_type); 


extern const char * pscript_type_core__cstr(const int pscript_type_core); 
#define pscript_type_core__get_cstr pscript_type_core__cstr

 
extern pscript_type_env_t * pscript_type_env__make(pscript_string_env_t * string_env); 
extern void pscript_type_env__delete(pscript_type_env_t * this);  

//extern int pscript_type_env__cell_alloc(pscript_type_env_t * this); 

extern int pscript_type__get_count(const pscript_type_env_t * this); 

extern int pscript_type__name__push_nolookup(pscript_type_env_t * this, const int name, const int type_i); 
extern int pscript_type__name__lookup(const pscript_type_env_t * this, const int name); 
#define pscript_type__name__push pscript_type__name__push_nolookup
extern int pscript_type__name__get_value(const pscript_type_env_t * this, const int type_env_i); 

extern void pscript_type__name__set_type__for_nb_names_on_top(pscript_type_env_t * this, const int type_i, const int env_nb); 

extern void pscript_type__push_standard_types(pscript_type_env_t * this); 

 
extern int pscript_type__get_core_type(const pscript_type_env_t * this, const int type_i); 
extern pscript_string_env_t * pscript_type__get_string_env(pscript_type_env_t * this); 

//extern int pscript_type__get__null   (const pscript_type_env_t * this); 
extern int pscript_type__get__void    (const pscript_type_env_t * this); 
extern int pscript_type__get__any     (const pscript_type_env_t * this); 
extern int pscript_type__get__bool    (const pscript_type_env_t * this); 
extern int pscript_type__get__integer (const pscript_type_env_t * this); 
extern int pscript_type__get__float   (const pscript_type_env_t * this); 
extern int pscript_type__get__char    (const pscript_type_env_t * this); 
extern int pscript_type__get__string  (const pscript_type_env_t * this); 
extern int pscript_type__get__nil_list(const pscript_type_env_t * this); 
extern int pscript_type__get__nil_ptr (const pscript_type_env_t * this); 
extern int pscript_type__get__variant (const pscript_type_env_t * this); 
extern int pscript_type__get__list    (const pscript_type_env_t * this); 
 
#define pscript_type__get__empty pscript_type__get__void

extern int pscript_type__get__int_range(const pscript_type_env_t * this, const int i_min, const int i_max); 
extern int pscript_type__make__int_range__no_lookup(pscript_type_env_t * this, const int i_min, const int i_max); 
extern int pscript_type__get_or_make__int_range(pscript_type_env_t * this, const int i_min, const int i_max); 
#define pscript_type__make__int_range pscript_type__get_or_make__int_range 
extern int pscript_type__int_range__get_i_min(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__int_range__get_i_max(const pscript_type_env_t * this, const int type_i); 


extern int pscript_type__get__int_range_dyn(const pscript_type_env_t * this, const int i_min__mem_offset, const int i_max__mem_offset); 

extern int pscript_type__make__pointer(pscript_type_env_t * this, const int subtype); 
extern int pscript_type__make__proxy(pscript_type_env_t * this, const int subtype); 
extern int pscript_type__make__proxy_bytecode(pscript_type_env_t * this, const int subtype); 
extern int pscript_type__make__array(pscript_type_env_t * this, const int subtype, const int i_min, const int i_max); 
extern int pscript_type__make__array_dyn(pscript_type_env_t * this, const int subtype, const int i_min__mem_offset, const int i_max__mem_offset); 
extern int pscript_type__make__record(pscript_type_env_t * this, const int field_nb, const int field_name[], const int field_type[]); 
extern int pscript_type__make__proc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[]); 
extern int pscript_type__make__fun(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int fun_retval_type); 
extern int pscript_type__make__funproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type); 
extern int pscript_type__make__ffiproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type); 
//extern int pscript_type__make__libproc(pscript_type_env_t * this, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type); 
extern int pscript_type__make__libproc(pscript_type_env_t * this, const int arg_nb, const int arg_byref[]); 
extern int pscript_type__make__lambda(pscript_type_env_t * this, const int lexical_env_nb, const int arg_nb, const int arg_name[], const int arg_byref[], const int arg_type[], const int retval_type); 
//extern int pscript_type__make__user_defined_type(const char * type_name); 
extern int pscript_type__make__user_defined_type(pscript_type_env_t * this, const int udf_name, const int subtype); 
extern int pscript_type__get__user_defined_type(const pscript_type_env_t * this, const int type_name); 

extern int pscript_type__make__record__empty(pscript_type_env_t * this); 
extern int pscript_type__make__record__push_field(pscript_type_env_t * this, int record_i, const int field_name, const int field_type); 

extern int pscript_type__pointer__get_subtype(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__proxy__get_subtype(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__proxy_bytecode__get_subtype(const pscript_type_env_t * this, const int type_i); 
//extern int pscript_type__variant__get_subtype(const pscript_type_env_t * this, const int type_i); 

extern int pscript_type__array__get_subtype(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__array__get_i_min  (const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__array__get_i_max  (const pscript_type_env_t * this, const int type_i); 

extern int pscript_type__record__get_field_nb    (const pscript_type_env_t * this, const int record_i); 
extern int pscript_type__record__get_field_index (const pscript_type_env_t * this, const int record_i, const int field_ident_pstr); 
extern int pscript_type__record__get_field_name  (const pscript_type_env_t * this, const int record_i, const int field_i); 
extern int pscript_type__record__get_field_type  (const pscript_type_env_t * this, const int record_i, const int field_i); 
extern int pscript_type__record__get_field_offset(const pscript_type_env_t * this, const int record_i, const int field_i); 
extern int pscript_type__record__get_field_sizeof(      pscript_type_env_t * this, const int record_i, const int field_i); 

extern int pscript_type__user_defined_type__get_name(pscript_type_env_t * this, const int type_i); 
extern const char * pscript_type__user_defined_type__get_name_cstr(pscript_type_env_t * this, const int type_i); 
extern int pscript_type__user_defined_type__get_subtype(pscript_type_env_t * this, const int type_i); 
extern void pscript_type__name__define_udf__for_nb_names_on_top(pscript_type_env_t * this, const int ident_nb, const int subtype); 

extern int pscript_type__funproc__get_retval_type(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__ffiproc__get_retval_type(const pscript_type_env_t * this, const int type_i); 

extern int pscript_type__funproc__get_arg_nb(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__funproc__get_arg_byref_huh(const pscript_type_env_t * this, const int type_i, const int arg_i); 

extern int pscript_type__ffiproc__get_arg_nb(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__ffiproc__get_at_least_arg_nb(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__ffiproc__get_arg_byref_huh(const pscript_type_env_t * this, const int type_i, const int arg_i); 

extern int pscript_type__methode__lookup(pscript_type_env_t * this, const int type_i, const int name); 
extern int pscript_type__methode__push_no_lookup(pscript_type_env_t * this, const int type_i, const int name, const int methode_type, const int funproc_i); 
extern int pscript_type__methode__push_lookup(pscript_type_env_t * this, const int type_i, const int name, const int methode_type, const int funproc_i); 
#define pscript_type__methode__push pscript_type__methode__push_lookup 
#define pscript_type__methode__push_funproc(this,type_i,name,funproc_i) pscript_type__methode__push((this),(type_i),(name),PSCRIPT_METHODE_TYPE__FUNPROC,(funproc_i))
#define pscript_type__methode__push_ffiproc(this,type_i,name,ffiproc_i) pscript_type__methode__push((this),(type_i),(name),PSCRIPT_METHODE_TYPE__FFIPROC,(ffiproc_i))


extern int pscript_type__methode__get_nb(const pscript_type_env_t * this, const int type_i); 
extern int pscript_type__methode__get_name(const pscript_type_env_t * this, const int type_i, const int method_i); 
extern int pscript_type__methode__get_funproc(const pscript_type_env_t * this, const int type_i, const int method_i); 
extern int pscript_type__methode__get_ffiproc(const pscript_type_env_t * this, const int type_i, const int method_i); 
extern int pscript_type__methode__get_type(const pscript_type_env_t * this, const int type_i, const int method_i); 


extern int pscript_type__sizeof(pscript_type_env_t * this, const int type_i); 
extern int pscript_type__sizeof__pointer(pscript_type_env_t * this); 
extern int pscript_type__sizeof__proxy(pscript_type_env_t * this); 
extern int pscript_type__sizeof__proxy_bytecode(pscript_type_env_t * this); 
#define  pscript_type__get__sizeof  pscript_type__sizeof 
//extern int pscript_type__sizeof__transparent_proxy(pscript_type_env_t * this, const int type_i); 

#define pscript_type__transparent_proxy__sizeof  pscript_type__sizeof__transparent_proxy 
extern int pscript_type__transparent_proxy__get_core_type(pscript_type_env_t * this, const int type_i); 
extern int pscript_type__transparent_proxy__get_type     (pscript_type_env_t * this, const int type_i); 
extern int pscript_type__transparent_proxy__get_sizeof   (pscript_type_env_t * this, const int type_i); 
//#define pscript_type__transparent_proxy__get_sizeof  pscript_type__sizeof__transparent_proxy 
 
extern int pscript_type__string(pscript_type_env_t * this, const int type_i); 
extern int pscript_type__string_expanded(pscript_type_env_t * this, const int type_i); 

extern const char * pscript_type__get_cstr(pscript_type_env_t * this, const int type_i); 
#define pscript_type__get_pstr pscript_type__string 


#if 0
extern void pscript_type__udf__stack;
#endif 




#endif /* PSCRIPT_TYPE_H */




