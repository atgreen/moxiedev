/* MIP_Problem class implementation: non-inline functions.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <ppl-config.h>
#include "MIP_Problem.defs.hh"
#include "globals.defs.hh"
#include "Checked_Number.defs.hh"
#include "Row.defs.hh"
#include "Linear_Expression.defs.hh"
#include "Constraint.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "Generator.defs.hh"
#include "Scalar_Products.defs.hh"
#include <stdexcept>
#include <deque>
#include <algorithm>
#include <cmath>

#ifndef PPL_NOISY_SIMPLEX
#define PPL_NOISY_SIMPLEX 0
#endif

#ifndef PPL_SIMPLEX_USE_MIP_HEURISTIC
#define PPL_SIMPLEX_USE_MIP_HEURISTIC 1
#endif

#ifdef PPL_NOISY_SIMPLEX
#include <iostream>
#endif


namespace PPL = Parma_Polyhedra_Library;

#if PPL_NOISY_SIMPLEX
namespace {

unsigned long num_iterations = 0;

} // namespace
#endif // PPL_NOISY_SIMPLEX

PPL::MIP_Problem::MIP_Problem(const dimension_type dim)
  : external_space_dim(dim),
    internal_space_dim(0),
    tableau(),
    working_cost(0, Row::Flags()),
    mapping(),
    base(),
    status(PARTIALLY_SATISFIABLE),
    pricing(PRICING_STEEPEST_EDGE_FLOAT),
    initialized(false),
    input_cs(),
    first_pending_constraint(0),
    input_obj_function(),
    opt_mode(MAXIMIZATION),
    last_generator(point()),
    i_variables() {
  // Check for space dimension overflow.
  if (dim > max_space_dimension())
    throw std::length_error("PPL::MIP_Problem::MIP_Problem(dim, cs, obj, "
			    "mode):\n"
			    "dim exceeds the maximum allowed "
			    "space dimension.");
  assert(OK());
}

PPL::MIP_Problem::MIP_Problem(const dimension_type dim,
			      const Constraint_System& cs,
			      const Linear_Expression& obj,
			      const Optimization_Mode mode)
  : external_space_dim(dim),
    internal_space_dim(0),
    tableau(),
    working_cost(0, Row::Flags()),
    mapping(),
    base(),
    status(PARTIALLY_SATISFIABLE),
    pricing(PRICING_STEEPEST_EDGE_FLOAT),
    initialized(false),
    input_cs(),
    first_pending_constraint(0),
    input_obj_function(obj),
    opt_mode(mode),
    last_generator(point()),
    i_variables() {
  // Check for space dimension overflow.
  if (dim > max_space_dimension())
    throw std::length_error("PPL::MIP_Problem::MIP_Problem(dim, cs, obj, "
			    "mode):\n"
			    "dim exceeds the maximum allowed"
			    "space dimension.");
  // Check the objective function.
  if (obj.space_dimension() > dim) {
    std::ostringstream s;
    s << "PPL::MIP_Problem::MIP_Problem(dim, cs, obj,"
      << " mode):\n"
      << "obj.space_dimension() == "<< obj.space_dimension()
      << " exceeds dim == "<< dim << ".";
    throw std::invalid_argument(s.str());
  }
  // Check the constraint system.
  if (cs.space_dimension() > dim) {
    std::ostringstream s;
    s << "PPL::MIP_Problem::MIP_Problem(dim, cs, obj, mode):\n"
      << "cs.space_dimension == " << cs.space_dimension() << " exceeds dim == "
      << dim << ".";
    throw std::invalid_argument(s.str());
  }
  if (cs.has_strict_inequalities())
    throw std::invalid_argument("PPL::MIP_Problem::"
				"MIP_Problem(d, cs, obj, m):\n"
				"cs contains strict inequalities.");
  // Actually copy the constraints.
  input_cs.insert(input_cs.end(), cs.begin(), cs.end());
  assert(OK());
}

void
PPL::MIP_Problem::add_constraint(const Constraint& c) {
  if (space_dimension() < c.space_dimension()) {
    std::ostringstream s;
    s << "PPL::MIP_Problem::add_constraint(c):\n"
      << "c.space_dimension() == "<< c.space_dimension() << " exceeds "
      "this->space_dimension == " << space_dimension() << ".";
    throw std::invalid_argument(s.str());
  }
  if (c.is_strict_inequality())
    throw std::invalid_argument("PPL::MIP_Problem::add_constraint(c):\n"
				"c is a strict inequality.");
  input_cs.push_back(c);
  if (status != UNSATISFIABLE)
    status = PARTIALLY_SATISFIABLE;
  assert(OK());
}

void
PPL::MIP_Problem::add_constraints(const Constraint_System& cs) {
  if (space_dimension() < cs.space_dimension()) {
    std::ostringstream s;
    s << "PPL::MIP_Problem::add_constraints(cs):\n"
      << "cs.space_dimension() == " << cs.space_dimension()
      << " exceeds this->space_dimension() == " << this->space_dimension()
      << ".";
    throw std::invalid_argument(s.str());
  }
  if (cs.has_strict_inequalities())
    throw std::invalid_argument("PPL::MIP_Problem::add_constraints(cs):\n"
				"cs contains strict inequalities.");
  input_cs.insert(input_cs.end(), cs.begin(), cs.end());
  if (status != UNSATISFIABLE)
    status = PARTIALLY_SATISFIABLE;
  assert(OK());
}

void
PPL::MIP_Problem::set_objective_function(const Linear_Expression& obj) {
  if (space_dimension() < obj.space_dimension()) {
    std::ostringstream s;
    s << "PPL::MIP_Problem::set_objective_function(obj):\n"
      << "obj.space_dimension() == " << obj.space_dimension()
      << " exceeds this->space_dimension == " << space_dimension()
      << ".";
    throw std::invalid_argument(s.str());
  }
  input_obj_function = obj;
  if (status == UNBOUNDED || status == OPTIMIZED)
    status = SATISFIABLE;
  assert(OK());
}

const PPL::Generator&
PPL::MIP_Problem::feasible_point() const {
  if (is_satisfiable())
    return last_generator;
  else
    throw std::domain_error("PPL::MIP_Problem::feasible_point():\n"
			    "*this is not satisfiable.");
}

const PPL::Generator&
PPL::MIP_Problem::optimizing_point() const {
  if (solve() == OPTIMIZED_MIP_PROBLEM)
    return last_generator;
  else
    throw std::domain_error("PPL::MIP_Problem::optimizing_point():\n"
 			    "*this doesn't have an optimizing point.");
}

bool
PPL::MIP_Problem::is_satisfiable() const {
  // Check `status' to filter out trivial cases.
  switch (status) {
  case UNSATISFIABLE:
    assert(OK());
    return false;
  case SATISFIABLE:
    // Intentionally fall through
  case UNBOUNDED:
    // Intentionally fall through.
  case OPTIMIZED:
    assert(OK());
    return true;
  case PARTIALLY_SATISFIABLE:
    { // LP case.
      if (i_variables.empty())
	return is_lp_satisfiable();
      // MIP Case.
      const Variables_Set this_variables_set = integer_space_dimensions();
      MIP_Problem& x = const_cast<MIP_Problem&>(*this);
      Generator p = point();
      // This disable the Variable integrality check in OK() until we will
      // find a feasible point.
      x.i_variables.clear();
      x.is_lp_satisfiable();
      if (is_mip_satisfiable(x, p, this_variables_set)) {
	x.last_generator = p;
	x.status = SATISFIABLE;
	// Restore i_variables;
    	x.i_variables = this_variables_set;
	return true;
      }
      else {
	x.status = UNSATISFIABLE;
	// Restore i_variables;
    	x.i_variables = this_variables_set;
	return false;
      }
    }
  }
  // We should not be here!
  throw std::runtime_error("PPL internal error");
}

PPL::MIP_Problem_Status
PPL::MIP_Problem::solve() const{
  switch (status) {
  case UNSATISFIABLE:
    assert(OK());
    return UNFEASIBLE_MIP_PROBLEM;
  case UNBOUNDED:
    assert(OK());
    return UNBOUNDED_MIP_PROBLEM;
  case OPTIMIZED:
    assert(OK());
    return OPTIMIZED_MIP_PROBLEM;
  case SATISFIABLE:
    // Intentionally fall through
  case PARTIALLY_SATISFIABLE:
    {
      MIP_Problem& x = const_cast<MIP_Problem&>(*this);
      if (i_variables.empty()) {
	// LP Problem case.
	if (is_lp_satisfiable()) {
	  x.second_phase();
	  if (x.status == UNBOUNDED)
	    return UNBOUNDED_MIP_PROBLEM;
	  else {
	    assert(x.status == OPTIMIZED);
	    return OPTIMIZED_MIP_PROBLEM;
	  }
	}
	return UNFEASIBLE_MIP_PROBLEM;
      }
      // MIP Problem case.
      // This disable the Variable integrality check in OK() until we will find
      // an optimizing point.
      const Variables_Set this_variables_set = integer_space_dimensions();
      x.i_variables.clear();
      if (x.is_lp_satisfiable())
	x.second_phase();
      else {
	x.status = UNSATISFIABLE;
	// Restore i_variables;
	x.i_variables = this_variables_set;
	return UNFEASIBLE_MIP_PROBLEM;
      }
      PPL_DIRTY_TEMP0(mpq_class, incumbent_solution);
      Generator g = point();
      bool have_incumbent_solution = false;

      MIP_Problem mip_copy(*this);
      // Treat this MIP_Problem as an LP one: we have to deal with
      // the relaxation in solve_mip().
      mip_copy.i_variables.clear();
      MIP_Problem_Status mip_status = solve_mip(have_incumbent_solution,
						incumbent_solution, g,
						mip_copy,
						this_variables_set);
      // Restore i_variables;
      x.i_variables = this_variables_set;
      switch (mip_status) {
      case UNFEASIBLE_MIP_PROBLEM:
	x.status = UNSATISFIABLE;
	break;
      case UNBOUNDED_MIP_PROBLEM:
	x.status = UNBOUNDED;
	// A feasible point has been set in `solve_mip()', so that
	// a call to `feasible_point' will be successful.
	x.last_generator = g;
	break;
      case OPTIMIZED_MIP_PROBLEM:
	x.status = OPTIMIZED;
	// Set the internal generator.
	x.last_generator = g;
	break;
      }
      assert(OK());
      return mip_status;
    }
  }
  // We should not be here!
  throw std::runtime_error("PPL internal error");
}

void
PPL::MIP_Problem::add_space_dimensions_and_embed(const dimension_type m) {
  // The space dimension of the resulting MIP problem should not
  // overflow the maximum allowed space dimension.
  if (m > max_space_dimension() - space_dimension())
    throw std::length_error("PPL::MIP_Problem::"
			    "add_space_dimensions_and_embed(m):\n"
			    "adding m new space dimensions exceeds "
			    "the maximum allowed space dimension.");
  external_space_dim += m;
  if (status != UNSATISFIABLE)
    status = PARTIALLY_SATISFIABLE;
  assert(OK());
}

void
PPL::MIP_Problem
::add_to_integer_space_dimensions(const Variables_Set& i_vars) {
  if (i_vars.space_dimension() > external_space_dim)
    throw std::invalid_argument("PPL::MIP_Problem::"
				"add_to_integer_space_dimension(i_vars):\n"
				"*this and i_vars are dimension"
				"incompatible.");
  const dimension_type original_size = i_variables.size();
  i_variables.insert(i_vars.begin(), i_vars.end());
  // If a new integral variable was inserted, set the internal status to
  // PARTIALLY_SATISFIABLE.
  if (i_variables.size() != original_size && status != UNSATISFIABLE)
    status = PARTIALLY_SATISFIABLE;
}

bool
PPL::MIP_Problem::is_in_base(const dimension_type var_index,
			     dimension_type& row_index) const {
  for (row_index = base.size(); row_index-- > 0; )
    if (base[row_index] == var_index)
      return true;
  return false;
}

void
PPL::MIP_Problem::merge_split_variables(dimension_type var_index,
					std::vector<dimension_type>&
					unfeasible_tableau_rows) {
  const dimension_type tableau_nrows = tableau.num_rows();
  const dimension_type column = mapping[var_index].second;

  for (dimension_type i = 0; i < tableau_nrows; ++i) {
    // In the following case the negative side of the split variable is
    // in base: this means that the constraint will be nonfeasible.
    if (base[i] == mapping[var_index].second) {
      // CHECKME: we do not know if is possible that the positive and
      // the negative part of a split variable can be together in
      // base: it seems that this case is not possible. The algorithm
      // requires that condition.
#ifndef NDEBUG
      for (dimension_type j = 0; j < tableau_nrows; ++j) {
 	dimension_type dummy = 0;
	assert(!is_in_base(mapping[var_index].first, dummy));
      }
#endif
      // We set base[i] to zero to keep track that that the constraint is not
      // feasible by `last_generator'.
      base[i] = 0;
      unfeasible_tableau_rows.push_back(i);
    }
  }

  const dimension_type tableau_cols = tableau.num_columns();
  // Remove the column.
  if (column != tableau_cols - 1) {
    std::vector<dimension_type> cycle;
    for (dimension_type j = tableau_cols - 1; j >= column; --j)
      cycle.push_back(j);
    cycle.push_back(0);
    tableau.permute_columns(cycle);
  }
  tableau.remove_trailing_columns(1);

  // var_index is no longer split.
  mapping[var_index].second = 0;

  // Adjust data structured, `shifting' the proper columns to the left by 1.
  const dimension_type base_size = base.size();
  for (dimension_type i = base_size; i-- > 0; )
    if (base[i] > column)
      --base[i];
  const dimension_type mapping_size = mapping.size();
  for (dimension_type i = mapping_size; i-- > 0; ) {
    if (mapping[i].first > column)
      --mapping[i].first;
    if (mapping[i].second > column)
      --mapping[i].second;
  }
}

bool
PPL::MIP_Problem::is_satisfied(const Constraint& c, const Generator& g) {
  // Scalar_Products::sign() requires the second argument to be at least
  // as large as the first one.
  int sp_sign = g.space_dimension() <= c.space_dimension()
    ? Scalar_Products::sign(g, c)
    : Scalar_Products::sign(c, g);
  return c.is_inequality() ? sp_sign >= 0 : sp_sign == 0;
}

bool
PPL::MIP_Problem::is_saturated(const Constraint& c, const Generator& g) {
  // Scalar_Products::sign() requires the second argument to be at least
  // as large as the first one.
  int sp_sign = g.space_dimension() <= c.space_dimension()
    ? Scalar_Products::sign(g, c)
    : Scalar_Products::sign(c, g);
  return sp_sign == 0;
}

bool
PPL::MIP_Problem::parse_constraints(dimension_type& tableau_num_rows,
				    dimension_type& num_slack_variables,
				    std::deque<bool>& is_tableau_constraint,
				    std::deque<bool>& nonnegative_variable,
				    std::vector<dimension_type>&
				    unfeasible_tableau_rows,
				    std::deque<bool>& satisfied_ineqs) {
  satisfied_ineqs.clear();
  satisfied_ineqs.insert(satisfied_ineqs.end(), input_cs.size(),
			 false);

  const dimension_type cs_num_rows = input_cs.size();
  const dimension_type cs_space_dim = external_space_dim;

  // Step 1:
  // determine variables that are constrained to be nonnegative,
  // detect (non-negativity or tautology) constraints that will not
  // be part of the tableau and count the number of slack variables.

  // Counters determining the dimensions of the tableau:
  // initialized here, they will be updated while examining `cs'.
  tableau_num_rows = cs_num_rows;
  dimension_type tableau_num_cols = 2*cs_space_dim;
  num_slack_variables = 0;

  // On exit, `is_tableau_constraint[i]' will be true if and only if
  // `cs[i]' is neither a tautology (e.g., 1 >= 0) nor a non-negativity
  // constraint (e.g., X >= 0).
  is_tableau_constraint = std::deque<bool> (cs_num_rows, true);

  // On exit, `nonnegative_variable[j]' will be true if and only if
  // Variable(j) is bound to be nonnegative in `cs'.
  nonnegative_variable = std::deque<bool> (cs_space_dim, false);

  // Check for already known information about space dimensions and
  // store them in `nonnegative_variable'.
  const dimension_type mapping_size = mapping.size();
  for (dimension_type i = std::min(mapping_size, cs_space_dim+1); i-- > 1; )
    if (mapping[i].second == 0) {
      nonnegative_variable[i-1] = true;
      --tableau_num_cols;
    }

  // Process each row of the `cs' matrix.
  for (dimension_type i = cs_num_rows; i-- > first_pending_constraint; ) {
    const Constraint& cs_i = input_cs[i];
    bool found_a_nonzero_coeff = false;
    bool found_many_nonzero_coeffs = false;
    dimension_type nonzero_coeff_column_index = 0;
    for (dimension_type sd = cs_i.space_dimension(); sd-- > 0; ) {
      if (cs_i.coefficient(Variable(sd)) != 0) {
	if (found_a_nonzero_coeff) {
	  found_many_nonzero_coeffs = true;
	  if (cs_i.is_inequality())
	    ++num_slack_variables;
	  break;
	}
	else {
	  nonzero_coeff_column_index = sd + 1;
	  found_a_nonzero_coeff = true;
	}
      }
    }
    // If more than one coefficient is nonzero,
    // continue with next constraint.
    if (found_many_nonzero_coeffs) {
      // CHECKME: Is it true that in the first phase we can apply
      // `is_satisfied()' with the generator `point()'?  If so, the following
      // code works even if we do not have a feasible point.
      // Check for satisfiability of the inequality. This can be done if we
      // have a feasible point of *this.
      if (cs_i.is_inequality() && is_satisfied(cs_i, last_generator))
	satisfied_ineqs[i] = true;
      continue;
    }

    if (!found_a_nonzero_coeff) {
      // All coefficients are 0.
      // The constraint is either trivially true or trivially false.
      if (cs_i.is_inequality()) {
	if (cs_i.inhomogeneous_term() < 0)
	  // A constraint such as -1 >= 0 is trivially false.
	  return false;
      }
      else
	// The constraint is an equality.
	if (cs_i.inhomogeneous_term() != 0)
	  // A constraint such as 1 == 0 is trivially false.
	  return false;
      // Here the constraint is trivially true.
      is_tableau_constraint[i] = false;
      --tableau_num_rows;
      continue;
    }
    else {
      // Here we have only one nonzero coefficient.
      /*

      We have the following methods:
      A) Do split the variable and do add the constraint
         in the tableau.
      B) Do not split the variable and do add the constraint
         in the tableau.
      C) Do not split the variable and do not add the constraint
         in the tableau.

      Let the constraint be (a*v + b relsym 0).
      These are the 12 possible combinations we can have:
                a |  b | relsym | method
      ----------------------------------
      1)       >0 | >0 |   >=   |   A
      2)       >0 | >0 |   ==   |   A
      3)       <0 | <0 |   >=   |   A
      4)       >0 | =0 |   ==   |   B
      5)       >0 | <0 |   ==   |   B
      Note:    <0 | >0 |   ==   | impossible by strong normalization
      Note:    <0 | =0 |   ==   | impossible by strong normalization
      Note:    <0 | <0 |   ==   | impossible by strong normalization
      6)       >0 | <0 |   >=   |   B
      7)       >0 | =0 |   >=   |   C
      8)       <0 | >0 |   >=   |   A
      9)       <0 | =0 |   >=   |   A

      The next lines will apply the correct method to each case.
      */

      // The variable index is not equal to the column index.
      const dimension_type nonzero_var_index = nonzero_coeff_column_index - 1;

      const int sgn_a
	= sgn(cs_i.coefficient(Variable(nonzero_coeff_column_index-1)));
      const int sgn_b = sgn(cs_i.inhomogeneous_term());
      // Cases 1-3: apply method A.
      if (sgn_a == sgn_b) {
	if (cs_i.is_inequality())
	  ++num_slack_variables;
      }
      // Cases 4-5: apply method B.
      else if (cs_i.is_equality()) {
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	}
      }
      // Case 6: apply method B.
      else if (sgn_b < 0) {
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	}
	++num_slack_variables;
      }
      // Case 7: apply method C.
      else if (sgn_a > 0) {
	// This is the most important case in the incrementality solving:
	// merge two variables.
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	  if (nonzero_coeff_column_index < mapping_size)
	    merge_split_variables(nonzero_coeff_column_index,
				  unfeasible_tableau_rows);
	  is_tableau_constraint[i] = false;
	}
	else
	  is_tableau_constraint[i] = false;
	--tableau_num_rows;
      }
      // Cases 8-9: apply method A.
      else
	++num_slack_variables;
    }
  }
  return true;
}

