
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                             block.c                               **
    **-------------------------------------------------------------------**
    **                   First version: june 11th 2005                   **
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

# include <stdlib.h>
# include <stdio.h>
# include "../include/cloog/cloog.h"


/******************************************************************************
 *                             Memory leaks hunting                           *
 ******************************************************************************/


/**
 * These functions and global variables are devoted to memory leaks hunting: we
 * want to know at each moment how many CloogBlock structures had been allocated
 * (cloog_block_allocated) and how many had been freed (cloog_block_freed).
 * Each time a CloogBlock structure is allocated, a call to the function
 * cloog_block_leak_up() must be carried out, and respectively
 * cloog_block_leak_down() when a CloogBlock structure is freed. The special
 * variable cloog_block_max gives the maximal number of CloogBlock structures
 * simultaneously alive (i.e. allocated and non-freed) in memory.
 * - June 11th 2005: first version.
 */


int cloog_block_allocated = 0 ;
int cloog_block_freed = 0 ;
int cloog_block_max = 0 ;


static void cloog_block_leak_up (void)
{ cloog_block_allocated ++ ;
  if ((cloog_block_allocated - cloog_block_freed) > cloog_block_max)
  cloog_block_max = cloog_block_allocated - cloog_block_freed ;
}


static void cloog_block_leak_down (void)
{ cloog_block_freed ++ ;
}

static inline int cloog_block_references (CloogBlock *b)
{
  return b->_references;
}

static inline void cloog_block_init_references (CloogBlock *b)
{
  b->_references = 1;
}

static inline void cloog_block_inc_references (CloogBlock *b)
{
  b->_references++;
}

static inline void cloog_block_dec_references (CloogBlock *b)
{
  b->_references--;
}




/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/


/**
 * cloog_domain_print_structure :
 * this function is a human-friendly way to display the CloogDomain data
 * structure, it includes an indentation level (level) in order to work with
 * others print_structure functions.
 * - June  16th 2005: first version.
 */
void cloog_block_print_structure(FILE * file, CloogBlock * block, int level)
{ int i ;
  
  /* Go to the right level. */
  for (i=0; i<level; i++)
  fprintf(file,"|\t") ;
    
  if (block != NULL)
  { fprintf(file,"+-- CloogBlock\n") ;
   
    /* A blank line. */
    for (i=0; i<level+2; i++)
    fprintf(file,"|\t") ;
    fprintf(file,"\n") ;    

    /* Print statement list. */
    cloog_statement_print_structure(file,cloog_block_stmt (block),level+1) ;
    
    /* A blank line. */
    for (i=0; i<level+2; i++)
    fprintf(file,"|\t") ;    
    fprintf(file,"\n") ;    
  
    /* A blank line. */
    for (i=0; i<level+2; i++)
    fprintf(file,"|\t") ;    
    fprintf(file,"\n") ;    

    /* Print scalar dimensions. */
    for (i=0; i<level+1; i++)
    fprintf(file,"|\t") ;
    
    if (cloog_block_nb_scaldims (block) == 0)
    fprintf(file,"No scalar dimensions\n") ;
    else
      {
	fprintf (file, "Scalar dimensions (%d):", cloog_block_nb_scaldims (block));
	for (i = 0; i < cloog_block_nb_scaldims (block); i++)
	  value_print (file, " "VALUE_FMT, block->scaldims[i]);
	fprintf (file, "\n");
      }
    
    /* A blank line. */
    for (i=0; i<level+2; i++)
    fprintf(file,"|\t") ;    
    fprintf(file,"\n") ;    

    /* Print depth. */
    for (i=0; i<level+1; i++)
    fprintf(file,"|\t") ;
    fprintf (file, "Depth: %d\n", cloog_block_depth (block));
    
    /* A blank line. */
    for (i=0; i<level+1; i++)
    fprintf(file,"|\t") ;   
    fprintf(file,"\n") ;    
  }
  else
  fprintf(file,"+-- Null CloogBlock\n") ;
}


/**
 * cloog_block_print function:
 * This function prints the content of a CloogBlock structure (block) into a
 * file (file, possibly stdout).
 * - June 11th 2005: first version.
 * - June  16th 2005: now just a call to cloog_block_print_structure.
 */
void cloog_block_print(FILE * file, CloogBlock * block)
{ cloog_block_print_structure(file,block,0) ;  
}


/**
 * cloog_block_list_print function:
 * This function prints the content of a CloogBlock structure (block) into a
 * file (file, possibly stdout).
 * - June 16th 2005: first version.
 */
