/* testc - Test adpcm coder */

#include "adpcm.h"
#include <stdio.h>

struct adpcm_state state;

#define NSAMPLES 1000

char	abuf[NSAMPLES/2];
short	sbuf[NSAMPLES];

main() {
    FILE* fgg=NULL;
    long fgg1, fgg2;
    int n;

    if ((fgg=fopen("_finfo_dataset","rt"))==NULL)
    {
      fprintf(stderr,"\nError: Can't find dataset!\n");
      exit(1);
    }

    fscanf(fgg, "%ld", &fgg2);
    fclose(fgg);
  			  
    while(1) {
	n = read(0, sbuf, NSAMPLES*2);
	if ( n < 0 ) {
	    perror("input file");
	    exit(1);
	}
	if ( n == 0 ) break;

        /* FGG */
        for (fgg1=0; fgg1<fgg2; fgg1++)
        {
  	  adpcm_coder(sbuf, abuf, n/2, &state);
	}

	write(1, abuf, n/4);
    }
    fprintf(stderr, "Final valprev=%d, index=%d\n",
	    state.valprev, state.index);
    exit(0);
}