bool
PPL::MIP_Problem::process_pending_constraints() {
  const dimension_type num_original_rows = tableau.num_rows();
  dimension_type new_rows = 0;
  dimension_type new_slacks = 0;
  dimension_type new_var_columns = 0;
  std::deque<bool> is_tableau_constraint;
  std::deque<bool> nonnegative_variable;
  std::vector<dimension_type> unfeasible_tableau_rows;
  std::deque<bool> satisfied_ineqs;
  // Check the new constraints to adjust the data structures.
  // If `false' is returned, the pending constraints are trivially
  // unfeasible.
  if (!parse_constraints(new_rows, new_slacks, is_tableau_constraint,
			 nonnegative_variable, unfeasible_tableau_rows,
			 satisfied_ineqs)) {
    status = UNSATISFIABLE;
    return false;
  };

  const dimension_type first_free_tableau_index = tableau.num_columns()-1;

  if (external_space_dim > internal_space_dim) {
    const dimension_type space_diff = external_space_dim - internal_space_dim;
    for (dimension_type i = 0, j = 0; i < space_diff; ++i, ++j) {
      // Set `mapping' properly to store that every variable is split.
      // In the following case the value of the original variable can be
      // negative.
      if (!nonnegative_variable[internal_space_dim+i]) {
	mapping.push_back(std::make_pair(first_free_tableau_index+j,
					 first_free_tableau_index+1+j));
	++j;
	new_var_columns += 2;
      }
      // The variable is nonnegative.
      else {
	mapping.push_back(std::make_pair(first_free_tableau_index+j, 0));
	++new_var_columns;
      }
    }
  }

  // Resize the tableau and adding the necessary columns for artificial and
  // slack variables.
  dimension_type num_satisfied_ineqs = std::count(satisfied_ineqs.begin(),
						  satisfied_ineqs.end(),
						  true);
  const dimension_type unfeasible_tableau_rows_size
    = unfeasible_tableau_rows.size();
  const dimension_type artificial_cols
    = new_rows + unfeasible_tableau_rows_size - num_satisfied_ineqs;
  const dimension_type new_total_columns
    = new_var_columns + new_slacks + artificial_cols;
  if (new_rows > 0)
    tableau.add_zero_rows(new_rows, Row::Flags());
  if (new_total_columns > 0)
    tableau.add_zero_columns(new_total_columns);
  dimension_type tableau_num_rows = tableau.num_rows();

  // The following vector will be useful know if a constraint is feasible
  // and does not require an additional artificial variable.
  std::deque<bool> worked_out_row (tableau_num_rows, false);
  dimension_type tableau_num_columns = tableau.num_columns();

  // Sync the `base' vector size to the new tableau: fill with zeros to encode
  // that these rows are not OK and must be adjusted.
  base.insert(base.end(), new_rows, 0);
  const dimension_type base_size = base.size();

  // These indexes will be used to insert slack and artificial variables.
  dimension_type slack_index = tableau_num_columns - artificial_cols - 1;
  dimension_type artificial_index = slack_index;

  // The first column index of the tableau that contains an
  // artificial variable. Encode with 0 the fact the there are not
  // artificial variables.
  const dimension_type begin_artificials = artificial_cols > 0
    ? artificial_index : 0;

  // Proceed with the insertion of the constraints.
  for (dimension_type k = tableau_num_rows, i = input_cs.size();
       i-- > first_pending_constraint;  )
    if (is_tableau_constraint[i]) {
      // Copy the original constraint in the tableau.
      Row& tableau_k = tableau[--k];
      const Constraint& cs_i = input_cs[i];
      for (dimension_type sd = cs_i.space_dimension(); sd-- > 0; ) {
	tableau_k[mapping[sd+1].first] = cs_i.coefficient(Variable(sd));
	// Split if needed.
	if (mapping[sd+1].second != 0)
	  neg_assign(tableau_k[mapping[sd+1].second],
		     tableau_k[mapping[sd+1].first]);
      }
      tableau_k[mapping[0].first] = cs_i.inhomogeneous_term();
      // Split if needed.
      if (mapping[0].second != 0)
	tableau_k[mapping[0].second] = -cs_i.inhomogeneous_term();

      // Add the slack variable, if needed.
      if (cs_i.is_inequality()) {
	tableau_k[--slack_index] = -1;
	// If the constraint is already satisfied, we will not use artificial
	// variables to compute a feasible base: this to speed up
	// the algorithm.
        if (satisfied_ineqs[i]) {
          base[k] = slack_index;
          worked_out_row[k] = true;
        }
      }
      for (dimension_type j = base_size; j-- > 0; )
	if (k != j && tableau_k[base[j]] != 0 && base[j] != 0)
	  linear_combine(tableau_k, tableau[j], base[j]);
    }

  // We negate the row if tableau[i][0] <= 0 to get the inhomogeneous term > 0.
  // This simplifies the insertion of the artificial variables: the value of
  // each artificial variable will be 1.
  for (dimension_type i = tableau_num_rows; i-- > 0 ; ) {
    Row& tableau_i = tableau[i];
    if (tableau_i[0] > 0)
      for (dimension_type j = tableau_num_columns; j-- > 0; )
	neg_assign(tableau_i[j]);
  }

  // Set the working cost function with the right size.
  working_cost = Row(tableau_num_columns, Row::Flags());

  // Insert artificial variables for the nonfeasible constraints.
  for (dimension_type i = 0; i < unfeasible_tableau_rows_size; ++i) {
    tableau[unfeasible_tableau_rows[i]][artificial_index] = 1;
    working_cost[artificial_index] = -1;
    base[unfeasible_tableau_rows[i]] = artificial_index;
    ++artificial_index;
  }

  // Modify the tableau and the new cost function by adding
  // the artificial variables (which enter the base). Note that if an
  // inequality was satisfied by `last_generator', this will be not processed.
  // This information in encoded in `worked_out_row'.
  // As for the cost function, all the artificial variables should have
  // coefficient -1.
  for (dimension_type i = num_original_rows; i < tableau_num_rows; ++i) {
    if (worked_out_row[i])
      continue;
    tableau[i][artificial_index] = 1;
    working_cost[artificial_index] = -1;
    base[i] = artificial_index;
    ++artificial_index;
  }
  // The last column index of the tableau containing an artificial variable.
  const dimension_type end_artificials = artificial_index - 1;

  // Set the extra-coefficient of the cost functions to record its sign.
  // This is done to keep track of the possible sign's inversion.
  const dimension_type last_obj_index = working_cost.size() - 1;
  working_cost[last_obj_index] = 1;

  // Express the problem in terms of the variables in base.
  for (dimension_type i = tableau_num_rows; i-- > 0; )
    if (working_cost[base[i]] != 0)
      linear_combine(working_cost, tableau[i], base[i]);

  // Deal with zero dimensional problems.
  if (space_dimension() == 0) {
    status = OPTIMIZED;
    last_generator = point();
    return true;
  }
  // Deal with trivial cases.
  // If there is no constraint in the tableau, then the feasible region
  // is only delimited by non-negativity constraints. Therefore,
  // the problem is unbounded as soon as the cost function has
  // a variable with a positive coefficient.
  if (tableau_num_rows == 0) {
    const dimension_type input_obj_function_size
      = input_obj_function.space_dimension();
    for (dimension_type i = input_obj_function_size; i-- > 0; )
      // If a the value of a variable in the objective function is
      // different from zero, the final status is unbounded.
      // In the first part the variable is constrained to be greater or equal
      // than zero.
      if ((((input_obj_function.coefficient(Variable(i)) > 0
	     && opt_mode == MAXIMIZATION)
	    || (input_obj_function.coefficient(Variable(i)) < 0
		&& opt_mode == MINIMIZATION)) && mapping[i].second == 0)
	  // In the following case the variable is unconstrained.
	  || (input_obj_function.coefficient(Variable(i)) != 0
	      && mapping[i].second != 0)) {
	// Ensure the right space dimension is obtained.
	last_generator = point(0 * Variable(space_dimension()-1));
	status = UNBOUNDED;
	return true;
      }

    // The problem is neither trivially unfeasible nor trivially unbounded.
    // The tableau was successful computed and the caller has to figure
    // out which case applies.
    status = OPTIMIZED;
    // Ensure the right space dimension is obtained.
    last_generator = point(0*Variable(space_dimension()-1));
    assert(OK());
    return true;
  }

  // Now we are ready to solve the first phase.
  bool first_phase_succesful
    = (get_control_parameter(PRICING) == PRICING_STEEPEST_EDGE_FLOAT)
    ? compute_simplex_using_steepest_edge_float()
    : compute_simplex_using_exact_pricing();

#if PPL_NOISY_SIMPLEX
  std::cout << "MIP_Problem::solve: 1st phase ended at iteration "
 	    << num_iterations << "." << std::endl;
#endif

  if (!first_phase_succesful || working_cost[0] != 0) {
    // The feasible region is empty.
    status = UNSATISFIABLE;
    return false;
  }

  // Prepare *this for a possible second phase.
  if (begin_artificials != 0)
    erase_artificials(begin_artificials, end_artificials);
  compute_generator();
  status = SATISFIABLE;
  assert(OK());
  return true;
}

