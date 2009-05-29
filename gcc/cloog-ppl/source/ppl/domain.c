
   /**-------------------------------------------------------------------**
    **                               CLooG                               **
    **-------------------------------------------------------------------**
    **                             domain.c                              **
    **-------------------------------------------------------------------**
    **                   First version: october 28th 2001                **
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
# include <ctype.h>
# include "cloog/cloog.h"
#include "matrix.h"

#ifndef USE_PPL_POWERSETS
# define USE_PPL_POWERSETS 1
#endif

/* Variables names for pretty printing.  */
static char wild_name[200][40];

static inline const char*
variable_output_function (ppl_dimension_type var)
{
  if (var < 40)
    return wild_name[var + 1];
  else
    return 0;
}

static inline void
error_handler (enum ppl_enum_error_code code, const char* description)
{
  fprintf (stderr, "PPL error code %d\n%s", code, description);
  exit (1);
}

void
cloog_initialize (void)
{
  sprintf (wild_name[0], "1");
  sprintf (wild_name[1], "a");
  sprintf (wild_name[2], "b");
  sprintf (wild_name[3], "c");
  sprintf (wild_name[4], "d");
  sprintf (wild_name[5], "e");
  sprintf (wild_name[6], "f");
  sprintf (wild_name[7], "g");
  sprintf (wild_name[8], "h");
  sprintf (wild_name[9], "i");
  sprintf (wild_name[10], "j");
  sprintf (wild_name[11], "k");
  sprintf (wild_name[12], "l");
  sprintf (wild_name[13], "m");
  sprintf (wild_name[14], "n");
  sprintf (wild_name[15], "o");
  sprintf (wild_name[16], "p");
  sprintf (wild_name[17], "q");
  sprintf (wild_name[18], "r");
  sprintf (wild_name[19], "s");
  sprintf (wild_name[20], "t");
  sprintf (wild_name[21], "alpha");
  sprintf (wild_name[22], "beta");
  sprintf (wild_name[23], "gamma");
  sprintf (wild_name[24], "delta");
  sprintf (wild_name[25], "tau");
  sprintf (wild_name[26], "sigma");
  sprintf (wild_name[27], "chi");
  sprintf (wild_name[28], "omega");
  sprintf (wild_name[29], "pi");
  sprintf (wild_name[30], "ni");
  sprintf (wild_name[31], "Alpha");
  sprintf (wild_name[32], "Beta");
  sprintf (wild_name[33], "Gamma");
  sprintf (wild_name[34], "Delta");
  sprintf (wild_name[35], "Tau");
  sprintf (wild_name[36], "Sigma");
  sprintf (wild_name[37], "Chi");
  sprintf (wild_name[38], "Omega");
  sprintf (wild_name[39], "xxx");

  if (ppl_initialize() < 0)
    {
      fprintf (stderr, "Cannot initialize the Parma Polyhedra Library.\n");
      exit (1);
    }

  if (ppl_restore_pre_PPL_rounding() < 0)
    {
      fprintf (stderr, "Cannot restore the pre-PPL rounding mode.\n");
      exit (1);
    }

  if (ppl_set_error_handler (error_handler) < 0)
    {
      fprintf (stderr, "Cannot install the custom error handler.\n");
      exit (1);
    }

  if (ppl_io_set_variable_output_function (variable_output_function) < 0)
    {
      fprintf (stderr, "Cannot install the PPL custom variable output function. \n");
      exit (1);
    }
}

polyhedron
cloog_pol_copy (polyhedron pol)
{
  polyhedron res;

  if (!pol)
    return 0;

  res = cloog_new_pol (cloog_pol_dim (pol), cloog_pol_nbc (pol));

  if (cloog_pol_nbc (pol))
    cloog_vector_copy (pol->Constraint[0], res->Constraint[0],
		       cloog_pol_nbc (pol) * (cloog_pol_dim (pol) + 2));

  return res;
}

/**
 * The maximal number of rays allowed to be allocated by PolyLib. In fact since
 * version 5.20, PolyLib automatically tune the number of rays by multiplying
 * by 2 this number each time the maximum is reached. For unknown reasons
 * PolyLib makes a segmentation fault if this number is too small. If this
 * number is too small, performances will be reduced, if it is too high, memory
 * will be saturated. Note that the option "-rays X" set this number to X.
 */
int MAX_RAYS = 50;

/* Unused in this backend.  */

int cloog_domain_allocated = 0;
int cloog_domain_freed = 0;
int cloog_domain_max = 0;

/* The same for Value variables since in GMP mode they have to be freed. */
int cloog_value_allocated = 0;
int cloog_value_freed = 0;
int cloog_value_max = 0;


static inline void
cloog_domain_polyhedron_set (CloogDomain * d, polyhedra_union p)
{
  d->_union = p;
}

static inline void
cloog_domain_set_references (CloogDomain * d, int i)
{
  d->_references = i;
}

static CloogDomain *
cloog_new_domain (polyhedra_union p)
{
  CloogDomain *domain = (CloogDomain *) malloc (sizeof (CloogDomain));
  domain->_union = p;
  cloog_domain_set_references (domain, 1);
  return domain;
}

static CloogDomain *
cloog_domain_alloc (polyhedron p)
{
  return cloog_new_domain (cloog_new_upol (p));
}

static inline CloogDomain *
cloog_check_domain_id (CloogDomain *dom)
{
  return dom;
}

static inline CloogDomain *
cloog_check_domain (CloogDomain *dom)
{
  if (!dom)
    return dom;

  return dom;
}

static inline void
cloog_vector_min_not_zero (Value * p, unsigned len, int *index, Value * min)
{
  Value x;
  int i = cloog_first_non_zero (p, len);

  if (i == -1)
    {
      value_set_si (*min, 1);
      return;
    }

  *index = i;
  value_absolute (*min, p[i]);
  value_init (x);

  for (i = i + 1; i < (int) len; i++)
    {
      if (value_zero_p (p[i]))
	continue;

      value_absolute (x, p[i]);
      if (value_lt (x, *min))
	{
	  value_assign (*min, x);
	  *index = i;
	}
    }

  value_clear (x);
}

void
cloog_vector_gcd (Value * p, int len, Value * gcd)
{
  Value *q, *cq, *cp;
  int i, non_zero, min_index = 0;

  q = (Value *) malloc (len * sizeof (Value));

  for (i = 0; i < len; i++)
    value_init (q[i]);

  for (cp = p, cq = q, i = 0; i < len; i++, cq++, cp++)
    value_absolute (*cq, *cp);

  do
    {
      cloog_vector_min_not_zero (q, len, &min_index, gcd);

      if (value_notone_p (*gcd))
	{
	  for (cq = q, non_zero = 0, i = 0; i < len; i++, cq++)
	    if (i != min_index)
	      {
		value_modulus (*cq, *cq, *gcd);
		non_zero |= value_notzero_p (*cq);
	      }
	}
      else
	break;

    }
  while (non_zero);

  for (i = 0; i < len; i++)
    value_clear (q[i]);

  free (q);
}

static inline void
cloog_matrix_combine (Value * p1, Value * p2, Value * p3, int x, unsigned len)
{
  Value a1, a2, gcd, b1, b2, n1;

  value_init (a1), value_init (a2), value_init (gcd),
    value_init (b1), value_init (b2), value_init (n1);

  value_assign (a1, p1[x]);
  value_assign (a2, p2[x]);

  value_absolute (b1, a1);
  value_absolute (b2, a2);

  Gcd (b1, b2, &gcd);

  value_division (a1, a1, gcd);
  value_division (a2, a2, gcd);
  value_oppose (n1, a1);
  cloog_vector_combine (p1 + 1, p2 + 1, p3 + 1, a2, n1, len);
  cloog_vector_normalize (p3 + 1, len);

  value_clear (a1), value_clear (a2), value_clear (gcd),
    value_clear (b1), value_clear (b2), value_clear (n1);
}

/* In the matrix representation an equality has a 0 in the first
   column.  When the value of the first column is 1, the row
   represents an inequality.  */

static inline int
cloog_matrix_row_is_eq_p (CloogMatrix *matrix, int row)
{
  return value_zero_p (matrix->p[row][0]);
}

static ppl_Constraint_t
cloog_build_ppl_cstr (ppl_Linear_Expression_t expr, int ineq)
{
  ppl_Constraint_t cstr;

  switch (ineq)
    {
    case 0:
      ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_EQUAL);
      break;

    case 1:
      ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
      break;

    default:
      /* Should not happen.  */
      exit (1);
    }

  return cstr;
}

/* Translates to PPL row I from MATRIX.  CST is the constant part that
   is added to the constraint.  When INEQ is 1 the constraint is
   translated as an inequality, when INEQ is 0 it is translated as an
   equality, when INEQ has another value, the first column of the
   matrix is read for determining the type of the constraint. */

static ppl_Constraint_t
cloog_translate_constraint (CloogMatrix *matrix, int i, int cst, int ineq)
{
  int j;
  ppl_Constraint_t res;
  ppl_Coefficient_t coef;
  ppl_Linear_Expression_t expr;
  ppl_dimension_type dim = matrix->NbColumns - 2;
  Value val;

  value_init (val);
  ppl_new_Coefficient (&coef);
  ppl_new_Linear_Expression_with_dimension (&expr, dim);

  for (j = 1; j < matrix->NbColumns - 1; j++)
    {
      ppl_assign_Coefficient_from_mpz_t (coef, matrix->p[i][j]);
      ppl_Linear_Expression_add_to_coefficient (expr, j - 1, coef);
    }

  value_set_si (val, cst);
  value_addto (val, matrix->p[i][matrix->NbColumns - 1], val);
  ppl_assign_Coefficient_from_mpz_t (coef, val);
  value_clear (val);
  ppl_Linear_Expression_add_to_inhomogeneous (expr, coef);
  ppl_delete_Coefficient (coef);

  if (ineq != 0 && ineq != 1)
    ineq = !cloog_matrix_row_is_eq_p (matrix, i);

  res = cloog_build_ppl_cstr (expr, ineq);
  ppl_delete_Linear_Expression (expr);
  return res;
}

/* Translates to PPL the opposite of row I from MATRIX.  When INEQ is
   1 the constraint is translated as an inequality, when INEQ is 0 it
   is translated as an equality, when INEQ has another value, the
   first column of the matrix is read for determining the type of the
   constraint.  */

static ppl_Constraint_t
cloog_translate_oppose_constraint (CloogMatrix *matrix, int i, int cst, int ineq)
{
  int j;
  ppl_Constraint_t res;
  ppl_Coefficient_t coef;
  ppl_Linear_Expression_t expr;
  ppl_dimension_type dim = matrix->NbColumns - 2;
  Value val, val1;

  value_init (val);
  value_init (val1);
  ppl_new_Coefficient (&coef);
  ppl_new_Linear_Expression_with_dimension (&expr, dim);

  for (j = 1; j < matrix->NbColumns - 1; j++)
    {
      value_oppose (val, matrix->p[i][j]);
      ppl_assign_Coefficient_from_mpz_t (coef, val);
      ppl_Linear_Expression_add_to_coefficient (expr, j - 1, coef);
    }

  value_oppose (val, matrix->p[i][matrix->NbColumns - 1]);
  value_set_si (val1, cst);
  value_addto (val, val, val1);
  ppl_assign_Coefficient_from_mpz_t (coef, val);
  ppl_Linear_Expression_add_to_inhomogeneous (expr, coef);
  ppl_delete_Coefficient (coef);

  if (ineq != 0 && ineq != 1)
    ineq = !cloog_matrix_row_is_eq_p (matrix, i);

  res = cloog_build_ppl_cstr (expr, ineq);
  ppl_delete_Linear_Expression (expr);
  return res;
}

/* Adds to PPL the constraints from MATRIX.  */

static void
cloog_translate_constraint_matrix_1 (ppl_Polyhedron_t ppl, CloogMatrix *matrix)
{
  int i;

  for (i = 0; i < matrix->NbRows; i++)
    {
      ppl_Constraint_t c = cloog_translate_constraint (matrix, i, 0, -1);
      ppl_Polyhedron_add_constraint (ppl, c);
      ppl_delete_Constraint (c);
    }
}

static ppl_Polyhedron_t
cloog_translate_constraint_matrix (CloogMatrix *matrix)
{
  ppl_Polyhedron_t ppl;
  ppl_dimension_type dim = matrix->NbColumns - 2;

  ppl_new_C_Polyhedron_from_space_dimension (&ppl, dim, 0);
  cloog_translate_constraint_matrix_1 (ppl, matrix);
  return ppl;
}

/* Put the constraint matrix of polyhedron RES under Cloog's normal
   form: Cloog expects to see

   0    1    1   -9
   1    0    1   -1

   instead of this:

   0    1    1   -9
   1   -1    0    8

   These two forms are equivalent but the expected form uses rightmost
   indices for inequalities.  */

static void
cloog_pol_normal_form (polyhedron res)
{
  int dim = cloog_pol_dim (res);
  int nrows = cloog_pol_nbc (res);
  int i, j;
  int neqs = cloog_pol_nbeq (res);

  for (j = 1; j <= dim; j++)
    {
      int rank;

      for (rank = 0; rank < neqs; rank++)
	if (j - 1 == cloog_first_non_zero (res->Constraint[rank] + 1, dim))
	  {
	    for (i = neqs; i < nrows; i++)
	      if (value_notzero_p (res->Constraint[i][j]))
		cloog_matrix_combine (res->Constraint[i],
				      res->Constraint[rank],
				      res->Constraint[i], j, dim + 1);

	    break;
	  }
    }
}

