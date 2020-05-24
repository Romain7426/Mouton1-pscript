#include "pscript_global.h"
#include "pscript_token_type.h"
#include "pscript_token.h"
#include "pscript_token_automata.h"
#include "pscript_input.h"
#include "pscript_token_parser.h"



// =================================================================================================== 
// LEXING / TOKENIZATION 

int pscript_token__parser(pscript_input_env_t * input_env, const int input_i, pscript_token_env_t * token_env) { 
  int recognizing_automaton_index; 
  int current_prefix_total_length; 
  int current_prefix_length_recognized; 
  //int8_t lexer_automata_states[PSCRIPT_TOKEN_COUNT]; 
  int8_t lexer_automata_states[pscript_token_automata__size]; 

  const int    input_filename_pstr = pscript_input__get_filename_pstr(input_env, input_i); 
  const char * input_filename_cstr = pscript_input__get_filename_cstr(input_env, input_i); 
  
  int offset0 = 0; 
  int line1 = 1; 
  int line1_offset0 = 0; 
  
  int c; 
  
  pscript_token__raz(token_env); 
  //input_reading__init(); // RL: Was already initialized when the input stream was opened/assigned. 
  
  
  recognizing_automaton_index = -1; 
  current_prefix_total_length = 0; 
  current_prefix_length_recognized = 0; 
  bzero(lexer_automata_states, sizeof(lexer_automata_states)); 
  
  for (;;) { 
    //c = input_reading__getc(); 
    c = pscript_input__getc(input_env, input_i); 
#if 0  
    dputs_array(stderr, "c = [", int_string__stack(c), "] ", (const char []) { (char)c, '\0'}, "\n"); 
#endif
    if (-2 == c) { 
      const char * srcval_rstr    = pscript_input__get_srcval_rstr(input_env, input_i); 
      const int    srcval_len     = pscript_input__get_srcval_len (input_env, input_i); 
      char srcval_cstr[srcval_len+1]; bcopy(srcval_rstr, srcval_cstr, srcval_len);  srcval_cstr[srcval_len] = '\0'; 
      const char msg[] = "There was an error while reading the input stream."; 
      const char * err_str = strconcat__stack(input_filename_cstr, ":", int_string__stack(line1), ":", int_string__stack(offset0 - line1_offset0), ":", " ", msg, srcval_cstr, "'"); 
#if 0 
      error_state -> code1 = -8; 
      error_state -> msg1  = "There was an error while reading the input stream."; 
      error_state -> str1  = err_str; 
#endif 
#if 1 
      dputs_array(stderr__fileno, err_str, "\n"); 
#else 
      assert(false); 
#endif 
      return false; 
    }; 
    if (EOF != c) current_prefix_total_length++; 
    
    // Moving forward in the automata. 
    //int_array__print(stderr, TOKEN_TYPE__COUNT, lexer_automata_states); dput_eol(stderr); 
    //for (int i = 1; i < PSCRIPT_TOKEN_COUNT; i++) { 
    for (int i = 0; i < pscript_token_automata__size; i++) { 
#if 1 
      lexer_automata_states[i] = pscript_token_automata__read_symbol(pscript_token_automata, i, lexer_automata_states[i], c); 
#else 
      pscript_token_automata_t * one_automaton; 
      one_automaton = token_automata[i]; 
      lexer_automata_states[i] = one_automaton(lexer_automata_states[i], c); 
#endif 
    }; 
    //int_array__print(stderr, TOKEN_TYPE__COUNT, lexer_automata_states); dput_eol(stderr); 
    
    // Is there one in a final state? 
    //for (int i = 1; i < PSCRIPT_TOKEN_COUNT; i++) {
    for (int i = 0; i < pscript_token_automata__size; i++) { 
      if (PSCRIPT_TOKEN_AUTOMATA__FINAL_STATE_BASE > lexer_automata_states[i]) continue; 
      recognizing_automaton_index = i; 
      current_prefix_length_recognized = current_prefix_total_length; 
      break; 
    }; 
    
    // Is there at least one automaton still alive? 
    int at_least_one_alive = -1; //false; 
    //for (int i = 1; i < PSCRIPT_TOKEN_COUNT; i++) {
    for (int i = 0; i < pscript_token_automata__size; i++) { 
      if (0 > lexer_automata_states[i]) continue; 
      at_least_one_alive = i; //true; 
      break; 
    }; 
    
    
#if 0 
    dputs_array(stderr, "at_least_one_alive = ", int_string__stack(at_least_one_alive), "\n"); 
#endif 
    if (at_least_one_alive >= 0) continue; 
    
    
#if 0 
    dputs(stderr, "No more alive\n"); 
#endif 
    
    
    if (-1 == recognizing_automaton_index) { 
      const char * srcval_rstr    = pscript_input__get_srcval_rstr(input_env, input_i); 
      const int    srcval_len     = pscript_input__get_srcval_len (input_env, input_i); 
      char srcval_cstr[srcval_len+1]; bcopy(srcval_rstr, srcval_cstr, srcval_len);  srcval_cstr[srcval_len] = '\0'; 
      const char * err_str = strconcat__stack(input_filename_cstr, ":", int_string__stack(line1), ":", int_string__stack(offset0 - line1_offset0), ":", " ", "Failure to recognize this input as a token: '", srcval_cstr, "'"); 
#if 0 
      error_state -> code1 = -7; 
      error_state -> str1 = err_str; 
#endif 
#if 1
      dputs_array(stderr__fileno, err_str, "\n"); 
#else 
      assert(false); 
#endif 
      return false; 
    }; 


    // RL: Something was recognized. 
    const int recognized_token_type = pscript_token_automata__get_token_type(/*automata*/pscript_token_automata, /*automaton_i*/recognizing_automaton_index); 
#if 0 
    dputs_array(stderr, __func__, ": ", pscript_token__type_get_cstr(recognizing_automaton_index), ":", pscript_token__type_get_cstr(recognized_token_type), "(", int_string__stack(recognized_token_type), ") \n"); 
#endif 


    
    if (PSCRIPT_TOKEN_EOF == recognized_token_type) { break; }; 
    

    const int nb_chars_to_push_back = (current_prefix_total_length - current_prefix_length_recognized); 
    //for (int i = 0; i < nb_chars_to_push_back; i++) { input_reading__move_backward(); }; 
    pscript_input__move_backward(input_env, input_i, nb_chars_to_push_back); 
    
    
    //if (PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__EOF == recognized_token_type) {
    if (int_member_huh(recognized_token_type,PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__EOF,PSCRIPT_TOKEN_STRING_C__EOL, PSCRIPT_TOKEN_STRING_C__EOF, PSCRIPT_TOKEN_STRING_P__EOL, PSCRIPT_TOKEN_STRING_P__EOF,PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__LONELY_ENDING)) { 
      const char * srcval_rstr    = pscript_input__get_srcval_rstr(input_env, input_i); 
      const int    srcval_len     = pscript_input__get_srcval_len (input_env, input_i); 
#if 0 
      dputs_array(stderr__fileno, "\t\t", "srcval_len: ", int_string__stack(srcval_len), "\n"); 
#endif 
      char srcval_cstr[srcval_len+1]; bcopy(srcval_rstr, srcval_cstr, srcval_len);  srcval_cstr[srcval_len] = '\0'; 
      for (int i = 0; i < srcval_len; i++) { if ('\n' != srcval_cstr[i]) continue; srcval_cstr[i] = '\0'; break; }; 
      const char * msg; 
      switch (recognized_token_type) { 
      default: msg = "<\?\?\?>"; break; 
      case PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__EOF: msg = "A starting multi-lines C-like comment (this kind of comment: /* .... */) was started, but was not closed"; break; 
      case PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__LONELY_ENDING: msg = "A multi-lines C-like comment (this kind of comment: /* .... */) ending symbol was found (this: \"*/\"), but it does not have any starting symbol matching (this: \"/*\")"; break;
      case PSCRIPT_TOKEN_STRING_C__EOL: msg = "A C-string (this kind of string: \" .... \") was started, but contains a raw newline; please use the escape char '\\n' to input newline characters in a C-string"; break; 
      case PSCRIPT_TOKEN_STRING_C__EOF: msg = "A C-string (this kind of string: \" .... \") was started, but was not closed"; break; 
      case PSCRIPT_TOKEN_STRING_P__EOL: msg = "A P-string (this kind of string: \' .... \') was started, but contains a raw newline; please use the escape char '\\n' to input newline characters in a P-string (or use \"newline\": \"'Hello' + newline + 'world'\")"; break; 
      case PSCRIPT_TOKEN_STRING_P__EOF: msg = "A P-string (this kind of string: \' .... \') was started, but was not closed"; break;  
      }; 
      const char * err_str = strconcat__stack(input_filename_cstr, ":", int_string__stack(line1), ":", int_string__stack(offset0 - line1_offset0), ":", " ",  msg, ": '", srcval_cstr, "'"); 
#if 0 
      error_state -> code1 = -27; 
      error_state -> str1 = err_str; 
#endif 
#if 1 
      dputs_array(stderr__fileno, err_str, "\n"); 
#else 
      assert(false); 
#endif 
      return false; 
    }; 
    
    //static const int to_be_skipped[] = { TOKEN_TYPE_SPACES, TOKEN_TYPE_EOF, TOKEN_TYPE_EOL, TOKEN_TYPE_COMMENT_ONELINE_DOUBLE_SLASH, TOKEN_TYPE_COMMENT_ONELINE_HASH, TOKEN_TYPE_COMMENT_ONELINE_MIDDLE }; 
    static const int to_be_skipped[] = { PSCRIPT_TOKEN_BLANC, PSCRIPT_TOKEN_EOF, PSCRIPT_TOKEN_EOL, PSCRIPT_TOKEN_COMMENT__ONE_LINE_CPP, PSCRIPT_TOKEN_COMMENT__ONE_LINE_SCRIPT, PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE, PSCRIPT_TOKEN_COMMENT__MULTI_LINES_C_STYLE__EOF };  
    const int should_be_skipped_huh = int_member_array(recognized_token_type, to_be_skipped); 
    if (should_be_skipped_huh) { 
      offset0 += current_prefix_length_recognized; 
      if (PSCRIPT_TOKEN_EOL == recognized_token_type) {
	line1++; 
	line1_offset0 = offset0; 
      }; 
      
      
      pscript_input__validate(input_env, input_i); 
      
      recognizing_automaton_index = -1; 
      current_prefix_total_length = 0; 
      current_prefix_length_recognized = 0; 
      bzero(lexer_automata_states, sizeof(lexer_automata_states)); 
      
      continue; 
    }; 
    
    
    
    int master_token_type = -1; 
    if (PSCRIPT_TOKEN_SYNTAX_MASTER == recognized_token_type) { 
#if 1 
      const int      master__nb   = pscript_token__master_count(token_env); 
      const int  *   master__len  = pscript_token__master_len  (token_env); 
      const char * * master__cstr = pscript_token__master_cstr (token_env); 
      master_token_type  = pscript_token_automaton__get__master(master__nb, master__len, master__cstr, current_prefix_length_recognized); 
      master_token_type += PSCRIPT_TOKEN__SYNTAX_MASTER__BASE; 
#endif 
    }; 
    
    int keyword_token_type = -1; 
    if (PSCRIPT_TOKEN_SYNTAX_KEYWORD == recognized_token_type) { 
#if 1 
      const int      keyword__nb   = pscript_token__syntax_keyword_count(token_env); 
      const int  *   keyword__len  = pscript_token__syntax_keyword_len  (token_env); 
      const char * * keyword__cstr = pscript_token__syntax_keyword_cstr (token_env); 
      keyword_token_type  = pscript_token_automaton__get__master(keyword__nb, keyword__len, keyword__cstr, current_prefix_length_recognized); 
      keyword_token_type += PSCRIPT_TOKEN__SYNTAX_KEYWORD__BASE; 
#endif 
    }; 
    
    
    const int    token_type     = master_token_type >= 0 ? master_token_type : keyword_token_type >= 0 ? keyword_token_type : recognized_token_type ; //recognized_token_type; //recognizing_automaton_index; 
    const char * srcval_rstr    = pscript_input__get_srcval_rstr(input_env, input_i); 
    const int    srcval_len     = pscript_input__get_srcval_len (input_env, input_i); 
#if 0 
    const int    token_index    = pscript_token__push(token_env, srcval_len, srcval_rstr, token_type, input_filename_pstr, offset0, line1, offset0 - line1_offset0); 
#else 
    int          input_line1; INT24_PACKED_READ(input_env -> token_line1[input_i], input_line1);
    const int    input_column0 = input_env -> token_column0[input_i]; 
    const int    token_index   = pscript_token__push(token_env, srcval_len, srcval_rstr, token_type, input_filename_pstr, offset0, input_line1, input_column0); 
#endif 
    if (0 > token_index) { 
      const char * err_str = strconcat__stack(input_filename_cstr, ":", int_string__stack(line1), ":", int_string__stack(offset0 - line1_offset0), ":", " ",  "Token stack overflow (this problem could be overcome in recompiling me with a bigger size for the token stack)."); 
#if 0 
      error_state -> code1 = -22; 
      error_state -> str1 = err_str; 
#endif 
#if 1 
      dputs_array(stderr__fileno, err_str, "\n"); 
#else 
      assert(false); 
#endif 
      return false; 
    }; 
    pscript_input__validate(input_env, input_i); 
    
    offset0 += current_prefix_length_recognized; 
    
    recognizing_automaton_index = -1; 
    current_prefix_total_length = 0; 
    current_prefix_length_recognized = 0; 
    bzero(lexer_automata_states, sizeof(lexer_automata_states)); 
    
    continue; 
  }; 
  
  return true; 
}; 


