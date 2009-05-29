m4_dnl  -*- C++ -*-
m4_define(`dnl', `m4_dnl')
m4_divert(-1)
This m4 file contains the code for generating ppl_java_<CLASS_NAME>.cc

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
site: http://www.cs.unipr.it/ppl/ .

FIXME: Find a way to avoid having these dummy macros.
No code is needed for these procedure schemas in the Java interface
as the tokens argument for widening and extrapolation is optional.

m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens_code', `')
m4_define(`ppl_@CLASS@_widening_assign_with_tokens_code', `')
m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens_code', `')
m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens_code', `')

Define here as empty any known schematic method macros for which
the definition is not yet implemented.

m4_define(`ppl_new_@CLASS@_iterator_from_iterator_code',
`dnl
#include "parma_polyhedra_library_@CLASS@_Iterator.h"

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_1Iterator_build_1cpp_1object
(JNIEnv* env, jobject j_this, jobject j_y) try {
  @CPP_CLASS@::iterator* y_ptr
    = reinterpret_cast<@CPP_CLASS@::iterator*>(get_ptr(env, j_y));
  @CPP_CLASS@::iterator* this_ptr
    = new @CPP_CLASS@::iterator(*y_ptr);
  set_ptr(env, j_this, this_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_iterator_equals_iterator_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_1Iterator_equals
(JNIEnv* env, jobject j_this, jobject j_y) {
  try {
    @CPP_CLASS@::iterator* this_ptr
      = reinterpret_cast<@CPP_CLASS@::iterator*>(get_ptr(env, j_this));
    @CPP_CLASS@::iterator* y_ptr
      = reinterpret_cast<@CPP_CLASS@::iterator*>(get_ptr(env, j_y));
    return *this_ptr == *y_ptr;
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@BEGINEND@_iterator_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_@BEGINEND@_1iterator
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    jclass j_it_class
      = env->FindClass("parma_polyhedra_library/@TOPOLOGY@@CLASS@_Iterator");
    jmethodID j_it_ctr_id = env->GetMethodID(j_it_class, "<init>", "()V");
    jobject j_it = env->NewObject(j_it_class, j_it_ctr_id);
    @TOPOLOGY@@CPP_CLASS@::iterator* ppl_it
      = new @TOPOLOGY@@CPP_CLASS@::iterator(this_ptr->@BEGINEND@());
    set_ptr(env, j_it, ppl_it);
    return j_it;
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

')

m4_define(`ppl_delete_@CLASS@_iterator_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_1Iterator_free
(JNIEnv* env, jobject j_this) try {
  if (!is_java_marked(env, j_this)) {
    @CPP_CLASS@::iterator* this_ptr
      = reinterpret_cast<@CPP_CLASS@::iterator*>(get_ptr(env, j_this));
    delete this_ptr;
    void* null_ptr = 0;
    set_ptr(env, j_this, null_ptr);
  }
}
CATCH_ALL

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_1Iterator_finalize
(JNIEnv* env, jobject j_this) try {
  if (!is_java_marked(env, j_this)) {
    @CPP_CLASS@::iterator* this_ptr
      = reinterpret_cast<@CPP_CLASS@::iterator*>(get_ptr(env, j_this));
    delete this_ptr;
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@INCDEC@_iterator_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_1Iterator_@A_INCDEC@
(JNIEnv* env, jobject j_this) try {
  @TOPOLOGY@@CPP_CLASS@::iterator* this_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@::iterator*>(get_ptr(env, j_this));
  m4_@INCDEC@_cpp_name()(*this_ptr);
}
CATCH_ALL

')

m4_define(`m4_increment_cpp_name', `++')
m4_define(`m4_decrement_cpp_name', `--')

m4_define(`ppl_@CLASS@_get_disjunct_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_1Iterator_get_1disjunct
(JNIEnv* env, jobject j_this) {
  try {
    @TOPOLOGY@@CPP_CLASS@::iterator* this_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@::iterator*>(get_ptr(env, j_this));
    jclass j_class = env->FindClass("parma_polyhedra_library/@DISJUNCT_TOPOLOGY@@DISJUNCT@");
    jmethodID j_ctr_id = env->GetMethodID(j_class, "<init>", "()V");
    jobject j_obj = env->NewObject(j_class, j_ctr_id);
    set_ptr(env, j_obj,  &((*this_ptr)->element()), true);
    return j_obj;
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

')

m4_define(`ppl_@CLASS@_drop_disjunct_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_drop_1disjunct
(JNIEnv* env, jobject j_this, jobject j_it) try {
  @TOPOLOGY@@CPP_CLASS@* this_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
  @TOPOLOGY@@CPP_CLASS@::iterator& itr
    = *(reinterpret_cast<@TOPOLOGY@@CPP_CLASS@::iterator*>(get_ptr(env, j_it)));
  itr = this_ptr->drop_disjunct(itr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_drop_disjuncts_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_drop_1disjuncts
(JNIEnv* env, jobject j_this, jobject j_first, jobject j_last) try {
  @TOPOLOGY@@CPP_CLASS@::iterator* first_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@::iterator*>(get_ptr(env, j_first));
  @TOPOLOGY@@CPP_CLASS@::iterator* last_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@::iterator*>(get_ptr(env, j_last));
  @TOPOLOGY@@CPP_CLASS@* this_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
  this_ptr->drop_disjuncts(*first_ptr, *last_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_add_disjunct_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_add_1disjunct
(JNIEnv* env, jobject j_this, jobject j_d) try {
  @TOPOLOGY@@CPP_CLASS@* this_ptr
    = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
  @DISJUNCT_TOPOLOGY@@A_DISJUNCT@* d_ptr
    = reinterpret_cast<@DISJUNCT_TOPOLOGY@@A_DISJUNCT@*>(get_ptr(env, j_d));
  this_ptr->add_disjunct(*d_ptr);
}
CATCH_ALL

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_build_1cpp_1object__JLparma_1polyhedra_1library_Degenerate_1Element_2
(JNIEnv* env, jobject j_this, jlong j_dim, jobject j_degenerate_element) try {
  dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(j_dim);
  jclass degenerate_element_class
    = env->FindClass("parma_polyhedra_library/Degenerate_Element");
  jmethodID degenerate_element_ordinal_id
    = env->GetMethodID(degenerate_element_class, "ordinal", "()I");
  jint j_degenerate_element_int
    = env->CallIntMethod(j_degenerate_element, degenerate_element_ordinal_id);
  @TOPOLOGY@@CPP_CLASS@* this_ptr;
  switch (j_degenerate_element_int) {
  case 0:
    this_ptr = new @TOPOLOGY@@CPP_CLASS@(ppl_dim, UNIVERSE);
    break;
  case 1:
    this_ptr = new @TOPOLOGY@@CPP_CLASS@(ppl_dim, EMPTY);
    break;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
  set_ptr(env, j_this, this_ptr);
}
CATCH_ALL

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@FRIEND@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_build_1cpp_1object__Lparma_1polyhedra_1library_@1FRIEND@_2
(JNIEnv* env, jobject  j_this, jobject j_y) try {
  @B_FRIEND@* y_ptr
    = reinterpret_cast<@B_FRIEND@*>(get_ptr(env, j_y));
  @TOPOLOGY@@CPP_CLASS@* this_ptr = new @TOPOLOGY@@CPP_CLASS@(*y_ptr);
  set_ptr(env, j_this, this_ptr);
}
CATCH_ALL

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@FRIEND@_with_complexity_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_build_1cpp_1object__Lparma_1polyhedra_1library_@1FRIEND@_2Lparma_1polyhedra_1library_Complexity_1Class_2
(JNIEnv* env, jobject j_this, jobject j_y, jobject j_complexity) try {
  @B_FRIEND@* y_ptr
    = reinterpret_cast<@B_FRIEND@*>(get_ptr(env, j_y));
  jclass complexity_class
    = env->FindClass("parma_polyhedra_library/Complexity_Class");
  jmethodID complexity_ordinal_id
    = env->GetMethodID(complexity_class, "ordinal", "()I");
  jint j_complexity_int
    = env->CallIntMethod(j_complexity, complexity_ordinal_id);
  @TOPOLOGY@@CPP_CLASS@* this_ptr;
  switch (j_complexity_int) {
  case 0:
    this_ptr = new @TOPOLOGY@@CPP_CLASS@(*y_ptr, POLYNOMIAL_COMPLEXITY);
    break;
  case 1:
    this_ptr = new @TOPOLOGY@@CPP_CLASS@(*y_ptr, SIMPLEX_COMPLEXITY);
    break;
  case 2:
    this_ptr = new @TOPOLOGY@@CPP_CLASS@(*y_ptr, ANY_COMPLEXITY);
    break;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
  set_ptr(env, j_this, this_ptr);
}
CATCH_ALL

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_build_1cpp_1object__Lparma_1polyhedra_1library_@1!BUILD_REPRESENT@_1System_2
(JNIEnv* env, jobject j_this, jobject j_iterable) try {
  @!BUILD_REPRESENT@_System cs
    = build_cxx_@BUILD_REPRESENT@_system(env, j_iterable);
  @TOPOLOGY@@CPP_CLASS@* this_ptr = new @TOPOLOGY@@CPP_CLASS@(cs@RECYCLE@);
  set_ptr(env, j_this, this_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@UB_EXACT@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_@1UB_EXACT@
(JNIEnv* env, jobject j_this, jobject j_y) {
  try {
    @TOPOLOGY@@CPP_CLASS@* this_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
    @TOPOLOGY@@CPP_CLASS@* y_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_y));
    return this_ptr->@UB_EXACT@(*y_ptr);
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_delete_@CLASS@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_finalize
(JNIEnv* env, jobject j_this) try {
  if (!is_java_marked(env, j_this)) {
    @TOPOLOGY@@CPP_CLASS@* this_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
    delete this_ptr;
  }
}
CATCH_ALL

')

m4_define(`ppl_free_@CLASS@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_free
(JNIEnv* env, jobject j_this) try {
  if (!is_java_marked(env, j_this)) {
    @TOPOLOGY@@CPP_CLASS@* this_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_this));
    delete this_ptr;
    void* null_ptr = 0;
    set_ptr(env, j_this, null_ptr);
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_swap_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_swap
(JNIEnv* env, jobject j_this, jobject j_y) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  this_ptr->swap(*y_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@DIMENSION@_code',
`dnl
JNIEXPORT jlong JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1DIMENSION@
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return this_ptr->@DIMENSION@();
  }
  CATCH_ALL;
  return 0;
}

')

m4_define(`ppl_@CLASS@_get_@CLASS_REPRESENT@s_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1CLASS_REPRESENT@s
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return build_java_@CLASS_REPRESENT@_system(env, this_ptr->@CLASS_REPRESENT@s());
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

')

m4_define(`ppl_@CLASS@_get_minimized_@CLASS_REPRESENT@s_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_minimized_1@1CLASS_REPRESENT@s
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return build_java_@CLASS_REPRESENT@_system(env,
             this_ptr->minimized_@CLASS_REPRESENT@s());
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

')

m4_define(`ppl_@CLASS@_relation_with_@RELATION_REPRESENT@_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_relation_1with__Lparma_1polyhedra_1library_@1!RELATION_REPRESENT@_2
(JNIEnv* env, jobject j_this, jobject j_c) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    @!RELATION_REPRESENT@ c = build_cxx_@RELATION_REPRESENT@(env, j_c);
    Poly_@!A_RELATION_REPRESENT@_Relation pcr = this_ptr->relation_with(c);
    return build_java_poly_@A_RELATION_REPRESENT@_relation(env, pcr);
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

')

m4_define(`ppl_@CLASS@_@HAS_PROPERTY@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1HAS_PROPERTY@
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return this_ptr->@HAS_PROPERTY@();
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@SIMPLIFY@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1SIMPLIFY@
(JNIEnv* env, jobject j_this) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  this_ptr->@SIMPLIFY@();
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_unconstrain_space_dimension_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_unconstrain_1space_1dimension
(JNIEnv* env, jobject j_this, jobject j_var) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_var);
  this_ptr->unconstrain(v);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_unconstrain_space_dimensions_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_unconstrain_1space_1dimensions
(JNIEnv* env, jobject j_this, jobject j_v_set) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variables_Set v_set = build_cxx_variables_set(env, j_v_set);
  this_ptr->unconstrain(v_set);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_bounds_from_@ABOVEBELOW@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_bounds_1from_1@1ABOVEBELOW@
(JNIEnv* env, jobject j_this, jobject j_le) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    Linear_Expression le = build_cxx_linear_expression(env, j_le);
    return this_ptr->bounds_from_@ABOVEBELOW@(le);
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@MAXMIN@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1MAXMIN@__Lparma_1polyhedra_1library_Linear_1Expression_2Lparma_1polyhedra_1library_Coefficient_2Lparma_1polyhedra_1library_Coefficient_2Lparma_1polyhedra_1library_By_1Reference_2
(JNIEnv* env, jobject j_this, jobject j_le,
 jobject j_num, jobject j_den, jobject j_ref_boolean) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    PPL_DIRTY_TEMP_COEFFICIENT(num);
    PPL_DIRTY_TEMP_COEFFICIENT(den);
    num = build_cxx_coeff(env, j_num);
    den = build_cxx_coeff(env, j_den);
    Linear_Expression le = build_cxx_linear_expression(env, j_le);
    bool b_value;
    if (this_ptr->@MAXMIN@(le, num, den, b_value)) {
      jobject j_num_result = build_java_coeff(env, num);
      jobject j_den_result = build_java_coeff(env, den);
      set_coefficient(env, j_num, j_num_result);
      set_coefficient(env, j_den, j_den_result);
      jobject j_boolean = bool_to_j_boolean(env, b_value);
      set_by_reference(env, j_ref_boolean, j_boolean);
      return true;
    }
    return false;
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@MAXMIN@_with_point_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1MAXMIN@__Lparma_1polyhedra_1library_Linear_1Expression_2Lparma_1polyhedra_1library_Coefficient_2Lparma_1polyhedra_1library_Coefficient_2Lparma_1polyhedra_1library_By_1Reference_2Lparma_1polyhedra_1library_Generator_2
(JNIEnv* env, jobject j_this, jobject j_le,
 jobject j_num, jobject j_den, jobject j_ref_boolean, jobject j_g) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    PPL_DIRTY_TEMP_COEFFICIENT(num);
    PPL_DIRTY_TEMP_COEFFICIENT(den);
    num = build_cxx_coeff(env, j_num);
    den = build_cxx_coeff(env, j_den);
    Linear_Expression le = build_cxx_linear_expression(env, j_le);
    bool b_value;
    Generator g = point();
    if (this_ptr->@MAXMIN@(le, num, den, b_value, g)) {
      jobject j_num_result = build_java_coeff(env, num);
      jobject j_den_result = build_java_coeff(env, den);
      jobject j_g_result = build_java_generator(env, g);
      set_coefficient(env, j_num, j_num_result);
      set_coefficient(env, j_den, j_den_result);
      jobject j_boolean = bool_to_j_boolean(env, b_value);
      set_by_reference(env, j_ref_boolean, j_boolean);
      set_generator(env, j_g, j_g_result);
      return true;
    }
    return false;
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@COMPARISON@_@CLASS@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1COMPARISON@
(JNIEnv* env, jobject j_this, jobject j_y) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    @CPP_CLASS@* y_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
    return this_ptr->@COMPARISON@(*y_ptr);
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_equals_@CLASS@_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_equals
(JNIEnv* env, jobject j_this, jobject j_y) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    @CPP_CLASS@* y_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
    return *this_ptr == *y_ptr;
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_hashcode_code',
`dnl
JNIEXPORT jint JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_hashCode
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return this_ptr->hash_code();
  }
  CATCH_ALL;
  return 0;
}

')


m4_define(`ppl_@CLASS@_OK_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_OK
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return this_ptr->OK();
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_add_@CLASS_REPRESENT@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_add_1@1CLASS_REPRESENT@
(JNIEnv* env, jobject j_this, jobject j_c) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @!CLASS_REPRESENT@ c = build_cxx_@CLASS_REPRESENT@(env, j_c);
  this_ptr->add_@CLASS_REPRESENT@(c);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_refine_with_@REFINE_REPRESENT@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_refine_1with_1@1REFINE_REPRESENT@
(JNIEnv* env, jobject j_this, jobject j_c) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @!REFINE_REPRESENT@ c = build_cxx_@REFINE_REPRESENT@(env, j_c);
  this_ptr->refine_with_@REFINE_REPRESENT@(c);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_add_@CLASS_REPRESENT@s_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_add_1@1CLASS_REPRESENT@s
(JNIEnv* env, jobject j_this, jobject j_cs) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @!CLASS_REPRESENT@_System cs = build_cxx_@CLASS_REPRESENT@_system(env, j_cs);
  this_ptr->add_@CLASS_REPRESENT@s(cs);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_refine_with_@REFINE_REPRESENT@s_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_refine_1with_1@1REFINE_REPRESENT@s
(JNIEnv* env, jobject j_this, jobject j_cs) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @!REFINE_REPRESENT@_System cs
    = build_cxx_@REFINE_REPRESENT@_system(env, j_cs);
  this_ptr->refine_with_@REFINE_REPRESENT@s(cs);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@BINOP@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1BINOP@
(JNIEnv* env, jobject j_this, jobject j_y) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  this_ptr->@BINOP@(*y_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_simplify_using_context_assign_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_simplify_1using_1context_1assign
(JNIEnv* env, jobject j_this, jobject j_y) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    @CPP_CLASS@* y_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
    return this_ptr->simplify_using_context_assign(*y_ptr);
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_@AFFIMAGE@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1AFFIMAGE@
(JNIEnv* env, jobject j_this, jobject j_v, jobject j_le, jobject j_c) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_v);
  Linear_Expression le = build_cxx_linear_expression(env, j_le);
  PPL_DIRTY_TEMP_COEFFICIENT(c);
  c = build_cxx_coeff(env, j_c);
  this_ptr->@AFFIMAGE@(v, le, c);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_generalized_1@1AFFIMAGE@__Lparma_1polyhedra_1library_Linear_1Expression_2Lparma_1polyhedra_1library_Relation_1Symbol_2Lparma_1polyhedra_1library_Linear_1Expression_2
(JNIEnv* env, jobject j_this,
 jobject j_lhs, jobject j_relsym, jobject j_rhs) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Linear_Expression lhs = build_cxx_linear_expression(env, j_lhs);
  Linear_Expression rhs = build_cxx_linear_expression(env, j_rhs);
  Relation_Symbol relsym = build_cxx_relsym(env, j_relsym);
  this_ptr->generalized_@AFFIMAGE@(lhs, relsym, rhs);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_generalized_1@1AFFIMAGE@__Lparma_1polyhedra_1library_Variable_2Lparma_1polyhedra_1library_Relation_1Symbol_2Lparma_1polyhedra_1library_Linear_1Expression_2Lparma_1polyhedra_1library_Coefficient_2
(JNIEnv* env, jobject j_this,
 jobject j_v, jobject j_relsym, jobject j_le, jobject j_c) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_v);
  Relation_Symbol relsym = build_cxx_relsym(env, j_relsym);
  Linear_Expression le = build_cxx_linear_expression(env, j_le);
  PPL_DIRTY_TEMP_COEFFICIENT(c);
  c = build_cxx_coeff(env, j_c);
  this_ptr->generalized_@AFFIMAGE@(v, relsym, le, c);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs_with_congruence_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_generalized_1@1AFFIMAGE@_1lhs_1rhs_1with_1congruence
(JNIEnv* env, jobject j_this,
 jobject j_lhs, jobject j_relsym, jobject j_rhs, jobject j_modulus) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Linear_Expression lhs = build_cxx_linear_expression(env, j_lhs);
  Linear_Expression rhs = build_cxx_linear_expression(env, j_rhs);
  Relation_Symbol relsym = build_cxx_relsym(env, j_relsym);
  PPL_DIRTY_TEMP_COEFFICIENT(modulus);
  modulus = build_cxx_coeff(env, j_modulus);
  this_ptr->generalized_@AFFIMAGE@(lhs, relsym, rhs, modulus);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_with_congruence_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@CLASS@_generalized_1@1AFFIMAGE@_1with_1congruence
(JNIEnv* env, jobject j_this,
 jobject j_v, jobject j_relsym, jobject j_le, jobject j_c,
 jobject j_modulus) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_v);
  Relation_Symbol relsym = build_cxx_relsym(env, j_relsym);
  Linear_Expression le = build_cxx_linear_expression(env, j_le);
  PPL_DIRTY_TEMP_COEFFICIENT(c);
  PPL_DIRTY_TEMP_COEFFICIENT(modulus);
  c = build_cxx_coeff(env, j_c);
  modulus = build_cxx_coeff(env, j_modulus);
  this_ptr->generalized_@AFFIMAGE@(v, relsym, le, c, modulus);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_bounded_@AFFIMAGE@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_bounded_1@1AFFIMAGE@
(JNIEnv* env, jobject j_this,
 jobject j_v, jobject j_lhs, jobject j_rhs, jobject j_c) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_v);
  Linear_Expression lhs = build_cxx_linear_expression(env, j_lhs);
  Linear_Expression rhs = build_cxx_linear_expression(env, j_rhs);
  PPL_DIRTY_TEMP_COEFFICIENT(c);
  c = build_cxx_coeff(env, j_c);
  this_ptr->bounded_@AFFIMAGE@(v, lhs, rhs, c);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1WIDEN@_1widening_1assign
(JNIEnv* env, jobject j_this, jobject j_y, jobject j_by_ref_int) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  if (is_null(env, j_by_ref_int))
    this_ptr->@WIDEN@_widening_assign(*y_ptr);
  else {
    jobject j_integer = get_by_reference(env, j_by_ref_int);
    unsigned int tokens =
      jtype_to_unsigned<unsigned int>(j_integer_to_j_int(env, j_integer));
    this_ptr->@WIDEN@_widening_assign(*y_ptr, &tokens);
    j_integer = j_int_to_j_integer(env, tokens);
    set_by_reference(env, j_by_ref_int, j_integer);
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_widening_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_widening_1assign
(JNIEnv* env, jobject j_this, jobject j_y, jobject j_by_ref_int) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  if (is_null(env, j_by_ref_int))
    this_ptr->widening_assign(*y_ptr);
  else {
    jobject j_integer = get_by_reference(env, j_by_ref_int);
    unsigned int tokens =
      jtype_to_unsigned<unsigned int>(j_integer_to_j_int(env, j_integer));
    this_ptr->widening_assign(*y_ptr, &tokens);
    j_integer = j_int_to_j_integer(env, tokens);
    set_by_reference(env, j_by_ref_int, j_integer);
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1EXTRAPOLATION@_1extrapolation_1assign
(JNIEnv* env, jobject j_this, jobject j_y, jobject j_by_ref_int) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  if (is_null(env, j_by_ref_int))
    this_ptr->@EXTRAPOLATION@_extrapolation_assign(*y_ptr);
  else {
    jobject j_integer = get_by_reference(env, j_by_ref_int);
    unsigned int tokens =
      jtype_to_unsigned<unsigned int>(j_integer_to_j_int(env, j_integer));
    this_ptr->@EXTRAPOLATION@_extrapolation_assign(*y_ptr, &tokens);
    j_integer = j_int_to_j_integer(env, tokens);
    set_by_reference(env, j_by_ref_int, j_integer);
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1EXTRAPOLATION@_1narrowing_1assign
(JNIEnv* env, jobject j_this, jobject j_y) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  this_ptr->@EXTRAPOLATION@_narrowing_assign(*y_ptr);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1LIMITEDBOUNDED@_1@1WIDENEXPN@_1extrapolation_1assign
(JNIEnv* env, jobject j_this, jobject j_y,
 jobject j_cs, jobject j_by_ref_int) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  @!CONSTRAINER@_System cs = build_cxx_@CONSTRAINER@_system(env, j_cs);
  if (is_null(env, j_by_ref_int))
    this_ptr->@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(*y_ptr, cs);
  else {
    jobject j_integer = get_by_reference(env, j_by_ref_int);
    unsigned int tokens =
      jtype_to_unsigned<unsigned int>(j_integer_to_j_int(env, j_integer));
    this_ptr->@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign
      (*y_ptr, cs, &tokens);
    j_integer = j_int_to_j_integer(env, tokens);
    set_by_reference(env, j_by_ref_int, j_integer);
  }
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_BGP99_1@1DISJUNCT_WIDEN@_1extrapolation_1assign
(JNIEnv* env, jobject j_this, jobject j_y, jobject j_disjuncts) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  unsigned int disjuncts =
    jtype_to_unsigned<unsigned int>(j_integer_to_j_int(env, j_disjuncts));
  this_ptr->BGP99_extrapolation_assign
    (*y_ptr,
     widen_fun_ref(&@DISJUNCT_TOPOLOGY@@A_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign),
     disjuncts);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_BHZ03_@A_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_BHZ03_1@1A_DISJUNCT_WIDEN@_1@1DISJUNCT_WIDEN@_1widening_1assign
(JNIEnv* env, jobject j_this, jobject j_y) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  @CPP_CLASS@* y_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_y));
  this_ptr->BHZ03_widening_assign<@A_DISJUNCT_WIDEN@_Certificate>
    (*y_ptr,
     widen_fun_ref(
       &@DISJUNCT_TOPOLOGY@@A_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign));
}
CATCH_ALL

')


m4_define(`ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_add_1space_1dimensions_1@1EMBEDPROJECT@
(JNIEnv* env, jobject j_this, jlong dim) try {
  dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(dim);
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  this_ptr->add_space_dimensions_@EMBEDPROJECT@(ppl_dim);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_remove_space_dimensions_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_remove_1space_1dimensions
(JNIEnv* env, jobject j_this, jobject j_v_set) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variables_Set v_set = build_cxx_variables_set(env, j_v_set);
  this_ptr->remove_space_dimensions(v_set);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_remove_higher_space_dimensions_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_remove_1higher_1space_1dimensions
(JNIEnv* env, jobject j_this, jlong dim) try {
  dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(dim);
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  this_ptr->remove_higher_space_dimensions(ppl_dim);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_expand_space_dimension_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_expand_1space_1dimension
(JNIEnv* env, jobject j_this, jobject j_variable, jlong dim) try {
  dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(dim);
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_variable);
  this_ptr->expand_space_dimension(v, ppl_dim);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_fold_space_dimensions_code',
`dnl
JNIEXPORT void JNICALL Java_parma_1polyhedra_1library_@1CLASS@_fold_1space_1dimensions
(JNIEnv* env, jobject j_this, jobject j_v_set, jobject j_var) try {
  Variables_Set v_set = build_cxx_variables_set(env, j_v_set);
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Variable v = build_cxx_variable(env, j_var);
  this_ptr->fold_space_dimensions(v_set, v);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_map_space_dimensions_code',
`dnl
JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_map_1space_1dimensions
(JNIEnv* env, jobject j_this, jobject j_p_func) try {
  @CPP_CLASS@* this_ptr
    = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
  Partial_Function ppl_pfunc(j_p_func, env);
  this_ptr->map_space_dimensions(ppl_pfunc);
}
CATCH_ALL

')

m4_define(`ppl_@CLASS@_string_code',
`dnl
JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_toString
(JNIEnv* env, jobject j_this) {
  try {
    using namespace Parma_Polyhedra_Library::IO_Operators;
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    std::ostringstream s;
    s << *this_ptr;
    std::string str = s.str();
    return env->NewStringUTF(str.c_str());
  }
  CATCH_ALL;
  return 0;
}

')

m4_define(`ppl_@CLASS@_@MEMBYTES@_code',
`dnl
JNIEXPORT jlong JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_@1MEMBYTES@
(JNIEnv* env, jobject j_this) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    return this_ptr->@MEMBYTES@();
  }
  CATCH_ALL;
  return 0;
}

')

m4_define(`ppl_@CLASS@_constrains_code',
`dnl
JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_constrains
(JNIEnv* env, jobject j_this, jobject j_var) {
  try {
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    Variable v = build_cxx_variable(env, j_var);
    return this_ptr->constrains(v);
  }
  CATCH_ALL;
  return false;
}

')

m4_define(`ppl_@CLASS@_ascii_dump_code',
`dnl
JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    using namespace Parma_Polyhedra_Library::IO_Operators;
    @CPP_CLASS@* this_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_this));
    std::ostringstream s;
    this_ptr->ascii_dump(s);
    std::string str = s.str();
    return env->NewStringUTF(str.c_str());
  }
  CATCH_ALL;
  return 0;
}

')

m4_define(`ppl_@CLASS@_linear_@PARTITION@_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1TOPOLOGY@@1CLASS@_linear_1@PARTITION@
(JNIEnv* env, jclass, jobject j_p, jobject j_q) {
  try {
    @TOPOLOGY@@CPP_CLASS@* p_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_p));
    @TOPOLOGY@@CPP_CLASS@* q_ptr
      = reinterpret_cast<@TOPOLOGY@@CPP_CLASS@*>(get_ptr(env, j_q));
    std::pair<@TOPOLOGY@@CPP_CLASS@|COMMA|
              Pointset_Powerset<NNC_Polyhedron> >
      r = linear_partition(*p_ptr, *q_ptr);

    @TOPOLOGY@@CPP_CLASS@* r1
      = new @TOPOLOGY@@CPP_CLASS@(0, EMPTY);
    Pointset_Powerset<NNC_Polyhedron>* r2
      = new Pointset_Powerset<NNC_Polyhedron>(0, EMPTY);
    r1->swap(r.first);
    r2->swap(r.second);

    jclass j_pair_class = env->FindClass("parma_polyhedra_library/Pair");
    jmethodID j_ctr_id_pair = env->GetMethodID(j_pair_class, "<init>", "()V");
    jobject j_pair_obj = env->NewObject(j_pair_class, j_ctr_id_pair);

    jclass j_class_r1 = env->FindClass("parma_polyhedra_library/@TOPOLOGY@@CLASS@");
    jmethodID j_ctr_id_r1 = env->GetMethodID(j_class_r1, "<init>", "()V");
    jobject j_obj_r1 = env->NewObject(j_class_r1, j_ctr_id_r1);
    set_ptr(env, j_obj_r1, r1);

    jclass j_class_r2 = env->FindClass("parma_polyhedra_library/Pointset_Powerset_NNC_Polyhedron");
    jmethodID j_ctr_id_r2 = env->GetMethodID(j_class_r2, "<init>", "()V");
    jobject j_obj_r2 = env->NewObject(j_class_r2, j_ctr_id_r2);
    set_ptr(env, j_obj_r2, r2);
    set_pair_element(env, j_pair_obj, 0, j_obj_r1);
    set_pair_element(env, j_pair_obj, 1, j_obj_r2);
    return j_pair_obj;
  }
  CATCH_ALL;
  return 0;
}

')

m4_define(`ppl_@CLASS@_approximate_@PARTITION@_code',
`dnl
JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_@1CLASS@_approximate_1@PARTITION@
(JNIEnv* env, jclass, jobject j_p, jobject j_q, jobject j_ref_finite_bool) {
  try {
    @CPP_CLASS@* p_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_p));
    @CPP_CLASS@* q_ptr
      = reinterpret_cast<@CPP_CLASS@*>(get_ptr(env, j_q));
    bool b_finite_val;
    std::pair<@CPP_CLASS@|COMMA| Pointset_Powerset<Grid> >
      r = approximate_partition(*p_ptr, *q_ptr, b_finite_val);

    @CPP_CLASS@* r1 = new @CPP_CLASS@(0, EMPTY);
    Pointset_Powerset<Grid>* r2 = new Pointset_Powerset<Grid>(0, EMPTY);
    r1->swap(r.first);
    r2->swap(r.second);

    jclass j_pair_class
      = env->FindClass("parma_polyhedra_library/Pair");
    jmethodID j_ctr_id_pair = env->GetMethodID(j_pair_class, "<init>", "()V");
    jobject j_pair_obj = env->NewObject(j_pair_class, j_ctr_id_pair);

    jclass j_class_r1 = env->FindClass("parma_polyhedra_library/@CLASS@");
    jmethodID j_ctr_id_r1 = env->GetMethodID(j_class_r1, "<init>", "()V");
    jobject j_obj_r1 = env->NewObject(j_class_r1, j_ctr_id_r1);
    set_ptr(env, j_obj_r1, r1);

    jclass j_class_r2
      = env->FindClass("parma_polyhedra_library/Pointset_Powerset_Grid");
    jmethodID j_ctr_id_r2 = env->GetMethodID(j_class_r2, "<init>", "()V");
    jobject j_obj_r2 = env->NewObject(j_class_r2, j_ctr_id_r2);
    set_ptr(env, j_obj_r2, r2);
    set_pair_element(env, j_pair_obj, 0, j_obj_r1);
    set_pair_element(env, j_pair_obj, 1, j_obj_r2);
    jobject j_finite_bool = bool_to_j_boolean(env, b_finite_val);
    set_by_reference(env, j_ref_finite_bool, j_finite_bool);
    return j_pair_obj;
  }
  CATCH_ALL;
  return 0;
}

')