static polyhedron
cloog_translate_ppl_polyhedron_1 (ppl_Polyhedron_t pol)
{
  polyhedron res;
  ppl_dimension_type dim;
  ppl_const_Constraint_System_t pcs;
  ppl_Constraint_System_const_iterator_t cit, end;
  int eqs, orig_ineqs, ineqs, row, i;
  ppl_const_Constraint_t pc;
  Value val;

  ppl_Polyhedron_get_minimized_constraints (pol, &pcs);
  ppl_new_Constraint_System_const_iterator (&cit);
  ppl_new_Constraint_System_const_iterator (&end);

  for (eqs = 0, ineqs = 0,
	 ppl_Constraint_System_begin (pcs, cit),
	 ppl_Constraint_System_end (pcs, end);
       !ppl_Constraint_System_const_iterator_equal_test (cit, end);
       ppl_Constraint_System_const_iterator_increment (cit))
    {
      ppl_Constraint_System_const_iterator_dereference (cit, &pc);
      (ppl_Constraint_type (pc) == PPL_CONSTRAINT_TYPE_EQUAL) ? eqs++ : ineqs++;
    }

  ppl_Polyhedron_space_dimension (pol, &dim);

  orig_ineqs = ineqs;
  if (1 || orig_ineqs == 0)
    res = cloog_new_pol (dim, eqs + ineqs + 1);
  else
    res = cloog_new_pol (dim, eqs + ineqs);


  /* Sort constraints: Cloog expects to see in matrices the equalities
     followed by inequalities.  */
  ineqs = eqs;
  eqs = 0;
  value_init (val);

  for (ppl_Constraint_System_begin (pcs, cit), ppl_Constraint_System_end (pcs, end);
       !ppl_Constraint_System_const_iterator_equal_test (cit, end);
       ppl_Constraint_System_const_iterator_increment (cit))
    {
      ppl_Coefficient_t coef;
      ppl_dimension_type col;
      int neg;

      ppl_new_Coefficient (&coef);
      ppl_Constraint_System_const_iterator_dereference (cit, &pc);

      neg = (ppl_Constraint_type (pc) == PPL_CONSTRAINT_TYPE_LESS_THAN
	     || ppl_Constraint_type (pc) == PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL) ? 1 : 0;
      row = (ppl_Constraint_type (pc) == PPL_CONSTRAINT_TYPE_EQUAL) ? eqs++ : ineqs++;

      for (col = 0; col < dim; col++)
	{
	  ppl_Constraint_coefficient (pc, col, coef);
	  ppl_Coefficient_to_mpz_t (coef, val);

	  if (neg)
	    value_oppose (val, val);

	  value_assign (res->Constraint[row][col + 1], val);
	}

      ppl_Constraint_inhomogeneous_term (pc, coef);
      ppl_Coefficient_to_mpz_t (coef, val);
      value_assign (res->Constraint[row][dim + 1], val);
      ppl_delete_Coefficient (coef);

      switch (ppl_Constraint_type (pc))
	{
	case PPL_CONSTRAINT_TYPE_EQUAL:
	  value_set_si (res->Constraint[row][0], 0);
	  break;

	case PPL_CONSTRAINT_TYPE_LESS_THAN:
	case PPL_CONSTRAINT_TYPE_GREATER_THAN:
	  value_decrement (res->Constraint[row][dim + 1],
			   res->Constraint[row][dim + 1]);
	  value_set_si (res->Constraint[row][0], 1);
	  break;

	case PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL:
	case PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL:
	  value_set_si (res->Constraint[row][0], 1);
	  break;

	default:
	  fprintf (stderr, "PPL_CONSTRAINT_TYPE_%d not implemented yet\n",
		   ppl_Constraint_type (pc));
	  exit (1);
	}
    }

  value_clear (val);
  ppl_delete_Constraint_System_const_iterator (cit);
  ppl_delete_Constraint_System_const_iterator (end);

  if (cloog_pol_nbeq (res) == 2 && cloog_pol_nbc (res) == 2
      && cloog_first_non_zero (res->Constraint[0], dim + 2) == (int) dim + 1)
    {
      cloog_pol_free (res);
      return cloog_empty_polyhedron (dim);
    }

  /* Add the positivity constraint.  */
  if (1 || orig_ineqs == 0)
    {
      row = ineqs;
      value_set_si (res->Constraint[row][0], 1);
      for (i = 0; i < (int) dim; i++)
	value_set_si (res->Constraint[row][i + 1], 0);
      value_set_si (res->Constraint[row][dim + 1], 1);
    }

  /* Put the matrix of RES in normal form.  */
  cloog_pol_normal_form (res);

  /* If we do not sort the matrices, Cloog is a random loop
     generator.  */
  cloog_pol_sort_rows (res);

  return res;
}

polyhedron
cloog_pol_from_matrix (CloogMatrix * m)
{
  polyhedron res;
  int ncolumns = cloog_matrix_ncolumns (m);
  int nrows = cloog_matrix_nrows (m);
  ppl_Polyhedron_t p;

  if (nrows == 0)
    return cloog_universe_polyhedron (ncolumns - 2);


  p = cloog_translate_constraint_matrix (m);
  res = cloog_translate_ppl_polyhedron_1 (p);
  ppl_delete_Polyhedron (p);
  if ((int) cloog_pol_nbc (res) < cloog_matrix_nrows (m))
    return res;

  cloog_pol_free (res);
  res = cloog_new_pol (ncolumns - 2, nrows);
  cloog_vector_copy (m->p[0], res->Constraint[0], m->NbRows * m->NbColumns);

  return res;
}

CloogDomain *
cloog_domain_matrix2domain (CloogMatrix * matrix)
{
  return cloog_domain_alloc (cloog_pol_from_matrix (matrix));
}

static CloogDomain *
cloog_translate_ppl_polyhedron (ppl_Polyhedron_t p)
{
  polyhedron res = cloog_translate_ppl_polyhedron_1 (p);
  return cloog_domain_alloc (res);
}

static void
cloog_pol_print (FILE * file, polyhedron pol)
{
  unsigned dim, nbc;
  int i, j;
  Value *p;

  if (!pol)
    {
      fprintf (file, "<null polyhedron>\n");
      return;
    }

  dim = cloog_pol_dim (pol) + 2;
  nbc = cloog_pol_nbc (pol);
  fprintf (file, "POLYHEDRON Dimension:%d\n", cloog_pol_dim (pol));
  fprintf (file,
	   "           Constraints:%d  Equations:%d\n",
	   cloog_pol_nbc (pol), cloog_pol_nbeq (pol));
  fprintf (file, "Constraints %d %d\n", nbc, dim);

  for (i = 0; i < (int) nbc; i++)
    {
      p = pol->Constraint[i];

      if (value_notzero_p (*p))
	fprintf (file, "Inequality: [");
      else
	fprintf (file, "Equality:   [");

      p++;

      for (j = 1; j < (int) dim; j++)
	value_print (file, "%4s ", *p++);

      fprintf (file, " ]\n");
    }
}

void debug_poly (polyhedron p)
{
  cloog_pol_print (stderr, p);
}

void
debug_ppl_poly (ppl_Polyhedron_t p)
{
  debug_poly (cloog_domain_polyhedron (cloog_translate_ppl_polyhedron (p)));
}

static inline int
cloog_domain_references (CloogDomain * d)
{
  return d->_references;
}

/**
 * cloog_domain_print function:
 * This function prints the content of a CloogDomain structure (domain) into
 * a file (foo, possibly stdout).
 */
void
cloog_domain_print (FILE * foo, CloogDomain * domain)
{
  polyhedra_union upol = cloog_domain_upol (domain);

  while (upol)
    {
      cloog_pol_print (foo, cloog_upol_polyhedron (upol));
      upol = cloog_upol_next (upol);
    }

  fprintf (foo, "Number of active references: %d\n",
	   cloog_domain_references (domain));
}

/**
 * cloog_domain_free function:
 * This function frees the allocated memory for a CloogDomain structure
 * (domain). It decrements the number of active references to this structure,
 * if there are no more references on the structure, it frees it (with the
 * included list of polyhedra).
 */
void
cloog_domain_free (CloogDomain * domain)
{
  if (domain)
    {
      cloog_domain_set_references (domain,
				   cloog_domain_references (domain) - 1);

      if (cloog_domain_references (domain) == 0)
	{

	  polyhedra_union upol = cloog_domain_upol (domain);

	  while (upol)
	    {
	      polyhedra_union next_upol;

	      cloog_pol_free (cloog_upol_polyhedron (upol));
	      next_upol = cloog_upol_next (upol);
	      cloog_upol_free (upol);
	      upol = next_upol;
	    }

	  free (domain);
	}
    }
}


/**
 * cloog_domain_copy function:
 * This function returns a copy of a CloogDomain structure (domain). To save
 * memory this is not a memory copy but we increment a counter of active
 * references inside the structure, then return a pointer to that structure.
 */
CloogDomain *
cloog_domain_copy (CloogDomain * domain)
{
  cloog_domain_set_references (domain, cloog_domain_references (domain) + 1);
  return domain;
}

/**
 * cloog_domain_convex function:
 * Given a polyhedral domain (polyhedron), this function concatenates the lists
 * of rays and lines of the two (or more) polyhedra in the domain into one
 * combined list, and  find the set of constraints which tightly bound all of
 * those objects. It returns the corresponding polyhedron.
 */
CloogDomain *
cloog_domain_convex (CloogDomain * domain)
{
  CloogDomain *res;
  ppl_Polyhedron_t p2;
  polyhedra_union upol = cloog_domain_upol (domain);
  CloogMatrix *m = cloog_upol_domain2matrix (upol);
  ppl_Polyhedron_t p1 = cloog_translate_constraint_matrix (m);

  cloog_matrix_free (m);
  upol = cloog_upol_next (upol);
  while (upol)
    {
      m = cloog_upol_domain2matrix (upol);
      p2 = cloog_translate_constraint_matrix (m);
      cloog_matrix_free (m);
      ppl_Polyhedron_upper_bound_assign (p1, p2);
      ppl_delete_Polyhedron (p2);

      upol = cloog_upol_next (upol);
    }

  res = cloog_translate_ppl_polyhedron (p1);
  ppl_delete_Polyhedron (p1);

  return res;
}

int
cloog_domain_isconvex (CloogDomain * domain)
{
  if (cloog_domain_polyhedron (domain))
    return !cloog_upol_next (cloog_domain_upol (domain));

  return 1;
}

/**
 * cloog_domain_simple_convex:
 * Given a list (union) of polyhedra, this function returns a "simple"
 * convex hull of this union.  In particular, the constraints of the
 * the returned polyhedron consist of (parametric) lower and upper
 * bounds on individual variables and constraints that appear in the
 * original polyhedra.
 *
 * nb_par is the number of parameters of the domain.
 */
CloogDomain *
cloog_domain_simple_convex (CloogDomain * domain, int nb_par)
{
  fprintf (stderr, "cloog_domain_simple_convex (domain, nb_par = %d) is not implemented yet.\n", nb_par);
  fprintf (stderr, "domain = \n");
  cloog_domain_print (stderr, domain);
  exit (1);
}

/* Returns non-zero when the constraint I in MATRIX is the positivity
   constraint: "0 >= 0".  */

static int
cloog_positivity_constraint_p (CloogMatrix *matrix, int i, int dim)
{
  int j;

  for (j = 1; j < dim; j++)
    if (value_notzero_p (matrix->p[i][j]))
      break;

  return j == dim;
}

/* Simplifies DOM1 in the context of DOM2.  For example, DOM1 can
   contain the following conditions: i >= 0, i <= 5, and DOM2 is a
   loop around, i.e. the context of DOM1, that also contains the
   conditions i >= 0, i <= 5.  So instead of generating a loop like:

   | for (i = 0; i < 6; i++)
   |   {
   |     if (i >= 0 && i <= 5)
   |       S;
   |   }

   this function allows to detect that in the context of DOM2, the
   constraints of DOM1 are redundant, and so the following code should
   be produced:

   | for (i = 0; i < 6; i++)
   |   S;
  */

#if USE_PPL_POWERSETS

CloogDomain *
cloog_domain_simplify (CloogDomain * dom1, CloogDomain * dom2)
{
  if (!dom1)
    return dom1;
  if (!dom2)
    return dom2;

  CloogDomain *res = NULL;

  ppl_Pointset_Powerset_C_Polyhedron_t ps1, ps2;
  ppl_dimension_type dim = cloog_domain_dim(dom1);
  /* Translate dom1 into PPL powerset ps1. */
  {
    ppl_new_Pointset_Powerset_C_Polyhedron_from_space_dimension(&ps1, dim, 1);
    polyhedra_union u1;
    for (u1 = cloog_domain_upol (dom1); u1; u1 = cloog_upol_next (u1))
      {
        CloogMatrix *m = cloog_upol_domain2matrix (u1);
        ppl_const_Polyhedron_t ph = cloog_translate_constraint_matrix (m);
        ppl_Pointset_Powerset_C_Polyhedron_add_disjunct(ps1, ph);
        ppl_delete_Polyhedron(ph);
        cloog_matrix_free (m);
      }
  }

  /* Translate dom2 into PPL powerset ps2. */
  {
    ppl_new_Pointset_Powerset_C_Polyhedron_from_space_dimension(&ps2, dim, 1);
    polyhedra_union u2;
    for (u2 = cloog_domain_upol (dom2); u2; u2 = cloog_upol_next (u2))
      {
        CloogMatrix *m = cloog_upol_domain2matrix (u2);
        ppl_Polyhedron_t ph = cloog_translate_constraint_matrix (m);
        ppl_Pointset_Powerset_C_Polyhedron_add_disjunct(ps2, ph);
        ppl_delete_Polyhedron(ph);
        cloog_matrix_free (m);
      }
  }

  ppl_Pointset_Powerset_C_Polyhedron_simplify_using_context_assign(ps1, ps2);

  /* Translate back simplified ps1 into res. */
  ppl_Pointset_Powerset_C_Polyhedron_const_iterator_t i;
  ppl_new_Pointset_Powerset_C_Polyhedron_const_iterator(&i);
  ppl_Pointset_Powerset_C_Polyhedron_const_iterator_t end;
  ppl_new_Pointset_Powerset_C_Polyhedron_const_iterator(&end);
  for (ppl_Pointset_Powerset_C_Polyhedron_const_iterator_begin(ps1, i),
         ppl_Pointset_Powerset_C_Polyhedron_const_iterator_end(ps1, end);
       !ppl_Pointset_Powerset_C_Polyhedron_const_iterator_equal_test(i, end);
       ppl_Pointset_Powerset_C_Polyhedron_const_iterator_increment(i))
    {
      ppl_const_Polyhedron_t ph;
      ppl_Pointset_Powerset_C_Polyhedron_const_iterator_dereference(i, &ph);
      res = cloog_domain_union (res, cloog_translate_ppl_polyhedron (ph));
    }

  /* Final clean-up. */
  ppl_delete_Pointset_Powerset_C_Polyhedron_const_iterator(i);
  ppl_delete_Pointset_Powerset_C_Polyhedron_const_iterator(end);
  ppl_delete_Pointset_Powerset_C_Polyhedron(ps1);
  ppl_delete_Pointset_Powerset_C_Polyhedron(ps2);
  return res;
}

