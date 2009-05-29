   /**-------------------------------------------------------------------**
    **                               CLooG                               **
    **-------------------------------------------------------------------**
    **                            polyhedron.h                           **
    **-------------------------------------------------------------------**
    **                   First version: July 22th 2008                   **
    **-------------------------------------------------------------------**/


/******************************************************************************
 *               CLooG : the Chunky Loop Generator (experimental)             *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2001-2008 Cedric Bastoul                                     *
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
 *                                                                            *
 ******************************************************************************/

#include <ppl_c.h>
#include <gmp.h>
#include <stdlib.h>
#include <string.h>

#ifndef CLOOG_PPL_BACKEND_H
#define CLOOG_PPL_BACKEND_H

#if PPL_VERSION_MAJOR == 0 && PPL_VERSION_MINOR < 10
# error "PPL version 0.10 or following is required"
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

  extern void cloog_initialize (void);
  static inline void cloog_finalize (void)
  {
    ppl_finalize ();
  }

  typedef mpz_t Value;
#define VALUE_FMT "%s"

#define value_init(val) mpz_init (val)
#define value_assign(v1, v2) mpz_set (v1, v2)
#define value_set_si(val, i) mpz_set_si (val, i)
#define value_get_si(val) mpz_get_si (val)
#define value_set_double(val, d) mpz_set_d (val, d)
#define value_clear(val) mpz_clear (val)
#define value_read(val, str) mpz_set_str (val, str, 10)
#define value_print(Dst, fmt, val) {char *str; \
    void (*gmp_free) (void *, size_t);			       \
    str = mpz_get_str(0,10,(val));			       \
    fprintf((Dst),(fmt),str);						\
    mp_get_memory_functions(NULL, NULL, &gmp_free);			\
    (*gmp_free) (str, strlen(str)+1);					\
                              }
#define value_swap(val1, val2) mpz_swap (val1, val2)
#define value_eq(v1, v2) (mpz_cmp((v1),(v2)) == 0)
#define value_ne(v1,v2) (mpz_cmp((v1),(v2)) != 0)
#define value_gt(v1,v2) (mpz_cmp((v1),(v2))  > 0)
#define value_ge(v1,v2) (mpz_cmp((v1),(v2)) >= 0)
#define value_lt(v1,v2) (mpz_cmp((v1),(v2))  < 0)
#define value_le(v1,v2) (mpz_cmp((v1),(v2)) <= 0)
#define value_abs_eq(v1,v2) (mpz_cmpabs((v1),(v2)) == 0)
#define value_abs_ne(v1,v2) (mpz_cmpabs((v1),(v2)) != 0)
#define value_abs_gt(v1,v2) (mpz_cmpabs((v1),(v2))  > 0)
#define value_abs_ge(v1,v2) (mpz_cmpabs((v1),(v2)) >= 0)
#define value_abs_lt(v1,v2) (mpz_cmpabs((v1),(v2))  < 0)
#define value_abs_le(v1,v2) (mpz_cmpabs((v1),(v2)) <= 0)
#define value_sign(val)      (mpz_sgn(val))
#define value_compare(v1,v2) (mpz_cmp((v1),(v2)))
#define value_addto(ref,val1,val2)     (mpz_add((ref),(val1),(val2)))
#define value_add_int(ref,val,vint)     (mpz_add_ui((ref),(val),(long)(vint)))
#define value_addmul(ref, val1, val2)   (mpz_addmul((ref), (val1), (val2)))
#define value_increment(ref,val)       (mpz_add_ui((ref),(val),1))
#define value_multiply(ref,val1,val2)  (mpz_mul((ref),(val1),(val2)))
#define value_subtract(ref,val1,val2) (mpz_sub((ref),(val1),(val2)))
#define value_sub_int(ref,val,vint)     (mpz_sub_ui((ref),(val),(long)(vint)))
#define value_decrement(ref,val)       (mpz_sub_ui((ref),(val),1))
#define value_division(ref,val1,val2)  (mpz_tdiv_q((ref),(val1),(val2)))
#define value_modulus(ref,val1,val2)   (mpz_tdiv_r((ref),(val1),(val2)))
#define value_pdivision(ref,val1,val2) (mpz_fdiv_q((ref),(val1),(val2)))
#define value_pmodulus(ref,val1,val2)  (mpz_fdiv_r((ref),(val1),(val2)))
#define value_oppose(ref,val)          (mpz_neg((ref),(val)))
#define value_absolute(ref,val)        (mpz_abs((ref),(val)))
#define value_pos_p(val)         (mpz_sgn(val) >  0)
#define value_neg_p(val)         (mpz_sgn(val) <  0)
#define value_zero_p(val)        (mpz_sgn(val) == 0)
#define value_notzero_p(val)     (mpz_sgn(val) != 0)
#define value_one_p(val)         (mpz_cmp_si(val,1) == 0)
#define value_notone_p(val)      (mpz_cmp_si(val,1) != 0)
#define value_mone_p(val)        (mpz_cmp_si(val,-1) ==0)
#define value_notmone_p(val)     (mpz_cmp_si(val,-1) !=0)
#define value_cmp_si(val, n)     (mpz_cmp_si(val,n))

