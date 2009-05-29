
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                            program.h                              **
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


#ifndef CLOOG_PROGRAM_H
#define CLOOG_PROGRAM_H
#if defined(__cplusplus)
extern "C" 
  {
#endif 


# define MAX_STRING 1024
# define MEGA 1000000  /* One million. */


/**
 * CloogProgram structure:
 * this structure contains all the informations of a program generated or to be
 * generated.  Never access this structure directly: use the function interface below.
 */
struct cloogprogram
{ /* Basic program description fields. */
  char _language ;              /**< The language of the program. */
  int _nb_scattdims ;          /**< Scattering dimension number. */
  CloogDomain    * _context ;   /**< The context of the program. */
  CloogLoop      * _loop ;      /**< The loops of the program. */
  CloogNames     * _names ;     /**< Iterators and parameters names. */
  CloogBlockList * _blocklist ; /**< The statement block list. */
  
  /* Internal service fields, filled up by cloog_program_scatter function. */
  int * _scaldims ;             /**< Boolean array saying whether a given
                                *   scattering dimension is scalar or not.
				*/
  /* Library user reserved field. */
  void * _usr;		       /**< User field, for library user convenience.
			        *   This pointer is not freed when the
			        *   CloogProgram structure is freed.
			        */
} ;
typedef struct cloogprogram CloogProgram ;


static inline char cloog_program_language (CloogProgram *p)
{
  return p->_language;
}

static inline void cloog_program_set_language (CloogProgram *p, char c)
{
  p->_language = c;
}

static inline int cloog_program_nb_scattdims (CloogProgram *p)
{
  return p->_nb_scattdims;
}

static inline void cloog_program_set_nb_scattdims (CloogProgram *p, int n)
{
  p->_nb_scattdims = n;
}

static inline CloogDomain *cloog_program_context (CloogProgram *p)
{
  return p->_context;
}

static inline void cloog_program_set_context (CloogProgram *p, CloogDomain *c)
{
  p->_context = c;
}

static inline CloogLoop *cloog_program_loop (CloogProgram *p)
{
  return p->_loop;
}

static inline void cloog_program_set_loop (CloogProgram *p, CloogLoop *l)
{
  p->_loop = l;
}

static inline CloogNames *cloog_program_names (CloogProgram *p)
{
  return p->_names;
}

static inline void cloog_program_set_names (CloogProgram *p, CloogNames *n)
{
  p->_names = n;
}

static inline CloogBlockList *cloog_program_blocklist (CloogProgram *p)
{
  return p->_blocklist;
}

static inline void cloog_program_set_blocklist (CloogProgram *p, CloogBlockList *b)
{
  p->_blocklist = b;
}

static inline int *cloog_program_scaldims (CloogProgram *p)
{
  return p->_scaldims;
}

static inline void cloog_program_set_scaldims (CloogProgram *p, int *s)
{
  p->_scaldims = s;
}

static inline int cloog_program_scaldim (CloogProgram *p, int i)
{
  return p->_scaldims[i];
}

static inline void cloog_program_set_scaldim (CloogProgram *p, int i, int k)
{
  p->_scaldims[i] = k;
}

static inline void *cloog_program_usr (CloogProgram *p)
{
  return p->_usr;
}

static inline void cloog_program_set_usr (CloogProgram *p, void *u)
{
  p->_usr = u;
}

/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void cloog_program_print_structure(FILE *, CloogProgram *, int) ;
void cloog_program_print(FILE *, CloogProgram *) ;
void cloog_program_pprint(FILE *, CloogProgram *, CloogOptions *) ;
void cloog_program_dump_cloog(FILE *, CloogProgram *) ;


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/
void cloog_program_free(CloogProgram *) ;


/******************************************************************************
 *                               Reading function                             *
 ******************************************************************************/
CloogProgram * cloog_program_read(FILE *, CloogOptions *) ;


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
CloogProgram * cloog_program_malloc(void);
CloogProgram * cloog_program_generate(CloogProgram *, CloogOptions *) ;
void cloog_program_block(CloogProgram *, CloogDomainList *) ;
void cloog_program_extract_scalars(CloogProgram *program, CloogDomainList *) ;
void cloog_program_scatter(CloogProgram *, CloogDomainList *) ;

#if defined(__cplusplus)
  }
#endif 
#endif /* define _H */