#else /* !USE_PPL_POWERSETS */

CloogDomain *
cloog_domain_simplify (CloogDomain * dom1, CloogDomain * dom2)
{
  CloogDomain *res = NULL;
  polyhedra_union u1, u2;
  CloogDomain *inter = cloog_domain_intersection (dom1, dom2);

  for (u1 = cloog_domain_upol (inter); u1; u1 = cloog_upol_next (u1))
    {
      CloogMatrix *m1 = cloog_upol_domain2matrix (u1);
      ppl_Polyhedron_t p1 = cloog_translate_constraint_matrix (m1);

      cloog_matrix_free (m1);
      for (u2 = cloog_domain_upol (dom2); u2; u2 = cloog_upol_next (u2))
	{
	  CloogMatrix *m2 = cloog_upol_domain2matrix (u2);
	  ppl_Polyhedron_t p2 = cloog_translate_constraint_matrix (m2);

	  cloog_matrix_free (m2);
	  ppl_Polyhedron_simplify_using_context_assign (p1, p2);
	  res = cloog_domain_union (res, cloog_translate_ppl_polyhedron (p1));
	  ppl_delete_Polyhedron (p2);
	}
    }

  return res;
}

#endif /* !USE_PPL_POWERSETS */

static polyhedra_union
cloog_upol_copy (polyhedra_union p)
{
  polyhedra_union res = cloog_new_upol (cloog_pol_copy (cloog_upol_polyhedron (p)));
  polyhedra_union upol = res;

  while (cloog_upol_next (p))
    {
      cloog_upol_set_next (upol, cloog_new_upol (cloog_pol_copy (cloog_upol_polyhedron (p))));
      upol = cloog_upol_next (upol);
      p = cloog_upol_next (p);
    }

  return res;
}

/* Adds to D1 the union of polyhedra from D2, with no checks of
   redundancies between polyhedra.  */

static CloogDomain *
cloog_domain_add_domain (CloogDomain *d1, CloogDomain *d2)
{
  polyhedra_union upol;

  if (!d1)
    return d2;

  if (!d2)
    return d1;

  upol = cloog_domain_upol (d1);
  while (cloog_upol_next (upol))
    upol = cloog_upol_next (upol);

  cloog_upol_set_next (upol, cloog_domain_upol (d2));
  return d1;
}

/**
 * cloog_domain_union function:
 * This function returns a new CloogDomain structure including a polyhedral
 * domain which is the union of two polyhedral domains (pol1) U (pol2) inside
 * two CloogDomain structures.
 */
CloogDomain *
cloog_domain_union (CloogDomain * dom1, CloogDomain * dom2)
{
  CloogDomain *res;
  polyhedra_union head1, head2, tail1, tail2;
  polyhedra_union d1, d2;

  if (!dom1)
    return dom2;

  if (!dom2)
    return dom1;

  if (cloog_domain_dim (dom1) != cloog_domain_dim (dom2))
    {
      fprintf (stderr, "cloog_domain_union should not be called on domains of different dimensions.\n");
      exit (1);
    }

  head1 = NULL;
  tail1 = NULL;
  for (d1 = cloog_domain_upol (dom1); d1; d1 = cloog_upol_next (d1))
    {
      int found = 0;
      CloogMatrix *m1 = cloog_upol_domain2matrix (d1);
      ppl_Polyhedron_t ppl1 = cloog_translate_constraint_matrix (m1);
      cloog_matrix_free (m1);

      for (d2 = cloog_domain_upol (dom2); d2; d2 = cloog_upol_next (d2))
	{
	  CloogMatrix *m2 = cloog_upol_domain2matrix (d2);
	  ppl_Polyhedron_t ppl2 = cloog_translate_constraint_matrix (m2);
	  cloog_matrix_free (m2);

	  if (ppl_Polyhedron_contains_Polyhedron (ppl2, ppl1))
	    {
	      ppl_delete_Polyhedron (ppl2);
	      found = 1;
	      break;
	    }
	  ppl_delete_Polyhedron (ppl2);
	}
      ppl_delete_Polyhedron (ppl1);

      if (!found)
	{
	  if (!tail1)
	    {
	      head1 = cloog_upol_copy (d1);
	      tail1 = head1;
	    }
	  else
	    {
	      cloog_upol_set_next (tail1, cloog_upol_copy (d1));
	      tail1 = cloog_upol_next (tail1);
	    }
	}
    }

  head2 = NULL;
  tail2 = NULL;
  for (d2 = cloog_domain_upol (dom2); d2; d2 = cloog_upol_next (d2))
    {
      int found = 0;
      CloogMatrix *m2 = cloog_upol_domain2matrix (d2);
      ppl_Polyhedron_t ppl2 = cloog_translate_constraint_matrix (m2);
      cloog_matrix_free (m2);

      for (d1 = head1; d1; d1 = cloog_upol_next (d1))
	{
	  CloogMatrix *m1 = cloog_upol_domain2matrix (d1);
	  ppl_Polyhedron_t ppl1 = cloog_translate_constraint_matrix (m1);
	  cloog_matrix_free (m1);

	  if (ppl_Polyhedron_contains_Polyhedron (ppl1, ppl2))
	    {
	      ppl_delete_Polyhedron (ppl1);
	      found = 1;
	      break;
	    }
	  ppl_delete_Polyhedron (ppl1);
	}
      ppl_delete_Polyhedron (ppl2);

      if (!found)
	{
	  if (!tail2)
	    {
	      head2 = cloog_upol_copy (d2);
	      tail2 = head2;
	    }
	  else
	    {
	      cloog_upol_set_next (tail2, cloog_upol_copy (d2));
	      tail2 = cloog_upol_next (tail2);
	    }
	}
    }

  if (!head1)
    res = cloog_new_domain (head2);
  else
    {
      cloog_upol_set_next (tail1, head2);
      res = cloog_new_domain (head1);
    }

  return cloog_check_domain (res);
}

/**
 * cloog_domain_intersection function:
 * This function returns a new CloogDomain structure including a polyhedral
 * domain which is the intersection of two polyhedral domains (pol1)inter(pol2)
 * inside two CloogDomain structures.
 */
CloogDomain *
cloog_domain_intersection (CloogDomain * dom1, CloogDomain * dom2)
{
  CloogDomain *res = NULL;
  polyhedra_union p1, p2;
  ppl_Polyhedron_t ppl1, ppl2;

  for (p1 = cloog_domain_upol (dom1); p1; p1 = cloog_upol_next (p1))
    {
      CloogMatrix *m1 = cloog_upol_domain2matrix (p1);
      ppl1 = cloog_translate_constraint_matrix (m1);
      cloog_matrix_free (m1);

      for (p2 = cloog_domain_upol (dom2); p2; p2 = cloog_upol_next (p2))
	{
	  CloogMatrix *m2 = cloog_upol_domain2matrix (p2);
	  ppl2 = cloog_translate_constraint_matrix (m2);
	  cloog_matrix_free (m2);
	  ppl_Polyhedron_intersection_assign (ppl2, ppl1);
	  res = cloog_domain_union (res, cloog_translate_ppl_polyhedron (ppl2));
	  ppl_delete_Polyhedron (ppl2);
	}
      ppl_delete_Polyhedron (ppl1);
    }

  return res;
}

/* Returns d1 minus d2.  */

CloogDomain *
cloog_domain_difference (CloogDomain * d1, CloogDomain * d2)
{
  CloogDomain *res = NULL, *d = d1;
  polyhedra_union p1, p2;

  if (cloog_domain_isempty (d2))
    return cloog_domain_copy (d1);

  for (p2 = cloog_domain_upol (d2); p2; p2 = cloog_upol_next (p2))
    {
      CloogMatrix *m2 = cloog_upol_domain2matrix (p2);

      for (p1 = cloog_domain_upol (d); p1; p1 = cloog_upol_next (p1))
	{
	  int i;
	  CloogMatrix *m1 = cloog_upol_domain2matrix (p1);

	  for (i = 0; i < m2->NbRows; i++)
	    {
	      ppl_Polyhedron_t p3;
	      ppl_Constraint_t cstr;

	      /* Don't handle "0 >= 0".  */
	      if (cloog_positivity_constraint_p (m2, i,
						 cloog_domain_dim (d) + 1))
		continue;

	      /* Add the constraint "-m2[i] - 1 >= 0".  */
	      p3 = cloog_translate_constraint_matrix (m1);
	      cstr = cloog_translate_oppose_constraint (m2, i, -1, 1);
	      ppl_Polyhedron_add_constraint (p3, cstr);
	      ppl_delete_Constraint (cstr);
	      res = cloog_domain_union (res, cloog_translate_ppl_polyhedron (p3));
	      ppl_delete_Polyhedron (p3);

	      /* For an equality, add the constraint "m2[i] - 1 >= 0".  */
	      if (cloog_matrix_row_is_eq_p (m2, i))
		{
		  p3 = cloog_translate_constraint_matrix (m1);
		  cstr = cloog_translate_constraint (m2, i, -1, 1);
		  ppl_Polyhedron_add_constraint (p3, cstr);
		  ppl_delete_Constraint (cstr);
		  res = cloog_domain_union (res, cloog_translate_ppl_polyhedron (p3));
		  ppl_delete_Polyhedron (p3);
		}
	    }
	  cloog_matrix_free (m1);
	}
      d = res;
      res = NULL;
      cloog_matrix_free (m2);
    }

  if (!d)
    res = cloog_domain_empty (cloog_domain_dim (d2));
  else
    res = d;

  return res;
}


/**
 * cloog_domain_addconstraints function :
 * This function adds source's polyhedron constraints to target polyhedron: for
 * each element of the polyhedron inside 'target' (i.e. element of the union
 * of polyhedra) it adds the constraints of the corresponding element in
 * 'source'.
 * - August 10th 2002: first version.
 * Nota bene for future : it is possible that source and target don't have the
 * same number of elements (try iftest2 without non-shared constraint
 * elimination in cloog_loop_separate !). This function is yet another part
 * of the DomainSimplify patching problem...
 */
CloogDomain *
cloog_domain_addconstraints (CloogDomain *domain_source, CloogDomain *domain_target)
{
  CloogDomain *res;
  ppl_Polyhedron_t ppl;
  polyhedra_union source, target, last;
  int dim = cloog_domain_dim (domain_target);
  CloogMatrix *m1, *m2;

  source = cloog_domain_upol (domain_source);
  target = cloog_domain_upol (domain_target);

  ppl_new_C_Polyhedron_from_space_dimension (&ppl, dim, 0);

  m1 = cloog_upol_domain2matrix (target);
  m2 = cloog_upol_domain2matrix (source);
  cloog_translate_constraint_matrix_1 (ppl, m1);
  cloog_translate_constraint_matrix_1 (ppl, m2);
  cloog_matrix_free (m1);
  cloog_matrix_free (m2);

  res = cloog_translate_ppl_polyhedron (ppl);
  ppl_delete_Polyhedron (ppl);
  last = cloog_domain_upol (res);

  source = cloog_upol_next (source);
  target = cloog_upol_next (target);

  while (target)
    {
      ppl_new_C_Polyhedron_from_space_dimension (&ppl, dim, 0);

      m1 = cloog_upol_domain2matrix (target);
      cloog_translate_constraint_matrix_1 (ppl, m1);
      cloog_matrix_free (m1);

      if (source)
	{
	  m2 = cloog_upol_domain2matrix (source);
	  cloog_translate_constraint_matrix_1 (ppl, m2);
	  cloog_matrix_free (m2);
	  source = cloog_upol_next (source);
	}

      cloog_upol_set_next
	(last, cloog_domain_upol (cloog_translate_ppl_polyhedron (ppl)));
      ppl_delete_Polyhedron (ppl);

      last = cloog_upol_next (last);
      target = cloog_upol_next (target);
    }

  return res;
}

/* Compares P1 to P2: returns 0 when the polyhedra don't overlap,
   returns 1 when p1 >= p2, and returns -1 when p1 < p2.  The ">"
   relation is the "contains" relation.  */

