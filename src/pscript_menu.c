#include "pscript_global.h" 
#include "pscript_string.h" 
#include "pscript_menu.h" 








pscript_menu_env_t * pscript_menu_env__make(pscript_string_env_t * string_env) {
  MALLOC_BZERO(pscript_menu_env_t,this); 
  this -> make      = pscript_menu_env__make; 
  this -> delete    = pscript_menu_env__delete; 
  this -> get_count = pscript_menu_env__get_count; 
  ASSIGN_METHOD(pscript_menu,this,new); 
  ASSIGN_METHOD(pscript_menu,this,set_title); 
  ASSIGN_METHOD(pscript_menu,this,push_item); 
  ASSIGN_METHOD(pscript_menu,this,get_title); 
  ASSIGN_METHOD(pscript_menu,this,get_item_count); 
  ASSIGN_METHOD(pscript_menu,this,get_item_nb); 
  //this -> get_item_nb = pscript_menu_env__get_item_nb; 
  ASSIGN_METHOD(pscript_menu,this,get_item); 
  this -> string_env = string_env; 
  return this;
}; 

void pscript_menu_env__delete(pscript_menu_env_t * this) {
  free(this);
};   

int pscript_menu_env__get_count(const pscript_menu_env_t * this) { 
  return this -> menu__nb; 
}; 
 
 
int pscript_menu__new(pscript_menu_env_t * this) {
  static int default_title = -1; 
  if (default_title < 0) { 
    const char * default_title_cstr = "<MENU W/O TITLE>"; 
    default_title = pscript_string__push(this -> string_env, default_title_cstr); 
  }; 
  assert(this -> menu__nb < pscript_menu__env__size); 
  const int index = this -> menu__nb; 
  this -> menu__title[index] = default_title; 
  this -> menu__item_nb[index] = 0; 
  this -> menu__nb++; 
  return index; 
}; 

int pscript_menu__set_title(pscript_menu_env_t * this, const int menu_i, const int title_pstr) {
  this -> menu__title[menu_i] = title_pstr; 
  return menu_i; 
}; 

int pscript_menu__push_item(pscript_menu_env_t * this, const int menu_i, const int item_pstr) {
  const int index = this -> menu__item_nb[menu_i]; 
  assert(index < pscript_menu__item__size); 
  this -> menu__item[menu_i][index] = item_pstr; 
  this -> menu__item_nb[menu_i]++; 
  return index; 
}; 

int pscript_menu__get_title(const pscript_menu_env_t * this, const int menu_i) {
  return this -> menu__title[menu_i]; 
}; 

int pscript_menu__get_item_count(const pscript_menu_env_t * this, const int menu_i) {
  return this -> menu__item_nb[menu_i]; 
}; 

int pscript_menu__get_item(const pscript_menu_env_t * this, const int menu_i, const int item_i) {
  return this -> menu__item[menu_i][item_i]; 
}; 


