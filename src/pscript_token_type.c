#include "pscript_global.h"
#include "pscript_token_type.h"

const char * pscript_token__type_cstr_array[] = {
  "NULL", 
  "EOF", 
  // RL: Syntatic sugar 
  "OF", 
  "TO", 
  "DOWNTO", 
  "STEP", 
  "PTVIRG", 
  "OPENPAR", 
  "CLOSEPAR", 
  "OPENBRACKET", 
  "CLOSEBRACKET", 
  "OPENBRACE", 
  "CLOSEBRACE", 
  "DEUXPOINTS", 
  "VIRGULE", 
  "AS", 
  "HUH", 
  // RL: EXPR 
  "AFFECTATION", 
  "AFFECTATION_SIMPLE", 
  "AFFECTATION_IADD", 
  "AFFECTATION_RADD", 
  "AFFECTATION_ISUB", 
  "AFFECTATION_RSUB", 
  "AFFECTATION_IMULT", 
  "AFFECTATION_RMULT", 
  "AFFECTATION_IDIV", 
  "AFFECTATION_RDIV", 
  "AFFECTATION_IMOD", 
  "AFFECTATION_L_AND", 
  "AFFECTATION_L_OR", 
  "AFFECTATION_L_XOR", 
  "AFFECTATION_L_NOT", 
  "AFFECTATION_B_AND", 
  "AFFECTATION_B_OR", 
  "AFFECTATION_B_XOR", 
  "AFFECTATION_B_NOT", 
  "AFFECTATION_B_RSHIFT", 
  "AFFECTATION_B_LSHIFT", 
  "LOGICAL_AND", 
  "LOGICAL_OR", 
  "LOGICAL_XOR", // RL: BITWISE_XOR is definitely different. 
  "LOGICAL_NOT", 
  "EQUAL", // RL: The question is the interpretation of "a == b and d != e". 
  "DIFF", 
  "INF", // RL: The question is the interpretation of "a < b and d >= e". 
  "SUP", 
  "INFEQ", 
  "SUPEQ", 
  "IDIV", // RL: This thing has definitely a low priority: "1 + 2 div 4 + 3" 
  "IMOD", 
  "BITWISE_AND", 
  "BITWISE_OR", 
  "BITWISE_XOR", // RL: BOOL_XOR is definitely different. 
  "BITWISE_NOT", 
  "BITWISE_SHIFT_LEFT", 
  "BITWISE_SHIFT_RIGHT", 
  "IPLUS", 
  "RPLUS", 
  "IMOINS", 
  "RMOINS", 
  "IMULT", 
  "RMULT", 
  "RDIV", 
  "POINT", 
  "FLECHE", 
  "PTR", 
  "REF", 
  "INC", 
  "DEC", 
  // RL: Constant values", which have the upmost priorities. 
  "TRUE", 
  "FALSE", 
  "ENTIER", 
  "REEL__VIRG", 
  "REEL__DOT", 
  "REEL__E", 
  "STRING_C", 
  "STRING_C__EOL", 
  "STRING_C__EOF", 
  "STRING_P", 
  "STRING_P__EOL", 
  "STRING_P__EOF", 
  "NIL", 
  "NULL_PTR", 
  // RL: LL(1) type 
  "TYPE", 
  "TYPE_ALIAS", 
  "BOOLEAN", 
  "INTEGER", 
  "FLOAT", 
  "STRING", 
  "SUBRANGE", 
  "ARRAY", 
  "RECORD", 
  "VARIANT", 
  // RL: LL(1) control flow 
  "IF", 
  "WHEN", 
  "UNLESS", 
  "WHILE", 
  "FOR", 
  "REPEAT", 
  "BEGIN", 
  "PROCEDURE", 
  "FONCTION", 
  "METHODE", 
  "LAMBDA", 
  "CONST", 
  "VAR", 
  //"REC", 
  // RL: control flow others 
  "DONE", 
  "DO", 
  "THEN", 
  "ELSE", 
  "FI", 
  "UNTIL", 
  "END", 
  // RL: Other qualifying keywords 
  "BYREF", 
  "BYVAL", 
  // RL: Ident 
  "SYNTAX_MASTER", 
  "SYNTAX_KEYWORD", 
  "IDENT", 
  // RL: Ignored tokens 
  "EOL", 
  "BLANC", 
  "COMMENT__ONE_LINE_CPP", 
  "COMMENT__ONE_LINE_SCRIPT", 
  "COMMENT__MULTI_LINES_C_STYLE", 
  "COMMENT__MULTI_LINES_C_STYLE__EOF", 
  "COMMENT__MULTI_LINES_C_STYLE__LONELY_ENDING",
  NULL
}; 