static int
cloog_domain_polyhedron_compare (CloogMatrix *m1, CloogMatrix *m2, int level, int nb_par, int dimension)
{
  int i, j;
  ppl_Polyhedron_t q1, q2, q3, q4, q5, q;
  ppl_Polyhedron_t p1, p2;

  p1 = cloog_translate_constraint_matrix (m1);
  if (ppl_Polyhedron_is_empty (p1))
    {
      ppl_delete_Polyhedron (p1);
      return 0;
    }

  p2 = cloog_translate_constraint_matrix (m2);
  if (ppl_Polyhedron_is_empty (p2))
    {
      ppl_delete_Polyhedron (p2);
      return 0;
    }

  ppl_new_C_Polyhedron_from_C_Polyhedron (&q1, p1);
  ppl_new_C_Polyhedron_from_C_Polyhedron (&q2, p2);

  for (i = level; i < dimension - nb_par + 1; i++)
    {
      Value val;
      ppl_Coefficient_t d;
      ppl_Linear_Expression_t expr;
      ppl_Generator_t g;

      value_init (val);
      value_set_si (val, 1);
      ppl_new_Coefficient_from_mpz_t (&d, val);
      ppl_new_Linear_Expression_with_dimension (&expr, dimension);
      ppl_Linear_Expression_add_to_coefficient (expr, i - 1, d);
      ppl_new_Generator (&g, expr, PPL_GENERATOR_TYPE_LINE, d);
      ppl_Polyhedron_add_generator (q1, g);
      ppl_Polyhedron_add_generator (q2, g);
      ppl_delete_Generator (g);
      ppl_delete_Linear_Expression (expr);
      ppl_delete_Coefficient (d);
    }

  ppl_new_C_Polyhedron_from_C_Polyhedron (&q, q1);
  ppl_Polyhedron_intersection_assign (q, q2);
  ppl_delete_Polyhedron (q1);
  ppl_delete_Polyhedron (q2);

  if (ppl_Polyhedron_is_empty (q))
    {
      ppl_delete_Polyhedron (p1);
      ppl_delete_Polyhedron (p2);
      ppl_delete_Polyhedron (q);
      return 0;
    }

  ppl_new_C_Polyhedron_from_C_Polyhedron (&q1, p1);
  ppl_new_C_Polyhedron_from_C_Polyhedron (&q2, p2);
  ppl_delete_Polyhedron (p1);
  ppl_delete_Polyhedron (p2);

  ppl_Polyhedron_intersection_assign (q1, q);
  ppl_Polyhedron_intersection_assign (q2, q);

  m1 = cloog_upol_domain2matrix (cloog_domain_upol (cloog_translate_ppl_polyhedron (q1)));
  m2 = cloog_upol_domain2matrix (cloog_domain_upol (cloog_translate_ppl_polyhedron (q2)));

  ppl_new_C_Polyhedron_from_C_Polyhedron (&q4, q);
  for (i = 0; i < m1->NbRows; i++)
    if (value_one_p (m1->p[i][0])
	&& value_pos_p (m1->p[i][level]))
      {
	ppl_Constraint_t c = cloog_translate_constraint (m1, i, 0, 1);
	ppl_Polyhedron_add_constraint (q4, c);
	ppl_delete_Constraint (c);
      }

  for (i = 0; i < m2->NbRows; i++)
    if (value_one_p (m2->p[i][0])
	&& value_neg_p (m2->p[i][level]))
      {
	ppl_Constraint_t c = cloog_translate_constraint (m2, i, 0, 1);
	ppl_Polyhedron_add_constraint (q4, c);
	ppl_delete_Constraint (c);
      }

  if (ppl_Polyhedron_is_empty (q4))
    {
      ppl_delete_Polyhedron (q);
      ppl_delete_Polyhedron (q1);
      ppl_delete_Polyhedron (q2);
      ppl_delete_Polyhedron (q4);
      return 1;
    }

  ppl_delete_Polyhedron (q4);
  ppl_new_C_Polyhedron_from_C_Polyhedron (&q3, q);
  for (i = 0; i < m1->NbRows; i++)
    {
      if (value_zero_p (m1->p[i][0]))
	{
	  if (value_zero_p (m1->p[i][level]))
	    continue;

	  else if (value_neg_p (m1->p[i][level]))
	    {
	      ppl_Constraint_t c = cloog_translate_oppose_constraint (m1, i, 0, 1);
	      ppl_Polyhedron_add_constraint (q3, c);
	      ppl_delete_Constraint (c);
	    }

	  else
	    {
	      ppl_Constraint_t c = cloog_translate_constraint (m1, i, 0, 1);
	      ppl_Polyhedron_add_constraint (q3, c);
	      ppl_delete_Constraint (c);
	    }
	}

      else if (value_neg_p (m1->p[i][level]))
	{
	  ppl_Constraint_t c = cloog_translate_oppose_constraint (m1, i, 0, 1);
	  ppl_Polyhedron_add_constraint (q3, c);
	  ppl_delete_Constraint (c);
	}

      else
	continue;

      ppl_new_C_Polyhedron_from_C_Polyhedron (&q5, q3);
      for (j = 0; j < m2->NbRows; j++)
	{
	  if (value_zero_p (m2->p[j][0]))
	    {
	      if (value_zero_p (m2->p[j][level]))
		continue;

	      else if (value_pos_p (m2->p[j][level]))
		{
		  ppl_Constraint_t c = cloog_translate_oppose_constraint (m2, j, 0, 1);
		  ppl_Polyhedron_add_constraint (q5, c);
		  ppl_delete_Constraint (c);
		}

	      else
		{
		  ppl_Constraint_t c = cloog_translate_constraint (m2, j, 0, 1);
		  ppl_Polyhedron_add_constraint (q5, c);
		  ppl_delete_Constraint (c);
		}
	    }

	  else if (value_pos_p (m2->p[j][level]))
	    {
	      ppl_Constraint_t c = cloog_translate_oppose_constraint (m2, j, 0, 1);
	      ppl_Polyhedron_add_constraint (q5, c);
	      ppl_delete_Constraint (c);
	    }

	  else
	    continue;

	  if (!ppl_Polyhedron_is_empty (q5))
	    {
	      ppl_delete_Polyhedron (q);
	      ppl_delete_Polyhedron (q1);
	      ppl_delete_Polyhedron (q2);
	      ppl_delete_Polyhedron (q3);
	      ppl_delete_Polyhedron (q5);
	      return -1;
	    }

	  /* Reinitialize Q5.  */
	  ppl_delete_Polyhedron (q5);
	  ppl_new_C_Polyhedron_from_C_Polyhedron (&q5, q3);
	}

      /* Reinitialize Q3.  */
      ppl_delete_Polyhedron (q3);
      ppl_new_C_Polyhedron_from_C_Polyhedron (&q3, q);
    }

  cloog_matrix_free (m1);
  cloog_matrix_free (m2);
  ppl_delete_Polyhedron (q);
  ppl_delete_Polyhedron (q1);
  ppl_delete_Polyhedron (q2);
  ppl_delete_Polyhedron (q3);
  ppl_delete_Polyhedron (q5);
  return 1;
}

/**
 * cloog_domain_sort function:
 * This function topologically sorts (nb_pols) polyhedra. Here (pols) is a an
 * array of pointers to polyhedra, (nb_pols) is the number of polyhedra,
 * (level) is the level to consider for partial ordering (nb_par) is the
 * parameter space dimension, (permut) if not NULL, is an array of (nb_pols)
 * integers that contains a permutation specification after call in order to
 * apply the topological sorting.
 */

void
cloog_domain_sort (CloogDomain **doms, unsigned nb_pols, unsigned level,
		   unsigned nb_par, int *permut)
{
  int i, j;
  int dim = cloog_domain_dim (doms[0]);

  for (i = 0; i < (int) nb_pols; i++)
    permut[i] = i + 1;

  /* Note that here we do a comparison per tuple of polyhedra.
     PolyLib does not do this, but instead it does fewer comparisons
     and with a complex reasoning they infer that it some comparisons
     are not useful.  The result is that PolyLib has wrong permutations.

     FIXME: In the PolyLib backend, Cloog should use this algorithm
     instead of PolyhedronTSort, and cloog_domain_polyhedron_compare
     should be implemented with a simple call to PolyhedronLTQ: these
     two functions produce identical answers.  */
  for (i = 0; i < (int) nb_pols; i++)
    for (j = i + 1; j < (int) nb_pols; j++)
    {
      CloogMatrix *m1 = cloog_upol_domain2matrix (cloog_domain_upol (doms[i]));
      CloogMatrix *m2 = cloog_upol_domain2matrix (cloog_domain_upol (doms[j]));

      if (cloog_domain_polyhedron_compare (m1, m2, level, nb_par, dim) == 1)
	{
	  int v = permut[i];
	  permut[i] = permut[j];
	  permut[j] = v;
	}

      cloog_matrix_free (m1);
      cloog_matrix_free (m2);
    }
}

/**
 * cloog_domain_empty function:
 * This function allocates the memory space for a CloogDomain structure and
 * sets its polyhedron to an empty polyhedron with 'dimension' dimensions.
 * Then it returns a pointer to the allocated space.
 * - June 10th 2005: first version.
 */
CloogDomain *
cloog_domain_empty (int dimension)
{
  return cloog_domain_alloc (cloog_empty_polyhedron (dimension));
}


/******************************************************************************
 *                          Structure display function                        *
 ******************************************************************************/


/**
 * cloog_domain_print_structure :
 * this function is a more human-friendly way to display the CloogDomain data
 * structure, it only shows the constraint system and includes an indentation
 * level (level) in order to work with others print_structure functions.
 * Written by Olivier Chorier, Luc Marchaud, Pierre Martin and Romain Tartiere.
 * - April 24th 2005: Initial version.
 * - May   26th 2005: Memory leak hunt.
 * - June  16th 2005: (Ced) Integration in domain.c.
 */
void
cloog_domain_print_structure (FILE * file, CloogDomain * domain, int level)
{
  int i;
  CloogMatrix *matrix;

  /* Go to the right level. */
  for (i = 0; i < level; i++)
    fprintf (file, "|\t");

  if (domain != NULL)
    {
      fprintf (file, "+-- CloogDomain\n");

      /* Print the matrix. */
      matrix = cloog_upol_domain2matrix (cloog_domain_upol (domain));
      cloog_matrix_print_structure (file, matrix, level);
      cloog_matrix_free (matrix);

      /* A blank line. */
      for (i = 0; i < level + 1; i++)
	fprintf (file, "|\t");
      fprintf (file, "\n");
    }
  else
    fprintf (file, "+-- Null CloogDomain\n");

}


/**
 * cloog_domain_list_print function:
 * This function prints the content of a CloogDomainList structure into a
 * file (foo, possibly stdout).
 * - November 6th 2001: first version.
 */
void
cloog_domain_list_print (FILE * foo, CloogDomainList * list)
{
  while (list != NULL)
    {
      cloog_domain_print (foo, cloog_domain (list));
      list = cloog_next_domain (list);
    }
}


/******************************************************************************
 *                         Memory deallocation function                       *
 ******************************************************************************/


/**
 * cloog_domain_list_free function:
 * This function frees the allocated memory for a CloogDomainList structure.
 * - November 6th 2001: first version.
 */
void
cloog_domain_list_free (CloogDomainList * list)
{
  CloogDomainList *temp;

  while (list != NULL)
    {
      temp = cloog_next_domain (list);
      cloog_domain_free (cloog_domain (list));
      free (list);
      list = temp;
    }
}


/******************************************************************************
 *                               Reading function                             *
 ******************************************************************************/


/**
 * cloog_domain_read function:
 * Adaptation from the PolyLib. This function reads a matrix into a file (foo,
 * posibly stdin) and returns a pointer to a polyhedron containing the read
 * information.
 * - October 18th 2001: first version.
 */
CloogDomain *
cloog_domain_read (FILE * foo)
{
  CloogMatrix *matrix;
  CloogDomain *domain;

  matrix = cloog_matrix_read (foo);
  domain = cloog_domain_matrix2domain (matrix);
  cloog_matrix_free (matrix);

  return domain;
}


/**
 * cloog_domain_union_read function:
 * This function reads a union of polyhedra into a file (foo, posibly stdin) and
 * returns a pointer to a Polyhedron containing the read information.
 * - September 9th 2002: first version.
 * - October  29th 2005: (debug) removal of a leak counting "correction" that
 *                       was just false since ages.
 */
CloogDomain *
cloog_domain_union_read (FILE * foo)
{
  int i, nb_components;
  char s[MAX_STRING];
  CloogDomain *domain, *temp, *old;

  /* domain reading: nb_components (constraint matrices). */
  while (fgets (s, MAX_STRING, foo) == 0);
  while ((*s == '#' || *s == '\n') || (sscanf (s, " %d", &nb_components) < 1))
    fgets (s, MAX_STRING, foo);

  if (nb_components > 0)
    {				/* 1. first part of the polyhedra union, */
      domain = cloog_domain_read (foo);
      /* 2. and the nexts. */
      for (i = 1; i < nb_components; i++)
	{			/* Leak counting is OK since next allocated domain is freed here. */
	  temp = cloog_domain_read (foo);
	  old = domain;
	  domain = cloog_domain_union (temp, old);
	  cloog_domain_free (temp);
	  cloog_domain_free (old);
	}
      return cloog_check_domain (domain);
    }
  else
    return NULL;
}


/**
 * cloog_domain_list_read function:
 * This function reads a list of polyhedra into a file (foo, posibly stdin) and
 * returns a pointer to a CloogDomainList containing the read information.
 * - November 6th 2001: first version.
 */
CloogDomainList *
cloog_domain_list_read (FILE * foo)
{
  int i, nb_pols;
  char s[MAX_STRING];
  CloogDomainList *list, *now, *next;


  /* We read first the number of polyhedra in the list. */
  while (fgets (s, MAX_STRING, foo) == 0);
  while ((*s == '#' || *s == '\n') || (sscanf (s, " %d", &nb_pols) < 1))
    fgets (s, MAX_STRING, foo);

  /* Then we read the polyhedra. */
  list = NULL;
  if (nb_pols > 0)
    {
      list = (CloogDomainList *) malloc (sizeof (CloogDomainList));
      cloog_set_domain (list, cloog_domain_read (foo));
      cloog_set_next_domain (list, NULL);
      now = list;
      for (i = 1; i < nb_pols; i++)
	{
	  next = (CloogDomainList *) malloc (sizeof (CloogDomainList));
	  cloog_set_domain (next, cloog_domain_read (foo));
	  cloog_set_next_domain (next, NULL);
	  cloog_set_next_domain (now, next);
	  now = cloog_next_domain (now);
	}
    }
  return list;
}


/******************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/

/**
 * cloog_domain_isempty function:
 * This function returns 1 if the polyhedron given as input is empty, 0
 * otherwise.
 * - October 28th 2001: first version.
 */
int
cloog_domain_isempty (CloogDomain * domain)
{
  if (cloog_domain_polyhedron (domain) == NULL)
    return 1;

  if (cloog_upol_next (cloog_domain_upol (domain)))
    return 0;

  return (cloog_domain_dim (domain) < cloog_domain_nbeq (domain)) ? 1 : 0;
}