#define value_substract(ref,val1,val2) (value_subtract((ref),(val1),(val2)))

  static inline void
  cloog_vector_set (Value * ptr, int n, unsigned length)
  {
    unsigned i;

    for (i = 0; i < length; i++, ptr++)
      value_set_si (*ptr, n);
  }

  static inline void cloog_vector_copy (Value * p1, Value * p2, unsigned length)
  {
    unsigned i;

    for (i = 0; i < length; i++)
      value_assign (*p2++, *p1++);
  }

  static inline void Gcd (Value a, Value b, Value * gcd)
  {
    Value a1, b1;

    value_init (a1), value_assign (a1, a);
    value_init (b1), value_assign (b1, b);

    while (value_notzero_p (a1))
      {
	value_modulus (*gcd, b1, a1);
	value_assign (b1, a1);
	value_assign (a1, *gcd);
      }

    value_absolute (*gcd, b1);
    value_clear (a1);
    value_clear (b1);
  }


  typedef struct
  {
    unsigned Size;
    Value *p;
  } Vector;


  static inline void Vector_Free (Vector * vector)
  {
    unsigned i;

    if (!vector)
      return;

    for (i = 0; i < vector->Size; i++)
      value_clear (vector->p[i]);

    free (vector->p);
    free (vector);
  }

  typedef struct polyhedron
  {
    unsigned Dimension;
    unsigned NbConstraints;
    Value **Constraint;
  } *polyhedron;

  static inline unsigned cloog_pol_dim (polyhedron p)
  {
    return p->Dimension;
  }

  static inline unsigned cloog_pol_nbc (polyhedron p)
  {
    return p->NbConstraints;
  }

  typedef struct polyhedra_union
  {
    polyhedron _polyhedron;
    struct polyhedra_union *_next;
  } *polyhedra_union;

  extern polyhedra_union cloog_new_upol (polyhedron);

  static inline polyhedra_union cloog_upol_next (polyhedra_union p)
  {
    return p->_next;
  }

  static inline void
    cloog_upol_set_next (polyhedra_union p, polyhedra_union n)
  {
    p->_next = n;
  }

  static inline polyhedron cloog_upol_polyhedron (polyhedra_union upol)
  {
    return upol->_polyhedron;
  }

  static inline void
    cloog_upol_set_polyhedron (polyhedra_union ppl, polyhedron p)
  {
    ppl->_polyhedron = p;
  }

  static inline unsigned cloog_upol_dim (polyhedra_union p)
  {
    return cloog_pol_dim (cloog_upol_polyhedron (p));
  }

  static inline unsigned cloog_upol_nbc (polyhedra_union p)
  {
    return cloog_pol_nbc (cloog_upol_polyhedron (p));
  }

  static inline void cloog_upol_free (polyhedra_union upol)
  {
    free (upol);
  }

  typedef struct cloogdomain
  {
    struct polyhedra_union *_union;
    int _references;
  } CloogDomain;

  static inline polyhedra_union cloog_domain_upol (CloogDomain * domain)
  {
    return domain->_union;
  }

  static inline polyhedron cloog_domain_polyhedron (CloogDomain * domain)
  {
    return cloog_upol_polyhedron (cloog_domain_upol (domain));
  }

  static inline unsigned cloog_domain_dim (CloogDomain * d)
  {
    return cloog_pol_dim (cloog_domain_polyhedron (d));
  }

  static inline int cloog_domain_nbconstraints (CloogDomain * domain)
  {
    return cloog_pol_nbc (cloog_domain_polyhedron (domain));
  }

  static inline unsigned cloog_pol_nbeq (polyhedron p)
  {
    unsigned i, res = 0;

    for (i = 0; i < p->NbConstraints; i++)
      res += value_zero_p (p->Constraint[i][0]) ? 1 : 0;

    return res;
  }

  static inline unsigned cloog_domain_nbeq (CloogDomain * d)
  {
    return cloog_pol_nbeq (cloog_upol_polyhedron (cloog_domain_upol (d)));
  }

  extern Vector *Vector_Alloc (unsigned);

  typedef struct cloog_matrix
  {
    int NbRows;
    int NbColumns;
    Value **p;
  } CloogMatrix;

  void cloog_matrix_print (FILE *, CloogMatrix *);
  void cloog_matrix_free (CloogMatrix *);
  CloogMatrix *cloog_matrix_alloc (unsigned, unsigned);
  void cloog_matrix_print_structure (FILE *, CloogMatrix *, int);
  CloogMatrix *cloog_matrix_read (FILE *);
  void cloog_matrix_normalize (CloogMatrix *, int);
  void cloog_matrix_equality_update (CloogMatrix *, int, int);
  CloogMatrix *cloog_matrix_copy (CloogMatrix *);
  Value *cloog_matrix_vector_copy (Value *, int);
  Value *cloog_matrix_vector_simplify (Value *, CloogMatrix *, int, int, int);
  CloogMatrix *cloog_matrix_simplify (CloogMatrix *, CloogMatrix *, int, int);
  void cloog_matrix_vector_free (Value *, int);

  static inline CloogMatrix *cloog_pol_matrix (polyhedron p)
  {
    int cols = cloog_pol_dim (p) + 2;
    int rows = cloog_pol_nbc (p);
    int i, j;
    CloogMatrix *res = cloog_matrix_alloc (rows, cols);

    for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
	value_assign (res->p[i][j], p->Constraint[i][j]);

    return res;
  }

  static inline int cloog_matrix_ncolumns (CloogMatrix * m)
  {
    return m->NbColumns;
  }

  static inline int cloog_matrix_nrows (CloogMatrix * m)
  {
    return m->NbRows;
  }

  static inline int cloog_first_non_zero (Value * p, unsigned len)
  {
    Value *ptr = p;
    unsigned i;

    for (i = 0; i < len; i++, ptr++)
      if (value_notzero_p (*ptr))
	break;

    return i == len ? -1 : (int) i;
  }

  polyhedron cloog_new_pol (int, int);

  static inline polyhedron cloog_universe_polyhedron (unsigned dim)
  {
    polyhedron res = cloog_new_pol (dim, 1);

    cloog_vector_set (res->Constraint[0], 0, dim + 2);
    value_set_si (res->Constraint[0][0], 1);
    value_set_si (res->Constraint[0][dim + 1], 1);

    return res;
  }

  static inline polyhedron cloog_empty_polyhedron (int dim)
  {
    int i;
    polyhedron res = cloog_new_pol (dim, dim + 1);

    cloog_vector_set (res->Constraint[0], 0, (dim + 1) * (dim + 2));

    for (i = 0; i <= dim; i++)
      value_set_si (res->Constraint[i][i + 1], 1);

    return res;
  }

  static inline void
  cloog_matrix_exchange_rows (CloogMatrix * m, int row1, int row2)
  {
    int i;

    for (i = 0; i < m->NbColumns; i++)
      value_swap (m->p[row1][i], m->p[row2][i]);
  }

  static inline void cloog_vector_exchange (Value * p1, Value * p2, int len)
  {
    for (; len > 0; len--)
      value_swap (p1[len - 1], p2[len - 1]);
  }

  polyhedron cloog_pol_from_matrix (CloogMatrix * m);

  static inline void cloog_pol_free (polyhedron pol)
  {
    int n, i;

    if (!pol)
      return;

    n = (cloog_pol_dim (pol) + 2) * cloog_pol_nbc (pol);

    for (i = 0; i < n; i++)
      value_clear (pol->Constraint[0][i]);

    free (pol->Constraint[0]);
    free (pol->Constraint);
    free (pol);
  }

  polyhedron cloog_pol_copy (polyhedron pol);
  void cloog_vector_gcd (Value *, int, Value *);

  static inline int
  cloog_pol_lexico_lt (polyhedron p, int i, int j)
  {
    int res = 0;
    unsigned k;
    Value a, b;

    value_init (a), value_init (b);

    for (k = 1; k < cloog_pol_dim (p) + 2; k++)
      {
	value_absolute (a, p->Constraint[i][k]);
	value_absolute (b, p->Constraint[j][k]);

	if (value_lt (a, b))
	  {
	    res = 1;
	    goto clear;
	  }

	if (value_gt (a, b))
	  {
	    res = 0;
	    goto clear;
	  }

	if (value_lt (p->Constraint[i][k], p->Constraint[j][k]))
	  {
	    res = 1;
	    goto clear;
	  }

	if (value_gt (p->Constraint[i][k], p->Constraint[j][k]))
	  {
	    res = 0;
	    goto clear;
	  }
      }

  clear:
    value_clear (a), value_clear (b);
    return res;
  }

  static inline void
  cloog_pol_exchange_rows (polyhedron p, int row1, int row2)
  {
    int i;
    
    for (i = 0; i < (int) cloog_pol_dim (p) + 2; i++)
      value_swap (p->Constraint[row1][i], p->Constraint[row2][i]);
  }

  static inline void
  cloog_pol_sort_rows (polyhedron p)
  {
    unsigned i, j;
    unsigned nbeq = cloog_pol_nbeq (p);

    /* First sort the equalities.  The equalities should be the first
       rows in the matrix.  */
    for (i = 0; i < nbeq; i++)
      for (j = i + 1; j < nbeq; j++)
	if (cloog_pol_lexico_lt (p, i, j))
	  cloog_pol_exchange_rows (p, i, j);

    /* Sort inequalities.  */
    for (i = nbeq; i < cloog_pol_nbc (p); i++)
      for (j = i + 1; j < cloog_pol_nbc (p); j++)
	if (cloog_pol_lexico_lt (p, i, j))
	  cloog_pol_exchange_rows (p, i, j);
  }

  static inline CloogMatrix *
  cloog_upol_domain2matrix (polyhedra_union upol)
  {
    return cloog_pol_matrix (cloog_upol_polyhedron (upol));
  }

  static inline CloogMatrix *
  cloog_domain_domain2matrix (CloogDomain *d)
  {
    return cloog_pol_matrix (cloog_domain_polyhedron (d));
  }

  static inline void
  cloog_vector_normalize (Value * p, unsigned len)
  {
    unsigned i;
    Value *ptr, gcd, one;

    value_init (gcd);
    cloog_vector_gcd (p, len, &gcd);
    value_init (one);
    value_set_si (one, 1);

    if (value_gt (gcd, one))
      for (ptr = p, i = 0; i < len; i++, ptr++)
	value_division (*ptr, *ptr, gcd);

    value_clear (one), value_clear (gcd);
  }

  static inline void cloog_vector_scale (Value * p1, Value * p2,
					 Value x,
					 unsigned length)
  {
    unsigned i;

    for (i = 0; i < length; i++)
      value_multiply (*p2++, *p1++, x);
  }

  static inline void
  cloog_vector_combine (Value * p1, Value * p2, Value * p3, Value x,
			Value y, unsigned length)
  {
    Value tmp;
    unsigned i;

    value_init (tmp);

    for (i = 0; i < length; i++)
      {
	value_multiply (tmp, x, p1[i]);
	value_addmul (tmp, y, p2[i]);
	value_assign (p3[i], tmp);
      }

    value_clear (tmp);
  }

  extern void debug_poly (polyhedron);
  extern void debug_ppl_poly (ppl_Polyhedron_t);
  extern void debug_cloog_matrix (CloogMatrix *);
  extern void debug_cloog_domain (CloogDomain *);
  extern void debug_value (Value);
  extern void debug_values (Value *, int);

#if defined(__cplusplus)
}
#endif
#endif /* define _H */
