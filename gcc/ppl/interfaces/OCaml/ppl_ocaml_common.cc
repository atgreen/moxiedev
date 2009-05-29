/* Domain-independent part of the OCaml interface: non-inline functions.
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

#include "ppl_ocaml_common.defs.hh"

namespace Parma_Polyhedra_Library {

namespace Interfaces {

namespace OCaml {

class PFunc {
 private:
   std::set<dimension_type> codomain;
   std::vector<dimension_type> vec;

 public:
   PFunc() {
   }

  bool has_empty_codomain() const {
    return codomain.empty();
   }

  dimension_type max_in_codomain() const {
    if (codomain.empty())
      throw std::runtime_error("PFunc::max_in_codomain()");
    return *codomain.rbegin();
  }

  bool maps(dimension_type i, dimension_type& j) const {
    if (i >= vec.size())
      return false;
    dimension_type vec_i = vec[i];
    if (vec_i == not_a_dimension())
      return false;
    j = vec_i;
    return true;
  }

  bool insert(dimension_type i, dimension_type j) {
    std::pair<std::set<dimension_type>::iterator, bool> s
      = codomain.insert(j);
    if (!s.second)
      // *this is not injective!
      return false;
    if (i > vec.size())
      vec.insert(vec.end(), i - vec.size(), not_a_dimension());
    if (i == vec.size()) {
      vec.insert(vec.end(), j);
      return true;
    }
    dimension_type& vec_i = vec[i];
    if (vec_i != not_a_dimension())
      // Already mapped: *this is not a function!
      return false;
    vec_i = j;
    return true;
  }
};


#ifdef PPL_WATCHDOG_LIBRARY_ENABLED

Parma_Watchdog_Library::Watchdog* p_timeout_object = 0;

#endif // PPL_WATCHDOG_LIBRARY_ENABLED

void
reset_timeout() {
#ifdef PPL_WATCHDOG_LIBRARY_ENABLED
  if (p_timeout_object) {
    delete p_timeout_object;
    p_timeout_object = 0;
    abandon_expensive_computations = 0;
  }
#endif // PPL_WATCHDOG_LIBRARY_ENABLED
}

namespace {

inline mpz_ptr
mpz_ptr_val(value val) {
  return static_cast<mpz_ptr>(Data_custom_val(val));
}

inline mpz_class&
mpz_class_val(value val) {
  return reinterpret_cast<mpz_class&>(*mpz_ptr_val(val));
}

// Function for the management of mpz_t integers.
extern "C" struct custom_operations _mlgmp_custom_z;

inline value
unregistered_value_p_zero_mpz(void) {
  value zero_mpz = caml_alloc_custom(&_mlgmp_custom_z, sizeof(mpz_t), 0, 1);
  mpz_init(mpz_ptr_val(zero_mpz));
  return zero_mpz;
}

} // anonymous namespace

value
build_ocaml_coefficient(const Coefficient& ppl_coeff) {
  CAMLparam0();
  CAMLlocal1(ml_coeff);
  ml_coeff = unregistered_value_p_zero_mpz();
  assign_r(mpz_class_val(ml_coeff), ppl_coeff, ROUND_NOT_NEEDED);
  CAMLreturn(ml_coeff);
}

Coefficient
build_ppl_Coefficient(value coeff) {
  mpz_class z(mpz_ptr_val(coeff));
  return Coefficient(z);
}

Linear_Expression
build_ppl_Linear_Expression(value e) {
  switch (Tag_val(e)) {
  case 0:
    // Variable
    return build_ppl_Variable(Field(e, 0));
  case 1: {
    // Coefficient
    mpz_class z(mpz_ptr_val(Field(e, 0)));
    return Linear_Expression(Coefficient(z));
  }
  case 2:
    // Unary_Plus
    return build_ppl_Linear_Expression(Field(e, 0));
  case 3:
    // Unary_Minus
    return -build_ppl_Linear_Expression(Field(e, 0));
  case 4:
    // Plus
    return build_ppl_Linear_Expression(Field(e, 0))
      + build_ppl_Linear_Expression(Field(e, 1));
  case 5:
    // Minus
    return build_ppl_Linear_Expression(Field(e, 0))
      - build_ppl_Linear_Expression(Field(e, 1));
  case 6: {
    // Times
    mpz_class z(mpz_ptr_val(Field(e, 0)));
    return Coefficient(z) * build_ppl_Linear_Expression(Field(e, 1));
  }
  default:
    throw std::invalid_argument("PPL OCaml interface invalid_argument\n:"
                                "error building PPL::Linear_Expression");
  }
}

Relation_Symbol
build_ppl_relsym(value caml_relsym) {
  assert(Is_long(caml_relsym));
  switch (Int_val(caml_relsym)) {
  case 0:
    return LESS_THAN;
  case 1:
    return LESS_OR_EQUAL;
  case 2:
    return EQUAL;
  case 3:
    return GREATER_OR_EQUAL;
  case 4:
    return GREATER_THAN;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error\n:"
                             "build_ppl_relsym(rel)");
  }
}

Optimization_Mode
build_ppl_opt_mode(value caml_opt_mode) {
  assert(Is_long(caml_opt_mode));
  switch (Int_val(caml_opt_mode)) {
  case 0:
    return MINIMIZATION;
  case 1:
    return MAXIMIZATION;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error\n:"
                             "build_ppl_opt_mode(opt)");
  }
}

Degenerate_Element
build_ppl_Degenerate_Element(value de) {
  assert(Is_long(de));
  switch (Int_val(de)) {
  case 0:
    return UNIVERSE;
  case 1:
    return EMPTY;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ppl_Degenerate_Element(de)");
  }
}

Complexity_Class
build_ppl_Complexity_Class(value cc) {
  assert(Is_long(cc));
  switch (Int_val(cc)) {
  case 0:
    return POLYNOMIAL_COMPLEXITY;
  case 1:
    return SIMPLEX_COMPLEXITY;
  case 2:
    return ANY_COMPLEXITY;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ppl_Complexity_Class(cc)");
  }
}

MIP_Problem::Control_Parameter_Name
build_ppl_control_parameter_name(value caml_cp_name) {
  assert(Is_long(caml_cp_name));
  switch (Int_val(caml_cp_name)) {
  case 0:
    return  MIP_Problem::PRICING;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ppl_control_parameter_name(cpn)");
  }
}

MIP_Problem::Control_Parameter_Value
build_ppl_control_parameter_value(value caml_cp_value) {
  assert(Is_long(caml_cp_value));
  switch (Int_val(caml_cp_value)) {
  case 0:
    return MIP_Problem::PRICING_STEEPEST_EDGE_FLOAT;
  case 1:
    return MIP_Problem::PRICING_STEEPEST_EDGE_EXACT;
  case 2:
    return MIP_Problem::PRICING_TEXTBOOK;
  default:
    // We should not be here!
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ppl_control_parameter_value(cpv)");
  }
}

Variables_Set
build_ppl_Variables_Set(value caml_vset) {
  Variables_Set ppl_vset;
  while (caml_vset != Val_emptylist) {
    ppl_vset.insert(value_to_ppl_dimension(Field(caml_vset, 0)));
    caml_vset = Field(caml_vset, 1);
  }
  return ppl_vset;
}

Constraint
build_ppl_Constraint(value c) {
  value e1 = Field(c, 0);
  value e2 = Field(c, 1);
  switch (Tag_val(c)) {
  case 0:
    // Less_Than
    return build_ppl_Linear_Expression(e1) < build_ppl_Linear_Expression(e2);
  case 1:
    // Less_Or_Equal
    return build_ppl_Linear_Expression(e1) <= build_ppl_Linear_Expression(e2);
  case 2:
    // Equal
    return build_ppl_Linear_Expression(e1) == build_ppl_Linear_Expression(e2);
  case 3:
    // Greater_Than
    return build_ppl_Linear_Expression(e1) > build_ppl_Linear_Expression(e2);
  case 4:
    // Greater_Or_Equal
    return build_ppl_Linear_Expression(e1) >= build_ppl_Linear_Expression(e2);
  default:
    throw std::invalid_argument("PPL OCaml interface invalid argument:\n"
                                "error building PPL::Constraint");
  }
}


template <typename R>
CAMLprim value
get_inhomogeneous_term(const R& r) {
  CAMLparam0();
  CAMLlocal1(coeff_term);
  PPL_DIRTY_TEMP_COEFFICIENT(coeff);
  neg_assign(coeff, r.inhomogeneous_term());
  coeff_term = caml_alloc(1,1);
  Store_field(coeff_term, 0, build_ocaml_coefficient(coeff));
  CAMLreturn(coeff_term);
}

// Takes from constraints, generators... the embedded linear
// expression.
template <typename R>
CAMLprim value
get_linear_expression(const R& r) {
  CAMLparam0();
  CAMLlocal2(zero_term, zero_mpz);
  CAMLlocal5(sum, term1, ml_le_var1, term2, ml_le_var2);
  dimension_type space_dimension = r.space_dimension();
  dimension_type varid = 0;
  PPL_DIRTY_TEMP_COEFFICIENT(coeff);
  while (varid < space_dimension
	 && (coeff = r.coefficient(Variable(varid))) == 0)
    ++varid;
  if (varid >= space_dimension) {
    zero_mpz = unregistered_value_p_zero_mpz();
    zero_term = caml_alloc(1,1);
    Store_field(zero_term, 0, zero_mpz);
    CAMLreturn(zero_term);
  }
  else {
    ml_le_var1 = caml_alloc(1,0);
    Store_field(ml_le_var1, 0, ppl_dimension_to_value(varid));
    term1 = caml_alloc(2,6);
    PPL_DIRTY_TEMP_COEFFICIENT(ppl_coeff);
    ppl_coeff = r.coefficient(Variable(varid));
    Store_field(term1, 0, build_ocaml_coefficient(ppl_coeff));
    Store_field(term1, 1, ml_le_var1);
    while (true) {
      ++varid;
      while (varid < space_dimension
	     && (coeff = r.coefficient(Variable(varid))) == 0)
	++varid;
      if (varid >= space_dimension)
	CAMLreturn(term1);
      else {
	ml_le_var2 = caml_alloc(1,0);
	Store_field(ml_le_var2, 0, ppl_dimension_to_value(varid));
	term2 = caml_alloc(2,6);
	ppl_coeff = r.coefficient(Variable(varid));
	Store_field(term2, 0, build_ocaml_coefficient(ppl_coeff));
	Store_field(term2, 1, ml_le_var2);
	sum = caml_alloc(2,4);
	Store_field(sum, 0, term1);
	Store_field(sum, 1, term2);
	term1 = sum;
      }
    }
  }
}

value
build_ocaml_generator(const Generator& ppl_generator) {
  CAMLparam0();
  CAMLlocal1(caml_generator);
  switch (ppl_generator.type()) {
  case Generator::LINE: {
    // Store the linear expression. (1,0) stands for
    // allocate one block (the linear expression) with Tag 0 (a line here).
    caml_generator = caml_alloc(1,0);
    Store_field(caml_generator, 0, get_linear_expression(ppl_generator));
    CAMLreturn(caml_generator);
  }
  case Generator::RAY: {
    caml_generator = caml_alloc(1,1);
    Store_field(caml_generator, 0, get_linear_expression(ppl_generator));
    CAMLreturn(caml_generator);
  }
  case Generator::POINT: {
    // Allocates two blocks (the linear expression and the divisor)
    // of tag 2 (Point).
    caml_generator = caml_alloc(2,2);
    Store_field(caml_generator, 0, get_linear_expression(ppl_generator));
    const Coefficient& divisor = ppl_generator.divisor();
    Store_field(caml_generator, 1, build_ocaml_coefficient(divisor));
    CAMLreturn(caml_generator);
  }
  case Generator::CLOSURE_POINT:  {
    caml_generator = caml_alloc(2,3);
    Store_field(caml_generator, 0, get_linear_expression(ppl_generator));
    const Coefficient& divisor = ppl_generator.divisor();
    Store_field(caml_generator, 1, build_ocaml_coefficient(divisor));
    CAMLreturn(caml_generator);
  }
  default:
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ocaml_generator(g)");
  }
}

value
build_ocaml_grid_generator(const Grid_Generator& ppl_grid_generator) {
  CAMLparam0();
  CAMLlocal1(caml_generator);
  switch (ppl_grid_generator.type()) {
  case Grid_Generator::LINE: {
    // Store the linear expression. (1,0) stands for
    // allocate one block (the linear expression) with Tag 0 (a line here).
    caml_generator = caml_alloc(1,0);
    Store_field(caml_generator, 0, get_linear_expression(ppl_grid_generator));
    CAMLreturn(caml_generator);
  }
  case Grid_Generator::PARAMETER: {
    caml_generator = caml_alloc(2,1);
    Store_field(caml_generator, 0, get_linear_expression(ppl_grid_generator));
    const Coefficient& divisor = ppl_grid_generator.divisor();
    Store_field(caml_generator, 1, build_ocaml_coefficient(divisor));
    CAMLreturn(caml_generator);
  }
  case Grid_Generator::POINT: {
    // Allocates two blocks (the linear expression and the divisor)
    // of tag 2 (Point).
    caml_generator = caml_alloc(2,2);
    Store_field(caml_generator, 0, get_linear_expression(ppl_grid_generator));
    const Coefficient& divisor = ppl_grid_generator.divisor();
    Store_field(caml_generator, 1, build_ocaml_coefficient(divisor));
    CAMLreturn(caml_generator);
  }
  default:
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ocaml_grid_generator(g)");
  }
}

value
build_ocaml_constraint(const Constraint& ppl_constraint) {
  CAMLparam0();
  CAMLlocal1(caml_constraint);
  switch (ppl_constraint.type()) {
  case Constraint::EQUALITY: {
    caml_constraint = caml_alloc(2,2);
    Store_field(caml_constraint, 0, get_linear_expression(ppl_constraint));
    Store_field(caml_constraint, 1, get_inhomogeneous_term(ppl_constraint));
    CAMLreturn(caml_constraint);
  }
  case Constraint::STRICT_INEQUALITY: {
    caml_constraint = caml_alloc(2,3);
    Store_field(caml_constraint, 0, get_linear_expression(ppl_constraint));
    Store_field(caml_constraint, 1, get_inhomogeneous_term(ppl_constraint));
    CAMLreturn(caml_constraint);
  }
  case Constraint::NONSTRICT_INEQUALITY: {
    caml_constraint = caml_alloc(2,4);
    Store_field(caml_constraint, 0, get_linear_expression(ppl_constraint));
    Store_field(caml_constraint, 1, get_inhomogeneous_term(ppl_constraint));
    CAMLreturn(caml_constraint);
  }
  default:
    throw std::runtime_error("PPL OCaml interface internal error:\n"
                             "build_ocaml_constraint(c)");
  }
}

value
build_ocaml_congruence(const Congruence& ppl_congruence) {
  CAMLparam0();
  CAMLlocal1(caml_congruence);
  caml_congruence = caml_alloc(3,0);
  Store_field(caml_congruence, 0, get_linear_expression(ppl_congruence));
  Store_field(caml_congruence, 1, get_inhomogeneous_term(ppl_congruence));
  const Coefficient& modulus = ppl_congruence.modulus();
  Store_field(caml_congruence, 2, build_ocaml_coefficient(modulus));
  CAMLreturn(caml_congruence);
}

value
build_ocaml_congruence_system(const Congruence_System& ppl_cgs) {
  CAMLparam0();
  CAMLlocal2(result, new_tail);
  result = Val_emptylist;
  for (Congruence_System::const_iterator v_begin = ppl_cgs.begin(),
  	 v_end = ppl_cgs.end(); v_begin != v_end; ++v_begin) {
    new_tail = caml_alloc_tuple(2);
    Store_field(new_tail, 0, build_ocaml_congruence(*v_begin));
    Store_field(new_tail, 1, result);
    result = new_tail;
  }
  CAMLreturn(result);
}

value
build_ocaml_constraint_system(const Constraint_System& ppl_cs) {
  CAMLparam0();
  CAMLlocal2(result, new_tail);
  result = Val_emptylist;
  for (Constraint_System::const_iterator v_begin = ppl_cs.begin(),
  	 v_end = ppl_cs.end(); v_begin != v_end; ++v_begin) {
    new_tail = caml_alloc_tuple(2);
    Store_field(new_tail, 0, build_ocaml_constraint(*v_begin));
    Store_field(new_tail, 1, result);
    result = new_tail;
  }
  CAMLreturn(result);
}

value
build_ocaml_generator_system(const Generator_System& ppl_gs) {
  CAMLparam0();
  CAMLlocal2(result, new_tail);
  result = Val_emptylist;
  for (Generator_System::const_iterator v_begin = ppl_gs.begin(),
  	 v_end = ppl_gs.end(); v_begin != v_end; ++v_begin) {
    new_tail = caml_alloc_tuple(2);
    Store_field(new_tail, 0, build_ocaml_generator(*v_begin));
    Store_field(new_tail, 1, result);
    result = new_tail;
  }
  CAMLreturn(result);
}

value
build_ocaml_grid_generator_system(const Grid_Generator_System& ppl_ggs) {
  CAMLparam0();
  CAMLlocal2(result, new_tail);
  result = Val_emptylist;
  for (Grid_Generator_System::const_iterator v_begin = ppl_ggs.begin(),
  	 v_end = ppl_ggs.end(); v_begin != v_end; ++v_begin) {
    new_tail = caml_alloc_tuple(2);
    Store_field(new_tail, 0, build_ocaml_grid_generator(*v_begin));
    Store_field(new_tail, 1, result);
    result = new_tail;
  }
  CAMLreturn(result);
}

value
build_ocaml_poly_con_relation(Poly_Con_Relation& r) {
  CAMLparam0();
  CAMLlocal2(result, cons);
  result = Val_emptylist;
  while (r != Poly_Con_Relation::nothing()) {
    if (r.implies(Poly_Con_Relation::is_disjoint())) {
      cons = caml_alloc_tuple(2);
      Store_field(cons, 0, Val_int(0));
      Store_field(cons, 1, result);
      result = cons;
      r = r - Poly_Con_Relation::is_disjoint();
    }
    else if (r.implies(Poly_Con_Relation::strictly_intersects())) {
      cons = caml_alloc_tuple(2);
      Store_field(cons, 0, Val_int(1));
      Store_field(cons, 1, result);
      result = cons;
      r = r - Poly_Con_Relation::strictly_intersects();
    }
    else if (r.implies(Poly_Con_Relation::is_included())) {
      cons = caml_alloc_tuple(2);
      Store_field(cons, 0, Val_int(2));
      Store_field(cons, 1, result);
      result = cons;
      r = r - Poly_Con_Relation::is_included();
    }
    else if (r.implies(Poly_Con_Relation::saturates())) {
      cons = caml_alloc_tuple(2);
      Store_field(cons, 0, Val_int(3));
      Store_field(cons, 1, result);
      result = cons;
      r = r - Poly_Con_Relation::saturates();
    }
  }
  CAMLreturn(result);
}

value
build_ocaml_poly_gen_relation(Poly_Gen_Relation& r) {
  CAMLparam0();
  CAMLlocal2(result, cons);
  result = Val_emptylist;
  while (r != Poly_Gen_Relation::nothing()) {
    if (r.implies(Poly_Gen_Relation::subsumes())) {
      cons = caml_alloc_tuple(2);
      Store_field(cons, 0, Val_int(0));
      Store_field(cons, 1, result);
      result = cons;
      r = r - Poly_Gen_Relation::subsumes();
    }
  }
  CAMLreturn(result);
}

Congruence
build_ppl_Congruence(value c) {
  value e1 = Field(c, 0);
  value e2 = Field(c, 1);
  mpz_class z(mpz_ptr_val(Field(c, 2)));
  Linear_Expression lhs = build_ppl_Linear_Expression(e1);
  Linear_Expression rhs = build_ppl_Linear_Expression(e2);
  return ((lhs %= rhs) / z);
}

Generator
build_ppl_Generator(value g) {
  switch (Tag_val(g)) {
  case 0:
    // Line
    return Generator::line(build_ppl_Linear_Expression(Field(g, 0)));
  case 1:
    // Ray
    return Generator::ray(build_ppl_Linear_Expression(Field(g, 0)));
  case 2: {
    // Point
    mpz_class z(mpz_ptr_val(Field(g, 1)));
    return Generator::point(build_ppl_Linear_Expression(Field(g, 0)),
			    Coefficient(z));
  }
  case 3: {
    // Closure_point
    mpz_class z(mpz_ptr_val(Field(g, 1)));
    return Generator::closure_point(build_ppl_Linear_Expression(Field(g, 0)),
				    Coefficient(z));
  }
  default:
    throw std::invalid_argument("PPL OCaml interface invalid argument:\n"
                                "error building PPL::Generator");
  }
}

Grid_Generator
build_ppl_Grid_Generator(value gg) {
  switch (Tag_val(gg)) {
   case 0:
     // Line
     return grid_line(build_ppl_Linear_Expression(Field(gg, 0)));
   case 1: {
     // Parameter
     mpz_class z(mpz_ptr_val(Field(gg, 1)));
     return parameter(build_ppl_Linear_Expression(Field(gg, 0)),
		      Coefficient(z));
   }
   case 2: {
     // Point
     mpz_class z(mpz_ptr_val(Field(gg, 1)));
     return grid_point(build_ppl_Linear_Expression(Field(gg, 0)),
		       Coefficient(z));
   }
  default:
    // We should not be here!
    throw std::invalid_argument("PPL OCaml interface invalid argument:\n"
                                "error building PPL::Grid_Generator");
  }
}

Constraint_System
build_ppl_Constraint_System(value cl) {
  Constraint_System cs;
  while (cl != Val_emptylist) {
    cs.insert(build_ppl_Constraint(Field(cl, 0)));
    cl = Field(cl, 1);
  }
  return cs;
}

Generator_System
build_ppl_Generator_System(value gl) {
  Generator_System gs;
  while (gl != Val_emptylist) {
    gs.insert(build_ppl_Generator(Field(gl, 0)));
    gl = Field(gl, 1);
  }
  return gs;
}

Congruence_System
build_ppl_Congruence_System(value cgl) {
  Congruence_System cgs;
  while (cgl != Val_emptylist) {
    cgs.insert(build_ppl_Congruence(Field(cgl, 0)));
    cgl = Field(cgl, 1);
  }
  return cgs;
}

Grid_Generator_System
build_ppl_Grid_Generator_System(value caml_ggs) {
  Grid_Generator_System ggs;
  while (caml_ggs != Val_emptylist) {
    ggs.insert(build_ppl_Grid_Generator(Field(caml_ggs, 0)));
    caml_ggs = Field(caml_ggs, 1);
  }
  return ggs;
}

//! Give access to the embedded MIP_Problem* in \p v.
inline MIP_Problem*&
p_MIP_Problem_val(value v) {
  return *reinterpret_cast<MIP_Problem**>(Data_custom_val(v));
}

void
custom_MIP_Problem_finalize(value v) {
  delete p_MIP_Problem_val(v);
}

static struct custom_operations MIP_Problem_custom_operations = {
  "it.unipr.cs.ppl" "." PPL_VERSION "." "MIP_Problem",
  custom_MIP_Problem_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

inline value
unregistered_value_p_MIP_Problem(const MIP_Problem& ph) {
  value v = caml_alloc_custom(&MIP_Problem_custom_operations,
                              sizeof(MIP_Problem*), 0, 1);
  p_MIP_Problem_val(v) = const_cast<MIP_Problem*>(&ph);
  return v;
}

} // namespace OCaml

} // namespace Interfaces

} // namespace Parma_Polyhedra_Library

using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::Interfaces::OCaml;

extern "C"
CAMLprim value
ppl_new_MIP_Problem_from_space_dimension(value d) try {
  CAMLparam1(d);
  dimension_type dd = value_to_ppl_dimension(d);
  MIP_Problem& ppl_mip = *new MIP_Problem(dd);
  CAMLreturn(unregistered_value_p_MIP_Problem(ppl_mip));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_new_MIP_Problem(value d, value caml_cs, value caml_cost,
		    value caml_opt_mode) try {
  CAMLparam4(d, caml_cs, caml_cost, caml_opt_mode);
  dimension_type dd = value_to_ppl_dimension(d);
  Constraint_System ppl_cs = build_ppl_Constraint_System(caml_cs);
  Linear_Expression ppl_cost = build_ppl_Linear_Expression(caml_cost);
  Optimization_Mode ppl_opt_mode = build_ppl_opt_mode(caml_opt_mode);
  MIP_Problem& ppl_mip = *new MIP_Problem(dd, ppl_cs, ppl_cost,	ppl_opt_mode);
  CAMLreturn(unregistered_value_p_MIP_Problem(ppl_mip));
}
CATCH_ALL


extern "C"
CAMLprim value
ppl_MIP_Problem_space_dimension(value ph) try {
  CAMLparam1(ph);
  const MIP_Problem& pph = *p_MIP_Problem_val(ph);
  dimension_type d = pph.space_dimension();
  CAMLreturn(ppl_dimension_to_value(d));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_integer_space_dimensions(value caml_mip) try {
  CAMLparam1(caml_mip);
  CAMLlocal2(result, new_tail);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  const Variables_Set& ppl_ivars = ppl_mip.integer_space_dimensions();
  result = Val_emptylist;
  for (Variables_Set::const_reverse_iterator i = ppl_ivars.rbegin(),
         i_end = ppl_ivars.rend(); i != i_end; ++i) {
    new_tail = caml_alloc_tuple(2);
    Store_field(new_tail, 0, ppl_dimension_to_value(*i));
    Store_field(new_tail, 1, result);
    result = new_tail;
  }
  CAMLreturn(result);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_constraints(value caml_mip) try {
  CAMLparam1(caml_mip);
  const MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  Constraint_System cs;
  for (MIP_Problem::const_iterator cs_it = ppl_mip.constraints_begin(),
	 cs_end = ppl_mip.constraints_end(); cs_it != cs_end; ++cs_it) {
    cs.insert(*cs_it);
  }
  CAMLreturn(build_ocaml_constraint_system(cs));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_add_space_dimensions_and_embed(value caml_mip, value dim) try {
  CAMLparam2(caml_mip, dim);
  dimension_type ppl_dim = value_to_ppl_dimension(dim);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.add_space_dimensions_and_embed(ppl_dim);
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_add_to_integer_space_dimensions(value caml_mip,
						value caml_ivars) try {
  CAMLparam2(caml_mip, caml_ivars);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.add_to_integer_space_dimensions(build_ppl_Variables_Set(caml_ivars));
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_add_constraint(value caml_mip,
			       value caml_constraint) try {
  CAMLparam2(caml_mip, caml_constraint);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.add_constraint(build_ppl_Constraint(caml_constraint));
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_add_constraints(value caml_mip,
                                value caml_constraints) try {
  CAMLparam2(caml_mip, caml_constraints);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.add_constraints(build_ppl_Constraint_System(caml_constraints));
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_set_objective_function(value caml_mip,
				       value caml_cost) try {
  CAMLparam2(caml_mip, caml_cost);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.set_objective_function(build_ppl_Linear_Expression(caml_cost));
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_is_satisfiable(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  CAMLreturn(ppl_mip.is_satisfiable());
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_solve(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  MIP_Problem_Status mip_status = ppl_mip.solve();
  switch (mip_status) {
  case UNFEASIBLE_MIP_PROBLEM:
    CAMLreturn(Val_int(0));
  case UNBOUNDED_MIP_PROBLEM:
    CAMLreturn(Val_int(1));
  case OPTIMIZED_MIP_PROBLEM:
    CAMLreturn(Val_int(2));
  default:
    ;
  }
  // We should not be here!
  throw std::runtime_error("PPL OCaml interface internal error");
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_optimization_mode(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  Optimization_Mode opt_mode = ppl_mip.optimization_mode();
  switch (opt_mode) {
  case MINIMIZATION:
    CAMLreturn(Val_int(0));
  case MAXIMIZATION:
    CAMLreturn(Val_int(1));
  default:
    ;
  }
  // We should not be here!
  throw std::runtime_error("PPL OCaml interface internal error");
 }
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_set_control_parameter(value caml_mip,
                                      value caml_cp_value) try {
  CAMLparam2(caml_mip, caml_cp_value);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  MIP_Problem::Control_Parameter_Value ppl_cp_value
    = build_ppl_control_parameter_value(caml_cp_value);
  ppl_mip.set_control_parameter(ppl_cp_value);
  CAMLreturn(Val_unit);
 }
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_get_control_parameter(value caml_mip,
                                      value caml_cp_name) try {
  CAMLparam2(caml_mip, caml_cp_name);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  MIP_Problem::Control_Parameter_Name ppl_cp_name
    = build_ppl_control_parameter_name(caml_cp_name);
  MIP_Problem::Control_Parameter_Value ppl_cp_value
    = ppl_mip.get_control_parameter(ppl_cp_name);
  switch (ppl_cp_value) {
  case MIP_Problem::PRICING_STEEPEST_EDGE_FLOAT:
    CAMLreturn(Val_int(0));
  case MIP_Problem::PRICING_STEEPEST_EDGE_EXACT:
    CAMLreturn(Val_int(1));
  case MIP_Problem::PRICING_TEXTBOOK:
    CAMLreturn(Val_int(2));
  default:
    ;
  }
  // We should not be here!
  throw std::runtime_error("PPL OCaml interface internal error");
 }
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_feasible_point(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  CAMLreturn(build_ocaml_generator(ppl_mip.feasible_point()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_optimizing_point(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  CAMLreturn(build_ocaml_generator(ppl_mip.optimizing_point()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_optimal_value(value caml_mip) try {
  CAMLparam1(caml_mip);
  CAMLlocal1(caml_return_value);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  PPL_DIRTY_TEMP_COEFFICIENT(num);
  PPL_DIRTY_TEMP_COEFFICIENT(den);
  ppl_mip.optimal_value(num, den);
  caml_return_value = caml_alloc(2,0);
  Store_field(caml_return_value, 0, build_ocaml_coefficient(num));
  Store_field(caml_return_value, 1, build_ocaml_coefficient(den));
  CAMLreturn(caml_return_value);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_evaluate_objective_function(value caml_mip,
					    value caml_generator) try {
  CAMLparam2(caml_mip, caml_generator);
  CAMLlocal1(caml_return_value);
  Generator g = build_ppl_Generator(caml_generator);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  PPL_DIRTY_TEMP_COEFFICIENT(num);
  PPL_DIRTY_TEMP_COEFFICIENT(den);
  ppl_mip.evaluate_objective_function(g, num, den);
  caml_return_value = caml_alloc(2,0);
  Store_field(caml_return_value, 0, build_ocaml_coefficient(num));
  Store_field(caml_return_value, 1, build_ocaml_coefficient(den));
  CAMLreturn(caml_return_value);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_OK(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  CAMLreturn(ppl_mip.OK());
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_objective_function(value caml_mip) try {
  CAMLparam1(caml_mip);
  CAMLlocal4(homogeneous_term, inhomogeneous_term, sum, coeff);
  const MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  const Linear_Expression& ppl_obj_func = ppl_mip.objective_function();
  homogeneous_term = get_linear_expression(ppl_obj_func);
  inhomogeneous_term
    = build_ocaml_coefficient(ppl_obj_func.inhomogeneous_term());
  coeff = caml_alloc(1,1);
  Store_field(coeff, 0, inhomogeneous_term);
  sum = caml_alloc(2,4);
  Store_field(sum, 0, homogeneous_term);
  Store_field(sum, 1, coeff);
  CAMLreturn(sum);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_clear(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.clear();
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_set_optimization_mode(value caml_mip, value caml_opt_mode) try{
  CAMLparam2(caml_mip, caml_opt_mode);
  Optimization_Mode ppl_opt_mode= build_ppl_opt_mode(caml_opt_mode);
  MIP_Problem& ppl_mip = *p_MIP_Problem_val(caml_mip);
  ppl_mip.set_optimization_mode(ppl_opt_mode);
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_swap(value caml_mip1, value caml_mip2) try{
  CAMLparam2(caml_mip1, caml_mip2);
  MIP_Problem& ppl_mip1 = *p_MIP_Problem_val(caml_mip1);
  MIP_Problem& ppl_mip2 = *p_MIP_Problem_val(caml_mip2);
  ppl_mip1.swap(ppl_mip2);
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_MIP_Problem_ascii_dump(value caml_mip) try {
  CAMLparam1(caml_mip);
  MIP_Problem& mip = *p_MIP_Problem_val(caml_mip);
  std::ostringstream s;
  mip.ascii_dump(s);
  CAMLreturn(caml_copy_string(s.str().c_str()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_version_major(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(Val_long(version_major()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_version_minor(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(Val_long(version_minor()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_version_revision(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(Val_long(version_revision()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_version_beta(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(Val_long(version_beta()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_version(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(caml_copy_string(version()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_banner(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(caml_copy_string(banner()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_io_wrap_string(value src,
                   value indent_depth,
                   value preferred_first_line_length,
                   value preferred_line_length) try {
  CAMLparam4(src, indent_depth, preferred_first_line_length,
             preferred_line_length);
  unsigned cpp_indent_depth
    = value_to_unsigned<unsigned>(indent_depth);
  unsigned cpp_preferred_first_line_length
    = value_to_unsigned<unsigned>(preferred_first_line_length);
  unsigned cpp_preferred_line_length
    = value_to_unsigned<unsigned>(preferred_line_length);
  using IO_Operators::wrap_string;
  CAMLreturn(caml_copy_string(wrap_string(String_val(src),
                                          cpp_indent_depth,
                                          cpp_preferred_first_line_length,
                                          cpp_preferred_line_length
                                          ).c_str()));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_Coefficient_is_bounded(value unit) try {
  CAMLparam1(unit);
  CAMLreturn(std::numeric_limits<Coefficient>::is_bounded
             ? Val_true : Val_false);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_Coefficient_min(value unit) try {
  CAMLparam1(unit);
  if (std::numeric_limits<Coefficient>::is_bounded) {
    const Coefficient& min = std::numeric_limits<Coefficient>::min();
    CAMLreturn(build_ocaml_coefficient(min));
  }
  else
    CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_Coefficient_max(value unit) try {
  CAMLparam1(unit);
  if (std::numeric_limits<Coefficient>::is_bounded) {
    const Coefficient& max = std::numeric_limits<Coefficient>::max();
    CAMLreturn(build_ocaml_coefficient(max));
  }
  else
    CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_max_space_dimension(value unit) try {
  CAMLparam1(unit);
  dimension_type d = max_space_dimension();
  CAMLreturn(ppl_dimension_to_value(d));
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_set_rounding_for_PPL(value unit) try {
  CAMLparam1(unit);
  set_rounding_for_PPL();
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_restore_pre_PPL_rounding(value unit) try {
  CAMLparam1(unit);
  restore_pre_PPL_rounding();
  CAMLreturn(Val_unit);
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_set_timeout(value time) try {
  CAMLparam1(time);
#ifndef PPL_WATCHDOG_LIBRARY_ENABLED
  const char* what = "PPL OCaml interface usage error:\n"
    "ppl_set_timeout: the PPL Watchdog library is not enabled.";
  throw std::runtime_error(what);
#else
  // In case a timeout was already set.
  reset_timeout();
  unsigned cpp_time = value_to_unsigned<unsigned>(time);
  static timeout_exception e;
  using Parma_Watchdog_Library::Watchdog;
  p_timeout_object = new Watchdog(cpp_time, abandon_expensive_computations, e);
  CAMLreturn(Val_unit);
#endif // PPL_WATCHDOG_LIBRARY_ENABLED
}
CATCH_ALL

extern "C"
CAMLprim value
ppl_reset_timeout(value unit) try {
  CAMLparam1(unit);
#ifndef PPL_WATCHDOG_LIBRARY_ENABLED
  throw std::runtime_error("PPL OCaml interface error:\n"
                           "the PPL Watchdog library is not enabled.");
#else
  reset_timeout();
  CAMLreturn(Val_unit);
#endif // PPL_WATCHDOG_LIBRARY_ENABLED
}
CATCH_ALL