/**
 * cloog_domain_project function:
 * From Quillere's LoopGen 0.4. This function returns the projection of
 * (domain) on the (level) first dimensions (i.e. outer loops). It returns a
 * pointer to the projected Polyhedron.
 * - nb_par is the number of parameters.
 **
 * - October 27th 2001: first version.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 */
CloogDomain *
cloog_domain_project (CloogDomain * domain, int level, int nb_par)
{
  CloogDomain *res = NULL;
  polyhedra_union upol = cloog_domain_upol (domain);
  int i, diff = cloog_domain_dim (domain) - level - nb_par;
  int n;
  ppl_dimension_type *to_remove;

  if (diff < 0)
    {
      fprintf (stderr, "cloog_domain_project should not be called with"
	       "cloog_domain_dim (domain) < level + nb_par");
      exit (1);
    }

  if (diff == 0)
    return cloog_domain_copy (domain);

  n = diff;
  to_remove = (ppl_dimension_type *) malloc (n * sizeof (ppl_dimension_type));

  for (i = 0; i < n; i++)
    to_remove[i] = i + level;

  while (upol)
    {
      CloogMatrix *m1 = cloog_upol_domain2matrix (upol);
      ppl_Polyhedron_t ppl = cloog_translate_constraint_matrix (m1);
      cloog_matrix_free (m1);

      ppl_Polyhedron_remove_space_dimensions (ppl, to_remove, n);
      res = cloog_domain_add_domain (res, cloog_translate_ppl_polyhedron (ppl));
      ppl_delete_Polyhedron (ppl);
      upol = cloog_upol_next (upol);
    }

  free (to_remove);
  return res;
}

/**
 * cloog_domain_extend function:
 * From Quillere's LoopGen 0.4. This function returns the (domain) given as
 * input with (dim)+(nb_par) dimensions. The new dimensions are added before
 * the (nb_par) parameters. This function does not free (domain), and returns
 * a new polyhedron.
 * - nb_par is the number of parameters.
 **
 * - October 27th 2001: first version.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                 CLooG 0.12.1).
 */
CloogDomain *
cloog_domain_extend (CloogDomain * domain, int dim, int nb_par)
{
  CloogDomain *res = NULL;
  polyhedra_union upol = cloog_domain_upol (domain);
  int i, k, n, diff = dim + nb_par - cloog_domain_dim (domain);
  ppl_dimension_type *map;
  ppl_dimension_type to_add = diff;

  if (diff == 0)
    return cloog_domain_copy (domain);

  n = dim + nb_par;
  map = (ppl_dimension_type *) malloc (n * sizeof (ppl_dimension_type));

  k = cloog_domain_dim (domain) - nb_par;
  for (i = 0; i < k; i++)
    map[i] = i;

  k += nb_par;
  for (; i < k; i++)
    map[i] = i + diff;

  k += diff;
  for (; i < k; i++)
    map[i] = i - nb_par;

  while (upol)
    {
      CloogMatrix *mat = cloog_upol_domain2matrix (upol);
      ppl_Polyhedron_t ppl = cloog_translate_constraint_matrix (mat);
      cloog_matrix_free (mat);

      ppl_Polyhedron_add_space_dimensions_and_embed (ppl, to_add);
      ppl_Polyhedron_map_space_dimensions (ppl, map, n);
      res = cloog_domain_add_domain (res, cloog_translate_ppl_polyhedron (ppl));
      ppl_delete_Polyhedron (ppl);
      upol = cloog_upol_next (upol);
    }

  free (map);
  return res;
}

/**
 * cloog_domain_never_integral function:
 * For us, an equality like 3*i -4 = 0 is always false since 4%3 != 0. This
 * function returns a boolean set to 1 if there is this kind of 'never true'
 * constraint inside a polyhedron, 0 otherwise.
 * - domain is the polyhedron to check,
 **
 * - November 28th 2001: first version.
 * - June 26th 2003: for iterators, more 'never true' constraints are found
 *                   (compare cholesky2 and vivien with a previous version),
 *                   checking for the parameters created (compare using vivien).
 * - June 28th 2003: Previously in loop.c and called
 *                   cloog_loop_simplify_nevertrue, now here !
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 * - October 14th 2005: Complete rewriting, not faster but code quite shorter.
 */
int
cloog_domain_never_integral (CloogDomain * domain)
{
  int i, dimension, nbc;
  Value gcd, modulo;
  polyhedron p;

  if ((domain == NULL) || (cloog_domain_polyhedron (domain) == NULL))
    return 1;

  value_init_c (gcd);
  value_init_c (modulo);
  p = cloog_domain_polyhedron (domain);
  dimension = cloog_domain_dim (domain) + 2;
  nbc = cloog_domain_nbconstraints (domain);

  /* For each constraint... */
  for (i = 0; i < nbc; i++)
    {				/* If we have an equality and the scalar part is not zero... */
      if (value_zero_p (p->Constraint[i][0]) &&
	  value_notzero_p (p->Constraint[i][dimension - 1]))
	{			/* Then we check whether the scalar can be divided by the gcd of the
				 * unknown vector (including iterators and parameters) or not. If not,
				 * there is no integer point in the polyhedron and we return 1.
				 */
	  cloog_vector_gcd (&(p->Constraint[i][1]), dimension - 2, &gcd);
	  value_modulus (modulo,
			 p->Constraint[i][dimension - 1],
			 gcd);

	  if (value_notzero_p (modulo))
	    {
	      value_clear_c (gcd);
	      value_clear_c (modulo);
	      return 1;
	    }
	}
    }

  value_clear_c (gcd);
  value_clear_c (modulo);
  return 0;
}

static int
cloog_vector_matrix_smallest_column (Value * a, int n, int p, int q)
{
  int res, numero = 0, k, allzero;
  Value minus, comp;
  Value *c;

  value_init (minus);
  value_init (comp);
  c = a + q - 1 + p * (n - 1);
  allzero = 1;
  value_absolute (comp, *c);

  for (k = n; k > q; k--, c -= p, value_absolute (comp, *c))
    {
      if (value_notzero_p (comp))
	{
	  if (allzero == 1)
	    {
	      value_assign (minus, comp);
	      numero = k;
	      allzero = 0;
	    }
	  else if (value_ge (minus, comp))
	    {
	      value_assign (minus, comp);
	      numero = k;
	    }
	}
    }

  if (allzero)
    res = 0;
  else
    {
      value_absolute (comp, *c);
      if ((value_notzero_p (comp)) && (value_ge (minus, comp)))
	res = q;
      else
	res = numero;
    }

  value_clear (minus);
  value_clear (comp);

  return res;
}

static void
cloog_vector_matrix_swap_rows (Value * a, int i, int j, int p)
{
  int k;
  Value *c1 = a + (i - 1) * p;
  Value *c2 = a + (j - 1) * p;
  Value s;

  value_init (s);

  for (k = 1; k <= p; k++)
    {
      value_assign (s, *c1);
      value_assign (*c1, *c2);
      value_assign (*c2, s);
      c1++;
      c2++;
    }
  value_clear (s);
  return;
}

static void
cloog_vector_matrix_swap_columns (Value * a, int i, int j, int n, int p)
{
  int k;
  Value s;
  Value *c1, *c2;

  value_init (s);
  c1 = a + (i - 1);
  c2 = a + (j - 1);

  for (k = 1; k <= n; k++)
    {
      value_assign (s, *c1);
      value_assign (*c1, *c2);
      value_assign (*c2, s);
      c1 = c1 + p;
      c2 = c2 + p;
    }
  value_clear (s);
  return;
}

static void
cloog_vector_matrix_oppose_line (Value * a, int i, int p)
{
  int k;
  Value *c = a + (i - 1) * p;

  for (k = 1; k <= p; k++, c++)
    value_oppose (*c, *c);
}

static void
cloog_vector_matrix_oppose_column (Value * a, int i, int n, int p)
{
  int k;
  Value *c = a + (i - 1);

  for (k = 1; k <= n; k++, c += p)
    value_oppose (*c, *c);
}

static void
cloog_vector_matrix_combine_line (Value * a, int i, int j,
				  Value x, int p)
{
  int k;
  Value *c1 = a + (i - 1) * p;
  Value *c2 = a + (j - 1) * p;

  for (k = 1; k <= p; k++, c1++, c2++)
    value_addmul (*c1, x, *c2);
}

static void
cloog_vector_matrix_combine_column (Value * a, int i, int j, Value x, int n, int p)
{
  int k;
  Value *c1 = a + (i - 1);
  Value *c2 = a + (j - 1);

  for (k = 1; k <= n; k++, c1 = c1 + p, c2 = c2 + p)
    value_addmul (*c1, x, *c2);
}

static void
cloog_matrix_hermite_combine (Value *a, Value *b, Value c, Value *d, int k, int n,
			      int p, int q, Value pivot, Value x, Value x_inv)
{
  Value tmp;

  value_init (tmp);
  value_division (x, c, pivot);
  value_modulus (tmp, c, pivot);

  if (value_neg_p (tmp))
    value_decrement (x, x);

  value_clear (tmp);

  value_oppose (x_inv, x);
  cloog_vector_matrix_combine_line (a, k, q, x_inv, p);
  cloog_vector_matrix_combine_column (b, q, k, x, n, n);
  cloog_vector_matrix_combine_line (d, k, q, x_inv, n);
}

static void
cloog_matrix_hermite_oppose (Value *a, Value *b, Value *d, int n, int p, int q, Value pivot)
{
  cloog_vector_matrix_oppose_line (a, q, p);
  cloog_vector_matrix_oppose_line (d, q, n);
  cloog_vector_matrix_oppose_column (b, q, n, n);
  value_oppose (pivot, pivot);
}

static void
cloog_matrix_hermite_1 (Value * a, Value * b, Value * d, int n, int p, int q)
{
  int i, k;
  Value x, pivot, x_inv;
  Value *c;

  if (q > p || q > n)
    return;

  value_init (pivot);
  value_init (x);
  value_init (x_inv);

  for (i = cloog_vector_matrix_smallest_column (a, n, p, q); i != 0;
       i = cloog_vector_matrix_smallest_column (a, n, p, q))
    {
      if (i != q)
	{
	  cloog_vector_matrix_swap_rows (a, i, q, p);
	  cloog_vector_matrix_swap_columns (b, i, q, n, n);
	  cloog_vector_matrix_swap_rows (d, i, q, n);
	}

      c = a + (q - 1) * (p + 1);
      value_assign (pivot, *c);
      if (value_neg_p (pivot))
	cloog_matrix_hermite_oppose (a, b, d, n, p, q, pivot);

      for (c += p, k = q + 1; k <= n; k++, c += p)
	if (value_notzero_p (*c))
	  cloog_matrix_hermite_combine (a, b, *c, d, k, n, p, q, pivot, x, x_inv);
    }

  c = a + (q - 1);
  value_assign (pivot, *(c + (q - 1) * p));
  if (value_neg_p (pivot))
    cloog_matrix_hermite_oppose (a, b, d, n, p, q, pivot);

  if (value_notzero_p (pivot))
    for (k = 1; k < q; k++, c += p)
      if (value_notzero_p (*c))
	cloog_matrix_hermite_combine (a, b, *c, d, k, n, p, q, pivot, x, x_inv);

  cloog_matrix_hermite_1 (a, b, d, n, p, q + 1);

  value_clear (pivot);
  value_clear (x);
  value_clear (x_inv);
  return;
}

static CloogMatrix *
cloog_matrix_add_null_row (CloogMatrix * M)
{
  int i, j;
  CloogMatrix *res = cloog_matrix_alloc (M->NbRows + 1, M->NbColumns);

  for (i = 0; i < M->NbRows; i++)
    for (j = 0; j < M->NbColumns; j++)
      value_assign (res->p[i][j], M->p[i][j]);

  for (j = 0; j < M->NbColumns; j++)
    value_set_si (res->p[i][j], 0);

  return res;
}

static CloogMatrix *
cloog_matrix_transpose (CloogMatrix * m)
{
  int i, j;
  CloogMatrix *res = cloog_matrix_alloc (m->NbColumns, m->NbRows);

  for (i = 0; i < (int) m->NbRows; i++)
    for (j = 0; j < (int) m->NbColumns; j++)
      value_assign (res->p[j][i], m->p[i][j]);

  return res;
}

static Value *
cloog_vector_matrix_vectorify (CloogMatrix * A)
{
  int i, j;
  Value *res = (Value *) malloc (sizeof (Value) * A->NbRows * A->NbColumns);

  for (i = 0; i < A->NbRows * A->NbColumns; i++)
    value_init (res[i]);

  for (i = 0; i < A->NbRows; i++)
    for (j = 0; j < A->NbColumns; j++)
      value_assign (res[i * A->NbColumns + j], A->p[i][j]);

  return res;
}

static CloogMatrix *
cloog_vector_matrix_matrixify (Value * A, int NbRows, int NbCols)
{
  int i, j;
  CloogMatrix *res = cloog_matrix_alloc (NbRows, NbCols);

  for (i = 0; i < NbRows; i++)
    for (j = 0; j < NbCols; j++)
      value_assign (res->p[i][j], A[i * NbCols + j]);

  return res;
}

static Value *
cloog_vector_matrix_identity (int n)
{
  int i, j;
  Value *res = (Value *) malloc (sizeof (Value) * n * n);
  Value *ptr = res;

  for (i = 0; i < n * n; i++)
    value_init (res[i]);

  for (i = 1; i <= n; i++)
    for (j = 1; j <= n; j++, ptr++)
      if (i == j)
	value_set_si (*ptr, 1);
      else
	value_set_si (*ptr, 0);

  return res;
}

