
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                            program.c                              **
    **-------------------------------------------------------------------**
    **                 First version: october 25th 2001                  **
    **-------------------------------------------------------------------**/


/******************************************************************************
 *               CLooG : the Chunky Loop Generator (experimental)             *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2001-2005 Cedric Bastoul                                     *
 *                                                                            *
 * This is free software; you can redistribute it and/or modify it under the  *
 * terms of the GNU General Public License as published by the Free Software  *
 * Foundation; either version 2 of the License, or (at your option) any later *
 * version.                                                                   *
 *                                                                            *
 * This software is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with software; if not, write to the Free Software Foundation, Inc.,        *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                     *
 *                                                                            *
 * CLooG, the Chunky Loop Generator                                           *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/
/* CAUTION: the english used for comments is probably the worst you ever read,
 *          please feel free to correct and improve it !
 */


# include <sys/types.h>
# include <sys/time.h>
#include <stdarg.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <unistd.h>
# include "../include/cloog/cloog.h"
#ifdef HAS_SYS_RESOURCE_H
# include <sys/resource.h>
#endif


/******************************************************************************
 *                             Memory leaks hunting                           *
 ******************************************************************************/


/**
 * These global variables are devoted to memory leaks hunting: we
 * want to know at each moment how many Value variables have been allocated
 * since in GMP mode they have to be freed (see domain.c for the declaration).
 * - July 3rd->11th 2003: first version (memory leaks hunt and correction).
 */
 
extern int cloog_value_allocated ;
extern int cloog_value_freed ;
extern int cloog_value_max ;


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/


/**
 * cloog_program_print function:
 * this function is a human-friendly way to display the CloogProgram data
 * structure, it shows all the different fields and includes an indentation
 * level (level) in order to work with others print_structure functions.
 * - July 1st 2005: first version based on the old cloog_program_print function.
 */
