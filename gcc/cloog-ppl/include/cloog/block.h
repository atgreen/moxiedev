
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                             block.h                               **
    **-------------------------------------------------------------------**
    **                    First version: June 11th 2005                  **
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


#ifndef CLOOG_BLOCK_H
#define CLOOG_BLOCK_H
#if defined(__cplusplus)
extern "C" 
  {
#endif 


/**
 * CloogBlock structure:
 * this structure contains the informations of a statement block. It may happen
 * that users are lazy enough to ask CLooG to generate the code for statements
 * with exactly the same domain/scattering pair (possibly differing by only one
 * constant) instead of giving only one pair. CLooG provides them a last chance
 * to save time and memory by trying to find these blocks itself. The block
 * contains the statement list and the common informations of the statements.
 * This structure contains also the number of existing active references to it:
 * because CLooG uses many copies of blocks there is no need to actually copy
 * these blocks but just to return a pointer to them and to increment the number
 * of active references. Each time a CloogBlock will be freed, we will decrement
 * the active reference counter and actually free it if its value is zero.
 */
struct cloogblock
{ CloogStatement * _statement ;  /**< The list of statements in the block. */
  int  _nb_scaldims ;            /**< Number of scalar dimensions. */
  Value * scaldims ;            /**< Scalar dimension values. */
  int _depth ;                   /**< Original block depth (outer loop number).*/
  int _references ;              /**< Number of references to this structure. */
  void * _usr;		        /**< User field, for library user convenience.
				 *   This pointer is not freed when the
				 *   CloogBlock structure is freed.
			         */
} ;
typedef struct cloogblock CloogBlock ;

static inline CloogStatement *cloog_block_stmt (CloogBlock *b)
{
  return b->_statement;
}

static inline void cloog_block_set_stmt (CloogBlock *b, CloogStatement *s)
{
  b->_statement = s;
}

static inline int cloog_block_nb_scaldims (CloogBlock *b)
{
  return b->_nb_scaldims;
}

static inline void cloog_block_set_nb_scaldims (CloogBlock *b, int n)
{
  b->_nb_scaldims = n;
}

static inline Value *cloog_block_scaldims (CloogBlock *b)
{
  return b->scaldims;
}

static inline void cloog_block_set_scaldims (CloogBlock *b, Value *s)
{
  b->scaldims = s;
}

static inline int cloog_block_depth (CloogBlock *b)
{
  return b->_depth;
}

static inline void cloog_block_set_depth (CloogBlock *b, int n)
{
  b->_depth = n;
}

static inline void *cloog_block_usr (CloogBlock *b)
{
  return b->_usr;
}

static inline void cloog_block_set_usr (CloogBlock *b, void *u)
{
  b->_usr = u;
}


/**
 * CloogBlockList structure:
 * this structure reprensents a node of a linked list of CloogBlock structures.
 */
struct cloogblocklist
{ CloogBlock * _block ;          /**< An element of the list. */
  struct cloogblocklist * _next ;/**< Pointer to the next element of the list.*/
} ;
typedef struct cloogblocklist CloogBlockList ;

static inline CloogBlockList *cloog_block_list_next (CloogBlockList *s)
{
  return s->_next;
}

static inline void cloog_block_list_set_next (CloogBlockList *s, CloogBlockList *n)
{
  s->_next = n;
}

static inline CloogBlock *cloog_block_list_block (CloogBlockList *s)
{
  return s->_block;
}

static inline void cloog_block_list_set_block (CloogBlockList *s, CloogBlock *n)
{
  s->_block = n;
}


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void cloog_block_print_structure(FILE *, CloogBlock *, int) ;
void cloog_block_print(FILE *, CloogBlock *) ;
void cloog_block_list_print(FILE *, CloogBlockList *) ;


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/
void cloog_block_free(CloogBlock *) ;
void cloog_block_list_free(CloogBlockList *) ;


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
CloogBlock     * cloog_block_malloc(void);
CloogBlock     * cloog_block_alloc(CloogStatement*,int,Value*,int);
CloogBlockList * cloog_block_list_malloc(void);
CloogBlockList * cloog_block_list_alloc(CloogBlock *) ;
CloogBlock     * cloog_block_copy(CloogBlock * block) ;
void             cloog_block_merge(CloogBlock *, CloogBlock *) ;

#if defined(__cplusplus)
  }
#endif 
#endif /* define _H */