static void
cloog_matrix_hermite (CloogMatrix * a, CloogMatrix ** H, CloogMatrix ** U)
{
  int i;
  CloogMatrix *tu, *transpose = cloog_matrix_transpose (a);
  Value *va = cloog_vector_matrix_vectorify (transpose);
  Value *vid = cloog_vector_matrix_identity (a->NbColumns);
  Value *vid_inv = cloog_vector_matrix_identity (a->NbColumns);

  cloog_matrix_free (transpose);

  cloog_matrix_hermite_1 (va, vid, vid_inv,
			  a->NbColumns, a->NbRows, 1);

  transpose = cloog_vector_matrix_matrixify (va, a->NbColumns, a->NbRows);
  H[0] = cloog_matrix_transpose (transpose);
  cloog_matrix_free (transpose);

  tu = cloog_vector_matrix_matrixify (vid, a->NbColumns, a->NbColumns);
  U[0] = cloog_matrix_transpose (tu);
  cloog_matrix_free (tu);

  for (i = 0; i < a->NbRows * a->NbColumns; i++)
    value_clear (va[i]);

  for (i = 0; i < a->NbColumns * a->NbColumns; i++)
    value_clear (vid[i]);

  for (i = 0; i < a->NbColumns * a->NbColumns; i++)
    value_clear (vid_inv[i]);

  free (va);
  free (vid);
  free (vid_inv);
}

static inline void
cloog_exchange_rows (CloogMatrix * m, int row1, int row2)
{
  int i;

  for (i = 0; i < (int) m->NbColumns; i++)
    value_swap (m->p[row1][i], m->p[row2][i]);
}

static CloogMatrix *
cloog_dio_copy_submatrix (CloogMatrix * matrix)
{
  int i, j ;
  CloogMatrix * copy = cloog_matrix_alloc (matrix->NbRows - 1, matrix->NbColumns - 1) ;

  for (i = 0; i < copy->NbRows; i++)
    for (j = 0; j < copy->NbColumns; j++)
      value_assign(copy->p[i][j], matrix->p[i][j]) ;

  return copy;
}

static void
cloog_dio_rearrange_redundant_rows (CloogMatrix * M)
{
  int i, end, row, rank = 1;
  CloogMatrix *A, *L, *H, *U;

  L = cloog_dio_copy_submatrix (M);
  if (L->NbRows < 2)
    goto done;

  A = cloog_matrix_alloc (1, L->NbColumns);
  for (i = 0; i < L->NbColumns; i++)
    value_assign (A->p[0][i], L->p[0][i]);

  end = L->NbRows - 1;
  row = 1;

  while (1)
    {
      int n;
      CloogMatrix *temp = cloog_matrix_add_null_row (A);

      for (i = 0; i < A->NbColumns; i++)
	value_assign (temp->p[row][i], L->p[row][i]);

      cloog_matrix_hermite (temp, &H, &U);
      cloog_matrix_free (U);

      n = H->NbRows;
      for (i = 0; i < n; i++)
	if (value_zero_p (H->p[i][i]))
	  break;

      cloog_matrix_free (H);

      if (i != n)
	{
	  /* This is a redundant row: put it at the end.  */
	  cloog_exchange_rows (M, row, end);
	  end--;
	}
      else
	{
	  row++;
	  rank++;
	  cloog_matrix_free (A);
	  A = cloog_matrix_copy (temp);
	  cloog_matrix_free (temp);
	}

      if (rank == (end >= L->NbColumns ? L->NbColumns : end)
	  || row >= end)
	break;
    }

  cloog_matrix_free (A);

 done:
  cloog_matrix_free (L);
  return;
}

static void
cloog_matrix_inverse_pivot (CloogMatrix *m, int low, int up, int i, int j,
			    Value m1, Value m2, Value x, Value piv)
{
  int c;

  for (c = low; c < up; ++c)
    {
      value_multiply (m1, piv, m->p[j][c]);
      value_multiply (m2, x, m->p[i][c]);
      value_subtract (m->p[j][c], m1, m2);
    }
}

static Value *
cloog_matrix_inverse_den (CloogMatrix *Mat, CloogMatrix *MatInv, int k, Value *x)
{
  int j, c;
  Value gcd;
  Value *res = (Value *) malloc (k * sizeof (Value));
  Value m1;

  value_init (m1);
  value_init (gcd);
  value_set_si (*x, 1);

  for (j = 0; j < cloog_matrix_nrows (Mat); ++j)
    {
      value_init (res[j]);
      value_assign (res[j], Mat->p[j][j]);
      cloog_vector_gcd (&MatInv->p[j][0], k, &gcd);
      Gcd (gcd, res[j], &gcd);

      if (value_neg_p (res[j]))
	value_oppose (gcd, gcd);

      if (value_notone_p (gcd))
	{
	  for (c = 0; c < k; c++)
	    value_division (MatInv->p[j][c], MatInv->p[j][c], gcd);
	  value_division (res[j], res[j], gcd);
	}

      Gcd (*x, res[j], &gcd);
      value_division (m1, res[j], gcd);
      value_multiply (*x, *x, m1);
    }

  value_clear (m1);
  value_clear (gcd);
  return res;
}

static void
cloog_matrix_inverse_div (CloogMatrix *MatInv, Value *den, Value m1, Value x)
{
  int j, c;

  if (value_notone_p (x))
    for (j = 0; j < MatInv->NbRows; ++j)
      {
	for (c = 0; c < MatInv->NbColumns; c++)
	  {
	    value_division (m1, x, den[j]);
	    value_multiply (MatInv->p[j][c], MatInv->p[j][c], m1);
	  }
      }
}

static int
cloog_matrix_inverse_triang (CloogMatrix *Mat, CloogMatrix *MatInv, Value *m1, Value *m2)
{
  int i, j;
  Value gcd, piv, x;
  int res = 0;

  value_init (gcd);
  value_init (piv);
  value_init (x);

  for (i = 0; i < cloog_matrix_ncolumns (Mat); ++i)
    {
      if (value_zero_p (Mat->p[i][i]))
	{
	  for (j = i; j < cloog_matrix_nrows (Mat); ++j)
	    if (value_notzero_p (Mat->p[j][i]))
	      break;

	  if (j == cloog_matrix_nrows (Mat))
	    goto done;

	  cloog_matrix_exchange_rows (Mat, i, j);
	  cloog_matrix_exchange_rows (MatInv, i, j);
	}

      for (j = 0; j < cloog_matrix_nrows (Mat) ; ++j)
	{
	  if (j == i
	      || !value_notzero_p (Mat->p[j][i]))
	    continue;

	  value_assign (x, Mat->p[j][i]);
	  value_assign (piv, Mat->p[i][i]);
	  Gcd (x, piv, &gcd);
	  if (value_notone_p (gcd))
	    {
	      value_division (x, x, gcd);
	      value_division (piv, piv, gcd);
	    }

	  cloog_matrix_inverse_pivot (Mat, ((j > i) ? i : 0),
				      cloog_matrix_nrows (Mat),
				      i, j, *m1, *m2, x, piv);
	  cloog_matrix_inverse_pivot (MatInv, 0, cloog_matrix_nrows (Mat),
				      i, j, *m1, *m2, x, piv);

	  cloog_vector_gcd (&MatInv->p[j][0], (unsigned) cloog_matrix_nrows (Mat), m1);
	  cloog_vector_gcd (&Mat->p[j][0], (unsigned) cloog_matrix_nrows (Mat), m2);
	  Gcd (*m1, *m2, &gcd);
	  if (value_notone_p (gcd))
	    {
	      int c;

	      for (c = 0; c < cloog_matrix_nrows (Mat); ++c)
		{
		  value_division (Mat->p[j][c], Mat->p[j][c], gcd);
		  value_division (MatInv->p[j][c], MatInv->p[j][c], gcd);
		}
	    }
	}
    }

  res = 1;

 done:
  value_clear (x);
  value_clear (piv);
  value_clear (gcd);
  return res;
}

static int
cloog_matrix_inverse (CloogMatrix * Mat, CloogMatrix * MatInv)
{
  int res = 0;
  int i, j;
  Value x;
  Value m1, m2;
  Value *den;

  if (Mat->NbRows != Mat->NbColumns)
    return 0;

  value_init (x);
  value_init (m1);
  value_init (m2);

  cloog_vector_set (MatInv->p[0], 0, cloog_matrix_nrows (Mat) * cloog_matrix_nrows (Mat));

  for (i = 0; i < cloog_matrix_nrows (Mat); ++i)
    value_set_si (MatInv->p[i][i], 1);

  if (!cloog_matrix_inverse_triang (Mat, MatInv, &m1, &m2))
    goto done;

  den = cloog_matrix_inverse_den (Mat, MatInv, cloog_matrix_nrows (Mat), &x);
  cloog_matrix_inverse_div (MatInv, den, m1, x);

  res = 1;

  for (j = 0; j < cloog_matrix_nrows (Mat); ++j)
    value_clear (den[j]);
  free (den);

 done:
  value_clear (x);
  value_clear (m1);
  value_clear (m2);

  return res;
}

static void
cloog_dio_copy (CloogMatrix *m1, CloogMatrix *m2)
{
  int i, j;
  int n = m2->NbRows - 1;
  int m = m2->NbColumns - 1;

  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      value_assign (m1->p[i][j], m2->p[i][j]);
}

static Value *
cloog_dio_negate_coeffs (CloogMatrix *m)
{
  int i;
  int n = m->NbRows - 1;
  int k = m->NbColumns - 1;
  Value *res = (Value *) malloc (sizeof (Value) * (n));

  for (i = 0; i < n; i++)
    {
      value_init (res[i]);
      value_oppose (res[i], m->p[i][k]);
    }

  return res;
}

static int
cloog_dio_get_first_diagonal_zero (CloogMatrix *m)
{
  int i, n = m->NbRows <= m->NbColumns ? m->NbRows : m->NbColumns;
  int res = 0;

  for (i = 0; i < n; i++)
    if (value_notzero_p (m->p[i][i]))
      res++;
    else
      break;

  return res;
}

static Value *
cloog_dio_reduce_diagonal (CloogMatrix *m, Value *coeffs, int nbc, int rank)
{
  int i, j;
  Value *res = (Value *) malloc (sizeof (Value) * nbc);
  Value sum, tmp;

  value_init (tmp);
  value_init (sum);

  for (i = 0; i < nbc; i++)
    value_init (res[i]);

  for (i = 0; i < rank; i++)
    {
      value_set_si (sum, 0);
      for (j = 0; j < i; j++)
	value_addmul (sum, res[j], m->p[i][j]);

      value_subtract (tmp, coeffs[i], sum);
      value_modulus (tmp, tmp, m->p[i][i]);
      if (value_notzero_p (tmp))
	{
	  for (i = 0; i < nbc; i++)
	    value_clear (res[i]);
	  free (res);

	  value_clear (tmp);
	  value_clear (sum);

	  return NULL;
	}

      value_subtract (tmp, coeffs[i], sum);
      value_division (res[i], tmp, m->p[i][i]);
    }

  for (i = rank; i < m->NbColumns; i++)
    value_set_si (res[i], 0);

  value_clear (tmp);
  value_clear (sum);

  return res;
}

static int
cloog_dio_check_coeffs (CloogMatrix *m, Value *T, Value *coeffs, int nbr, int nbc, int rank)
{
  int res = 1;
  Value sum, tmp;
  int i, j;

  value_init (sum);
  value_init (tmp);

  for (i = rank; i < m->NbRows; i++)
    {
      value_set_si (sum, 0);
      for (j = 0; j < m->NbColumns; j++)
	value_addmul (sum, T[j], m->p[i][j]);

      if (value_ne (sum, coeffs[i]))
	{
	  for (i = 0; i < nbr; i++)
	    value_clear (coeffs[i]);
	  for (i = 0; i < nbc; i++)
	    value_clear (T[i]);
	  free (coeffs);
	  free (T);

	  res = 0;
	  goto clear;
	}
    }

 clear:
  value_clear (sum);
  value_clear (tmp);
  return res;
}

static CloogMatrix *
cloog_dio_init_U (CloogMatrix *u, int n, int rank)
{
  int i, j;
  CloogMatrix *res;

  if (rank == n)
    return cloog_matrix_alloc (0, 0);

  res = cloog_matrix_alloc (n, n - rank);

  for (i = 0; i < res->NbRows; i++)
    for (j = 0; j < res->NbColumns; j++)
      value_assign (res->p[i][j], u->p[i][j + rank]);

  return res;
}

static Vector *
cloog_dio_init_X (CloogMatrix *u, Value *t, int n)
{
  int i, j;
  Vector *res = Vector_Alloc (n);
  Value sum;

  value_init (sum);
  for (i = 0; i < u->NbRows; i++)
    {
      value_set_si (sum, 0);

      for (j = 0; j < u->NbColumns; j++)
	value_addmul (sum, u->p[i][j], t[j]);

      value_assign (res->p[i], sum);
    }

  value_clear (sum);
  return res;
}

static int
cloog_solve_diophantine (CloogMatrix * m, CloogMatrix ** u, Vector ** x)
{
  int i, nbr, nbc, rank;
  CloogMatrix *A, *temp, *hermi, *unimod, *unimodinv;
  Value *coeffs;
  Value *T;

  A = cloog_matrix_copy (m);
  nbr = A->NbRows - 1;

  cloog_dio_rearrange_redundant_rows (A);

  temp = cloog_matrix_alloc (A->NbRows - 1, A->NbColumns - 1);
  cloog_dio_copy (temp, A);
  coeffs = cloog_dio_negate_coeffs (A);
  cloog_matrix_free (A);

  cloog_matrix_hermite (temp, &hermi, &unimod);
  rank = cloog_dio_get_first_diagonal_zero (hermi);
  nbc = temp->NbColumns;

  T = cloog_dio_reduce_diagonal (hermi, coeffs, nbc, rank);
  unimodinv = cloog_matrix_alloc (unimod->NbRows, unimod->NbColumns);

  if (!T
      || !cloog_dio_check_coeffs (hermi, T, coeffs, nbr, nbc, rank)
      || !cloog_matrix_inverse (unimod, unimodinv))
    {
      *u = cloog_matrix_alloc (0, 0);
      *x = Vector_Alloc (0);

      for (i = 0; i < nbr; i++)
	value_clear (coeffs[i]);

      free (coeffs);
      return -1;
    }

  *u = cloog_dio_init_U (unimodinv, hermi->NbColumns, rank);
  *x = cloog_dio_init_X (unimodinv, T, m->NbColumns - 1);

  cloog_matrix_free (unimod);
  cloog_matrix_free (unimodinv);
  cloog_matrix_free (hermi);
  cloog_matrix_free (temp);

  for (i = 0; i < nbr; i++)
    value_clear (coeffs[i]);

  for (i = 0; i < nbc; i++)
    value_clear (T[i]);

  free (coeffs);
  free (T);
  return rank;
}

