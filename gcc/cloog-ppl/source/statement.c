
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                           statement.c                             **
    **-------------------------------------------------------------------**
    **                 First version: november 4th 2001                  **
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
# include <string.h>
# include "../include/cloog/cloog.h"


/******************************************************************************
 *                             Memory leaks hunting                           *
 ******************************************************************************/


/**
 * These functions and global variables are devoted to memory leaks hunting: we
 * want to know at each moment how many CloogStatement structures had been
 * allocated (cloog_statement_allocated) and how many had been freed
 * (cloog_statement_freed). Each time a CloogStatement structure is allocated,
 * a call to the function cloog_statement_leak_up() must be carried out, and
 * respectively cloog_statement_leak_down() when a CloogStatement structure is
 * freed. The special variable cloog_statement_max gives the maximal number of
 * CloogStatement structures simultaneously alive (i.e. allocated and
 * non-freed) in memory.
 * - July 3rd->11th 2003: first version (memory leaks hunt and correction).
 */


int cloog_statement_allocated = 0 ;
int cloog_statement_freed = 0 ;
int cloog_statement_max = 0 ;


static void cloog_statement_leak_up (void)
{ cloog_statement_allocated ++ ;
  if ((cloog_statement_allocated-cloog_statement_freed) > cloog_statement_max)
  cloog_statement_max = cloog_statement_allocated - cloog_statement_freed ;
}


static void cloog_statement_leak_down (void)
{ cloog_statement_freed ++ ;
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
void cloog_statement_print_structure(FILE *file, CloogStatement *statement, int level)
{ int i ;
      
  if (statement != NULL)
  { /* Go to the right level. */
    for (i=0; i<level; i++)
    fprintf(file,"|\t") ;
    fprintf (file, "+-- CloogStatement %d \n", cloog_statement_number (statement));
    
    statement = cloog_statement_next (statement);
 
    while (statement != NULL)
    { for (i=0; i<level; i++)
      fprintf(file,"|\t") ;
      fprintf(file,"|          |\n");
      for (i=0; i<level; i++)
      fprintf(file,"|\t") ;
      fprintf(file,"|          V\n");
      
      for (i=0; i<level; i++)
      fprintf(file,"|\t") ;
      fprintf (file, "|   CloogStatement %d \n", cloog_statement_number (statement));
      statement = cloog_statement_next (statement) ;
    }
  }
  else
  { for (i=0; i<level; i++)
    fprintf(file,"|\t") ;
    
    fprintf(file,"+-- No CloogStatement\n") ;
  }  
}


/**
 * cloog_statement_print function:
 * This function prints the content of a CloogStatement structure (statement)
 * into a file (file, possibly stdout).
 */
void cloog_statement_print(FILE * file, CloogStatement * statement)
{ cloog_statement_print_structure(file,statement,0) ;
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/**
 * cloog_statement_free function:
 * This function frees the allocated memory for a CloogStatement structure.
 */
void cloog_statement_free(CloogStatement * statement)
{ CloogStatement * next ;

  while (statement != NULL)
  { cloog_statement_leak_down() ;
    
    next = cloog_statement_next (statement) ;
    free(statement) ;
    statement = next ;
  }
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/


/**
 * cloog_statement_malloc function:
 * This function allocates the memory space for a CloogStatement structure and
 * sets its fields with default values. Then it returns a pointer to the
 * allocated space.
 * - November 21th 2005: first version.
 */
CloogStatement * cloog_statement_malloc (void)
{ CloogStatement * statement ;
  
  /* Memory allocation for the CloogStatement structure. */
  statement = (CloogStatement *)malloc(sizeof(CloogStatement)) ;
  if (statement == NULL) 
  { fprintf(stderr, "[CLooG]ERROR: memory overflow.\n") ;
    exit(1) ;
  }
  cloog_statement_leak_up() ;
  
  /* We set the various fields with default values. */
  cloog_statement_set_number (statement, 0);
  cloog_statement_set_usr (statement, NULL);
  cloog_statement_set_next (statement, NULL);
  
  return statement ;
}  


/**
 * cloog_statement_alloc function:
 * This function allocates the memory space for a CloogStatement structure and
 * sets its fields with those given as input. Then it returns a pointer to the
 * allocated space.
 * - number is the statement number.
 **
 * - September 9th 2002: first version.
 * - March    17th 2003: fix for the usr field in CloogStatement structure.
 * - April    16th 2005: adaptation to new CloogStatement structure (with
 *                       number), cloog_statement_read becomes
 *                       cloog_statement_alloc sincethere is nothing more to
 *                       read on a file.
 * - November 21th 2005: use of cloog_statement_malloc.
 */
CloogStatement * cloog_statement_alloc(int number)
{ CloogStatement * statement ;
    
  /* Memory allocation and initialization of the structure. */
  statement = cloog_statement_malloc() ;

  cloog_statement_set_number (statement, number);
  
  return statement ;
}


/**
 * cloog_statement_copy function:
 * This function returns a copy of the CloogStatement structure given as input.
 * - October 28th 2001: first version (in loop.c). 
 * - March   17th 2003: fix for the usr field in CloogStatement structure.
 * - April   16th 2005: adaptation to new CloogStatement struct (with number). 
 */ 
CloogStatement * cloog_statement_copy(CloogStatement * source)
{ CloogStatement * statement, * temp, * now = NULL ;
  
  statement = NULL ;

  while (source != NULL)
  { cloog_statement_leak_up() ;

    temp = (CloogStatement *)malloc(sizeof(CloogStatement)) ;
    if (temp == NULL)
    { fprintf(stderr, "Memory Overflow.\n") ;
      exit(1) ;
    }
    
    cloog_statement_set_number (temp, cloog_statement_number (source));
    cloog_statement_set_usr (temp, cloog_statement_usr (source));
    cloog_statement_set_next (temp, NULL);
    
    if (statement == NULL)
    { statement = temp ;
      now = statement ;
    }
    else
      { cloog_statement_set_next (now, temp);
	now = cloog_statement_next (now) ;
    }
    source = cloog_statement_next (source) ;
  }
  return(statement) ;
}


/** 
 * cloog_statement_add function:
 * This function adds a CloogStatement structure (statement) at a given place
 * (now) of a NULL terminated list of CloogStatement structures. The beginning
 * of this list is (start). This function updates (now) to (loop), and
 * updates (start) if the added element is the first one -that is when (start)
 * is NULL-.
 * - March 27th 2004: first version. 
 */ 
void cloog_statement_add(CloogStatement **start, CloogStatement **now, CloogStatement *statement)
{ if (*start == NULL)
  { *start = statement ;
    *now = *start ;
  }
  else
    { cloog_statement_set_next (*now, statement);
      *now = cloog_statement_next (*now);
  }
}

