/* Domain-independent part of the Java interface: declarations.
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

#ifndef PPL_ppl_java_common_defs_hh
#define PPL_ppl_java_common_defs_hh 1

#include "ppl.hh"
#include <jni.h>
#include "interfaced_boxes.hh"
#include "marked_pointers.hh"

#define CATCH_ALL \
  catch(const std::overflow_error& e) { \
    handle_exception(env, e); \
  } \
  catch(const std::length_error& e) { \
    handle_exception(env, e); \
  } \
  catch (const std::bad_alloc& e) { \
    handle_exception(env, e); \
  } \
  catch (const std::domain_error& e) { \
    handle_exception(env, e); \
  } \
 catch (const std::invalid_argument& e) { \
    handle_exception(env, e); \
  } \
 catch(const std::logic_error& e) { \
    handle_exception(env, e); \
  } \
  catch (const std::exception& e) { \
    handle_exception(env, e); \
  } \
  catch (...) { \
    handle_exception(env); \
  }

namespace Parma_Polyhedra_Library {

namespace Interfaces {

namespace Java {

void
handle_exception(JNIEnv* env, const std::logic_error& e);

void
handle_exception(JNIEnv* env, const std::invalid_argument& e);

void
handle_exception(JNIEnv* env, const std::domain_error& e);

void
handle_exception(JNIEnv* env, const std::overflow_error& e);

void
handle_exception(JNIEnv* env, const std::length_error& e);

void
handle_exception(JNIEnv* env, const std::bad_alloc&);

void
handle_exception(JNIEnv* env, const std::exception& e);

void
handle_exception(JNIEnv* env);

/*! \brief
  Builds an unsigned C++ number from the Java native number \p value.

  \param value
  The Java native number of type V to be converted.

  \exception std::invalid_argument
  Thrown if \p value is negative.
*/
template <typename U, typename V>
U
jtype_to_unsigned(const V& value);

//! Builds a Java Boolean from C++ bool \p value.
jobject
bool_to_j_boolean(JNIEnv* env, const bool value);

//! Returns the Java int stored in Java Integer \p j_integer.
jint
j_integer_to_j_int(JNIEnv* env, const jobject& j_integer);

//! Builds a Java Integer from Java int \p value.
jobject
j_int_to_j_integer(JNIEnv* env, const jint& value);

//! Returns the Java long stored in Java Long \p j_long.
jlong
j_long_class_to_j_long(JNIEnv* env, const jobject& j_long);

//! Builds a Java Long from Java long \p value.
jobject
j_long_to_j_long_class(JNIEnv* env, const jlong& value);

/*! \brief
  Returns \c true if and only if the Java object \p ppl_object
  refers to a C++ object.
*/
bool
is_java_marked(JNIEnv* env, const jobject& ppl_object);


/*! \brief
  Builds a Java parma_polyhedra_library::Poly_Gen_Relation
  from C++ Poly_Gen_Relation \p r.
*/
jobject
build_java_poly_gen_relation(JNIEnv* env, Poly_Gen_Relation& r);

/*! \brief
  Builds a Java parma_polyhedra_library::Poly_Con_Relation
  from C++ Poly_Con_Relation \p r.
*/
jobject
build_java_poly_con_relation(JNIEnv* env, Poly_Con_Relation& r);

/*! \brief
  Builds a C++ Variables_Set
  from Java parma_polyhedra_library::Variables_Set \p v_set.
*/
Variables_Set
build_cxx_variables_set(JNIEnv* env, const jobject& v_set);

/*! \brief
  Builds a Java parma_polyhedra_library::Variables_Set
  from C++ Variables_Set \p v_set.
*/
jobject
build_java_variables_set(JNIEnv* env, const Variables_Set& v_set);

/*! \brief
  Builds a C++ Relation_Symbol
  from Java parma_polyhedra_library::Relation_Symbol \p j_relsym.
*/
Relation_Symbol
build_cxx_relsym(JNIEnv* env, const jobject& j_relsym);

/*! \brief
  Builds a C++ Optimization_Mode
  from Java parma_polyhedra_library::Optimization_Mode \p j_opt_mode.
*/
Optimization_Mode
build_cxx_optimization_mode(JNIEnv* env, const jobject& j_opt_mode);

/*! \brief
  Builds a Java parma_polyhedra_library::Optimization_Mode
  from C++ Optimization_Mode \p opt_mode.
*/
jobject
build_java_optimization_mode(JNIEnv* env, const Optimization_Mode& opt_mode);

/*! \brief
  Builds a C++ MIP_Problem::Control_Parameter_Name
  from Java parma_polyhedra_library::Control_Parameter_Name \p j_cp_name.
*/
MIP_Problem::Control_Parameter_Name
build_cxx_control_parameter_name(JNIEnv* env, const jobject& j_cp_name);

