#include <stdio.h>

#define MOXIE_EX_DIV0 0 /* Divide by zero */
#define MOXIE_EX_BAD  1 /* Illegal instruction */
#define MOXIE_EX_IRQ  2 /* Interrupt request */
#define MOXIE_EX_SWI  3 /* Software interrupt */

void handler (void)
{
  int et;

  /* Get the exception handler from special register 2.  */
  asm("gsr %0, 2" : "=r"(et) : "0"(et) );

  switch (et)
    {
    case MOXIE_EX_DIV0:
      printf("DIVIDE BY ZERO EXCEPTION\n");
      break;
    case MOXIE_EX_BAD:
      printf("ILLEGAL INSTRUCTION EXCEPTION\n");
      break;
    case MOXIE_EX_IRQ:
      {
	int irq;
	asm("gsr %0, 3" : "=r"(irq) : "0"(irq) );
	printf("INTERRUPT REQUEST %d\n", irq);
      }
      break;
    case MOXIE_EX_SWI:
      {
	int swi;
	asm("gsr %0, 3" : "=r"(swi) : "0"(swi) );
	printf("SOFTWARE INTERRUPT REQUEST %d\n", swi);
      }
      break;
    default:
      printf("UNKNOWN EXCEPTION 0x%x\n", et);
      break;
    }
}

void install_handler(void (*handler)(void))
{
  printf ("Installing handler 0x%x\n", (unsigned) handler);
  asm("ssr %0, 1" : : "r" (handler));
}

int divide (int a, int b)
{
  return a/b;
}

int main()
{
  install_handler((void*)handler);

  puts ("Hello, World!");
#if 0
  divide(2,0);
  asm("swi 1");
  asm("swi 2");
  asm("swi 3");
  asm("swi 4");
#endif
  puts ("Goodbye, World!");
  return 0;
}