enum { pscript_token__type_cstr_array__size = ARRAY_SIZE(pscript_token__type_cstr_array) }; 

const int8_t pscript_token_count = PSCRIPT_TOKEN_COUNT; 

static void assert_compile__dkushdsuf2387645kjhsdfr(void) {
  ASSERT_COMPILE(PSCRIPT_TOKEN_COUNT <= INT8_MAX); //127); // 256 // INT8_MAX 
  ASSERT_COMPILE(pscript_token__type_cstr_array__size == PSCRIPT_TOKEN_COUNT + 1); 
}; 



static const char * pscript_token__type_master__cstr_array__shifted[PSCRIPT_TOKEN__SYNTAX_MASTER__SIZE] = {
  "SYNTAX_MASTER_00", 
  "SYNTAX_MASTER_01", 
  "SYNTAX_MASTER_02", 
  "SYNTAX_MASTER_03", 
  "SYNTAX_MASTER_04", 
  "SYNTAX_MASTER_05", 
  "SYNTAX_MASTER_06", 
  "SYNTAX_MASTER_07", 
  "SYNTAX_MASTER_08", 
  "SYNTAX_MASTER_09", 
  "SYNTAX_MASTER_10", 
  "SYNTAX_MASTER_11", 
  "SYNTAX_MASTER_12", 
  "SYNTAX_MASTER_13", 
  "SYNTAX_MASTER_14", 
  "SYNTAX_MASTER_15", 
}; 
static const char * * pscript_token__type_master__cstr_array = pscript_token__type_master__cstr_array__shifted - PSCRIPT_TOKEN__SYNTAX_MASTER__BASE; 


static const char * pscript_token__type_keyword__cstr_array__shifted[PSCRIPT_TOKEN__SYNTAX_KEYWORD__SIZE] = {
  "SYNTAX_KEYWORD_00", 
  "SYNTAX_KEYWORD_01", 
  "SYNTAX_KEYWORD_02", 
  "SYNTAX_KEYWORD_03", 
  "SYNTAX_KEYWORD_04", 
  "SYNTAX_KEYWORD_05", 
  "SYNTAX_KEYWORD_06", 
  "SYNTAX_KEYWORD_07", 
  "SYNTAX_KEYWORD_08", 
  "SYNTAX_KEYWORD_09", 
  "SYNTAX_KEYWORD_10", 
  "SYNTAX_KEYWORD_11", 
  "SYNTAX_KEYWORD_12", 
  "SYNTAX_KEYWORD_13", 
  "SYNTAX_KEYWORD_14", 
  "SYNTAX_KEYWORD_15", 
}; 
static const char * * pscript_token__type_keyword__cstr_array = pscript_token__type_keyword__cstr_array__shifted - PSCRIPT_TOKEN__SYNTAX_KEYWORD__BASE; 




const char * pscript_token__type_get_cstr(const uint8_t token_type) { 
  if (token_type < PSCRIPT_TOKEN_COUNT) { return pscript_token__type_cstr_array[token_type]; }; 
  if (token_type < PSCRIPT_TOKEN__SYNTAX_MASTER__TOP) { return pscript_token__type_master__cstr_array[token_type]; }; 
  if (token_type < PSCRIPT_TOKEN__SYNTAX_KEYWORD__TOP) { return pscript_token__type_keyword__cstr_array[token_type]; }; 
  return NULL; //"UNKNOWN_TOKEN_TYPE"; 
}; 