namespace {

inline void
assign(double& d, const mpz_class& c) {
  d = c.get_d();
}

template <typename T, typename Policy>
inline void
assign(double& d,
       const Parma_Polyhedra_Library::Checked_Number<T, Policy>& c) {
  d = raw_value(c);
}

} // namespace

PPL::dimension_type
PPL::MIP_Problem::steepest_edge_float_entering_index() const {
  PPL_DIRTY_TEMP0(mpq_class, real_coeff);
  const dimension_type tableau_num_rows = tableau.num_rows();
  assert(tableau_num_rows == base.size());
  double challenger_num = 0.0;
  double challenger_den = 0.0;
  double current_value = 0.0;
  dimension_type entering_index = 0;
  const int cost_sign = sgn(working_cost[working_cost.size() - 1]);
  for (dimension_type j = tableau.num_columns() - 1; j-- > 1; ) {
    const Coefficient& cost_j = working_cost[j];
    if (sgn(cost_j) == cost_sign) {
      // We cannot compute the (exact) square root of abs(\Delta x_j).
      // The workaround is to compute the square of `cost[j]'.
      assign(challenger_num, cost_j);
      challenger_num = fabs(challenger_num);
      // Due to our integer implementation, the `1' term in the denominator
      // of the original formula has to be replaced by `squared_lcm_basis'.
      challenger_den = 1.0;
      for (dimension_type i = tableau_num_rows; i-- > 0; ) {
	const Row& tableau_i = tableau[i];
	const Coefficient& tableau_ij = tableau_i[j];
	if (tableau_ij != 0) {
	  assert(tableau_i[base[i]] != 0);
	  assign_r(real_coeff.get_num(), tableau_ij, ROUND_NOT_NEEDED);
	  assign_r(real_coeff.get_den(), tableau_i[base[i]], ROUND_NOT_NEEDED);
	  real_coeff.canonicalize();
	  double float_tableau_value;
	  assign(float_tableau_value, real_coeff);
	  challenger_den += float_tableau_value * float_tableau_value;
	}
      }
      double challenger_value = sqrt(challenger_den);
      // Initialize `current_value' during the first iteration.
      // Otherwise update if the challenger wins.
      if (entering_index == 0 || challenger_value > current_value) {
	current_value = challenger_value;
	entering_index = j;
      }
    }
  }
  return entering_index;
}

