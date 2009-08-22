
#define MSG "Hello, World!\n"

void __attribute__((noinline)) gloss_write (int fd, char *ptr, int len) 
{
  asm("swi 5");
}

int main(int argc, char *argv[])
{
#if 0
  while (1)
    gloss_write (0, MSG, strlen(MSG));
#else
  while (1)
    {
      int i = 0;
      puts ("*********************** Hello World from uClibc! **********");
      printf ("argc = %d\n", argc);
      while (i < argc)
	printf ("argv[%d] = \"%s\"\n", i, argv[i++]);
    }
#endif
  return 0;
}
