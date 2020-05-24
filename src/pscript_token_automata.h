#ifndef PSCRIPT_TOKEN_AUTOMATA_H
#define PSCRIPT_TOKEN_AUTOMATA_H


enum { PSCRIPT_TOKEN_AUTOMATA__TYPE__NULL, PSCRIPT_TOKEN_AUTOMATA__TYPE__STRING, PSCRIPT_TOKEN_AUTOMATA__TYPE__FUNCTION, PSCRIPT_TOKEN_AUTOMATA__TYPE__MASTER, PSCRIPT_TOKEN_AUTOMATA__TYPE__SYNTAX_KEYWORD }; 

#ifndef PSCRIPT_TOKEN_AUTOMATA_T 
#define PSCRIPT_TOKEN_AUTOMATA_T 
struct pscript_token_automata_t; 
typedef struct pscript_token_automata_t pscript_token_automata_t; 
#endif  
struct pscript_token_automata_t { uint8_t token_type; int8_t automata_type; int8_t value1; const char * value_ptr1; const void * automata; }; 
extern pscript_token_automata_t pscript_token_automata[]; 
extern const uint8_t pscript_token_automata__size; 
 
enum {  PSCRIPT_TOKEN_AUTOMATA__FINAL_STATE_BASE = (1 << 6) }; //(INT8_MAX >> 1) }; 
#define PSCRIPT_TOKEN_AUTOMATA__STATE_FINAL_HUH(s) ((s) >= PSCRIPT_TOKEN_AUTOMATA__FINAL_STATE_BASE)

extern  int8_t pscript_token_automata__read_symbol   (const pscript_token_automata_t * automata, const uint8_t automaton_i, const int8_t current_state, const int16_t char_to_be_read); 

extern uint8_t pscript_token_automata__get_token_type(const pscript_token_automata_t * automata, const uint8_t automaton_i); 

extern void    pscript_token_automata__set_master(pscript_token_automata_t * automata, const pscript_token_env_t * token_env); 

extern int8_t  pscript_token_automaton__get__master(const int master__nb, const int master__len[], const char * master__array[], const int8_t current_state0); 






#endif /* PSCRIPT_TOKEN_AUTOMATA_H */
