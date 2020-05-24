#ifndef PSCRIPT_MENU_H
#define PSCRIPT_MENU_H

#ifndef PSCRIPT_MENU_ENV_T 
#define PSCRIPT_MENU_ENV_T 
struct pscript_menu_env_t; 
typedef struct pscript_menu_env_t pscript_menu_env_t; 
#endif 

enum { pscript_menu__env__size = (1 << 7) };  
enum { pscript_menu__item__size = (1 << 4) };  
struct pscript_menu_env_t { 
  pscript_string_env_t * string_env; 
  
  int menu__title[pscript_menu__env__size]; 
  int menu__item_nb[pscript_menu__env__size]; 
  int menu__item[pscript_menu__env__size][pscript_menu__item__size]; 
  int menu__nb; 
  
  pscript_menu_env_t * (* make)(pscript_string_env_t * string_env); 
  void (* delete)(pscript_menu_env_t * this);  
  int (* get_count)(const pscript_menu_env_t * this); 
  int (* new)(pscript_menu_env_t * this); 
  int (* set_title)(pscript_menu_env_t * this, const int menu_i, const int title_pstr); 
  int (* push_item)(pscript_menu_env_t * this, const int menu_i, const int item_pstr); 
  int (* get_title)(const pscript_menu_env_t * this, const int menu_i); 
  int (* get_item_count)(const pscript_menu_env_t * this, const int menu_i); 
  int (* get_item_nb)(const pscript_menu_env_t * this, const int menu_i); 
  int (* get_item)(const pscript_menu_env_t * this, const int menu_i, const int item_i); 
}; 

extern pscript_menu_env_t * pscript_menu_env__make(pscript_string_env_t * string_env); 
extern void pscript_menu_env__delete(pscript_menu_env_t * this);  
extern int pscript_menu_env__get_count(const pscript_menu_env_t * this); 
#define pscript_menu_env__get_nb pscript_menu_env__get_count 

extern int pscript_menu__new(pscript_menu_env_t * this); 
extern int pscript_menu__set_title(pscript_menu_env_t * this, const int menu_i, const int title_pstr); 
extern int pscript_menu__push_item(pscript_menu_env_t * this, const int menu_i, const int item_pstr); 

extern int pscript_menu__get_title(const pscript_menu_env_t * this, const int menu_i); 
extern int pscript_menu__get_item_count(const pscript_menu_env_t * this, const int menu_i); 
extern int pscript_menu__get_item(const pscript_menu_env_t * this, const int menu_i, const int item_i); 
#define pscript_menu__get_item_nb pscript_menu__get_item_count 



#endif /* PSCRIPT_MENU_H */



