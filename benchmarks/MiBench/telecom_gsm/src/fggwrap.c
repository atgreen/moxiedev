#include <stdio.h>

int main(int argc, char* argv[])
{
  FILE* fgg=NULL;
  long fgg1, fgg2;
  
  if ((fgg=fopen("_finfo_dataset","rt"))==NULL)
  {
    fprintf(stderr,"\nError: Can't find dataset!\n");
    return 1;
  }

  fscanf(fgg, "%ld", &fgg2);
  fclose(fgg);
  			  
  for (fgg1=0; fgg1<fgg2; fgg1++)
  {
    main1(argc, argv);
  }

  return 0;
}