PPL::dimension_type
PPL::MIP_Problem::steepest_edge_exact_entering_index() const {
  const dimension_type tableau_num_rows = tableau.num_rows();
  assert(tableau_num_rows == base.size());
  // The square of the lcm of all the coefficients of variables in base.
  PPL_DIRTY_TEMP_COEFFICIENT(squared_lcm_basis);
  // The normalization factor for each coefficient in the tableau.
  std::vector<Coefficient> norm_factor(tableau_num_rows);
  {
    // Compute the lcm of all the coefficients of variables in base.
    PPL_DIRTY_TEMP_COEFFICIENT(lcm_basis);
    lcm_basis = 1;
    for (dimension_type i = tableau_num_rows; i-- > 0; )
      lcm_assign(lcm_basis, lcm_basis, tableau[i][base[i]]);
    // Compute normalization factors.
    for (dimension_type i = tableau_num_rows; i-- > 0; )
      exact_div_assign(norm_factor[i], lcm_basis, tableau[i][base[i]]);
    // Compute the square of `lcm_basis', exploiting the fact that
    // `lcm_basis' will no longer be needed.
    lcm_basis *= lcm_basis;
    std::swap(squared_lcm_basis, lcm_basis);
  }

  // Defined here to avoid repeated (de-)allocations.
  PPL_DIRTY_TEMP_COEFFICIENT(challenger_num);
  PPL_DIRTY_TEMP_COEFFICIENT(scalar_value);
  PPL_DIRTY_TEMP_COEFFICIENT(challenger_den);
  PPL_DIRTY_TEMP_COEFFICIENT(challenger_value);
  PPL_DIRTY_TEMP_COEFFICIENT(current_value);

  PPL_DIRTY_TEMP_COEFFICIENT(current_num);
  PPL_DIRTY_TEMP_COEFFICIENT(current_den);
  dimension_type entering_index = 0;
  const int cost_sign = sgn(working_cost[working_cost.size() - 1]);
  for (dimension_type j = tableau.num_columns() - 1; j-- > 1; ) {
    const Coefficient& cost_j = working_cost[j];
    if (sgn(cost_j) == cost_sign) {
      // We cannot compute the (exact) square root of abs(\Delta x_j).
      // The workaround is to compute the square of `cost[j]'.
      challenger_num = cost_j * cost_j;
      // Due to our integer implementation, the `1' term in the denominator
      // of the original formula has to be replaced by `squared_lcm_basis'.
      challenger_den = squared_lcm_basis;
      for (dimension_type i = tableau_num_rows; i-- > 0; ) {
	const Coefficient& tableau_ij = tableau[i][j];
	// The test against 0 gives rise to a consistent speed up: see
        // http://www.cs.unipr.it/pipermail/ppl-devel/2009-February/014000.html
	if (tableau_ij != 0) {
	  scalar_value = tableau_ij * norm_factor[i];
	  add_mul_assign(challenger_den, scalar_value, scalar_value);
        }
      }
      // Initialization during the first loop.
      if (entering_index == 0) {
	std::swap(current_num, challenger_num);
	std::swap(current_den, challenger_den);
	entering_index = j;
 	continue;
      }
      challenger_value = challenger_num * current_den;
      current_value = current_num * challenger_den;
      // Update the values, if the challenger wins.
      if (challenger_value > current_value) {
	std::swap(current_num, challenger_num);
	std::swap(current_den, challenger_den);
	entering_index = j;
      }
    }
  }
  return entering_index;
}


// See page 47 of [PapadimitriouS98].
PPL::dimension_type
PPL::MIP_Problem::textbook_entering_index() const {
  // The variable entering the base is the first one whose coefficient
  // in the cost function has the same sign the cost function itself.
  // If no such variable exists, then we met the optimality condition
  // (and return 0 to the caller).

  // Get the "sign" of the cost function.
  const dimension_type cost_sign_index = working_cost.size() - 1;
  const int cost_sign = sgn(working_cost[cost_sign_index]);
  assert(cost_sign != 0);
  for (dimension_type i = 1; i < cost_sign_index; ++i)
    if (sgn(working_cost[i]) == cost_sign)
      return i;
  // No variable has to enter the base:
  // the cost function was optimized.
  return 0;
}

void
PPL::MIP_Problem::linear_combine(Row& x,
				 const Row& y,
				 const dimension_type k) {
  assert(x.size() == y.size());
  assert(y[k] != 0 && x[k] != 0);
  // Let g be the GCD between `x[k]' and `y[k]'.
  // For each i the following computes
  //   x[i] = x[i]*y[k]/g - y[i]*x[k]/g.
  PPL_DIRTY_TEMP_COEFFICIENT(normalized_x_k);
  PPL_DIRTY_TEMP_COEFFICIENT(normalized_y_k);
  normalize2(x[k], y[k], normalized_x_k, normalized_y_k);
  for (dimension_type i = x.size(); i-- > 0; )
    if (i != k) {
      Coefficient& x_i = x[i];
      x_i *= normalized_y_k;
      // The test against 0 gives rise to a consistent speed up: see
      // http://www.cs.unipr.it/pipermail/ppl-devel/2009-February/014000.html
      const Coefficient& y_i = y[i];
      if (y_i != 0)
	sub_mul_assign(x_i, y_i, normalized_x_k);
    }
  x[k] = 0;
  x.normalize();
}

// See pages 42-43 of [PapadimitriouS98].
void
PPL::MIP_Problem::pivot(const dimension_type entering_var_index,
			const dimension_type exiting_base_index) {
  const Row& tableau_out = tableau[exiting_base_index];
  // Linearly combine the constraints.
  for (dimension_type i = tableau.num_rows(); i-- > 0; ) {
    Row& tableau_i = tableau[i];
    if (i != exiting_base_index && tableau_i[entering_var_index] != 0)
      linear_combine(tableau_i, tableau_out, entering_var_index);
  }
  // Linearly combine the cost function.
  if (working_cost[entering_var_index] != 0)
    linear_combine(working_cost, tableau_out, entering_var_index);
  // Adjust the base.
  base[exiting_base_index] = entering_var_index;
}