/*! \brief
  Builds a Java parma_polyhedra_library::Control_Parameter_Name
  from C++ MIP_Problem::Control_Parameter_Name \p cp_name.
*/
jobject
build_java_control_parameter_name
(JNIEnv* env, const MIP_Problem::Control_Parameter_Name& cp_name);

/*! \brief
  Builds a C++ MIP_Problem::Control_Parameter_Value
  from Java parma_polyhedra_library::Control_Parameter_Value \p j_cp_value.
*/
MIP_Problem::Control_Parameter_Value
build_cxx_control_parameter_value(JNIEnv* env, const jobject& j_cp_value);

/*! \brief
  Builds a Java parma_polyhedra_library::Control_Parameter_Value
  from C++ MIP_Problem::Control_Parameter_Value \p cp_value.
*/
jobject
build_java_control_parameter_value
(JNIEnv* env, const MIP_Problem::Control_Parameter_Value& cp_value);

/*! \brief
  Builds a Java parma_polyhedra_library::MIP_Problem_Status
  from C++ MIP_Problem_Status \p mip_status.
*/
jobject
build_java_mip_status(JNIEnv* env, const MIP_Problem_Status& mip_status);

/*! \brief
  Builds a C++ Variable
  from Java parma_polyhedra_library::Variable \p j_var.
*/
Variable
build_cxx_variable(JNIEnv* env, const jobject& j_var);

/*! \brief
  Builds a Java parma_polyhedra_library::Variable
  from C++ Variable \p var.
*/
jobject
build_java_variable(JNIEnv* env, const Variable& var);

/*! \brief
  Builds a C++ Coefficient
  from Java parma_polyhedra_library::Coefficient \p j_coeff.
*/
Coefficient
build_cxx_coeff(JNIEnv* env, const jobject& j_coeff);

/*! \brief
  Builds a Java parma_polyhedra_library::Coefficient
  from C++ Coefficient \p ppl_coeff.
*/
jobject
build_java_coeff(JNIEnv* env, const Coefficient& ppl_coeff);

/*! \brief
  Builds a C++ Constraint
  from Java parma_polyhedra_library::Constraint \p j_constraint.
*/
Constraint
build_cxx_constraint(JNIEnv* env, const jobject& j_constraint);

/*! \brief
  Builds a C++ Linear_Expression
  from Java parma_polyhedra_library::Linear_Expression \p j_le.
*/
Linear_Expression
build_cxx_linear_expression(JNIEnv* env, const jobject& j_le);

/*! \brief
  Builds a C++ Congruence
  from Java parma_polyhedra_library::Congruence \p j_cg.
*/
Congruence
build_cxx_congruence(JNIEnv* env, const jobject& j_cg);

/*! \brief
  Builds a C++ Generator
  from Java parma_polyhedra_library::Generator \p j_g.
*/
Generator
build_cxx_generator(JNIEnv* env, const jobject& j_g);

/*! \brief
  Builds a C++ Grid_Generator
  from Java parma_polyhedra_library::Grid_Generator \p j_g.
*/
Grid_Generator
build_cxx_grid_generator(JNIEnv* env, const jobject& j_g);

/*! \brief
  Builds a C++ Grid_Generator_System
  from Java parma_polyhedra_library::Grid_Generator_System \p j_gs.
*/
Grid_Generator_System
build_cxx_grid_generator_system(JNIEnv* env, const jobject& j_gs);

/*! \brief
  Builds a C++ Constraint_System
  from Java parma_polyhedra_library::Constraint_System \p j_cs.
*/
Constraint_System
build_cxx_constraint_system(JNIEnv* env, const jobject& j_cs);

/*! \brief
  Builds a C++ Generator_System
  from Java parma_polyhedra_library::Generator_System \p j_gs.
*/
Generator_System
build_cxx_generator_system(JNIEnv* env, const jobject& j_gs);

/*! \brief
  Builds a C++ Congruence_System
  from Java parma_polyhedra_library::Congruence_System \p j_cgs.
*/
Congruence_System
build_cxx_congruence_system(JNIEnv* env, const jobject& j_cgs);

/*! \brief
  Builds a Java parma_polyhedra_library::Constraint
  from C++ Constraint \p c.
*/
jobject
build_java_constraint(JNIEnv* env, const Constraint& c);

/*! \brief
  Builds a Java parma_polyhedra_library::Congruence
  from C++ Congruence \p cg.
*/
jobject
build_java_congruence(JNIEnv* env, const Congruence& cg);

/*! \brief
  Builds a Java parma_polyhedra_library::Generator
  from C++ Generator \p g.
*/
jobject
build_java_generator(JNIEnv* env, const Generator& g);

/*! \brief
  Builds a Java parma_polyhedra_library::Grid_Generator
  from C++ Grid_Generator \p g.
*/
jobject
build_java_grid_generator(JNIEnv* env, const Grid_Generator& g);