/**
 * cloog_domain_stride function:
 * This function finds the stride imposed to unknown with the column number
 * 'strided_level' in order to be integral. For instance, if we have a
 * constraint like -i - 2j + 2k = 0, and we consider k, then k can be integral
 * only if (i + 2j)%2 = 0. Then only if i%2 = 0. Then k imposes a stride 2 to
 * the unknown i. The function returns the imposed stride in a parameter field.
 * - domain is the set of constraint we have to consider,
 * - strided_level is the column number of the unknown for which a stride have
 *   to be found,
 * - looking_level is the column number of the unknown that impose a stride to
 *   the first unknown.
 * - stride is the stride that is returned back as a function parameter.
 * - offset is the value of the constant c if the condition is of the shape
 *   (i + c)%s = 0, s being the stride.
 **
 * - June 28th 2003: first version.
 * - July 14th 2003: can now look for multiple striding constraints and returns
 *                   the GCD of the strides and the common offset.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 */
void
cloog_domain_stride (CloogDomain *domain, int strided_level, int nb_par, Value *stride, Value *offset)
{
  int i;
  int n_col, n_row, rank;
  CloogMatrix *M;
  CloogMatrix *U;
  Vector *V;
  polyhedron p = cloog_domain_polyhedron (domain);
  int dimension = cloog_domain_dim (domain);
  int nbeq = cloog_domain_nbeq (domain);

  /* Look at all equalities involving strided_level and the inner
   * iterators.  We can ignore the outer iterators and the parameters
   * here because the lower bound on strided_level is assumed to
   * be a constant.
   */
  n_col = (1 + dimension - nb_par) - strided_level;
  for (i = 0, n_row = 0; i < nbeq; i++)
    if (cloog_first_non_zero
	(p->Constraint[i] + strided_level, n_col) != -1)
      ++n_row;

  M = cloog_matrix_alloc (n_row + 1, n_col + 1);
  for (i = 0, n_row = 0; i < nbeq; i++)
    {
      if (cloog_first_non_zero
	  (p->Constraint[i] + strided_level, n_col) == -1)
	continue;
      cloog_vector_copy (p->Constraint[i] + strided_level,
			 M->p[n_row], n_col);
      value_assign (M->p[n_row][n_col],
		    p->Constraint[i][1 + dimension]);
      ++n_row;
    }
  value_set_si (M->p[n_row][n_col], 1);

  /* Then look at the general solution to the above equalities. */
  rank = cloog_solve_diophantine (M, &U, &V);
  cloog_matrix_free (M);

  if (rank == -1)
    {
      /* There is no solution, so the body of this loop will
       * never execute.  We just leave the constraints alone here so
       * that they will ensure the body will not be executed.
       * We should probably propagate this information up so that
       * the loop can be removed entirely.
       */
      value_set_si (*offset, 0);
      value_set_si (*stride, 1);
    }
  else
    {
      /* Compute the gcd of the coefficients defining strided_level. */
      cloog_vector_gcd (U->p[0], U->NbColumns, stride);
      value_oppose (*offset, V->p[0]);
      value_pmodulus (*offset, *offset, *stride);
    }

  cloog_matrix_free (U);
  Vector_Free (V);
  return;
}


/**
 * cloog_domain_integral_lowerbound function:
 * This function returns 1 if the lower bound of an iterator (such as its
 * column rank in the constraint set 'domain' is 'level') is integral,
 * 0 otherwise. If the lower bound is actually integral, the function fills
 * the 'lower' field with the lower bound value.
 * - June 29th 2003: first version.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 */
int
cloog_domain_integral_lowerbound (CloogDomain *domain, int level, Value *lower)
{
  int i, first_lower = 1, dimension, lower_constraint = -1, nbc;
  Value iterator, constant, tmp;
  polyhedron p = cloog_domain_polyhedron (domain);
  dimension = cloog_domain_dim (domain);
  nbc = cloog_domain_nbconstraints (domain);

  /* We want one and only one lower bound (e.g. no equality, no maximum
   * calculation...).
   */
  for (i = 0; i < nbc; i++)
    if (value_zero_p (p->Constraint[i][0])
	&& value_notzero_p (p->Constraint[i][level]))
      return 0;

  for (i = 0; i < nbc; i++)
    if (value_pos_p (p->Constraint[i][level]))
      {
	if (first_lower)
	  {
	    first_lower = 0;
	    lower_constraint = i;
	  }
	else
	  return 0;
      }

  if (first_lower)
    return 0;

  /* We want an integral lower bound: no other non-zero entry except the
   * iterator coefficient and the constant.
   */
  for (i = 1; i < level; i++)
    if (value_notzero_p
	(p->Constraint[lower_constraint][i]))
      return 0;

  for (i = level + 1; i <= dimension; i++)
    if (value_notzero_p
	(p->Constraint[lower_constraint][i]))
      {
	return 0;
      }

  value_init_c (iterator);
  value_init_c (constant);
  value_init_c (tmp);

  /* If all is passed, then find the lower bound and return 1. */
  value_assign (iterator,
		p->Constraint[lower_constraint][level]);
  value_oppose (constant,
		p->Constraint[lower_constraint][dimension + 1]);

  value_modulus (tmp, constant, iterator);
  value_division (*lower, constant, iterator);

  if (!(value_zero_p (tmp) || value_neg_p (constant)))
    value_increment (*lower, *lower);

  value_clear_c (iterator);
  value_clear_c (constant);
  value_clear_c (tmp);
  return 1;
}


/**
 * cloog_domain_lowerbound_update function:
 * This function updates the integral lower bound of an iterator (such as its
 * column rank in the constraint set 'domain' is 'level') into  'lower'.
 * - Jun  29th 2003: first version.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 */
void
cloog_domain_lowerbound_update (CloogDomain *domain, int level, Value lower)
{
  int i;
  int nbc = cloog_domain_nbconstraints (domain);
  int dim = cloog_domain_dim (domain);
  polyhedron p = cloog_domain_polyhedron (domain);

  /* There is only one lower bound, the first one is the good one. */
  for (i = 0; i < nbc; i++)
    if (value_pos_p (p->Constraint[i][level]))
      {
	value_set_si (p->Constraint[i][level], 1);
	value_oppose (p->Constraint[i][dim + 1], lower);
	break;
      }
}


/**
 * cloog_domain_lazy_equal function:
 * This function returns 1 if the domains given as input are the same, 0 if it
 * is unable to decide. This function makes an entry-to-entry comparison between
 * the constraint systems, if all the entries are the same, the domains are
 * obviously the same and it returns 1, at the first difference, it returns 0.
 * This is a very fast way to verify this property. It has been shown (with the
 * CLooG benchmarks) that operations on equal domains are 17% of all the
 * polyhedral computations. For 75% of the actually identical domains, this
 * function answer that they are the same and allow to give immediately the
 * trivial solution instead of calling the heavy general functions of PolyLib.
 * - August 22th 2003: first version.
 * - June 21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                   CLooG 0.12.1).
 */
int
cloog_domain_lazy_equal (CloogDomain * d1, CloogDomain * d2)
{
  int i, j;
  polyhedra_union u1 = cloog_domain_upol (d1);
  polyhedra_union u2 = cloog_domain_upol (d2);

  while (u1 && u2)
    {
      if ((cloog_upol_nbc (u1) != cloog_upol_nbc (u2)) ||
	  (cloog_upol_dim (u1) != cloog_upol_dim (u2)))
	return 0;

      for (i = 0; i < (int) cloog_upol_nbc (u1); i++)
	for (j = 0; j < (int) cloog_upol_dim (u1) + 2; j++)
	  if (value_ne (cloog_upol_polyhedron (u1)->Constraint[i][j],
			cloog_upol_polyhedron (u2)->Constraint[i][j]))
	    return 0;

      u1 = cloog_upol_next (u1);
      u2 = cloog_upol_next (u2);
    }

  if (u1 || u2)
    return 0;

  return 1;
}


/**
 * cloog_domain_lazy_block function:
 * This function returns 1 if the two domains d1 and d2 given as input are the
 * same (possibly except for a dimension equal to a constant where we accept
 * a difference of 1) AND if we are sure that there are no other domain in
 * the code generation problem that may put integral points between those of
 * d1 and d2 (0 otherwise). In fact this function answers the question "can I
 * safely consider the two domains as only one with two statements (a block) ?".
 * This function is lazy: it asks for very standard scattering representation
 * (only one constraint per dimension which is an equality, and the constraints
 * are ordered per dimension depth: the left hand side of the constraint matrix
 * is the identity) and will answer NO at the very first problem.
 * - d1 and d2 are the two domains to check for blocking,
 * - scattering is the linked list of all domains,
 * - scattdims is the total number of scattering dimentions.
 **
 * - April   30th 2005: beginning
 * - June     9th 2005: first working version.
 * - June    10th 2005: debugging.
 * - June    21rd 2005: Adaptation for GMP.
 * - October 16th 2005: (debug) some false blocks have been removed.
 */
int
cloog_domain_lazy_block (CloogDomain *d1, CloogDomain *d2, CloogDomainList *scattering, int scattdims)
{
  int i, j, difference = 0, different_constraint = 0, nbc;
  int dim1, dim2;
  Value date1, date2, date3, temp;
  polyhedron p1, p2;

  /* Some basic checks: we only accept convex domains, with same constraint
   * and dimension numbers.
   */
  if (!cloog_domain_isconvex (d1) || !cloog_domain_isconvex (d2) ||
      (cloog_domain_nbconstraints (d1) != cloog_domain_nbconstraints (d2)) ||
      (cloog_domain_dim (d1) != cloog_domain_dim (d2)))
    return 0;

  p1 = cloog_domain_polyhedron (d1);
  p2 = cloog_domain_polyhedron (d2);
  nbc = cloog_domain_nbconstraints (d1);
  dim1 = cloog_domain_dim (d1);
  dim2 = cloog_domain_dim (d2);

  /* There should be only one difference between the two domains, it
   * has to be at the constant level and the difference must be of +1,
   * moreover, after the difference all domain coefficient has to be 0.
   * The matrix shape is:
   *
   * |===========|=====|<- 0 line
   * |===========|=====|
   * |===========|====?|<- different_constraint line (found here)
   * |===========|0000=|
   * |===========|0000=|<- pX->NbConstraints line
   *  ^         ^     ^
   *  |         |     |
   *  |         |     (pX->Dimension + 2) column
   *  |         scattdims column
   *  0 column
   */

  value_init_c (temp);
  for (i = 0; i < nbc; i++)
    {
      if (difference == 0)
	{			/* All elements except scalar must be equal. */
	  for (j = 0; j < dim1 + 1; j++)
	    if (value_ne (p1->Constraint[i][j],
			  p2->Constraint[i][j]))
	      {
		value_clear_c (temp);
		return 0;
	      }
	  /* The scalar may differ from +1 (now j=(p1->Dimension + 1)). */
	  if (value_ne (p1->Constraint[i][j],
			p2->Constraint[i][j]))
	    {
	      value_increment (temp, p2->Constraint[i][j]);
	      if (value_ne (p1->Constraint[i][j], temp))
		{
		  value_clear_c (temp);
		  return 0;
		}
	      else
		{
		  difference = 1;
		  different_constraint = i;
		}
	    }
	}
      else
	{			/* Scattering coefficients must be equal. */
	  for (j = 0; j < (scattdims + 1); j++)
	    if (value_ne (p1->Constraint[i][j],
			  p2->Constraint[i][j]))
	      {
		value_clear_c (temp);
		return 0;
	      }

	  /* Domain coefficients must be 0. */
	  for (; j < dim1 + 1; j++)
	    if (value_notzero_p (p1->Constraint[i][j])
		|| value_notzero_p (p2->Constraint[i][j]))
	      {
		value_clear_c (temp);
		return 0;
	      }

	  /* Scalar must be equal. */
	  if (value_ne (p1->Constraint[i][j],
			p2->Constraint[i][j]))
	    {
	      value_clear_c (temp);
	      return 0;
	    }
	}
    }
  value_clear_c (temp);

  /* If the domains are exactly the same, this is a block. */
  if (difference == 0)
    return 1;

  /* Now a basic check that the constraint with the difference is an
   * equality of a dimension with a constant.
   */
  for (i = 0; i <= different_constraint; i++)
    if (value_notzero_p (p1->Constraint[different_constraint][i]))
      return 0;

  if (value_notone_p (p1->Constraint[different_constraint][different_constraint + 1]))
    return 0;

  for (i = different_constraint + 2; i < dim1 + 1; i++)
    if (value_notzero_p (p1->Constraint[different_constraint][i]))
      return 0;

  /* For the moment, d1 and d2 are a block candidate. There remains to check
   * that there is no other domain that may put an integral point between
   * them. In our lazy test we ensure this property by verifying that the
   * constraint matrices have a very strict shape: let us consider that the
   * dimension with the difference is d. Then the first d dimensions are
   * defined in their depth order using equalities (thus the first column begins
   * with d zeroes, there is a d*d identity matrix and a zero-matrix for
   * the remaining simensions). If a domain can put integral points between the
   * domains of the block candidate, this means that the other entries on the
   * first d constraints are equal to those of d1 or d2. Thus we are looking for
   * such a constraint system, if it exists d1 and d2 is considered to not be
   * a block, it is a bock otherwise.
   *
   *  1. Only equalities (for the first different_constraint+1 lines).
   *  |  2. Must be the identity.
   *  |  |    3. Must be zero.
   *  |  |    |     4. Elements are equal, the last one is either date1 or 2.
   *  |  |    |     |
   *  | /-\ /---\ /---\
   * |0|100|00000|=====|<- 0 line
   * |0|010|00000|=====|
   * |0|001|00000|====?|<- different_constraint line
   * |*|***|*****|*****|
   * |*|***|*****|*****|<- pX->NbConstraints line
   *  ^   ^     ^     ^
   *  |   |     |     |
   *  |   |     |     (pX->Dimension + 2) column
   *  |   |     scattdims column
   *  |   different_constraint+1 column
   *  0 column
   */

  /* Step 1 and 2. This is only necessary to check one domain because
   * we checked that they are equal on this part before.
   */
  for (i = 0; i <= different_constraint; i++)
    {
      for (j = 0; j < i + 1; j++)
	if (value_notzero_p (p1->Constraint[i][j]))
	  return 0;

      if (value_notone_p (p1->Constraint[i][i + 1]))
	return 0;

      for (j = i + 2; j <= different_constraint + 1; j++)
	if (value_notzero_p (p1->Constraint[i][j]))
	  return 0;
    }

  /* Step 3. */
  for (i = 0; i <= different_constraint; i++)
    for (j = different_constraint + 2; j <= scattdims; j++)
      if (value_notzero_p (p1->Constraint[i][j]))
	return 0;

  value_init_c (date1);
  value_init_c (date2);
  value_init_c (date3);

  /* Now we have to check that the two different dates are unique. */
  value_assign (date1, p1->Constraint[different_constraint][dim1 + 1]);
  value_assign (date2, p2->Constraint[different_constraint][dim2 + 1]);

  /* Step 4. We check all domains except d1 and d2 and we look for at least
   * a difference with d1 or d2 on the first different_constraint+1 dimensions.
   */
  while (scattering != NULL)
    {
      if ((cloog_domain (scattering) != d1)
	  && (cloog_domain (scattering) != d2))
	{
	  CloogDomain *d3 = cloog_domain (scattering);
	  polyhedron p3 = cloog_domain_polyhedron (d3);
	  int dim3 = cloog_domain_dim (d3);

	  value_assign (date3,
			p3->Constraint[different_constraint][dim3 + 1]);
	  difference = 0;

	  if (value_ne (date3, date2) && value_ne (date3, date1))
	    difference = 1;

	  for (i = 0; (i < different_constraint) && (!difference); i++)
	    for (j = 0; (j < dim3 + 2) && !difference; j++)
	      if (value_ne
		  (p1->Constraint[i][j],
		   p3->Constraint[i][j]))
		difference = 1;

	  for (j = 0; (j < dim3 + 1) && !difference; j++)
	    if (value_ne
		(p1->Constraint[different_constraint][j],
		 p3->Constraint[different_constraint][j]))
	      difference = 1;

	  if (!difference)
	    {
	      value_clear_c (date1);
	      value_clear_c (date2);
	      value_clear_c (date3);
	      return 0;
	    }
	}

      scattering = cloog_next_domain (scattering);
    }

  value_clear_c (date1);
  value_clear_c (date2);
  value_clear_c (date3);
  return 1;
}