// See pages 47 and 50 of [PapadimitriouS98].
PPL::dimension_type
PPL::MIP_Problem
::get_exiting_base_index(const dimension_type entering_var_index) const  {
  // The variable exiting the base should be associated to a tableau
  // constraint such that the ratio
  // tableau[i][entering_var_index] / tableau[i][base[i]]
  // is strictly positive and minimal.

  // Find the first tableau constraint `c' having a positive value for
  // tableau[i][entering_var_index] / tableau[i][base[i]]
  const dimension_type tableau_num_rows = tableau.num_rows();
  dimension_type exiting_base_index = tableau_num_rows;
  for (dimension_type i = 0; i < tableau_num_rows; ++i) {
    const Row& t_i = tableau[i];
    const int num_sign = sgn(t_i[entering_var_index]);
    if (num_sign != 0 && num_sign == sgn(t_i[base[i]])) {
      exiting_base_index = i;
      break;
    }
  }
  // Check for unboundedness.
  if (exiting_base_index == tableau_num_rows)
    return tableau_num_rows;

  // Reaching this point means that a variable will definitely exit the base.
  PPL_DIRTY_TEMP_COEFFICIENT(lcm);
  PPL_DIRTY_TEMP_COEFFICIENT(current_min);
  PPL_DIRTY_TEMP_COEFFICIENT(challenger);
  for (dimension_type i = exiting_base_index + 1; i < tableau_num_rows; ++i) {
    const Row& t_i = tableau[i];
    const Coefficient& t_ie = t_i[entering_var_index];
    const Coefficient& t_ib = t_i[base[i]];
    const int t_ie_sign = sgn(t_ie);
    if (t_ie_sign != 0 && t_ie_sign == sgn(t_ib)) {
      const Row& t_e = tableau[exiting_base_index];
      const Coefficient& t_ee = t_e[entering_var_index];
      lcm_assign(lcm, t_ee, t_ie);
      exact_div_assign(current_min, lcm, t_ee);
      current_min *= t_e[0];
      abs_assign(current_min);
      exact_div_assign(challenger, lcm, t_ie);
      challenger *= t_i[0];
      abs_assign(challenger);
      current_min -= challenger;
      const int sign = sgn(current_min);
      if (sign > 0
	  || (sign == 0 && base[i] < base[exiting_base_index]))
	exiting_base_index = i;
    }
  }
  return exiting_base_index;
}

// See page 49 of [PapadimitriouS98].
bool
PPL::MIP_Problem::compute_simplex_using_steepest_edge_float() {
  // We may need to temporarily switch to the textbook pricing.
  const unsigned long allowed_non_increasing_loops = 200;
  unsigned long non_increased_times = 0;
  bool textbook_pricing = false;

  PPL_DIRTY_TEMP_COEFFICIENT(cost_sgn_coeff);
  PPL_DIRTY_TEMP_COEFFICIENT(current_num);
  PPL_DIRTY_TEMP_COEFFICIENT(current_den);
  PPL_DIRTY_TEMP_COEFFICIENT(challenger);
  PPL_DIRTY_TEMP_COEFFICIENT(current);

  cost_sgn_coeff = working_cost[working_cost.size()-1];
  current_num = working_cost[0];
  if (cost_sgn_coeff < 0)
    neg_assign(current_num);
  abs_assign(current_den, cost_sgn_coeff);
  assert(tableau.num_columns() == working_cost.size());
  const dimension_type tableau_num_rows = tableau.num_rows();

  while (true) {
    // Choose the index of the variable entering the base, if any.
    const dimension_type entering_var_index
      = textbook_pricing
      ? textbook_entering_index()
      : steepest_edge_float_entering_index();

    // If no entering index was computed, the problem is solved.
    if (entering_var_index == 0)
      return true;

    // Choose the index of the row exiting the base.
    const dimension_type exiting_base_index
      = get_exiting_base_index(entering_var_index);
    // If no exiting index was computed, the problem is unbounded.
    if (exiting_base_index == tableau_num_rows)
      return false;

    // Check if the client has requested abandoning all expensive
    // computations. If so, the exception specified by the client
    // is thrown now.
    maybe_abandon();

    // We have not reached the optimality or unbounded condition:
    // compute the new base and the corresponding vertex of the
    // feasible region.
    pivot(entering_var_index, exiting_base_index);

    // Now begins the objective function's value check to choose between
    // the `textbook' and the float `steepest-edge' technique.
    cost_sgn_coeff = working_cost[working_cost.size()-1];

    challenger = working_cost[0];
    if (cost_sgn_coeff < 0)
      neg_assign(challenger);
    challenger *= current_den;
    abs_assign(current, cost_sgn_coeff);
    current *= current_num;
#if PPL_NOISY_SIMPLEX
    ++num_iterations;
    if (num_iterations % 200 == 0)
      std::cout << "Primal Simplex: iteration "
		<< num_iterations << "." << std::endl;
#endif
    // If the following condition fails, probably there's a bug.
    assert(challenger >= current);
    // If the value of the objective function does not improve,
    // keep track of that.
    if (challenger == current) {
      ++non_increased_times;
      // In the following case we will proceed using the `textbook'
      // technique, until the objective function is not improved.
      if (non_increased_times > allowed_non_increasing_loops)
	textbook_pricing = true;
    }
    // The objective function has an improvement:
    // reset `non_increased_times' and `textbook_pricing'.
    else {
      non_increased_times = 0;
      textbook_pricing = false;
    }
    current_num = working_cost[0];
    if (cost_sgn_coeff < 0)
      neg_assign(current_num);
    abs_assign(current_den, cost_sgn_coeff);
  }
}

bool
PPL::MIP_Problem::compute_simplex_using_exact_pricing() {
  assert(tableau.num_columns() == working_cost.size());
  assert(get_control_parameter(PRICING) == PRICING_STEEPEST_EDGE_EXACT
         || get_control_parameter(PRICING) == PRICING_TEXTBOOK);

  const dimension_type tableau_num_rows = tableau.num_rows();
  const bool textbook_pricing
    = (PRICING_TEXTBOOK == get_control_parameter(PRICING));

  while (true) {
    // Choose the index of the variable entering the base, if any.
    const dimension_type entering_var_index
      = textbook_pricing
      ? textbook_entering_index()
      : steepest_edge_exact_entering_index();
    // If no entering index was computed, the problem is solved.
    if (entering_var_index == 0)
      return true;

    // Choose the index of the row exiting the base.
    const dimension_type exiting_base_index
      = get_exiting_base_index(entering_var_index);
    // If no exiting index was computed, the problem is unbounded.
    if (exiting_base_index == tableau_num_rows)
      return false;

    // Check if the client has requested abandoning all expensive
    // computations. If so, the exception specified by the client
    // is thrown now.
    maybe_abandon();

    // We have not reached the optimality or unbounded condition:
    // compute the new base and the corresponding vertex of the
    // feasible region.
    pivot(entering_var_index, exiting_base_index);
#if PPL_NOISY_SIMPLEX
    ++num_iterations;
    if (num_iterations % 200 == 0)
      std::cout << "Primal Simplex: iteration "
                << num_iterations << "." << std::endl;
#endif
  }
}


// See pages 55-56 of [PapadimitriouS98].
void
PPL::MIP_Problem::erase_artificials(const dimension_type begin_artificials,
				    const dimension_type end_artificials) {
  const dimension_type tableau_last_index = tableau.num_columns() - 1;
  dimension_type tableau_n_rows = tableau.num_rows();
  // Step 1: try to remove from the base all the remaining slack variables.
  for (dimension_type i = 0; i < tableau_n_rows; ++i)
    if (begin_artificials <= base[i] && base[i] <= end_artificials) {
      // Search for a non-zero element to enter the base.
      Row& tableau_i = tableau[i];
      bool redundant = true;
      for (dimension_type j = end_artificials+1; j-- > 1; )
	if (!(begin_artificials <= j && j <= end_artificials)
	    && tableau_i[j] != 0) {
	  pivot(j, i);
	  redundant = false;
	  break;
	}
      if (redundant) {
	// No original variable entered the base:
	// the constraint is redundant and should be deleted.
	--tableau_n_rows;
	if (i < tableau_n_rows) {
	  // Replace the redundant row with the last one,
	  // taking care of adjusting the iteration index.
	  tableau_i.swap(tableau[tableau_n_rows]);
	  base[i] = base[tableau_n_rows];
	  --i;
	}
	tableau.erase_to_end(tableau_n_rows);
	base.pop_back();
      }
    }


  // Step 2: Adjust data structures so as to enter phase 2 of the simplex.

  // Compute the dimensions of the new tableau.
  dimension_type num_artificials = 0;
  for (dimension_type i = end_artificials + 1; i-- > 1; )
    if (begin_artificials <= i && i <= end_artificials) {
      ++num_artificials;
      tableau.remove_trailing_columns(1);
    }

  // Zero the last column of the tableau.
  for (dimension_type i = tableau_n_rows; i-- > 0; )
    tableau[i][tableau.num_columns()-1] = 0;

  // ... then properly set the element in the (new) last column,
  // encoding the kind of optimization; ...
  working_cost[tableau.num_columns()-1] = working_cost[tableau_last_index];
  // ... and finally remove redundant columns.
  const dimension_type working_cost_new_size = working_cost.size() -
    num_artificials;
  working_cost.shrink(working_cost_new_size);
}