/*! \brief
  Builds a Java parma_polyhedra_library::Constraint_System
  from C++ Constraint_System \p cs.
*/
jobject
build_java_constraint_system(JNIEnv* env, const Constraint_System& cs);

/*! \brief
  Builds a Java parma_polyhedra_library::Grid_Generator_System
  from C++ Grid_Generator_System \p gs.
*/
jobject
build_java_grid_generator_system(JNIEnv* env,
                                 const Grid_Generator_System& gs);

/*! \brief
  Builds a Java parma_polyhedra_library::Generator_System
  from C++ Generator_System \p gs.
*/
jobject
build_java_generator_system(JNIEnv* env, const Generator_System& gs);

/*! \brief
  Builds a Java parma_polyhedra_library::Congruence_System
  from C++ Congruence_System \p cgs.
*/
jobject
build_java_congruence_system(JNIEnv* env, const Congruence_System& cgs);

/*! \brief
  Builds a Java parma_polyhedra_library::Linear_Expression_Coefficient
  from C++ Coefficient \p coeff.
*/
jobject
build_java_linear_expression_coefficient(JNIEnv* env,
                                         const Coefficient& coeff);

/*! \brief
  Sets Java reference \p to_be_set
  to the parma_polyhedra_library::Generator object \p gen.
*/
void
set_generator(JNIEnv* env, jobject& to_be_set,
              const jobject& gen);

/*! \brief
  Sets Java reference \p to_be_set
  to the parma_polyhedra_library::Grid_Generator object \p g_gen.
*/
void
set_grid_generator(JNIEnv* env, jobject& to_be_set,
                   const jobject& g_gen);

/*! \brief
  Sets Java reference \p to_be_set
  to the parma_polyhedra_library::Coefficient object \p c.
*/
void
set_coefficient(JNIEnv* env, jobject& to_be_set,
                const jobject& c);

/*! \brief
  Modifies parma_polyhedra_library::By_Reference object \p by_ref_to_be_set
  so that it references object \p to_insert.
*/
void
set_by_reference(JNIEnv* env, jobject& by_ref_to_be_set,
                 const jobject& to_insert);

/*! \brief
  Returns the object referenced by
  parma_polyhedra_library::By_Reference object \p by_reference.
*/
jobject
get_by_reference(JNIEnv* env, const jobject& by_reference);


/*! \brief
  Assigns \p obj_to_insert to one of the fields of
  parma_polyhedra_library::Pair object \p pair_to_be_set.

  If \p arg is 0, the first element of \p pair_to_be_set is overwritten;
  if \p arg is 1, the second element of \p pair_to_be_set is overwritten.

  \exception std::runtime_error
  Thrown if \p arg is neither 0 nor 1.
*/
void
set_pair_element(JNIEnv* env, jobject& pair_to_be_set, int arg,
                 const jobject& obj_to_insert);

/*! \brief
  Returns one of the fields of the
  parma_polyhedra_library::Pair object \p pair.

  If \p arg is 0, the first element of \p pair is returned;
  if \p arg is 1, the second element of \p pair is returned.

  \exception std::runtime_error
  Thrown if \p arg is neither 0 nor 1.
*/
jobject get_pair_element(JNIEnv* env, int arg, const jobject& pair);

//! Returns \c true if and only if \p obj is a null Java reference.
jboolean is_null(JNIEnv* env, jobject obj);

//! Returns a pointer to the C++ object wrapped by \p ppl_object.
void*
get_ptr(JNIEnv* env, const jobject& ppl_object);

//! Sets the pointer of the underlying C++ object in the Java object.
template <typename T>
void
set_ptr(JNIEnv* env, const jobject& ppl_object,
	const T* address, bool to_be_marked = false);

/*! \brief
  Builds a Java parma_polyhedra_library::Linar_Expression
  from the C++ constraint/congruence \p r.
*/
template <typename R>
jobject
build_linear_expression(JNIEnv* env, const R& r);

//! A C++ wrapper for Java parma_polyhedra_library::Partial_Function.
class Partial_Function {
public:
  //! Constructor.
  Partial_Function(jobject j_p_func, JNIEnv* env);

  /*! \brief
    Returns \c true if and only if the partial function has
    an empty codomain (i.e., it is always undefined).
  */
  bool has_empty_codomain() const;

  /*! \brief
    Returns the maximum value that belongs to the codomain
    of the partial function.
  */
  dimension_type max_in_codomain() const;

  /*! \brief
    Sets \p j to the value (if any) of the partial function on index \p i.
  */
  bool maps(dimension_type i, dimension_type& j) const;

private:
  //! The Java parma_polyhedra_library::Partial_Function object.
  jobject j_p_func;

  //! The JNI environment pointer.
  JNIEnv* env;
};

} // namespace Java

} // namespace Interfaces

} // namespace Parma_Polyhedra_Library


#include "ppl_java_common.inlines.hh"

#endif // !defined(PPL_ppl_prolog_common_defs_hh)