/**
 * cloog_domain_lazy_disjoint function:
 * This function returns 1 if the domains given as input are disjoint, 0 if it
 * is unable to decide. This function finds the unknown with fixed values in
 * both domains (on a given constraint, their column entry is not zero and
 * only the constant coefficient can be different from zero) and verify that
 * their values are the same. If not, the domains are obviously disjoint and
 * it returns 1, if there is not such case it returns 0.  This is a very fast
 * way to verify this property. It has been shown (with the CLooG benchmarks)
 * that operations on disjoint domains are 36% of all the polyhedral
 * computations. For 94% of the actually identical domains, this
 * function answer that they are disjoint and allow to give immediately the
 * trivial solution instead of calling the heavy general functions of PolyLib.
 * - August 22th 2003: first version.
 * - June   21rd 2005: Adaptation for GMP (based on S. Verdoolaege's version of
 *                     CLooG 0.12.1).
 */
int
cloog_domain_lazy_disjoint (CloogDomain * d1, CloogDomain * d2)
{
  int i1, j1, i2, j2, scat_dim, nbc1, nbc2;
  int dim1, dim2;
  Value scat_val;
  polyhedron p1, p2;

  if (!cloog_domain_isconvex (d1) || !cloog_domain_isconvex (d2))
    return 0;

  p1 = cloog_domain_polyhedron (d1);
  p2 = cloog_domain_polyhedron (d2);
  nbc1 = cloog_domain_nbconstraints (d1);
  nbc2 = cloog_domain_nbconstraints (d2);
  dim1 = cloog_domain_dim (d1);
  dim2 = cloog_domain_dim (d2);
  value_init_c (scat_val);

  for (i1 = 0; i1 < nbc1; i1++)
    {
      if (value_notzero_p (p1->Constraint[i1][0]))
	continue;

      scat_dim = 1;
      while (value_zero_p (p1->Constraint[i1][scat_dim]) &&
	     (scat_dim < dim1))
	scat_dim++;

      if (value_notone_p (p1->Constraint[i1][scat_dim]))
	continue;
      else
	{
	  for (j1 = scat_dim + 1; j1 <= dim1; j1++)
	    if (value_notzero_p (p1->Constraint[i1][j1]))
	      break;

	  if (j1 != dim1 + 1)
	    continue;

	  value_assign (scat_val,
			p1->Constraint[i1][dim1 + 1]);

	  for (i2 = 0; i2 < nbc2; i2++)
	    {
	      for (j2 = 0; j2 < scat_dim; j2++)
		if (value_notzero_p (p2->Constraint[i2][j2]))
		  break;

	      if ((j2 != scat_dim)
		  ||
		  value_notone_p (p2->Constraint[i2][scat_dim]))
		continue;

	      for (j2 = scat_dim + 1; j2 < dim2; j2++)
		if (value_notzero_p (p2->Constraint[i2][j2]))
		  break;

	      if (j2 != dim2)
		continue;

	      if (value_ne
		  (p2->Constraint[i2][dim2 + 1], scat_val))
		{
		  value_clear_c (scat_val);
		  return 1;
		}
	    }
	}
    }

  value_clear_c (scat_val);
  return 0;
}


/**
 * cloog_domain_list_lazy_same function:
 * This function returns 1 if two domains in the list are the same, 0 if it
 * is unable to decide.
 * - February 9th 2004: first version.
 */
int
cloog_domain_list_lazy_same (CloogDomainList * list)
{				/*int i=1, j=1 ; */
  CloogDomainList *current, *next;

  current = list;
  while (current != NULL)
    {
      next = cloog_next_domain (current);
      /*j=i+1; */
      while (next != NULL)
	{
	  if (cloog_domain_lazy_equal (cloog_domain (current),
				       cloog_domain (next)))
	    {			/*printf("Same domains: %d and %d\n",i,j) ; */
	      return 1;
	    }
	  /*j++ ; */
	  next = cloog_next_domain (next);
	}
      /*i++ ; */
      current = cloog_next_domain (current);
    }

  return 0;
}

/**
 * cloog_domain_cut_first function:
 * this function returns a CloogDomain structure with everything except the
 * first part of the polyhedra union of the input domain as domain. After a call
 * to this function, there remains in the CloogDomain structure provided as
 * input only the first part of the original polyhedra union.
 * - April 20th 2005: first version, extracted from different part of loop.c.
 */
CloogDomain *
cloog_domain_cut_first (CloogDomain * domain)
{
  CloogDomain *rest;

  if (domain && cloog_domain_polyhedron (domain))
    {
      if (!cloog_upol_next (cloog_domain_upol (domain)))
	return NULL;

      rest = cloog_new_domain (cloog_upol_next (cloog_domain_upol (domain)));
      cloog_upol_set_next (cloog_domain_upol (domain), NULL);
    }
  else
    rest = NULL;

  return cloog_check_domain (rest);
}


/**
 * cloog_domain_lazy_isscalar function:
 * this function returns 1 if the dimension 'dimension' in the domain 'domain'
 * is scalar, this means that the only constraint on this dimension must have
 * the shape "x.dimension + scalar = 0" with x an integral variable. This
 * function is lazy since we only accept x=1 (further calculations are easier
 * in this way).
 * - June 14th 2005: first version.
 * - June 21rd 2005: Adaptation for GMP.
 */
int
cloog_domain_lazy_isscalar (CloogDomain * domain, int dimension)
{
  int i, j;
  polyhedron p = cloog_domain_polyhedron (domain);
  int nbc = cloog_domain_nbconstraints (domain);
  int dim = cloog_domain_dim (domain);

  /* For each constraint... */
  for (i = 0; i < nbc; i++)
    {				/* ...if it is concerned by the potentially scalar dimension... */
      if (value_notzero_p
	  (p->Constraint[i][dimension + 1]))
	{			/* ...check that the constraint has the shape "dimension + scalar = 0". */
	  for (j = 0; j <= dimension; j++)
	    if (value_notzero_p (p->Constraint[i][j]))
	      return 0;

	  if (value_notone_p
	      (p->Constraint[i][dimension + 1]))
	    return 0;

	  for (j = dimension + 2; j < dim + 1; j++)
	    if (value_notzero_p (p->Constraint[i][j]))
	      return 0;
	}
    }

  return 1;
}


/**
 * cloog_domain_scalar function:
 * when we call this function, we know that "dimension" is a scalar dimension,
 * this function finds the scalar value in "domain" and returns it in "value".
 * - June 30th 2005: first version.
 */
void
cloog_domain_scalar (CloogDomain * domain, int dimension, Value * value)
{
  int i;
  polyhedron p = cloog_domain_polyhedron (domain);
  int nbc = cloog_domain_nbconstraints (domain);
  int dim = cloog_domain_dim (domain);

  /* For each constraint... */
  for (i = 0; i < nbc; i++)
    {				/* ...if it is the equality defining the scalar dimension... */
      if (value_notzero_p
	  (p->Constraint[i][dimension + 1])
	  && value_zero_p (p->Constraint[i][0]))
	{			/* ...Then send the scalar value. */
	  value_assign (*value, p->Constraint[i][dim + 1]);
	  value_oppose (*value, *value);
	  return;
	}
    }

  /* We should have found a scalar value: if not, there is an error. */
  fprintf (stderr, "[CLooG]ERROR: dimension %d is not scalar as expected.\n",
	   dimension);
  exit (0);
}


/**
 * cloog_domain_erase_dimension function:
 * this function returns a CloogDomain structure builds from 'domain' where
 * we removed the dimension 'dimension' and every constraint considering this
 * dimension. This is not a projection ! Every data concerning the
 * considered dimension is simply erased.
 * - June 14th 2005: first version.
 * - June 21rd 2005: Adaptation for GMP.
 */
CloogDomain *
cloog_domain_erase_dimension (CloogDomain * domain, int dimension)
{
  int i, j, mi, nb_dim, nbc;
  CloogMatrix *matrix;
  CloogDomain *erased;
  polyhedron p = cloog_domain_polyhedron (domain);

  nb_dim = cloog_domain_dim (domain);
  nbc = cloog_domain_nbconstraints (domain);

  /* The matrix is one column less and at least one constraint less. */
  matrix = cloog_matrix_alloc (nbc - 1, nb_dim + 1);

  /* mi is the constraint counter for the matrix. */
  mi = 0;
  for (i = 0; i < nbc; i++)
    if (value_zero_p (p->Constraint[i][dimension + 1]))
      {
	for (j = 0; j <= dimension; j++)
	  value_assign (matrix->p[mi][j],
			p->Constraint[i][j]);

	for (j = dimension + 2; j < nb_dim + 2; j++)
	  value_assign (matrix->p[mi][j - 1],
			p->Constraint[i][j]);

	mi++;
      }

  erased = cloog_domain_matrix2domain (matrix);
  cloog_matrix_free (matrix);

  return cloog_check_domain (erased);
}

/* Number of polyhedra inside the union of disjoint polyhedra.  */

unsigned
cloog_domain_nb_polyhedra (CloogDomain * domain)
{
  unsigned res = 0;
  polyhedra_union upol = cloog_domain_upol (domain);

  while (upol)
    {
      res++;
      upol = cloog_upol_next (upol);
    }

  return res;
}

void
cloog_domain_print_polyhedra (FILE * foo, CloogDomain * domain)
{
  polyhedra_union upol = cloog_domain_upol (domain);

  while (upol != NULL)
    {
      CloogMatrix *matrix = cloog_upol_domain2matrix (upol);
      cloog_matrix_print (foo, matrix);
      cloog_matrix_free (matrix);
      upol = cloog_upol_next (upol);
    }
}

void
debug_cloog_domain (CloogDomain *domain)
{
  cloog_domain_print_polyhedra (stderr, domain);
}

void
debug_cloog_matrix (CloogMatrix *m)
{
  cloog_matrix_print (stderr, m);
}

void
debug_value (Value v)
{
  value_print (stderr, VALUE_FMT, v);
}

void
debug_values (Value *p, int length)
{
  int i;
  for (i = 0; i < length; i++)
    debug_value (p[i]);
}

polyhedra_union cloog_new_upol (polyhedron p)
{
  polyhedra_union ppl =
    (polyhedra_union) malloc (sizeof (struct polyhedra_union));
  ppl->_polyhedron = p;
  ppl->_next = NULL;
  return ppl;
}

Vector *Vector_Alloc (unsigned length)
{
  unsigned i;
  Vector *vector = (Vector *) malloc (sizeof (Vector));

  vector->Size = length;
  vector->p = (Value *) malloc (length * sizeof (Value));

  for (i = 0; i < length; i++)
    value_init (vector->p[i]);

  return vector;
}

polyhedron cloog_new_pol (int dim, int nrows)
{
  int i;
  polyhedron res = (polyhedron) malloc (sizeof (struct polyhedron));
  int ncolumns = dim + 2;
  int n = nrows * ncolumns;
  Value *p = (Value *) malloc (n * sizeof (Value));

  res->Dimension = dim;
  res->NbConstraints = nrows;
  res->Constraint = (Value **) malloc (nrows * sizeof (Value *));

  for (i = 0; i < n; ++i)
    value_init (p[i]);

  for (i = 0; i < nrows; i++, p += ncolumns)
    res->Constraint[i] = p;

  return res;
}
