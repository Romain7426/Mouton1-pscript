#include <stdio.h>
#include <assert.h>

enum bool_t {true = (0 == 0), false = (0 != 0)};
typedef enum bool_t bool_t;
typedef bool_t bool;



#define SPACE_HUH(c) ((' ' == (c)) || ('\t' == (c)))

//#define READ() 

//static inline bool LOOK_FOR




int main(void) {
  int c;


  // la grande boucle
  do {

    // d'abord on recherche un caractère newline
    do {
      c = getchar();
      if (c == EOF) return 0;
    } while (c != '\n');
    
    // on skippe les espaces
    do {
      c = getchar();
      if (c == EOF) return 0;
    } while (SPACE_HUH(c));
    
    // si le caractère suivant n'est pas un #, on boucle
    if (c != '#')
      continue;

    // on skippe les espaces
    do {
      c = getchar();
      if (c == EOF) return 0;
    } while (SPACE_HUH(c));
    
    // le mot suivant doit être "include"
    if (c != 'i') continue;
    c = getchar(); if (c != 'n') continue;
    c = getchar(); if (c != 'c') continue;
    c = getchar(); if (c != 'l') continue;
    c = getchar(); if (c != 'u') continue;
    c = getchar(); if (c != 'd') continue;
    c = getchar(); if (c != 'e') continue;

    // on skippe les espaces
    do {
      c = getchar();
      if (c == EOF) return -1;
    } while (SPACE_HUH(c));
    
    // le caractère suivant doit être un "
    if (c != '"') continue;

    // on print tant que pas EOF || '"'
    do {
      c = getchar();
      if (c == '"') break;
      if (c == EOF) return -1;
      putchar(c);
    } while (true);

    // on put un newline
    putchar('\n');

  } while (true);
  

  assert(false);
  return -1;
}