// See page 55 of [PapadimitriouS98].
void
PPL::MIP_Problem::compute_generator() const {
  // We will store in num[] and in den[] the numerators and
  // the denominators of every variable of the original problem.
  std::vector<Coefficient> num(external_space_dim);
  std::vector<Coefficient> den(external_space_dim);
  dimension_type row = 0;

  PPL_DIRTY_TEMP_COEFFICIENT(lcm);
  // Speculatively allocate temporaries out of loop.
  PPL_DIRTY_TEMP_COEFFICIENT(split_num);
  PPL_DIRTY_TEMP_COEFFICIENT(split_den);

  // We start to compute num[] and den[].
  for (dimension_type i = external_space_dim; i-- > 0; ) {
    Coefficient& num_i = num[i];
    Coefficient& den_i = den[i];
    // Get the value of the variable from the tableau
    // (if it is not a basic variable, the value is 0).
    const dimension_type original_var = mapping[i+1].first;
    if (is_in_base(original_var, row)) {
      const Row& t_row = tableau[row];
      if (t_row[original_var] > 0) {
	neg_assign(num_i, t_row[0]);
	den_i = t_row[original_var];
      }
      else {
	num_i = t_row[0];
	neg_assign(den_i, t_row[original_var]);
      }
    }
    else {
      num_i = 0;
      den_i = 1;
    }
    // Check whether the variable was split.
    const dimension_type split_var = mapping[i+1].second;
    if (split_var != 0) {
      // The variable was split: get the value for the negative component,
      // having index mapping[i+1].second .
      // Like before, we he have to check if the variable is in base.
      if (is_in_base(split_var, row)) {
	const Row& t_row = tableau[row];
	if (t_row[split_var] > 0) {
	  split_num = -t_row[0];
	  split_den = t_row[split_var];
	}
	else {
	  split_num = t_row[0];
 	  split_den = -t_row[split_var];
	}
	// We compute the lcm to compute subsequently the difference
	// between the 2 variables.
	lcm_assign(lcm, den_i, split_den);
	exact_div_assign(den_i, lcm, den_i);
	exact_div_assign(split_den, lcm, split_den);
	num_i *= den_i;
	sub_mul_assign(num_i, split_num, split_den);
	if (num_i == 0)
	  den_i = 1;
	else
	  den_i = lcm;
      }
      // Note: if the negative component was not in base, then
      // it has value zero and there is nothing left to do.
    }
  }

  // Compute the lcm of all denominators.
  lcm = den[0];
  for (dimension_type i = 1; i < external_space_dim; ++i)
    lcm_assign(lcm, lcm, den[i]);
  // Use the denominators to store the numerators' multipliers
  // and then compute the normalized numerators.
  for (dimension_type i = external_space_dim; i-- > 0; ) {
    exact_div_assign(den[i], lcm, den[i]);
    num[i] *= den[i];
  }

  // Finally, build the generator.
  Linear_Expression expr;
  for (dimension_type i = external_space_dim; i-- > 0; )
    expr += num[i] * Variable(i);

  MIP_Problem& x = const_cast<MIP_Problem&>(*this);
  x.last_generator = point(expr, lcm);
}

void
PPL::MIP_Problem::second_phase() {
  // Second_phase requires that *this is satisfiable.
  assert(status == SATISFIABLE || status == UNBOUNDED || status == OPTIMIZED);
  // In the following cases the problem is already solved.
  if (status == UNBOUNDED || status == OPTIMIZED)
    return;

  // Build the objective function for the second phase.
  const dimension_type input_obj_function_sd
    = input_obj_function.space_dimension();
  Row new_cost(input_obj_function_sd + 1, Row::Flags());
  for (dimension_type i = input_obj_function_sd; i-- > 0; )
    new_cost[i+1] = input_obj_function.coefficient(Variable(i));
  new_cost[0] = input_obj_function.inhomogeneous_term();

  // Negate the cost function if we are minimizing.
  if (opt_mode == MINIMIZATION)
    for (dimension_type i = new_cost.size(); i-- > 0; )
      neg_assign(new_cost[i]);

  // Substitute properly the cost function in the `costs' matrix.
  const dimension_type cost_zero_size = working_cost.size();
  Row tmp_cost = Row(new_cost, cost_zero_size, cost_zero_size);
  tmp_cost.swap(working_cost);
  working_cost[cost_zero_size-1] = 1;

  // Split the variables the cost function.
  for (dimension_type i = new_cost.size(); i-- > 1; ) {
    const dimension_type original_var = mapping[i].first;
    const dimension_type split_var = mapping[i].second;
    working_cost[original_var] = new_cost[i];
    if (mapping[i].second != 0)
      working_cost[split_var] = - new_cost[i];
  }
  // Here the first phase problem succeeded with optimum value zero.
  // Express the old cost function in terms of the computed base.
  for (dimension_type i = tableau.num_rows(); i-- > 0; ) {
    const dimension_type base_i = base[i];
    if (working_cost[base_i] != 0)
      linear_combine(working_cost, tableau[i], base_i);
  }
  // Solve the second phase problem.
  bool second_phase_successful
    = (get_control_parameter(PRICING) == PRICING_STEEPEST_EDGE_FLOAT)
    ? compute_simplex_using_steepest_edge_float()
    : compute_simplex_using_exact_pricing();
  compute_generator();
#if PPL_NOISY_SIMPLEX
  std::cout << "MIP_Problem::solve: 2nd phase ended at iteration "
	    << num_iterations << "." << std::endl;
#endif
  status = second_phase_successful ? OPTIMIZED : UNBOUNDED;
  assert(OK());
}

void
PPL::MIP_Problem
::evaluate_objective_function(const Generator& evaluating_point,
			      Coefficient& ext_n,
			      Coefficient& ext_d) const {
  const dimension_type ep_space_dim = evaluating_point.space_dimension();
  if (space_dimension() < ep_space_dim)
    throw std::invalid_argument("PPL::MIP_Problem::"
				"evaluate_objective_function(p, n, d):\n"
				"*this and p are dimension incompatible.");
  if (!evaluating_point.is_point())
    throw std::invalid_argument("PPL::MIP_Problem::"
				"evaluate_objective_function(p, n, d):\n"
				"p is not a point.");

  // Compute the smallest space dimension  between `input_obj_function'
  // and `evaluating_point'.
  const dimension_type working_space_dim
    = std::min(ep_space_dim, input_obj_function.space_dimension());
  // Compute the optimal value of the cost function.
  const Coefficient& divisor = evaluating_point.divisor();
  ext_n = input_obj_function.inhomogeneous_term() * divisor;
  for (dimension_type i = working_space_dim; i-- > 0; )
    ext_n += evaluating_point.coefficient(Variable(i))
      * input_obj_function.coefficient(Variable(i));
  // Numerator and denominator should be coprime.
  normalize2(ext_n, divisor, ext_n, ext_d);
}

bool
PPL::MIP_Problem::is_lp_satisfiable() const {
#if PPL_NOISY_SIMPLEX
  num_iterations = 0;
#endif
  switch (status) {
  case UNSATISFIABLE:
    return false;
  case SATISFIABLE:
    // Intentionally fall through.
  case UNBOUNDED:
    // Intentionally fall through.
  case OPTIMIZED:
    // Intentionally fall through.
    return true;
  case PARTIALLY_SATISFIABLE:
    {
      MIP_Problem& x = const_cast<MIP_Problem&>(*this);
      // This code tries to handle the case that happens if the tableau is
      // empty, so it must be initialized.
      if (tableau.num_columns() == 0) {
        // Add two columns, the first that handles the inhomogeneous term and
        // the second that represent the `sign'.
        x.tableau.add_zero_columns(2);
        // Sync `mapping' for the inhomogeneous term.
        x.mapping.push_back(std::make_pair(0, 0));
        // The internal data structures are ready, so prepare for more
        // assertion to be checked.
        x.initialized = true;
      }

      // Apply incrementality to the pending constraint system.
      x.process_pending_constraints();
      // Update `first_pending_constraint': no more pending.
      x.first_pending_constraint = input_cs.size();
      // Update also `internal_space_dim'.
      x.internal_space_dim = x.external_space_dim;
      assert(OK());
      return (status != UNSATISFIABLE);
    }
  }
  // We should not be here!
  throw std::runtime_error("PPL internal error");
}

