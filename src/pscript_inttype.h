#ifndef PSCRIPT_INTTYPE_H
#define PSCRIPT_INTTYPE_H

// *** PSCRIPT_INTTYPE *** 
enum { 
  PSCRIPT_VOID, 
  PSCRIPT_INT8, 
  PSCRIPT_UINT8, 
  PSCRIPT_INT16, 
  PSCRIPT_UINT16, 
  PSCRIPT_INT24, 
  PSCRIPT_UINT24, 
  PSCRIPT_INT32, 
  PSCRIPT_UINT32, 
  PSCRIPT_INT64, 
  PSCRIPT_UINT64, 
}; 

static const int8_t pscript_sizeof[] = {
  0, 
  1,
  1,
  2,
  2,
  3,
  3,
  4,
  4,
  8,
  8,
}; 

#define PSCRIPT_SIZEOF(a) (	\
  (a) == PSCRIPT_VOID   ? 0 :	\
  (a) == PSCRIPT_INT8   ? 1 :				\
  (a) == PSCRIPT_UINT8  ? 1 :				\
  (a) == PSCRIPT_INT16  ? 2 :				\
  (a) == PSCRIPT_UINT16 ? 2 :				\
  (a) == PSCRIPT_INT24  ? 3 :				\
  (a) == PSCRIPT_UINT24 ? 3 :				\
  (a) == PSCRIPT_INT32  ? 4 :				\
  (a) == PSCRIPT_UINT32 ? 4 :				\
  (a) == PSCRIPT_INT64  ? 8 :				\
  (a) == PSCRIPT_UINT64 ? 8 : \
  -1)				

#define PSCRIPT_INTTYPE_MAX(a) (	\
  (a) == PSCRIPT_VOID   ? 0 :	\
  (a) == PSCRIPT_INT8   ? INT8_MAX :				\
  (a) == PSCRIPT_UINT8  ? UINT8_MAX :				\
  (a) == PSCRIPT_INT16  ? INT16_MAX :				\
  (a) == PSCRIPT_UINT16 ? UINT16_MAX :				\
  (a) == PSCRIPT_INT24  ? INT24_MAX :				\
  (a) == PSCRIPT_UINT24 ? UINT24_MAX :				\
  (a) == PSCRIPT_INT32  ? INT32_MAX :				\
  (a) == PSCRIPT_UINT32 ? UINT32_MAX :				\
  (a) == PSCRIPT_INT64  ? INT64_MAX :				\
  (a) == PSCRIPT_UINT64 ? UINT64_MAX : \
  -1)				



#endif /* PSCRIPT_INTTYPE_H */


