/* PPL Java interface: domain-independent functions.
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

#include "parma_polyhedra_library_Parma_Polyhedra_Library.h"
#include "ppl_java_common.defs.hh"
#include "parma_polyhedra_library_MIP_Problem.h"
#include "parma_polyhedra_library_Linear_Expression.h"
#include "parma_polyhedra_library_Constraint.h"
#include "parma_polyhedra_library_Constraint_System.h"
#include "parma_polyhedra_library_Congruence.h"
#include "parma_polyhedra_library_Congruence_System.h"
#include "parma_polyhedra_library_Generator.h"
#include "parma_polyhedra_library_Generator_System.h"
#include "parma_polyhedra_library_Grid_Generator.h"
#include "parma_polyhedra_library_Grid_Generator_System.h"
#include "parma_polyhedra_library_IO.h"

using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::Interfaces::Java;

JNIEXPORT jint JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_version_1major
(JNIEnv *, jclass) {
  return version_major();
}

JNIEXPORT jint JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_version_1minor
(JNIEnv *, jclass)  {
  return version_minor();
}

JNIEXPORT jint JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_version_1revision
(JNIEnv *, jclass) {
  return version_revision();
}

JNIEXPORT jint JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_version_1beta
(JNIEnv *, jclass) {
  return version_beta();
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_version
(JNIEnv* env, jclass) {
  return env->NewStringUTF(version());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_banner
(JNIEnv* env, jclass) {
  return env->NewStringUTF(banner());
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_set_1rounding_1for_1PPL
(JNIEnv* env, jclass) {
  try {
    set_rounding_for_PPL();
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_Parma_1Polyhedra_1Library_restore_1pre_1PPL_1rounding
(JNIEnv* env, jclass) {
  try {
    restore_pre_PPL_rounding();
  }
  CATCH_ALL;
}

JNIEXPORT jlong JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_max_1space_1dimension
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return mip->max_space_dimension();
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jlong JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_space_1dimension
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return mip->space_dimension();
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_integer_1space_1dimensions
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return build_java_variables_set(env, mip->integer_space_dimensions());
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_objective_1function
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    jclass j_le_coeff_class
      = env->FindClass("parma_polyhedra_library/Linear_Expression_Coefficient");
    jclass j_le_class
      = env->FindClass("parma_polyhedra_library/Linear_Expression");
    jmethodID j_le_sum_id
      = env->GetMethodID(j_le_class,
			 "sum",
			 "(Lparma_polyhedra_library/Linear_Expression;)"
			 "Lparma_polyhedra_library/Linear_Expression;");
    jmethodID j_le_coeff_ctr_id
      = env->GetMethodID(j_le_coeff_class, "<init>",
			 "(Lparma_polyhedra_library/Coefficient;)V");

    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    PPL_DIRTY_TEMP_COEFFICIENT(inhomogeneous_term);
    inhomogeneous_term = mip->objective_function().inhomogeneous_term();
    jobject j_coeff_inhomogeneous_term
      = build_java_coeff(env, inhomogeneous_term);
    jobject j_le_coeff = env->NewObject(j_le_coeff_class, j_le_coeff_ctr_id,
					j_coeff_inhomogeneous_term);

    jobject j_le = build_linear_expression(env, mip->objective_function());
    return env->CallObjectMethod(j_le, j_le_sum_id, j_le_coeff);
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_optimization_1mode
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return build_java_optimization_mode(env, mip->optimization_mode());
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_get_1control_1parameter
(JNIEnv* env , jobject j_this_mip_problem,
 jobject j_cpn) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    MIP_Problem::Control_Parameter_Name cpn
      = build_cxx_control_parameter_name(env, j_cpn);
    return
      build_java_control_parameter_value(env,
                                         mip->get_control_parameter(cpn));
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_set_1control_1parameter
(JNIEnv* env , jobject j_this_mip_problem,
 jobject j_cpv) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    MIP_Problem::Control_Parameter_Value cpv
      = build_cxx_control_parameter_value(env, j_cpv);
    mip->set_control_parameter(cpv);
  }
  CATCH_ALL;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_constraints
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    jclass j_cs_class
      = env->FindClass("parma_polyhedra_library/Constraint_System");
    jmethodID j_cs_ctr_id = env->GetMethodID(j_cs_class, "<init>", "()V");
    jmethodID j_cs_add_id = env->GetMethodID(j_cs_class, "add",
					     "(Ljava/lang/Object;)Z");
    jobject j_cs = env->NewObject(j_cs_class, j_cs_ctr_id);

    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    for (MIP_Problem::const_iterator cs_it = mip->constraints_begin(),
	   cs_end = mip->constraints_end(); cs_it != cs_end; ++cs_it) {
      jobject j_constraint = build_java_constraint(env, *cs_it);
      env->CallBooleanMethod(j_cs, j_cs_add_id, j_constraint);
    }
    return j_cs;
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_clear
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    mip->clear();
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_add_1space_1dimensions_1and_1embed
(JNIEnv* env , jobject j_this_mip_problem, jlong j_dim) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(j_dim);
    mip->add_space_dimensions_and_embed(ppl_dim);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_add_1to_1integer_1space_1dimensions
(JNIEnv* env , jobject j_this_mip_problem, jobject j_vset) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Variables_Set v_set = build_cxx_variables_set(env, j_vset);
    mip->add_to_integer_space_dimensions(v_set);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_add_1constraint
(JNIEnv* env , jobject j_this_mip_problem, jobject j_c) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Constraint c = build_cxx_constraint(env, j_c);
    mip->add_constraint(c);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_add_1constraints
(JNIEnv* env , jobject j_this_mip_problem, jobject j_cs) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Constraint_System cs = build_cxx_constraint_system(env, j_cs);
    mip->add_constraints(cs);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_set_1objective_1function
(JNIEnv* env , jobject j_this_mip_problem, jobject j_le) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Linear_Expression le = build_cxx_linear_expression(env, j_le);
    mip->set_objective_function(le);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_set_1optimization_1mode
(JNIEnv* env , jobject j_this_mip_problem, jobject j_opt_mode) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Optimization_Mode opt_mode = build_cxx_optimization_mode(env, j_opt_mode);
    mip->set_optimization_mode(opt_mode);
  }
  CATCH_ALL;
}

JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_is_1satisfiable
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return mip->is_satisfiable();
  }
  CATCH_ALL;
  return false;
}

JNIEXPORT jobject JNICALL Java_parma_1polyhedra_1library_MIP_1Problem_solve
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return build_java_mip_status(env, mip->solve());
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_evaluate_1objective_1function
(JNIEnv* env, jobject j_this_mip_problem, jobject j_gen, jobject j_coeff_num,
 jobject j_coeff_den) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Generator g = build_cxx_generator(env, j_gen);
    PPL_DIRTY_TEMP_COEFFICIENT(num);
    PPL_DIRTY_TEMP_COEFFICIENT(den);
    num = build_cxx_coeff(env, j_coeff_num);
    den = build_cxx_coeff(env, j_coeff_den);
    mip->evaluate_objective_function(g, num, den);
    set_coefficient(env, j_coeff_num, build_java_coeff(env, num));
    set_coefficient(env, j_coeff_den, build_java_coeff(env, den));
  }
  CATCH_ALL;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_feasible_1point
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Generator g = mip->feasible_point();
    return build_java_generator(env, g);
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT jobject JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_optimizing_1point
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    Generator g = mip->optimizing_point();
    return build_java_generator(env, g);
  }
  CATCH_ALL;
  jobject null = 0;
  return null;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_optimal_1value
(JNIEnv* env, jobject j_this_mip_problem, jobject j_coeff_num,
 jobject j_coeff_den) {
  try {
    PPL_DIRTY_TEMP_COEFFICIENT(coeff_num);
    PPL_DIRTY_TEMP_COEFFICIENT(coeff_den);

    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    mip->optimal_value(coeff_num, coeff_den);
    jobject j_coeff_num_result = build_java_coeff(env, coeff_num);
    jobject j_coeff_den_result = build_java_coeff(env, coeff_den);
    set_coefficient(env, j_coeff_num, j_coeff_num_result);
    set_coefficient(env, j_coeff_den, j_coeff_den_result);
  }
  CATCH_ALL;
}

JNIEXPORT jboolean JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_OK
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return mip->OK();
  }
  CATCH_ALL;
  return false;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_build_1cpp_1object__J
(JNIEnv* env, jobject j_this_mip_problem, jlong j_dim) {
  try {
    dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(j_dim);
    MIP_Problem* mip_ptr = new MIP_Problem(ppl_dim);
    set_ptr(env, j_this_mip_problem,  mip_ptr);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_build_1cpp_1object__JLparma_1polyhedra_1library_Constraint_1System_2Lparma_1polyhedra_1library_Linear_1Expression_2Lparma_1polyhedra_1library_Optimization_1Mode_2
(JNIEnv* env , jobject j_this_mip_problem, jlong j_dim, jobject j_cs,
 jobject j_le, jobject j_opt_mode) {
  try {
    dimension_type ppl_dim = jtype_to_unsigned<dimension_type>(j_dim);
    Constraint_System cs = build_cxx_constraint_system(env, j_cs);
    Linear_Expression le = build_cxx_linear_expression(env, j_le);
    Optimization_Mode opt_mode =  build_cxx_optimization_mode(env, j_opt_mode);
    MIP_Problem* mip_ptr = new MIP_Problem(ppl_dim, cs, le, opt_mode);
    set_ptr(env, j_this_mip_problem, mip_ptr);
  }
  CATCH_ALL;
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_build_1cpp_1object__Lparma_1polyhedra_1library_MIP_1Problem_2
(JNIEnv* env, jobject  j_this, jobject j_y)
{
  MIP_Problem* y_ptr = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_y));
  MIP_Problem* this_ptr = new MIP_Problem(*y_ptr);
  set_ptr(env, j_this, this_ptr);
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_free
(JNIEnv* env, jobject j_this) {
  MIP_Problem* mip  = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this));
  if (!is_java_marked(env, j_this)) {
    delete mip;
    void* null_ptr = 0;
    set_ptr(env, j_this, null_ptr);
  }
}

JNIEXPORT void JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_finalize
(JNIEnv* env, jobject j_this) {
  MIP_Problem* mip = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this));
  if (!is_java_marked(env, j_this))
    delete mip;
}

JNIEXPORT jlong JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_total_1memory_1in_1bytes
(JNIEnv* env , jobject j_this_mip_problem) {
  try {
    MIP_Problem* mip
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this_mip_problem));
    return mip->total_memory_in_bytes();
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_toString
(JNIEnv* env, jobject j_this) {
  MIP_Problem* this_ptr
    = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this));
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  s << *this_ptr;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_MIP_1Problem_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    MIP_Problem* this_ptr
      = reinterpret_cast<MIP_Problem*>(get_ptr(env, j_this));
    std::ostringstream s;
    this_ptr->ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Linear_1Expression_toString
(JNIEnv* env, jobject j_this) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  Linear_Expression ppl_le = build_cxx_linear_expression(env, j_this);
  std::ostringstream s;
  s << ppl_le;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Linear_1Expression_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Linear_Expression le = build_cxx_linear_expression(env, j_this);
    le.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Generator_toString
(JNIEnv* env, jobject g) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Generator ppl_g = build_cxx_generator(env, g);
  s << ppl_g;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Generator_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Generator g = build_cxx_generator(env, j_this);
    g.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Constraint_toString
(JNIEnv* env, jobject c) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Constraint ppl_c = build_cxx_constraint(env, c);
  s << ppl_c;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Constraint_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Constraint c = build_cxx_constraint(env, j_this);
    c.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Grid_1Generator_toString
(JNIEnv* env, jobject g) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Grid_Generator ppl_g = build_cxx_grid_generator(env, g);
  s << ppl_g;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Grid_1Generator_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Grid_Generator g = build_cxx_grid_generator(env, j_this);
    g.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Congruence_toString
(JNIEnv* env, jobject g) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Congruence ppl_g = build_cxx_congruence(env, g);
  s << ppl_g;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Congruence_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Congruence c = build_cxx_congruence(env, j_this);
    c.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Grid_1Generator_1System_toString
(JNIEnv* env, jobject ggs) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Grid_Generator_System ppl_ggs = build_cxx_grid_generator_system(env, ggs);
  s << ppl_ggs;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Grid_1Generator_1System_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Grid_Generator_System gs = build_cxx_grid_generator_system(env, j_this);
    gs.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Generator_1System_toString
(JNIEnv* env, jobject gs) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Generator_System ppl_gs = build_cxx_generator_system(env, gs);
  s << ppl_gs;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Generator_1System_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Generator_System gs = build_cxx_generator_system(env, j_this);
    gs.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Constraint_1System_toString
(JNIEnv* env, jobject cs) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Constraint_System ppl_cs = build_cxx_constraint_system(env, cs);
  s << ppl_cs;
  return env->NewStringUTF(s.str().c_str());
}


JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Constraint_1System_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Constraint_System cs = build_cxx_constraint_system(env, j_this);
    cs.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Congruence_1System_toString
(JNIEnv* env, jobject cgs) {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  std::ostringstream s;
  Congruence_System ppl_cgs = build_cxx_congruence_system(env, cgs);
  s << ppl_cgs;
  return env->NewStringUTF(s.str().c_str());
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_Congruence_1System_ascii_1dump
(JNIEnv* env, jobject j_this) {
  try {
    std::ostringstream s;
    Congruence_System cs = build_cxx_congruence_system(env, j_this);
    cs.ascii_dump(s);
    return env->NewStringUTF(s.str().c_str());
  }
  CATCH_ALL;
  return 0;
}

JNIEXPORT jstring JNICALL
Java_parma_1polyhedra_1library_IO_wrap_1string
(JNIEnv* env, jclass, jstring str, jint indent_depth,
 jint preferred_first_line_length, jint preferred_line_length) {
  try {
    unsigned ind = jtype_to_unsigned<unsigned int>(indent_depth);
    unsigned pfll = jtype_to_unsigned<unsigned int>(preferred_first_line_length);
    unsigned pll = jtype_to_unsigned<unsigned int>(preferred_line_length);
    const char* chars = env->GetStringUTFChars(str, 0);
    if (!chars)
      return 0;
    using namespace Parma_Polyhedra_Library::IO_Operators;
    std::string s = wrap_string(chars, ind, pfll, pll);
    env->ReleaseStringUTFChars(str, chars);
    return env->NewStringUTF(s.c_str());
  }
  CATCH_ALL;
  return 0;
}