PPL::MIP_Problem_Status
PPL::MIP_Problem::solve_mip(bool& have_incumbent_solution,
			    mpq_class& incumbent_solution_value,
			    Generator& incumbent_solution_point,
			    MIP_Problem& lp,
			    const Variables_Set& i_vars) {
  // Solve the problem as a non MIP one, it must be done internally.
  PPL::MIP_Problem_Status lp_status;
  if (lp.is_lp_satisfiable()) {
    lp.second_phase();
    lp_status = (lp.status == OPTIMIZED) ? OPTIMIZED_MIP_PROBLEM
      : UNBOUNDED_MIP_PROBLEM;
  }
  else
    return UNFEASIBLE_MIP_PROBLEM;

  PPL_DIRTY_TEMP0(mpq_class, tmp_rational);

  Generator p = point();
  PPL_DIRTY_TEMP_COEFFICIENT(tmp_coeff1);
  PPL_DIRTY_TEMP_COEFFICIENT(tmp_coeff2);

  if (lp_status == UNBOUNDED_MIP_PROBLEM)
    p = lp.last_generator;
  else {
    assert(lp_status == OPTIMIZED_MIP_PROBLEM);
    // Do not call optimizing_point().
    p = lp.last_generator;
    lp.evaluate_objective_function(p, tmp_coeff1, tmp_coeff2);
    assign_r(tmp_rational.get_num(), tmp_coeff1, ROUND_NOT_NEEDED);
    assign_r(tmp_rational.get_den(), tmp_coeff2, ROUND_NOT_NEEDED);
    assert(is_canonical(tmp_rational));
    if (have_incumbent_solution
	&& ((lp.optimization_mode() == MAXIMIZATION
 	     && tmp_rational <= incumbent_solution_value)
 	    || (lp.optimization_mode() == MINIMIZATION
		&& tmp_rational >= incumbent_solution_value)))
      // Abandon this path.
      return lp_status;
  }

  bool found_satisfiable_generator = true;
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  const Coefficient& p_divisor = p.divisor();
  dimension_type nonint_dim;
  for (Variables_Set::const_iterator v_begin = i_vars.begin(),
	 v_end = i_vars.end(); v_begin != v_end; ++v_begin) {
    gcd_assign(gcd, p.coefficient(Variable(*v_begin)), p_divisor);
    if (gcd != p_divisor) {
      nonint_dim = *v_begin;
      found_satisfiable_generator = false;
      break;
    }
  }
  if (found_satisfiable_generator) {
    // All the coordinates of `point' are satisfiable.
    if (lp_status == UNBOUNDED_MIP_PROBLEM) {
      // This is a point that belongs to the MIP_Problem.
      // In this way we are sure that we will return every time
      // a feasible point if requested by the user.
      incumbent_solution_point = p;
      return lp_status;
    }
    if (!have_incumbent_solution
	|| (lp.optimization_mode() == MAXIMIZATION
	    && tmp_rational > incumbent_solution_value)
	|| tmp_rational < incumbent_solution_value) {
      incumbent_solution_value = tmp_rational;
      incumbent_solution_point = p;
      have_incumbent_solution = true;
#if PPL_NOISY_SIMPLEX
      PPL_DIRTY_TEMP_COEFFICIENT(num);
      PPL_DIRTY_TEMP_COEFFICIENT(den);
      lp.evaluate_objective_function(p, num, den);
      std::cerr << "new value found: " << num << "/" << den << std::endl;
#endif
    }
    return lp_status;
  }

  assert(nonint_dim < lp.space_dimension());

  assign_r(tmp_rational.get_num(), p.coefficient(Variable(nonint_dim)),
	   ROUND_NOT_NEEDED);
  assign_r(tmp_rational.get_den(), p_divisor, ROUND_NOT_NEEDED);
  tmp_rational.canonicalize();
  assign_r(tmp_coeff1, tmp_rational, ROUND_DOWN);
  assign_r(tmp_coeff2, tmp_rational, ROUND_UP);
  {
    MIP_Problem lp_aux = lp;
    lp_aux.add_constraint(Variable(nonint_dim) <= tmp_coeff1);
    solve_mip(have_incumbent_solution, incumbent_solution_value,
	      incumbent_solution_point, lp_aux, i_vars);
  }
  // TODO: change this when we will be able to remove constraints.
  lp.add_constraint(Variable(nonint_dim) >= tmp_coeff2);
  solve_mip(have_incumbent_solution, incumbent_solution_value,
	    incumbent_solution_point, lp, i_vars);
  return have_incumbent_solution ? lp_status : UNFEASIBLE_MIP_PROBLEM;
}

bool
PPL::MIP_Problem::choose_branching_variable(const MIP_Problem& mip,
					    const Variables_Set& i_vars,
					    dimension_type& branching_index) {
  // Insert here the variables that don't satisfy the integrality condition.
  const Constraint_Sequence& input_cs = mip.input_cs;
  const Generator& last_generator = mip.last_generator;
  const Coefficient& last_generator_divisor = last_generator.divisor();
  Variables_Set candidate_variables;

  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  for (Variables_Set::const_iterator v_it = i_vars.begin(),
	 v_end = i_vars.end(); v_it != v_end; ++v_it) {
    gcd_assign(gcd,
	       last_generator.coefficient(Variable(*v_it)),
	       last_generator_divisor);
    if (gcd != last_generator_divisor)
      candidate_variables.insert(*v_it);
  }
  // If this set is empty, we have finished.
  if (candidate_variables.empty())
    return true;

  // Check how many `active constraints' we have and track them.
  const dimension_type input_cs_num_rows = input_cs.size();
  std::deque<bool> satisfiable_constraints (input_cs_num_rows, false);
  for (dimension_type i = input_cs_num_rows; i-- > 0; )
    // An equality is an `active constraint' by definition.
    // If we have an inequality, check if it is an `active constraint'.
    if (input_cs[i].is_equality()
	|| is_saturated(input_cs[i], last_generator))
      satisfiable_constraints[i] = true;

  dimension_type current_num_appearances = 0;
  dimension_type winning_num_appearances = 0;

  // For every candidate variable, check how many times this appear in the
  // active constraints.
  for (Variables_Set::const_iterator v_it = candidate_variables.begin(),
	 v_end = candidate_variables.end(); v_it != v_end; ++v_it) {
    current_num_appearances = 0;
    for (dimension_type i = input_cs_num_rows; i-- > 0; )
      if (satisfiable_constraints[i]
	  && *v_it < input_cs[i].space_dimension()
	  && input_cs[i].coefficient(Variable(*v_it)) != 0)
	++current_num_appearances;
    if (current_num_appearances >= winning_num_appearances) {
      winning_num_appearances = current_num_appearances;
      branching_index = *v_it;
    }
  }
  return false;
}

bool
PPL::MIP_Problem::is_mip_satisfiable(MIP_Problem& lp, Generator& p,
				     const Variables_Set& i_vars) {
  // Solve the problem as a non MIP one, it must be done internally.
  if (!lp.is_lp_satisfiable())
    return false;
  PPL_DIRTY_TEMP0(mpq_class, tmp_rational);

  PPL_DIRTY_TEMP_COEFFICIENT(tmp_coeff1);
  PPL_DIRTY_TEMP_COEFFICIENT(tmp_coeff2);
  p = lp.last_generator;

  bool found_satisfiable_generator = true;
  dimension_type nonint_dim;
  const Coefficient& p_divisor = p.divisor();

#if PPL_SIMPLEX_USE_MIP_HEURISTIC
  found_satisfiable_generator
    = choose_branching_variable(lp, i_vars, nonint_dim);
#else
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  for (Variables_Set::const_iterator v_begin = i_vars.begin(),
	 v_end = i_vars.end(); v_begin != v_end; ++v_begin) {
    gcd_assign(gcd, p.coefficient(Variable(*v_begin)), p_divisor);
    if (gcd != p_divisor) {
      nonint_dim = *v_begin;
      found_satisfiable_generator = false;
      break;
    }
  }
#endif

  if (found_satisfiable_generator)
    return true;


  assert(nonint_dim < lp.space_dimension());

  assign_r(tmp_rational.get_num(), p.coefficient(Variable(nonint_dim)),
	   ROUND_NOT_NEEDED);
  assign_r(tmp_rational.get_den(), p_divisor, ROUND_NOT_NEEDED);
  tmp_rational.canonicalize();
  assign_r(tmp_coeff1, tmp_rational, ROUND_DOWN);
  assign_r(tmp_coeff2, tmp_rational, ROUND_UP);
  {
    MIP_Problem lp_aux = lp;
    lp_aux.add_constraint(Variable(nonint_dim) <= tmp_coeff1);
    if (is_mip_satisfiable(lp_aux, p, i_vars))
      return true;
  }
  lp.add_constraint(Variable(nonint_dim) >= tmp_coeff2);
  return is_mip_satisfiable(lp, p, i_vars);
}

bool
PPL::MIP_Problem::OK() const {
#ifndef NDEBUG
  using std::endl;
  using std::cerr;
#endif
  const dimension_type input_cs_num_rows = input_cs.size();
  // Check that every member used is OK.

  for (dimension_type i = input_cs_num_rows; i-- > 0; )
    if (!input_cs[i].OK())
      return false;

  if (!tableau.OK() || !input_obj_function.OK() || !last_generator.OK())
    return false;

  // Constraint system should contain no strict inequalities.
  for (dimension_type i = input_cs_num_rows; i-- > 0; )
    if (input_cs[i].is_strict_inequality()) {
#ifndef NDEBUG
      cerr << "The feasible region of the MIP_Problem is defined by "
	   << "a constraint system containing strict inequalities."
	   << endl;
      ascii_dump(cerr);
#endif
      return false;
    }

  // Constraint system and objective function should be dimension compatible.
  if (external_space_dim < input_obj_function.space_dimension()) {
#ifndef NDEBUG
    cerr << "The MIP_Problem and the objective function have "
	 << "incompatible space dimensions ("
	 << external_space_dim << " < "
	 << input_obj_function.space_dimension() << ")."
	 << endl;
    ascii_dump(cerr);
#endif
    return false;
  }

  if (status != UNSATISFIABLE && initialized) {
    // Here `last_generator' has to be meaningful.
    // Check for dimension compatibility and actual feasibility.
    if (external_space_dim != last_generator.space_dimension()) {
#ifndef NDEBUG
      cerr << "The MIP_Problem and the cached feasible point have "
 	   << "incompatible space dimensions ("
 	   << external_space_dim << " != "
	   << last_generator.space_dimension() << ")."
 	   << endl;
      ascii_dump(cerr);
#endif
      return false;
    }

    for (dimension_type i = 0; i < first_pending_constraint; ++i)
      if (!is_satisfied(input_cs[i], last_generator)) {
#ifndef NDEBUG
	cerr << "The cached feasible point does not belong to "
	     << "the feasible region of the MIP_Problem."
	     << endl;
	ascii_dump(cerr);
#endif
	return false;
      }

    // Check that every integer declared variable is really integer.
    // in the solution found.
    if (!i_variables.empty()) {
      PPL_DIRTY_TEMP_COEFFICIENT(gcd);
      for (Variables_Set::const_iterator v_it = i_variables.begin(),
	     v_end = i_variables.end(); v_it != v_end; ++v_it) {
	gcd_assign(gcd, last_generator.coefficient(Variable(*v_it)),
		   last_generator.divisor());
	if (gcd != last_generator.divisor())
	  return false;
      }
    }

    const dimension_type tableau_nrows = tableau.num_rows();
    const dimension_type tableau_ncols = tableau.num_columns();

    // The number of rows in the tableau and base should be equal.
    if (tableau_nrows != base.size()) {
#ifndef NDEBUG
      cerr << "tableau and base have incompatible sizes" << endl;
      ascii_dump(cerr);
#endif
      return false;
    }
    // The size of `mapping' should be equal to the space dimension
    // of `input_cs' plus one.
    if (mapping.size() != external_space_dim + 1) {
#ifndef NDEBUG
      cerr << "`input_cs' and `mapping' have incompatible sizes" << endl;
      ascii_dump(cerr);
#endif
      return false;
    }

    // The number of columns in the tableau and working_cost should be equal.
    if (tableau_ncols != working_cost.size()) {
#ifndef NDEBUG
      cerr << "tableau and working_cost have incompatible sizes" << endl;
      ascii_dump(cerr);
#endif
      return false;
    }

    // The vector base should contain indices of tableau's columns.
    for (dimension_type i = base.size(); i-- > 0; ) {
      if (base[i] > tableau_ncols) {
#ifndef NDEBUG
	cerr << "base contains an invalid column index" << endl;
	ascii_dump(cerr);
#endif
	return false;
      }
      // tableau[i][base[i] must be different from zero.
      // tableau[i][base[j], with i different from j, must not be a zero.
      for (dimension_type j = tableau_nrows; j-- > 0; )
	if (i != j && tableau[j][base[i]] != 0) {
#ifndef NDEBUG
	  cerr << "tableau[i][base[i] must be different from zero" << endl;
	  ascii_dump(cerr);
#endif
	  return false;
	}
      if (tableau[i][base[i]] == 0) {
#ifndef NDEBUG
	cerr << "tableau[i][base[j], with i different from j, must not be "
	     << "a zero" << endl;
	ascii_dump(cerr);
#endif
	return false;
      }
    }

    // The last column of the tableau must contain only zeroes.
    for (dimension_type i = tableau_nrows; i-- > 0; )
      if (tableau[i][tableau_ncols-1] != 0) {
#ifndef NDEBUG
	cerr << "the last column of the tableau must contain only"
	  "zeroes"<< endl;
	ascii_dump(cerr);
#endif
	return false;
      }
   }

   // All checks passed.
   return true;
}

