
   /**-------------------------------------------------------------------**
    **                              CLooG                                **
    **-------------------------------------------------------------------**
    **                             names.h                               **
    **-------------------------------------------------------------------**
    **                  First version: august 1st 2002                   **
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


#ifndef CLOOG_NAMES_H
#define CLOOG_NAMES_H
#if defined(__cplusplus)
extern "C" 
  {
#endif 


# define MAX_NAME 50
# define FIRST_PARAMETER 'M'
# define FIRST_ITERATOR  'i'


/**
 * CloogNames structure:
 * this structure contains all the informations about parameter and iterator
 * names (as strings).
 */
struct cloognames
{ int _nb_scalars ;         /**< Scalar dimension number. */
  int _nb_scattering ;      /**< Scattering iterator number. */
  int _nb_iterators ;       /**< Iterator number. */
  int _nb_parameters ;      /**< Parameter number. */
  char ** _scalars ;        /**< The scalar names     (an array of strings). */
  char ** _scattering ;     /**< The scattering names (an array of strings). */
  char ** _iterators ;      /**< The iterator names   (an array of strings). */
  char ** _parameters ;     /**< The parameter names  (an array of strings). */
  int _references;          /**< Number of references to this structure. */
} ;
typedef struct cloognames CloogNames ;


static inline int cloog_names_nb_scalars (CloogNames *n)
{
  return n->_nb_scalars;
}

static inline void cloog_names_set_nb_scalars (CloogNames *n, int nb)
{
  n->_nb_scalars = nb;
}

static inline int cloog_names_nb_scattering (CloogNames *n)
{
  return n->_nb_scattering;
}

static inline void cloog_names_set_nb_scattering (CloogNames *n, int nb)
{
  n->_nb_scattering = nb;
}

static inline int cloog_names_nb_iterators (CloogNames *n)
{
  return n->_nb_iterators;
}

static inline void cloog_names_set_nb_iterators (CloogNames *n, int nb)
{
  n->_nb_iterators = nb;
}

static inline int cloog_names_nb_parameters (CloogNames *n)
{
  return n->_nb_parameters;
}

static inline void cloog_names_set_nb_parameters (CloogNames *n, int nb)
{
  n->_nb_parameters = nb;
}

static inline char **cloog_names_scalars (CloogNames *n)
{
  return n->_scalars;
}

static inline void cloog_names_set_scalars (CloogNames *n, char **s)
{
  n->_scalars = s;
}

static inline char *cloog_names_scalar_elt (CloogNames *n, int i)
{
  return n->_scalars[i];
}

static inline void cloog_names_set_scalar_elt (CloogNames *n, int i, char *s)
{
  n->_scalars[i] = s;
}

static inline char **cloog_names_scattering (CloogNames *n)
{
  return n->_scattering;
}

static inline void cloog_names_set_scattering (CloogNames *n, char **s)
{
  n->_scattering = s;
}

static inline char *cloog_names_scattering_elt (CloogNames *n, int i)
{
  return n->_scattering[i];
}

static inline void cloog_names_set_scattering_elt (CloogNames *n, int i, char *s)
{
  n->_scattering[i] = s;
}

static inline char **cloog_names_iterators (CloogNames *n)
{
  return n->_iterators;
}

static inline void cloog_names_set_iterators (CloogNames *n, char **s)
{
  n->_iterators = s;
}

static inline char *cloog_names_iterator_elt (CloogNames *n, int i)
{
  return n->_iterators[i];
}

static inline void cloog_names_set_iterator_elt (CloogNames *n, int i, char *s)
{
  n->_iterators[i] = s;
}

static inline char **cloog_names_parameters (CloogNames *n)
{
  return n->_parameters;
}

static inline void cloog_names_set_parameters (CloogNames *n, char **s)
{
  n->_parameters = s;
}

static inline char *cloog_names_parameter_elt (CloogNames *n, int i)
{
  return n->_parameters[i];
}

static inline void cloog_names_set_parameter_elt (CloogNames *n, int i, char *s)
{
  n->_parameters[i] = s;
}


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/
void cloog_names_print_structure(FILE *, CloogNames *, int) ;
void cloog_names_print(FILE *, CloogNames *) ;


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/
void cloog_names_free(CloogNames *) ;


/******************************************************************************
 *                              Reading functions                             *
 ******************************************************************************/
char ** cloog_names_read_strings(FILE *, int, char *, char) ;


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
CloogNames * cloog_names_malloc(void);
CloogNames * cloog_names_copy(CloogNames *names);
CloogNames * cloog_names_alloc(int,int,int,int,char **,char **,char **,char **);
char ** cloog_names_generate_items(int, char *, char) ;
CloogNames * cloog_names_generate(int, int, int, int, char, char, char, char) ;
void cloog_names_scalarize(CloogNames *, int, int *) ;

#if defined(__cplusplus)
  }
#endif 
#endif /* define _H */
