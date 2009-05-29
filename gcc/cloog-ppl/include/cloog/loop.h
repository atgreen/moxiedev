
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                             loop.h                                **
    **-------------------------------------------------------------------**
    **                 First version: october 26th 2001                  **
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


#ifndef CLOOG_LOOP_H
#define CLOOG_LOOP_H
#if defined(__cplusplus)
extern "C" 
  {
#endif 

/**
 * CloogLoop structure:
 * this structure contains all the informations of a loop generated or to be
 * generated.
 * - if the loop has not been processed yet (it is not a result of a call to
 *   cloog_loop_generate), the domain is the whole iteration domain of a given
 *   block, the stride is 1 (i.e. there is no stride), block is necessarily not
 *   NULL and inner is NULL.
 * - if the loop comes as a result of a cloog_loop_generate call, the domain
 *   describes the constraints (guards and loop bounds) for only one dimension
 *   (the last one: outer dimensions being considered as parameters), the stride
 *   may differ from one (this means that on the considered dimension, a step of
 *   'stride' must be considered between integral point, the first integral
 *   point to be considered being the lower bound of the loop), inner may differ
 *   from NULL, meaning that there are further dimensions and nesting levels in
 *   the loop.
 */
struct cloogloop
{ CloogDomain * _domain ;      /**< The iteration domain. */
  Value stride ;              /**< The stride for the corresponding iterator
                               *   (filled only after loop generation).
                               */
  CloogBlock * _block ;        /**< The included statement block, NULL if none.*/
  void * _usr;		      /**< User field, for library user convenience.
			       *   This pointer is not freed when the
			       *   CloogLoop structure is freed.
			       */
  struct cloogloop * _inner ;  /**< Loops at the next level. */
  struct cloogloop * _next ;   /**< Next loop at the same level. */
} ;
typedef struct cloogloop CloogLoop ;


static inline CloogDomain *cloog_loop_domain (CloogLoop *l)
{
  return l->_domain;
}

static inline void cloog_loop_set_domain (CloogLoop *l, CloogDomain *d)
{
  l->_domain = d;
}

#if 0
static inline Value cloog_loop_stride (CloogLoop *l)
{
  return l->stride;
}

static inline void cloog_loop_set_stride (CloogLoop *l, Value v)
{
  value_assign (l->stride, v);
}

static inline void cloog_loop_clear_stride (CloogLoop *l)
{
  value_clear_c (l->stride);
}

static inline void cloog_loop_init_stride (CloogLoop *l)
{
  value_init_c (l->stride);
}

static inline void cloog_loop_set_si_stride (CloogLoop *l, int i)
{
  value_set_si (l->stride, i);
}
#endif

static inline CloogBlock *cloog_loop_block (CloogLoop *l)
{
  return l->_block;
}

static inline void cloog_loop_set_block (CloogLoop *l, CloogBlock *b)
{
  l->_block = b;
}

static inline void *cloog_loop_usr (CloogLoop *l)
{
  return l->_usr;
}

static inline void cloog_loop_set_usr (CloogLoop *l, void *u)
{
  l->_usr = u;
}

static inline CloogLoop *cloog_loop_inner (CloogLoop *l)
{
  return l->_inner;
}

static inline void cloog_loop_set_inner (CloogLoop *l, CloogLoop *i)
{
  l->_inner = i;
}

static inline CloogLoop *cloog_loop_next (CloogLoop *l)
{
  return l->_next;
}

static inline CloogLoop **cloog_loop_next_addr (CloogLoop *l)
{
  return &l->_next;
}

static inline void cloog_loop_set_next (CloogLoop *l, CloogLoop *n)
{
  l->_next = n;
}


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void cloog_loop_print_structure(FILE *, CloogLoop *, int) ;
void cloog_loop_print(FILE *, CloogLoop *) ;


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/
void cloog_loop_free(CloogLoop *) ;


/******************************************************************************
 *                              Reading functions                             *
 ******************************************************************************/
CloogLoop * cloog_loop_read(FILE *, int, int) ;


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
CloogLoop * cloog_loop_malloc(void);
CloogLoop * cloog_loop_generate(CloogLoop *, CloogDomain *, int, int,
                                int *, int, int, CloogOptions *) ;
CloogLoop * cloog_loop_simplify(CloogLoop *, CloogDomain *, int, int) ;
void cloog_loop_scatter(CloogLoop *, CloogDomain *) ;


#if defined(__cplusplus)
  }
#endif 
#endif /* define _H */