void cloog_program_print_structure(FILE *file, CloogProgram *program, int level)
{ int i, j ;

  /* Go to the right level. */
  for (i=0; i<level; i++)
  fprintf(file,"|\t") ;
  
  fprintf(file,"+-- CloogProgram\n") ;
  
  /* A blank line. */
  for (i=0; i<=level+1; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;
  
  /* Print the language. */
  for (i=0; i<=level; i++)
  fprintf(file,"|\t") ;
  fprintf(file, "Language: %c\n",cloog_program_language (program)) ;
  
  /* A blank line. */
  for (i=0; i<=level+1; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;

  /* Print the scattering dimension number. */
  for (i=0; i<=level; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"Scattering dimension number: %d\n",
	  cloog_program_nb_scattdims (program)) ;
  
  /* A blank line. */
  for (i=0; i<=level+1; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;
  
  /* Print the scalar scattering dimension informations. */
  for (i=0; i<=level; i++)
  fprintf(file,"|\t") ;
  if (cloog_program_scaldims (program))
    {
      fprintf (file,"Scalar dimensions:");
      for (i = 0; i < cloog_program_nb_scattdims (program); i++)
	fprintf (file," %d:%d ", i, cloog_program_scaldim (program, i));
      fprintf (file,"\n");
    }
  else
    fprintf (file, "No scalar scattering dimensions\n");
  
  /* A blank line. */
  for (i=0; i<=level+1; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;

  /* Print the parameter and the iterator names. */
  cloog_names_print_structure (file, cloog_program_names (program), level + 1);
 
  /* A blank line. */
  for (i=0; i<=level+1; i++)
  fprintf(file,"|\t") ;
  fprintf(file,"\n") ;
  
  /* Print the context. */
  cloog_domain_print_structure(file, cloog_program_context (program), level+1);
    
  /* Print the loop. */
  cloog_loop_print_structure (file,cloog_program_loop (program), level + 1);

  /* One more time something that is here only for a better look. */
  for (j=0; j<2; j++)
  { for (i=0; i<=level; i++)
    fprintf(file,"|\t") ;
      
    fprintf(file,"\n") ;
  }
}


/**
 * cloog_program_dump_cloog function:
 * This function dumps a CloogProgram structure supposed to be completely
 * filled in a CLooG input file (foo possibly stdout) such as CLooG can
 * rebuild almost exactly the data structure from the input file (the number
 * of scattering functions is lost since they are included inside the
 * iteration domains, this can only lead to a less beautiful pretty printing).
 * WARNING: this function do not respect CloogDomain as an object.
 * - June     27th 2003: first version.
 * - May      15th 2005: (debug) several patches by Kristof Beyls.
 * - November 16th 2005: adaptation for CLooG 0.14.0 structures.
 */
void cloog_program_dump_cloog(FILE * foo, CloogProgram * program)
{ int i;
  CloogLoop * loop ;

  fprintf(foo,
  "# CLooG -> CLooG\n"
  "# This is an automatic dump of a CLooG input file from a CloogProgram data\n"
  "# structure. WARNING: it is highly dangerous and MAY be correct ONLY if\n"
  "# - it has been dumped before loop generation.\n"
  "# - option -noscalars is used (it removes scalar dimensions otherwise)\n"
  "# - option -l is at least the original scattering dimension number\n"
  "# ASK THE AUTHOR IF YOU *NEED* SOMETHING MORE ROBUST\n") ;

  /* Language. */
  if (cloog_program_language (program) == 'c')
  fprintf(foo,"# Language: C\n") ;
  else
  fprintf(foo,"# Language: FORTRAN\n") ;
  fprintf(foo,"%c\n\n", cloog_program_language (program)) ;

  /* Context. */
  fprintf (foo,"# Context (%d parameter(s)):\n",
	   cloog_domain_dim (cloog_program_context (program)));
  cloog_domain_print_structure (foo, cloog_program_context (program), 0);
  fprintf(foo,"1 # Parameter name(s)\n") ;
  for (i = 0; i < cloog_names_nb_parameters (cloog_program_names (program)); i++)
    fprintf (foo, "%s ", cloog_names_parameter_elt (cloog_program_names (program), i));

  /* Statement number. */
  i = 0 ;
  loop = cloog_program_loop (program);
  while (loop != NULL)
  { i++ ;
    loop = cloog_loop_next (loop) ;
  }
  fprintf(foo,"\n\n# Statement number:\n%d\n\n",i) ;

  /* Iteration domains. */
  i = 1 ;
  loop = cloog_program_loop (program) ;
  while (loop != NULL)
  { /* Name of the domain. */
    fprintf(foo,"# Iteration domain of statement %d.\n",i) ;

    /* Number of polyhedra inside the union of disjoint polyhedra.  */
    fprintf (foo, "%d\n", cloog_domain_nb_polyhedra (cloog_loop_domain (loop))) ;

    /* The polyhedra themselves. */
    cloog_domain_print_polyhedra (foo, cloog_loop_domain (loop));
    fprintf(foo,"0 0 0 # For future options.\n\n") ;
    
    i++ ;
    loop = cloog_loop_next (loop) ;
  }
  fprintf(foo,"\n1 # Iterator name(s)\n") ;
  for (i = 0; i < cloog_names_nb_scattering (cloog_program_names (program)); i++)
    fprintf (foo, "%s ", cloog_names_scattering_elt (cloog_program_names (program), i));
  for (i = 0; i < cloog_names_nb_iterators (cloog_program_names (program)); i++)
    fprintf (foo, "%s ", cloog_names_iterator_elt (cloog_program_names (program), i));
  fprintf(foo,"\n\n") ;

  /* Scattering functions (none since included inside domains). */
  fprintf(foo,"# No scattering functions.\n0\n\n") ;
}


/**
 * cloog_program_print function:
 * This function prints the content of a CloogProgram structure (program) into a
 * file (file, possibly stdout).
 * - July 1st 2005: Now this very old function (probably as old as CLooG) is
 *                  only a frontend to cloog_program_print_structure, with a
 *                  quite better human-readable representation.
 */
void cloog_program_print(FILE * file, CloogProgram * program)
{ cloog_program_print_structure(file,program,0) ;
}


static void print_comment(FILE *file, CloogOptions *options,
			  const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  if (options->language == LANGUAGE_FORTRAN) {
    fprintf(file, "! ");
    vfprintf(file, fmt, args);
    fprintf(file, "\n");
  } else {
    fprintf(file, "/* ");
    vfprintf(file, fmt, args);
    fprintf(file, " */\n");
  }
  va_end (args);
}

/**
 * cloog_program_pprint function:
 * This function prints the content of a CloogProgram structure (program) into a
 * file (file, possibly stdout), in a C-like language.
 * - June 22nd 2005: Adaptation for GMP.
 */
void cloog_program_pprint(FILE *file, CloogProgram *program, CloogOptions *options)
{ int i, j, nb_scattering, indentation=0 ;
  CloogStatement * statement ;
  CloogBlockList * blocklist ;
  CloogBlock * block ;
  struct clast_stmt *root;
   
  if (cloog_program_language (program) == 'f')
    options->language = LANGUAGE_FORTRAN ;
  else
    options->language = LANGUAGE_C ;
 
  print_comment(file, options, "Generated from %s by %s in %.2fs.",
		options->name, cloog_version(), options->time);
#ifdef CLOOG_MEMORY
  print_comment(file, options, "CLooG asked for %d KBytes.", options->memory);
  fprintf(stderr, "[CLooG]INFO: %.2fs and %dKB used for code generation.\n",
	  options->time,options->memory);
#endif
  
  /* If the option "compilable" is set, we provide the whole stuff to generate
   * a compilable code. This code just do nothing, but now the user can edit
   * the source and set the statement macros and parameters values.
   */
  nb_scattering = cloog_program_nb_scattdims (program) ;
  if (options->compilable && (cloog_program_language (program) == 'c'))
  { /* The headers. */
    fprintf(file,"/* DON'T FORGET TO USE -lm OPTION TO COMPILE. */\n\n") ;
    fprintf(file,"/* Useful headers. */\n") ;
    fprintf(file,"#include <stdio.h>\n") ;
    fprintf(file,"#include <stdlib.h>\n") ;
    fprintf(file,"#include <math.h>\n\n") ;

    /* The value of parameters. */
    fprintf(file,"/* Parameter value. */\n") ;
    for (i = 1; i <= cloog_names_nb_parameters (cloog_program_names (program)); i++)
      fprintf(file, "#define PARVAL%d %d\n", i, options->compilable);
    
    /* The macros. */
    fprintf(file,"/* Useful macros. */\n") ;
    fprintf(file,"#define ceild(n,d)  ceil(((double)(n))/((double)(d)))\n") ;
    fprintf(file,"#define floord(n,d) floor(((double)(n))/((double)(d)))\n") ;
    fprintf(file,"#define max(x,y)    ((x) > (y)? (x) : (y))  \n") ; 
    fprintf(file,"#define min(x,y)    ((x) < (y)? (x) : (y))  \n\n") ; 

    /* The statement macros. */
    fprintf(file,"/* Statement macros (please set). */\n") ;
    blocklist = cloog_program_blocklist (program) ;
    while (blocklist != NULL)
      {
	block = cloog_block_list_block (blocklist) ;
	statement = cloog_block_stmt (block) ;
	while (statement != NULL)
	  {
	    fprintf (file, "#define S%d(", cloog_statement_number (statement));
	    if (cloog_block_depth (block) > 0)
	      { 
		fprintf (file, "%s", cloog_names_iterator_elt (cloog_program_names (program), 0));
		for (j = 1; j < cloog_block_depth (block); j++)
		  fprintf (file, ",%s", cloog_names_iterator_elt (cloog_program_names (program), j)) ;
	      }
	    fprintf(file,") {total++;") ;
	    if (cloog_block_depth (block) > 0)
	      {
		fprintf (file, " printf(\"S%d %%d", cloog_statement_number (statement));
		for (j = 1; j < cloog_block_depth (block); j++)
		  fprintf (file, " %%d");
          
		fprintf(file,"\\n\",%s", cloog_names_iterator_elt (cloog_program_names (program), 0));
		for (j = 1;j < cloog_block_depth (block); j++)
		  fprintf (file, ",%s", cloog_names_iterator_elt (cloog_program_names (program), j)) ;
		fprintf (file, ");");
	      }
	    fprintf(file,"}\n") ;
        
	    statement = cloog_statement_next (statement);
	  }
	blocklist = cloog_block_list_next (blocklist) ;
      }
    
    /* The iterator and parameter declaration. */
    fprintf(file,"\nint main() {\n") ; 
    if ((cloog_names_nb_scalars (cloog_program_names (program)) > 0) && (!options->csp))
    { fprintf(file,"  /* Scalar dimension iterators. */\n") ;
      fprintf (file,"  int %s", cloog_names_scalar_elt (cloog_program_names (program), 0));
      for (i = 2; i <= cloog_names_nb_scalars (cloog_program_names (program)); i++)
	fprintf (file, ", %s", cloog_names_scalar_elt (cloog_program_names (program), i - 1));
      
      fprintf(file," ;\n") ;
    }
    if (cloog_names_nb_scattering (cloog_program_names (program)) > 0)
    { fprintf(file,"  /* Scattering iterators. */\n") ;
      fprintf (file, "  int %s", cloog_names_scattering_elt (cloog_program_names (program), 0));
      for(i=2;i<=cloog_names_nb_scattering (cloog_program_names (program));i++)
	fprintf (file, ", %s", cloog_names_scattering_elt (cloog_program_names (program), i - 1)); 
      
      fprintf(file," ;\n") ;
    }
    if (cloog_names_nb_iterators (cloog_program_names (program)) > 0)
    { fprintf(file,"  /* Original iterators. */\n") ;
      fprintf (file,"  int %s", cloog_names_iterator_elt (cloog_program_names (program), 0)) ; 
      for(i=2;i<=cloog_names_nb_iterators (cloog_program_names (program));i++)
	fprintf(file,", %s", cloog_names_iterator_elt (cloog_program_names (program), i-1)) ; 
      
      fprintf(file," ;\n") ;
    }
    if (cloog_names_nb_parameters (cloog_program_names (program)) > 0)
    { fprintf(file,"  /* Parameters. */\n") ;
      fprintf(file, "  int %s=PARVAL1", cloog_names_parameter_elt (cloog_program_names (program), 0));
      for(i=2;i<=cloog_names_nb_parameters (cloog_program_names (program));i++)
        fprintf(file, ", %s=PARVAL%d", cloog_names_parameter_elt (cloog_program_names (program), i - 1), i);
      
      fprintf(file,";\n");
    }
    fprintf(file,"  int total=0;\n");
    fprintf(file,"\n") ;
    
    /* And we adapt the identation. */
    indentation += 2 ;
  }
  
  root = cloog_clast_create(program, options);
  pprint(file, root, indentation, options);
  cloog_clast_free(root);
  
  /* The end of the compilable code in case of 'compilable' option. */
  if (options->compilable && (cloog_program_language (program) == 'c'))
  { fprintf(file,"\n  printf(\"Number of integral points: %%d.\\n\",total) ;") ;
    fprintf(file,"\n  return 0 ;\n}\n") ;
  }
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/**
 * cloog_program_free function:
 * This function frees the allocated memory for a CloogProgram structure.
 */
void cloog_program_free(CloogProgram * program)
{ cloog_names_free(cloog_program_names (program)) ;
  cloog_loop_free(cloog_program_loop (program)) ;
  cloog_domain_free (cloog_program_context (program)) ;
  cloog_block_list_free (cloog_program_blocklist (program)) ;
  if (cloog_program_scaldims (program))
    free (cloog_program_scaldims (program));
  
  free(program) ;
}


/******************************************************************************
 *                               Reading function                             *
 ******************************************************************************/


/**
 * cloog_program_read function:
 * This function read the informations to put in a CloogProgram structure from
 * a file (file, possibly stdin). It returns a pointer to a CloogProgram
 * structure containing the read informations.
 * - October 25th 2001: first version.
 * - September 9th 2002: - the big reading function is now split in several
 *                         functions (one per read data structure).
 *                       - adaptation to the new file format with naming.
 */
CloogProgram * cloog_program_read(FILE * file, CloogOptions * options)
{ int i, nb_statements, nb_parameters, nb_iterators, nb_scattering ;
  char s[MAX_STRING], language, prefix[2]={'c','\0'},
       ** scattering, ** iterators, ** parameters;
  CloogLoop * current, * next ;
  CloogBlockList * previous ;
  CloogDomainList * scatteringl ;
  CloogProgram * p ;
  
  nb_scattering = 0 ;
  scattering = NULL ;
      
  /* Memory allocation for the CloogProgram structure. */
  p = cloog_program_malloc() ;
  
  /* First of all, we read the language to use. */
  while (fgets(s,MAX_STRING,file) == 0) ;
  while ((*s=='#'||*s=='\n') || (sscanf(s," %c",&language)<1))
  fgets(s,MAX_STRING,file) ;
  cloog_program_set_language (p, language);

  /* We then read the context data. */
  cloog_program_set_context (p, cloog_domain_read (file));
  nb_parameters = cloog_domain_dim (cloog_program_context (p)) ;
  
  /* First part of the CloogNames structure: reading of the parameter names. */
  parameters=cloog_names_read_strings(file,nb_parameters,NULL,FIRST_PARAMETER) ;
      
  /* We read the statement number. */
  while (fgets(s,MAX_STRING,file) == 0) ;
  while ((*s=='#'||*s=='\n') || (sscanf(s," %d",&nb_statements)<1))
  fgets(s,MAX_STRING,file) ;

  /* Statements and domains reading for each statement. */
  if (nb_statements > 0)
  { /* Reading of the first domain. */
    cloog_program_set_loop (p, cloog_loop_read (file,0,nb_parameters));
    cloog_program_set_blocklist 
      (p, cloog_block_list_alloc (cloog_loop_block (cloog_program_loop (p))));
    previous = cloog_program_blocklist (p);
    
    if (cloog_loop_domain (cloog_program_loop (p)) != NULL)
      nb_iterators = cloog_domain_dim(cloog_loop_domain (cloog_program_loop (p))) - nb_parameters ;
    else
    nb_iterators = 0 ;
    
    /* And the same for each next domain. */
    current = cloog_program_loop (p) ;
    for (i=2;i<=nb_statements;i++)
    { next = cloog_loop_read(file,i-1,nb_parameters) ;
      if (cloog_loop_domain (next) != NULL)
	if ((int) cloog_domain_dim(cloog_loop_domain (next)) - nb_parameters > nb_iterators)
	  nb_iterators = cloog_domain_dim (cloog_loop_domain (next)) - nb_parameters ;
      
      cloog_block_list_set_next (previous, cloog_block_list_alloc (cloog_loop_block (next)));
      previous = cloog_block_list_next (previous) ;    
    
      cloog_loop_set_next (current, next);
      current = cloog_loop_next (current) ;
    }     
        
    /* Reading of the iterator names. */
    iterators = cloog_names_read_strings(file,nb_iterators,NULL,FIRST_ITERATOR);

    /* Reading and putting the scattering data in program structure. */
    scatteringl = cloog_domain_list_read(file) ;
    
    if (scatteringl != NULL)
      {
	if (cloog_domain_list_lazy_same(scatteringl)
	    /* Cloog should never print to stderr.  */
	    && 0)
	  fprintf(stderr, "[CLooG]WARNING: some scattering functions are "
		  "similar.\n") ;
      
	cloog_program_set_nb_scattdims (p, 
					cloog_domain_dim (cloog_domain (scatteringl)) - 
					cloog_domain_dim (cloog_loop_domain (cloog_program_loop (p)))) ;
	nb_scattering = cloog_program_nb_scattdims (p);
	scattering = cloog_names_read_strings (file, cloog_program_nb_scattdims (p), prefix, -1);
    
	/* The boolean array for scalar dimensions is created and set to 0. */
	cloog_program_set_scaldims (p, (int *) malloc (cloog_program_nb_scattdims (p) * sizeof (int)));
	if (cloog_program_scaldims (p) == NULL)
	  {
	    fprintf(stderr, "[CLooG]ERROR: memory overflow.\n") ;
	    exit(1) ;
	  }
	for (i=0;i<cloog_program_nb_scattdims (p);i++)
	  cloog_program_set_scaldim (p, i, 0);
      
	/* We try to find blocks in the input problem to reduce complexity. */
	if (!options->noblocks)
	  cloog_program_block(p,scatteringl) ;
	if (!options->noscalars)
	  cloog_program_extract_scalars(p,scatteringl) ;
      
	cloog_program_scatter(p,scatteringl) ;
	cloog_domain_list_free(scatteringl) ;
      }
    else
      {
	cloog_program_set_nb_scattdims (p, 0);
	cloog_program_set_scaldims (p, NULL);
      }
    
    cloog_program_set_names 
      (p, cloog_names_alloc (0, nb_scattering, nb_iterators, nb_parameters,
			     NULL, scattering, iterators, parameters));
  
    cloog_names_scalarize (cloog_program_names (p), cloog_program_nb_scattdims (p), 
			   cloog_program_scaldims (p));
  }
  else
    {
      cloog_program_set_loop (p, NULL);
      cloog_program_set_names (p, NULL);
      cloog_program_set_blocklist (p, NULL);
      cloog_program_set_scaldims (p, NULL);
    }
   
  return(p) ;
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
 
 
/**
 * cloog_program_malloc function:
 * This function allocates the memory space for a CloogProgram structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - November 21th 2005: first version.
 */
CloogProgram * cloog_program_malloc (void)
{ CloogProgram * program ;
  
  /* Memory allocation for the CloogProgram structure. */
  program = (CloogProgram *)malloc(sizeof(CloogProgram)) ;
  if (program == NULL) 
  { fprintf(stderr, "[CLooG]ERROR: memory overflow.\n") ;
    exit(1) ;
  }
  
  /* We set the various fields with default values. */
  cloog_program_set_language (program, 'c');
  cloog_program_set_nb_scattdims (program, 0);
  cloog_program_set_context (program, NULL);
  cloog_program_set_loop (program, NULL);
  cloog_program_set_names (program, NULL);
  cloog_program_set_blocklist (program, NULL);
  cloog_program_set_scaldims (program, NULL);
  cloog_program_set_usr (program, NULL);
  
  return program ;
}  


/**
 * cloog_program_generate function:
 * This function calls the Quillere algorithm for loop scanning. (see the
 * Quillere paper) and calls the loop simplification function.
 * - depth is the loop depth we want to optimize (guard free as possible),
 *   the first loop depth is 1 and anegative value is the infinity depth.
 * - sep_level is the level number where we want to start loop separation.
 **
 * - October 26th 2001: first version. 
 * - April   19th 2005: some basic fixes and memory usage feature.
 * - April   29th 2005: (bug fix, bug found by DaeGon Kim) see case 2 below.
 */ 
CloogProgram * cloog_program_generate(CloogProgram *program, CloogOptions *options)
{ float time ;
#ifdef HAS_SYS_RESOURCE_H
  struct rusage start, end ;
#endif
  CloogLoop * loop ;
#ifdef CLOOG_MEMORY
  char status_path[MAX_STRING_VAL] ;
  FILE * status ;
 
  /* We initialize the memory need to 0. */
  options->memory = 0 ;
#endif

  if (options->override)
  {
    /* Cloog should never print to stderr.  */
    /* fprintf(stderr,
       "[CLooG]WARNING: you are using -override option, be aware that the "
       "generated\n                code may be incorrect.\n") ;
    */
  }
  else
  { /* Playing with options may be dangerous, here are two possible issues :
     * 1. Using -l option less than scattering dimension number may lead to
     *    an illegal target code (since the scattering is not respected), if
     *    it is the case, we set -l depth to the first acceptable value.
     */
    if ((cloog_program_nb_scattdims (program) > options->l) && (options->l >= 0))
    {
      /* Cloog should never print to stderr.  */
      /* fprintf(stderr,
	 "[CLooG]WARNING: -l depth is less than the scattering dimension number "
	 "(the \n                generated code may be incorrect), it has been "
	 "automaticaly set\n                to this value (use option -override "
	 "to override).\n") ;
      */
      options->l = cloog_program_nb_scattdims (program);
    }
      
    /* 2. Using -f option greater than one while -l depth is greater than the
     *    scattering dimension number may lead to iteration duplication (try
     *    test/daegon_lu_osp.cloog with '-f 3' to test) because of the step 4b
     *    of the cloog_loop_generate function, if it is the case, we set -l to
     *    the first acceptable value.
     */
    if (((options->f > 1) || (options->f < 0)) &&
        ((options->l > cloog_program_nb_scattdims (program)) || (options->l < 0)))
    { 
      /* Cloog should never print to stderr.  */
      /* fprintf(stderr,
	 "[CLooG]WARNING: -f depth is more than one, -l depth has been "
	 "automaticaly set\n                to the scattering dimension number "
	 "(target code may have\n                duplicated iterations), -l depth "
	 "has been automaticaly set to\n                this value (use option "
	 "-override to override).\n") ;
      */
      options->l = cloog_program_nb_scattdims (program);
    }
  }
  
#ifdef HAS_SYS_RESOURCE_H
  getrusage(RUSAGE_SELF, &start) ;
#endif

  if (cloog_program_loop (program))
  {
    loop = cloog_program_loop (program) ;
    
    /* Here we go ! */
    loop = cloog_loop_generate(loop, cloog_program_context (program), 1, 0,
                               cloog_program_scaldims (program),
			       cloog_program_nb_scattdims (program),
                               cloog_domain_dim (cloog_program_context (program)),
			       options);
			          
#ifdef CLOOG_MEMORY
    /* We read into the status file of the process how many memory it uses. */
    sprintf(status_path,"/proc/%d/status",getpid()) ;
    status = fopen(status_path, "r") ;
    while (fscanf(status,"%s",status_path) && strcmp(status_path,"VmData:")!=0);
    fscanf(status,"%d",&(options->memory)) ;
    fclose(status) ;
#endif
    
    if ((!options->nosimplify) && cloog_program_loop (program))
      loop = cloog_loop_simplify(loop, cloog_program_context (program), 1,
				 cloog_domain_dim (cloog_program_context (program))) ;
   
    cloog_program_set_loop (program, loop);
  }
    
#ifdef HAS_SYS_RESOURCE_H
  getrusage(RUSAGE_SELF, &end) ;
  /* We calculate the time spent in code generation. */
  time =  (end.ru_utime.tv_usec -  start.ru_utime.tv_usec)/(float)(MEGA) ;
  time += (float)(end.ru_utime.tv_sec - start.ru_utime.tv_sec) ;
  options->time = time ;
#endif

  return program ;
}


/**
 * cloog_program_block function:
 * this function gives a last chance to the lazy user to consider statement
 * blocks instead of some statement lists where the whole list may be
 * considered as a single statement from a code generation point of view.
 * For instance two statements with the same iteration domain and the same
 * scattering functions may be considered as a block. This function is lazy
 * and can only find very simple forms of trivial blocks (see
 * cloog_domain_lazy_block function for more details). The useless loops and
 * scattering functions are removed and freed while the statement list of
 * according blocks are filled.
 * - program is the whole program structure (befaore applying scattering),
 * - scattering is the list of scattering functions.
 **
 * - April   30th 2005: first attempt.
 * - June 10-11th 2005: first working version.
 */
void cloog_program_block(CloogProgram * program, CloogDomainList * scattering)
{ int blocked_reference=0, blocked=0, nb_blocked=0 ;
  CloogLoop * reference, * start, * loop ;
  CloogDomainList * scatt_reference, * scatt_loop, * scatt_start ;
  CloogBlockList * previous ;
  
  if ((cloog_program_loop (program) == NULL) 
      || (cloog_loop_next (cloog_program_loop (program)) == NULL))
  return ;

  /* We will have to rebuild the block list. */
  cloog_block_list_free (cloog_program_blocklist (program)) ;
  cloog_program_set_blocklist 
    (program, cloog_block_list_alloc (cloog_loop_block (cloog_program_loop (program))));
  previous = cloog_program_blocklist (program);
  
  /* The process will use three variables for the linked list :
   * - 'start' is the starting point of a new block,
   * - 'reference' is the node of the block used for the block checking,
   * - 'loop' is the candidate to be inserted inside the block.
   * At the beginning of the process, the linked lists are as follow:
   *         O------>O------>O------>O------>NULL
   *         |       |
   *       start    loop
   *     reference
   */

  reference       = cloog_program_loop (program) ;
  start           = cloog_program_loop (program) ;
  loop            = cloog_loop_next (reference) ;
  scatt_reference = scattering ;
  scatt_start     = scattering ;
  scatt_loop      = cloog_next_domain (scattering) ;
   
  while (loop != NULL)
    { 
      if (cloog_domain_lazy_equal (cloog_loop_domain (reference),
				   cloog_loop_domain (loop)) &&
	  cloog_domain_lazy_block(cloog_domain (scatt_reference),
				  cloog_domain (scatt_loop),
				  scattering, cloog_program_nb_scattdims (program)))
    { /* If we find a block we update the links:
       *     +---------------+
       *     |               v
       *     O       O------>O------>O------>NULL
       *     |       |
       *   start    loop
       * reference
       */
      blocked = 1 ;
      nb_blocked ++ ;
      cloog_block_merge(cloog_loop_block (start), cloog_loop_block (loop)); /* merge frees cloog_block (loop) */
      cloog_loop_set_block (loop, NULL);
      cloog_loop_set_next (start, cloog_loop_next (loop));
      cloog_set_next_domain (scatt_start, cloog_next_domain (scatt_loop));
    }
    else
    { /* If we didn't find a block, the next start of a block is updated:
       *     O------>O------>O------>O------>NULL
       *     |       |
       * reference start
       *           loop
       */
      blocked= 0 ;
      start = loop ;
      scatt_start = scatt_loop ;
      
      /* We update the block list. */
      cloog_block_list_set_next (previous, cloog_block_list_alloc (cloog_loop_block (start)));
      previous = cloog_block_list_next (previous) ;    
    }

    /* If the reference node has been included into a block, we can free it. */
    if (blocked_reference)
      { cloog_loop_set_next (reference, NULL);
      cloog_loop_free (reference) ;
      cloog_domain_free (cloog_domain (scatt_reference)) ;
      free (scatt_reference) ;
    }
    
    /* The reference and the loop are now updated for the next try, the
     * starting position depends on the previous step.
     *       O   ?   O------>O------>O------>NULL
     *               |       |
     *           reference loop
     */
    reference       = loop ;
    loop            = cloog_loop_next (loop) ;
    scatt_reference = scatt_loop ;
    scatt_loop      = cloog_next_domain (scatt_loop) ;
    
    /* We mark the new reference as being blocked or not, if will be freed
     * during the next while loop execution.
     */
    if (blocked)
    blocked_reference = 1 ;
    else
    blocked_reference = 0 ;
  }
  
  /* We free the last blocked reference if any (since in the while loop it was
   * freed during the next loop execution, it was not possible to free the
   * last one inside).
   */
  if (blocked_reference)
    { cloog_loop_set_next (reference, NULL);
    cloog_loop_free (reference) ;
    cloog_domain_free (cloog_domain (scatt_reference)) ;
    free (scatt_reference) ;
  }
  
  /* Cloog should never print to stderr.  */
  /* if (nb_blocked != 0)
     fprintf(stderr, "[CLooG]INFO: %d domains have been blocked.\n",nb_blocked) ; */
}


/**
 * cloog_program_extract_scalars function:
 * this functions finds and removes the dimensions of the scattering functions
 * when they are scalar (i.e. of the shape "dim + scalar = 0") for all
 * scattering functions. The reason is that the processing of such dimensions
 * is trivial and do not need neither a row and a column in the matrix
 * representation of the domain (this will save memory) neither the full
 * Quillere processing (this will save time). The scalar dimensions data are
 * dispatched in the CloogProgram structure (the boolean vector scaldims will
 * say which original dimensions are scalar or not) and to the CloogBlock
 * structures (each one has a scaldims vector that contains the scalar values).
 * - June 14th 2005: first developments.
 * - June 30th 2005: first version.
 */ 
void cloog_program_extract_scalars(CloogProgram *program, CloogDomainList *scattering)
{ int i, j, scalar, current, nb_scaldims=0 ;
  CloogDomainList * start ;
  CloogDomain * old ;
  CloogBlockList * blocklist ;
  CloogBlock * block ;

  start = scattering ;
    
  for (i = 0; i < cloog_program_nb_scattdims (program); i++)
  { scalar = 1 ;
    scattering = start ;
    while (scattering != NULL)
      {
	if (!cloog_domain_lazy_isscalar (cloog_domain (scattering),i))
	  {
	    scalar = 0 ;
	    break ;
	  }
	scattering = cloog_next_domain (scattering);
      }
    
    if (scalar)
      {
	nb_scaldims ++ ;
	cloog_program_set_scaldim (program, i, 1);
      }
  }
  
  /* If there are no scalar dimensions, we can continue directly. */
  if (!nb_scaldims)
  return ;

  /* Otherwise, in each block, we have to put the number of scalar dimensions,
   * and to allocate the memory for the scalar values.
   */
  blocklist = cloog_program_blocklist (program);
  while (blocklist != NULL)
    {
      block = cloog_block_list_block (blocklist) ;
      cloog_block_set_nb_scaldims (block, nb_scaldims);
      cloog_block_set_scaldims (block, (Value *) malloc (nb_scaldims * sizeof (Value)));

      for (i = 0; i < nb_scaldims; i++)
	value_init_c (block->scaldims[i]);
    
      blocklist = cloog_block_list_next (blocklist);
    }
  
  /* Then we have to fill these scalar values, so we can erase those dimensions
   * from the scattering functions. It's easier to begin with the last one,
   * since there would be an offset otherwise (if we remove the i^th dimension,
   * then the next one is not the (i+1)^th but still the i^th...).
   */
  current = nb_scaldims - 1 ;
  for (i = cloog_program_nb_scattdims (program) - 1; i >= 0; i--)
    if (cloog_program_scaldim (program, i))
      {
	blocklist = cloog_program_blocklist (program);
	scattering = start;

      while (blocklist != NULL)
	{ 
	  block = cloog_block_list_block (blocklist) ;
	  cloog_domain_scalar (cloog_domain (scattering), i,
			       &block->scaldims[current]);
	  blocklist = cloog_block_list_next (blocklist);
	  scattering = cloog_next_domain (scattering);
	}
  
    scattering = start ;
    while (scattering != NULL)
      {
	old = cloog_domain (scattering) ;
	cloog_set_domain (scattering, cloog_domain_erase_dimension (old, i)) ;
	cloog_domain_free (old) ;
	scattering = cloog_next_domain (scattering);
      }
    current-- ;
  }
  
  /* We postprocess the scaldims array in such a way that each entry is how
   * many scalar dimensions follows + 1 (the current one). This will make 
   * some other processing easier (e.g. knowledge of some offsets).
   */
  for (i = 0; i < cloog_program_nb_scattdims (program) - 1; i++)
    { 
      if (cloog_program_scaldim (program, i))
	{
	  j = i + 1 ;
	  while ((j < cloog_program_nb_scattdims (program))
		 && cloog_program_scaldim (program, j))
	    {
	      cloog_program_set_scaldim (program, i, 
					 cloog_program_scaldim (program, i) + 1);
	      j ++ ;
	    }
	}
    }
  
  /* Cloog should never print to stderr.  */
  /* if (nb_scaldims != 0)
     fprintf(stderr, "[CLooG]INFO: %d dimensions (over %d) are scalar.\n",
     nb_scaldims, cloog_program_nb_scattdims (program)) ; */
}


/**
 * cloog_program_scatter function:
 * This function adds the scattering (scheduling) informations in a program.
 * If names is NULL, this function create names itself such that the i^th
 * name is ci.
 * - November 6th 2001: first version. 
 */
void cloog_program_scatter(CloogProgram *program, CloogDomainList *scattering)
{ 
  CloogLoop * loop ;
  
  if ((program != NULL) && (scattering != NULL))
    {
      loop = cloog_program_loop (program) ;

      /* Finally we scatter all loops. */
      cloog_loop_scatter(loop, cloog_domain (scattering)) ;
      loop = cloog_loop_next (loop) ;
      scattering = cloog_next_domain (scattering);    
    
      while ((loop != NULL) && (scattering != NULL))
	{      
	  cloog_loop_scatter(loop,cloog_domain (scattering)) ;
	  loop = cloog_loop_next (loop) ;
	  scattering = cloog_next_domain (scattering);
	}
    }
}