void
PPL::MIP_Problem::ascii_dump(std::ostream& s) const {
  using namespace IO_Operators;
  s << "\nexternal_space_dim: " << external_space_dim << " \n";
  s << "\ninternal_space_dim: " << internal_space_dim << " \n";

  const dimension_type input_cs_size = input_cs.size();

  s << "\ninput_cs( " << input_cs_size << " )\n";
  for (dimension_type i = 0; i < input_cs_size; ++i)
    input_cs[i].ascii_dump(s);

  s << "\nfirst_pending_constraint: " <<  first_pending_constraint
    << std::endl;

  s << "\ninput_obj_function\n";
  input_obj_function.ascii_dump(s);
  s << "\nopt_mode "
    << (opt_mode == MAXIMIZATION ? "MAXIMIZATION" : "MINIMIZATION") << "\n";

  s << "\ninitialized: " << (initialized ? "YES" : "NO") << "\n";
  s << "\npricing: ";
  switch (pricing) {
  case PRICING_STEEPEST_EDGE_FLOAT:
    s << "PRICING_STEEPEST_EDGE_FLOAT";
    break;
  case PRICING_STEEPEST_EDGE_EXACT:
    s << "PRICING_STEEPEST_EDGE_EXACT";
    break;
  case PRICING_TEXTBOOK:
    s << "PRICING_TEXTBOOK";
    break;
  }
  s << "\n";

  s << "\nstatus: ";
  switch (status) {
  case UNSATISFIABLE:
    s << "UNSATISFIABLE";
    break;
  case SATISFIABLE:
    s << "SATISFIABLE";
    break;
  case UNBOUNDED:
    s << "UNBOUNDED";
    break;
  case OPTIMIZED:
    s << "OPTIMIZED";
    break;
  case PARTIALLY_SATISFIABLE:
    s << "PARTIALLY_SATISFIABLE";
    break;
  }
  s << "\n";

  s << "\ntableau\n";
  tableau.ascii_dump(s);
  s << "\nworking_cost( " << working_cost.size()<< " )\n";
  working_cost.ascii_dump(s);

  const dimension_type base_size = base.size();
  s << "\nbase( " << base_size << " )\n";
  for (dimension_type i = 0; i != base_size; ++i)
    s << base[i] << ' ';

  s << "\nlast_generator\n";
  last_generator.ascii_dump(s);

  const dimension_type mapping_size = mapping.size();
  s << "\nmapping( " << mapping_size << " )\n";
  for (dimension_type i = 1; i < mapping_size; ++i)
    s << "\n"<< i << " -> " << mapping[i].first << " -> " << mapping[i].second
      << ' ';

  s << "\n\ninteger_variables";
  i_variables.ascii_dump(s);
}

PPL_OUTPUT_DEFINITIONS(MIP_Problem)

bool
PPL::MIP_Problem::ascii_load(std::istream& s) {
  std::string str;
  if (!(s >> str) || str != "external_space_dim:")
    return false;

  if (!(s >> external_space_dim))
    return false;

  if (!(s >> str) || str != "internal_space_dim:")
    return false;

  if (!(s >> internal_space_dim))
    return false;

  if (!(s >> str) || str != "input_cs(")
    return false;

  dimension_type input_cs_size;

  if (!(s >> input_cs_size))
    return false;

  if (!(s >> str) || str != ")")
    return false;

  Constraint c(Constraint::zero_dim_positivity());
  for (dimension_type i = 0; i < input_cs_size; ++i) {
    if (!c.ascii_load(s))
      return false;
    input_cs.push_back(c);
  }

  if (!(s >> str) || str != "first_pending_constraint:")
    return false;

  if (!(s >> first_pending_constraint))
    return false;

  if (!(s >> str) || str != "input_obj_function")
    return false;

  if (!input_obj_function.ascii_load(s))
    return false;

  if (!(s >> str) || str != "opt_mode")
    return false;

  if (!(s >> str))
    return false;

  if (str == "MAXIMIZATION")
    set_optimization_mode(MAXIMIZATION);
  else {
    if (str != "MINIMIZATION")
      return false;
    set_optimization_mode(MINIMIZATION);
  }

  if (!(s >> str) || str != "initialized:")
    return false;
  if (!(s >> str))
    return false;
  if (str == "YES")
    initialized = true;
  else if (str == "NO")
    initialized = false;
  else
    return false;

  if (!(s >> str) || str != "pricing:")
    return false;
  if (!(s >> str))
    return false;
  if (str == "PRICING_STEEPEST_EDGE_FLOAT")
    pricing = PRICING_STEEPEST_EDGE_FLOAT;
  else if (str == "PRICING_STEEPEST_EDGE_EXACT")
    pricing = PRICING_STEEPEST_EDGE_EXACT;
  else if (str == "PRICING_TEXTBOOK")
    pricing = PRICING_TEXTBOOK;
  else
    return false;

  if (!(s >> str) || str != "status:")
    return false;

  if (!(s >> str))
    return false;

  if (str == "UNSATISFIABLE")
    status = UNSATISFIABLE;
  else if (str == "SATISFIABLE")
    status = SATISFIABLE;
  else if (str == "UNBOUNDED")
    status = UNBOUNDED;
  else if (str == "OPTIMIZED")
    status = OPTIMIZED;
  else if (str == "PARTIALLY_SATISFIABLE")
    status = PARTIALLY_SATISFIABLE;
  else
    return false;

  if (!(s >> str) || str != "tableau")
    return false;

  if (!tableau.ascii_load(s))
    return false;

  if (!(s >> str) || str != "working_cost(")
    return false;

  dimension_type working_cost_dim;

  if (!(s >> working_cost_dim))
    return false;

  if (!(s >> str) || str != ")")
    return false;

  if (!working_cost.ascii_load(s))
    return false;

  if (!(s >> str) || str != "base(")
    return false;

  dimension_type base_size;
  if (!(s >> base_size))
    return false;

  if (!(s >> str) || str != ")")
    return false;

  dimension_type base_value;
  for (dimension_type i = 0; i != base_size; ++i) {
    if (!(s >> base_value))
      return false;
    base.push_back(base_value);
  }

  if (!(s >> str) || str != "last_generator")
    return false;

  if (!last_generator.ascii_load(s))
    return false;

  if (!(s >> str) || str != "mapping(")
    return false;

  dimension_type mapping_size;
  if (!(s >> mapping_size))
    return false;

  if (!(s >> str) || str != ")")
    return false;

  dimension_type first_value;
  dimension_type second_value;
  dimension_type index;

  // The first `mapping' index is never used, so we initialize
  // it pushing back a dummy value.
  if (tableau.num_columns() != 0)
    mapping.push_back(std::make_pair(0, 0));

  for (dimension_type i = 1; i < mapping_size; ++i) {
    if (!(s >> index))
      return false;
    if (!(s >> str) || str != "->")
      return false;
    if (!(s >> first_value))
      return false;
    if (!(s >> str) || str != "->")
      return false;
    if (!(s >> second_value))
      return false;
    mapping.push_back(std::make_pair(first_value, second_value));
  }

  if (!(s >> str) || str != "integer_variables")
    return false;

  if (!i_variables.ascii_load(s))
    return false;

  assert(OK());
  return true;
}

/*! \relates Parma_Polyhedra_Library::MIP_Problem */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const MIP_Problem& lp) {
  s << "Constraints:";
  for (MIP_Problem::const_iterator i = lp.constraints_begin(),
	 i_end = lp.constraints_end(); i != i_end; ++i)
    s << "\n" << *i;
  s << "\nObjective function: "
    << lp.objective_function()
    << "\nOptimization mode: "
    << (lp.optimization_mode() == MAXIMIZATION
	? "MAXIMIZATION"
	: "MINIMIZATION");
  s << "\nInteger variables: " << lp.integer_space_dimensions();
 return s;
}
