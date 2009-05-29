/* Domain-independent part of the Java interface: inline functions.
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

#ifndef PPL_ppl_java_common_inlines_hh
#define PPL_ppl_java_common_inlines_hh 1

#include <cassert>

namespace Parma_Polyhedra_Library {

namespace Interfaces {

namespace Java {

template <typename U, typename V>
U
jtype_to_unsigned(const V& value) {
  if (value < 0)
    throw std::invalid_argument("not an unsigned integer.");

  if (sizeof(U) < sizeof(V)) {
    if (value > static_cast<V>(std::numeric_limits<U>::max()))
      throw std::invalid_argument("unsigned integer out of range.");
  }

  return value;
}

template <typename T>
void
set_ptr(JNIEnv* env, const jobject& ppl_object,
	const T* address, bool to_be_marked) {
  jclass ppl_object_class = env->GetObjectClass(ppl_object);
  jfieldID pointer_field = env->GetFieldID(ppl_object_class, "ptr","J");
  const T* ptr = (to_be_marked ? mark(address) : address);
  jlong pointer_value = reinterpret_cast<jlong>(ptr);
  assert(reinterpret_cast<const T*>(pointer_value) == ptr);
  env->SetLongField(ppl_object, pointer_field, pointer_value);
}

template <typename R>
jobject
build_linear_expression(JNIEnv* env, const R& r) {
  jclass j_le_coeff_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Coefficient");
  jclass j_le_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression");
  jclass j_le_variable_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Variable");
  jclass j_variable_class
    = env->FindClass("parma_polyhedra_library/Variable");
  PPL_DIRTY_TEMP_COEFFICIENT(coefficient);
  dimension_type varid = 0;
  dimension_type space_dimension = r.space_dimension();
  jobject j_le_term;
  jmethodID j_variable_ctr_id
    = env->GetMethodID(j_variable_class,
		       "<init>",
		       "(I)V");
  jmethodID j_le_variable_ctr_id
    = env->GetMethodID(j_le_variable_class,
		       "<init>",
		       "(Lparma_polyhedra_library/Variable;)V");

  jmethodID j_le_times_id
    = env->GetMethodID(j_le_class,
		       "times",
		       "(Lparma_polyhedra_library/Coefficient;)Lparma_polyhedra_library/Linear_Expression;");

  while (varid < space_dimension
 	 && (coefficient = r.coefficient(Variable(varid))) == 0)
    ++varid;
  if (varid >= space_dimension) {
    jobject j_coefficient_zero = build_java_coeff(env, Coefficient(0));
    jmethodID j_le_coeff_ctr_id
      = env->GetMethodID(j_le_coeff_class, "<init>",
			 "(Lparma_polyhedra_library/Coefficient;)V");
    return env->NewObject(j_le_coeff_class, j_le_coeff_ctr_id,
			  j_coefficient_zero);
  }
  else {
    jobject j_coefficient = build_java_coeff(env, coefficient);
    jobject j_variable = env->NewObject(j_variable_class, j_variable_ctr_id,
					varid);
    jobject j_le_variable = env->NewObject(j_le_variable_class,
					   j_le_variable_ctr_id,
					   j_variable);
    j_le_term =  env->CallObjectMethod(j_le_variable,
				       j_le_times_id, j_coefficient);
    while (true) {
      ++varid;
      while (varid < space_dimension
	     && (coefficient = r.coefficient(Variable(varid))) == 0)
	++varid;
      if (varid >= space_dimension)
	break;
      else {
 	j_coefficient = build_java_coeff(env, coefficient);
 	j_variable = env->NewObject(j_variable_class,
				    j_variable_ctr_id,
				    varid);
  	j_le_variable = env->NewObject(j_le_variable_class,
				       j_le_variable_ctr_id,
				       j_variable);
 	jobject j_le_term2 = env->CallObjectMethod(j_le_variable,
						   j_le_times_id,
						   j_coefficient);
  	jmethodID j_le_sum_id
  	  = env->GetMethodID(j_le_class,
  			     "sum",
  			     "(Lparma_polyhedra_library/Linear_Expression;)"
			     "Lparma_polyhedra_library/Linear_Expression;");
 	j_le_term = env->CallObjectMethod(j_le_term, j_le_sum_id, j_le_term2);
      }
    }
  }
  return j_le_term;
}

inline
Partial_Function::Partial_Function(jobject j_p_func, JNIEnv* env)
  : j_p_func(j_p_func),
    env(env) {
}

inline bool
Partial_Function::has_empty_codomain() const {
  jclass j_partial_function_class
    = env->FindClass("parma_polyhedra_library/Partial_Function");
  jmethodID j_has_empty_codomain_id
    = env->GetMethodID(j_partial_function_class,
                       "has_empty_codomain",
                       "()Z");
  return env->CallBooleanMethod(j_p_func, j_has_empty_codomain_id);
}

inline dimension_type
Partial_Function::max_in_codomain() const {
  jclass j_partial_function_class
    = env->FindClass("parma_polyhedra_library/Partial_Function");
  jmethodID j_max_in_codomain_id
    = env->GetMethodID(j_partial_function_class,
                       "max_in_codomain",
                       "()J");
  jlong value = env->CallLongMethod(j_p_func, j_max_in_codomain_id);
  return jtype_to_unsigned<dimension_type>(value);
}

inline bool
Partial_Function::maps(dimension_type i, dimension_type& j) const {
  jclass j_partial_function_class
    = env->FindClass("parma_polyhedra_library/Partial_Function");
  jclass j_by_reference_class
    = env->FindClass("parma_polyhedra_library/By_Reference");
  jmethodID j_by_reference_ctr_id
    = env->GetMethodID(j_by_reference_class,
                       "<init>",
                       "(Ljava/lang/Object;)V");
  jobject coeff = j_long_to_j_long_class(env, 0);
  jobject new_by_ref = env->NewObject(j_by_reference_class,
                                      j_by_reference_ctr_id,
                                      coeff);
  jmethodID j_maps_id = env->GetMethodID(j_partial_function_class,
                                         "maps",
                                         "(Ljava/lang/Long;Lparma_polyhedra_library/By_Reference;)Z");
  if (env->CallBooleanMethod(j_p_func, j_maps_id,
                             j_long_to_j_long_class(env, i),
                             new_by_ref)) {
    jobject long_value = get_by_reference(env, new_by_ref);
    j = jtype_to_unsigned<dimension_type>(j_long_class_to_j_long(env,
                                                                 long_value));
    return true;
  }
  return false;
}

} // namespace Java

} // namespace Interfaces

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_ppl_java_common_inlines_hh)
