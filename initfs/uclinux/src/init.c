#include <string.h>

#define MSG "Hello, World!\n"

void __attribute__((noinline)) gloss_write (int fd, char *ptr, int len) 
{
  asm("swi 5");
}

int main()
{
  while (1)
    gloss_write (0, MSG, strlen(MSG));
  return 0;
}