void cloog_block_list_print(FILE * file, CloogBlockList * blocklist)
{ int i=0 ;
  
  while (blocklist != NULL)
    {
      fprintf(file,"+-- CloogBlockList node %d\n",i) ;
      cloog_block_print_structure (file, cloog_block_list_block (blocklist), 1);
      blocklist = cloog_block_list_next (blocklist);
      i++ ;
    }
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/**
 * cloog_block_free function:
 * This function frees the allocated memory for a CloogStatement structure.
 * - June 11th 2005: first version.
 * - June 30th 2005: scaldims field management.
 */
void cloog_block_free(CloogBlock * block)
{ int i ;

  if (block != NULL)
    {
      cloog_block_dec_references (block);
    
      if (cloog_block_references (block) == 0)
	{ cloog_block_leak_down() ;
	  if (cloog_block_scaldims (block))
	    {
	      for (i = 0; i < cloog_block_nb_scaldims (block); i++)
		value_clear_c (block->scaldims[i]);
      
	      free (cloog_block_scaldims (block)) ;
	    }
	  cloog_statement_free(cloog_block_stmt (block)) ;
	  free(block) ;
	}
    }
}


/**
 * cloog_block_list_free function:
 * This function frees the allocated memory for a CloogBlockList structure.
 * - June 11th 2005: first version.
 */
void cloog_block_list_free(CloogBlockList * blocklist)
{ CloogBlockList * temp ;
  
  while (blocklist != NULL)
    {
      temp = cloog_block_list_next (blocklist);
      cloog_block_free (cloog_block_list_block (blocklist));
      free(blocklist) ;
      blocklist = temp ;
    }
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/

/**
 * cloog_block_malloc function:
 * This function allocates the memory space for a CloogBlock structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - November 21th 2005: first version.
 */
CloogBlock * cloog_block_malloc (void)
{ CloogBlock * block ;
  
  /* Memory allocation for the CloogBlock structure. */
  block = (CloogBlock *)malloc(sizeof(CloogBlock)) ;
  if (block == NULL) 
  { fprintf(stderr, "[CLooG]ERROR: memory overflow.\n") ;
    exit(1) ;
  }
  cloog_block_leak_up() ;
  
  /* We set the various fields with default values. */
  cloog_block_set_stmt (block, NULL);
  cloog_block_set_nb_scaldims (block, 0);
  cloog_block_set_scaldims (block, NULL);
  cloog_block_set_depth (block, 0);
  cloog_block_init_references (block);
  cloog_block_set_usr (block, NULL);
  
  return block ;
}  


/**
 * cloog_block_alloc function:
 * This function allocates the memory space for a CloogBlock structure and
 * sets its fields with those given as input. Then it returns a pointer to the
 * allocated space. The two parameters nb_scaldims and scaldims are for internal
 * service, put to respectively 0 and NULL if you don't know what they are
 * useful for !
 * - statement is the statement list of the block,
 * - nb_scaldims is the number of scalar dimensions (0 if unsure !),
 * - scaldims is the array with the scalar dimensions values (NULL if unsure !),
 * - depth is the original block depth (the number of outer loops).
 **
 * - June     11th 2005: first version.
 * - June     30th 2005: addition of the nb_scaldims and scaldims parameters.
 * - November 21th 2005: use of cloog_block_malloc.
 */
CloogBlock * cloog_block_alloc(CloogStatement * statement, int nb_scaldims,
			       Value * scaldims, int depth)
{
  CloogBlock * block ;
    
  /* Block allocation. */
  block = cloog_block_malloc() ;

  cloog_block_set_stmt (block, statement);
  cloog_block_set_nb_scaldims (block, nb_scaldims);
  cloog_block_set_scaldims (block, scaldims);
  cloog_block_set_depth (block, depth);
  cloog_block_init_references (block);
  
  return block ;
}


/**
 * cloog_block_list_malloc function:
 * This function allocates the memory space for a CloogBlockList structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - November 21th 2005: first version.
 */
CloogBlockList * cloog_block_list_malloc (void)
{
  CloogBlockList * blocklist ;
  
  /* Memory allocation for the CloogBlock structure. */
  blocklist = (CloogBlockList *)malloc(sizeof(CloogBlockList)) ;
  if (blocklist == NULL) 
  { fprintf(stderr, "[CLooG]ERROR: memory overflow.\n") ;
    exit(1) ;
  }
  
  /* We set the various fields with default values. */
  cloog_block_list_set_block (blocklist, NULL);
  cloog_block_list_set_next (blocklist, NULL);
  
  return blocklist ;
}  


/**
 * cloog_block_list_alloc function:
 * This function allocates the memory space for a CloogBlockList structure and
 * sets its fields with those given as input. Then it returns a pointer to the
 * allocated space.
 * - block is the block element of the list node,
 **
 * - June     11th 2005: first version.
 * - November 21th 2005: use of cloog_block_list_malloc.
 */
CloogBlockList * cloog_block_list_alloc(CloogBlock * block)
{ CloogBlockList * blocklist ;
  
  /* Block list node allocation. */
  blocklist = cloog_block_list_malloc() ;

  cloog_block_list_set_block (blocklist, block);
  cloog_block_inc_references (cloog_block_list_block (blocklist)); /* The block has a new reference to it. */
  cloog_block_list_set_next (blocklist, NULL);
  
  return blocklist ;
}


/**
 * cloog_block_copy function:
 * This function returns a copy of a CloogBlock structure 'block'. To save
 * memory this is not a memory copy but we increment a counter of active
 * references inside the structure, then return a pointer to that structure.
 */ 
CloogBlock * cloog_block_copy(CloogBlock * block)
{ if (block == NULL)
  return NULL ;

  cloog_block_inc_references (block);
  return block ;
}


/**
 * cloog_block_merge function:
 * this function adds at the end of the statement list of the block 'block',
 * the statement list of the block 'merged'. Then the  CloogBlock structure
 * of 'merged' is freed (obviously not its statement list that is now
 * included in 'block').
 * - June 11th 2005: first version.
 */
void cloog_block_merge(CloogBlock * block, CloogBlock * merged)
{ CloogStatement * statement ;

  if ((block == NULL) || (merged == NULL))
  return ;
  
  if (cloog_block_stmt (block))
    {
      statement = cloog_block_stmt (block) ;
    
      while (cloog_statement_next (statement))
	statement = cloog_statement_next (statement) ;
    
      cloog_statement_set_next (statement, cloog_block_stmt (merged));
    }
  else
    cloog_block_set_stmt (block, cloog_block_stmt (merged));

  cloog_block_leak_down() ;
  free(merged) ;
}










