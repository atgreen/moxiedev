/* PPL Java interface common routines implementation.
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

#include "ppl_java_common.defs.hh"

namespace Parma_Polyhedra_Library {

namespace Interfaces {

namespace Java {

void
handle_exception(JNIEnv* env, const std::overflow_error& e) {
  jclass newExcCls
    = env->FindClass("parma_polyhedra_library/Overflow_Error_Exception");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env, const std::invalid_argument& e) {
  jclass newExcCls
    = env->FindClass("parma_polyhedra_library/Invalid_Argument_Exception");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env, const std::logic_error& e) {
  jclass newExcCls
    = env->FindClass("parma_polyhedra_library/Logic_Error_Exception");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env, const std::length_error& e) {
  jclass newExcCls
    = env->FindClass("parma_polyhedra_library/Length_Error_Exception");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env, const std::domain_error& e) {
  jclass newExcCls
    = env->FindClass("parma_polyhedra_library/Domain_Error_Exception");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env, const std::bad_alloc&) {
  jclass newExcCls
    = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, "Out of memory");
}

void
handle_exception(JNIEnv* env, const std::exception& e) {
  jclass newExcCls = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, e.what());
}

void
handle_exception(JNIEnv* env) {
  jclass newExcCls = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, "PPL bug: unknown exception raised");
}

jobject
build_java_poly_gen_relation(JNIEnv* env,
			     Poly_Gen_Relation& r) {
  jclass j_poly_gen_relation_class
    = env->FindClass("parma_polyhedra_library/Poly_Gen_Relation");
  jmethodID j_poly_gen_relation_ctr_id
    = env->GetMethodID(j_poly_gen_relation_class, "<init>", "(I)V");

  jint j_value = 0;
  while (r != Poly_Gen_Relation::nothing()) {
    if (r.implies(Poly_Gen_Relation::subsumes())) {
      j_value += 1;
      r = r - Poly_Gen_Relation::subsumes();
    }
  }
  return env->NewObject(j_poly_gen_relation_class,
			j_poly_gen_relation_ctr_id, j_value);
}

jobject
build_java_poly_con_relation(JNIEnv* env,
                             Poly_Con_Relation& r) {
  jclass j_poly_con_relation_class
    = env->FindClass("parma_polyhedra_library/Poly_Con_Relation");
  jmethodID j_poly_con_relation_ctr_id
    = env->GetMethodID(j_poly_con_relation_class, "<init>", "(I)V");

  jint j_value = 0;
  while (r != Poly_Con_Relation::nothing()) {
    if (r.implies(Poly_Con_Relation::is_disjoint())) {
      j_value += 1;
      r = r - Poly_Con_Relation::is_disjoint();
    }
    else if (r.implies(Poly_Con_Relation::strictly_intersects())) {
      j_value += 2;
      r = r - Poly_Con_Relation::strictly_intersects();
    }
    else if (r.implies(Poly_Con_Relation::is_included())) {
      j_value += 4;
      r = r - Poly_Con_Relation::is_included();
    }
    else if (r.implies(Poly_Con_Relation::saturates())) {
      j_value += 8;
      r = r - Poly_Con_Relation::saturates();
    }
  }
  return env->NewObject(j_poly_con_relation_class,
			j_poly_con_relation_ctr_id, j_value);
}


Congruence
build_cxx_congruence(JNIEnv* env, const jobject& j_congruence) {
  jclass congruence_class
    = env->FindClass("parma_polyhedra_library/Congruence");
  jfieldID modulus_field_id
    = env->GetFieldID(congruence_class,
                      "modulus",
                      "Lparma_polyhedra_library/Coefficient;");
  jobject j_modulus = env->GetObjectField(j_congruence, modulus_field_id);
  PPL_DIRTY_TEMP_COEFFICIENT(ppl_modulus);
  ppl_modulus = build_cxx_coeff(env, j_modulus);
  jfieldID lhs_field_id
    = env->GetFieldID(congruence_class,
                      "lhs",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jfieldID rhs_field_id
    = env->GetFieldID(congruence_class,
                      "rhs",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jobject j_lhs = env->GetObjectField(j_congruence, lhs_field_id);
  jobject j_rhs = env->GetObjectField(j_congruence, rhs_field_id);
  Linear_Expression lhs = build_cxx_linear_expression(env, j_lhs);
  Linear_Expression rhs = build_cxx_linear_expression(env, j_rhs);
  return (lhs %= rhs) / ppl_modulus;
}

jobject
bool_to_j_boolean(JNIEnv* env, const bool value) {
  jclass boolean_java_class = env->FindClass("java/lang/Boolean");
  jmethodID getboolean_method_id
    = env->GetStaticMethodID(boolean_java_class,
                             "valueOf",
                             "(Z)Ljava/lang/Boolean;");
  return env->CallStaticObjectMethod(boolean_java_class,
                                     getboolean_method_id,
                                     value);
}

jobject
j_long_to_j_long_class(JNIEnv* env, const jlong& jlong_value) {
  jclass long_java_class = env->FindClass("java/lang/Long");
  jmethodID get_long_method_id
    = env->GetStaticMethodID(long_java_class,
                             "valueOf",
                             "(J)Ljava/lang/Long;");
  return env->CallStaticObjectMethod(long_java_class,
 				     get_long_method_id,
 				     jlong_value);
}

jlong
j_long_class_to_j_long(JNIEnv* env,  const jobject& j_long) {
  jclass long_java_class = env->FindClass("java/lang/Long");
  jmethodID get_int_method_id = env->GetMethodID(long_java_class,
						 "longValue", "()J");
  return env->CallIntMethod(j_long, get_int_method_id);
}


jobject
j_int_to_j_integer(JNIEnv* env,  const jint&  jint_value) {
  jclass integer_java_class = env->FindClass("java/lang/Integer");
  jmethodID get_integer_method_id
    = env->GetStaticMethodID(integer_java_class,
			     "valueOf",
			     "(I)Ljava/lang/Integer;");
  return env->CallStaticObjectMethod(integer_java_class,
                                     get_integer_method_id,
                                     jint_value);
}

jint
j_integer_to_j_int(JNIEnv* env,  const jobject& j_integer) {
  jclass integer_java_class = env->FindClass("java/lang/Integer");
  jmethodID get_int_method_id = env->GetMethodID(integer_java_class,
                                                 "intValue", "()I");
  return env->CallIntMethod(j_integer, get_int_method_id);
}

// bool
// j_boolean_to_bool(JNIEnv* env,
// 		  const jobject& j_boolean) {
//   jclass boolean_class = env->GetObjectClass(j_boolean);
//   jmethodID booleanvalue_method_id = env->GetMethodID(boolean_class,
// 						      "booleanValue",
// 						      "()Z");
//   return env->CallBooleanMethod(j_boolean, booleanvalue_method_id);

// }

Variables_Set
build_cxx_variables_set(JNIEnv* env,
			const jobject& j_v_set) {
  jclass variables_set_class = env->GetObjectClass(j_v_set);
  jclass iterator_java_class = env->FindClass("java/util/Iterator");
  Variables_Set v_set;
  jmethodID iterator_method_id = env->GetMethodID(variables_set_class,
						  "iterator",
 						  "()Ljava/util/Iterator;");
  jobject j_iterator = env->CallObjectMethod(j_v_set, iterator_method_id);
  jmethodID has_next_method_id = env->GetMethodID(iterator_java_class,
  						  "hasNext",
  						  "()Z");
  jboolean has_next_value = env->CallBooleanMethod(j_iterator,
						   has_next_method_id);
  jmethodID next_method_id = env->GetMethodID(iterator_java_class,
					      "next",
					      "()Ljava/lang/Object;");

  while (has_next_value) {
    jobject j_variable = env->CallObjectMethod(j_iterator,
					       next_method_id);
    v_set.insert(build_cxx_variable(env, j_variable));
    has_next_value = env->CallBooleanMethod(j_iterator,
					    has_next_method_id);
  }
  return v_set;
}

jobject
build_java_variables_set(JNIEnv* env,
			 const Variables_Set& v_set) {
  jclass j_vs_class = env->FindClass("parma_polyhedra_library/Variables_Set");
  jmethodID j_vs_ctr_id = env->GetMethodID(j_vs_class, "<init>", "()V");
  jmethodID j_vs_add_id = env->GetMethodID(j_vs_class, "add",
					   "(Ljava/lang/Object;)Z");
  jobject j_vs = env->NewObject(j_vs_class, j_vs_ctr_id);
  for (Variables_Set::const_iterator v_begin = v_set.begin(),
	 v_end = v_set.end(); v_begin != v_end; ++v_begin) {
    Variable var(*v_begin);
    jobject j_variable = build_java_variable(env, var);
    env->CallBooleanMethod(j_vs, j_vs_add_id, j_variable);
  }
  return j_vs;
}

Variable
build_cxx_variable(JNIEnv* env, const jobject& j_var) {
  jclass j_variable_class = env->FindClass("parma_polyhedra_library/Variable");
  jfieldID varid_field_id = env->GetFieldID(j_variable_class,
                                            "varid",
                                            "I");
  // Retrieve the value.
  return Variable(env->GetIntField(j_var, varid_field_id));
}

jobject
build_java_variable(JNIEnv* env, const Variable& var) {
  jclass variable_class = env->FindClass("parma_polyhedra_library/Variable");
  jmethodID j_variable_ctr_id = env->GetMethodID(variable_class, "<init>",
						 "(I)V");
  return env->NewObject(variable_class, j_variable_ctr_id,
 			var.id());
}

Relation_Symbol
build_cxx_relsym(JNIEnv* env, const jobject& j_relsym) {
  jclass rel_sym_class
    = env->FindClass("parma_polyhedra_library/Relation_Symbol");
  jmethodID rel_sym_ordinal_id = env->GetMethodID(rel_sym_class, "ordinal",
						  "()I");
  jint rel_sym = env->CallIntMethod(j_relsym, rel_sym_ordinal_id);
  switch (rel_sym) {
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
    throw std::runtime_error("PPL Java interface internal error");
  }
}

Optimization_Mode
build_cxx_optimization_mode(JNIEnv* env, const jobject& j_opt_mode) {
  jclass opt_mode_class
    = env->FindClass("parma_polyhedra_library/Optimization_Mode");
  jmethodID opt_mode_ordinal_id = env->GetMethodID(opt_mode_class, "ordinal",
                                                   "()I");
  jint opt_mode = env->CallIntMethod(j_opt_mode, opt_mode_ordinal_id);
  switch (opt_mode) {
  case 0:
    return MINIMIZATION;
  case 1:
    return MAXIMIZATION;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_mip_status(JNIEnv* env, const MIP_Problem_Status& mip_status) {
  jclass j_mip_problem_status_class
    = env->FindClass("parma_polyhedra_library/MIP_Problem_Status");
  jfieldID mp_status_unfeasible_get_id
    = env->GetStaticFieldID(j_mip_problem_status_class,
			    "UNFEASIBLE_MIP_PROBLEM",
			    "Lparma_polyhedra_library/MIP_Problem_Status;");
  jfieldID mp_status_unbounded_get_id
    = env->GetStaticFieldID(j_mip_problem_status_class,
			    "UNBOUNDED_MIP_PROBLEM",
			    "Lparma_polyhedra_library/MIP_Problem_Status;");
  jfieldID mp_status_optimized_get_id
    = env->GetStaticFieldID(j_mip_problem_status_class,
			    "OPTIMIZED_MIP_PROBLEM",
			    "Lparma_polyhedra_library/MIP_Problem_Status;");
  switch (mip_status) {
  case UNFEASIBLE_MIP_PROBLEM:
    return env->GetStaticObjectField(j_mip_problem_status_class,
				     mp_status_unfeasible_get_id);
  case UNBOUNDED_MIP_PROBLEM:
    return env->GetStaticObjectField(j_mip_problem_status_class,
				     mp_status_unbounded_get_id);
  case OPTIMIZED_MIP_PROBLEM:
    return env->GetStaticObjectField(j_mip_problem_status_class,
				     mp_status_optimized_get_id);
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_optimization_mode(JNIEnv* env, const Optimization_Mode& opt_mode) {
  jclass j_optimization_mode_class
    = env->FindClass("parma_polyhedra_library/Optimization_Mode");
  jfieldID optimization_mode_min_get_id
    = env->GetStaticFieldID(j_optimization_mode_class,
			    "MINIMIZATION",
			    "Lparma_polyhedra_library/Optimization_Mode;");
  jfieldID optimization_mode_max_get_id
    = env->GetStaticFieldID(j_optimization_mode_class,
			    "MAXIMIZATION",
			    "Lparma_polyhedra_library/Optimization_Mode;");
  switch (opt_mode) {
  case MINIMIZATION:
    return env->GetStaticObjectField(j_optimization_mode_class,
				     optimization_mode_min_get_id);
  case MAXIMIZATION:
    return  env->GetStaticObjectField(j_optimization_mode_class,
				      optimization_mode_max_get_id);
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

MIP_Problem::Control_Parameter_Name
build_cxx_control_parameter_name(JNIEnv* env, const jobject& j_cp_name) {
  jclass cp_name_class
    = env->FindClass("parma_polyhedra_library/Control_Parameter_Name");
  jmethodID cp_name_ordinal_id = env->GetMethodID(cp_name_class, "ordinal",
						  "()I");
  jint cp_name = env->CallIntMethod(j_cp_name, cp_name_ordinal_id);
  switch (cp_name) {
  case 0:
    return MIP_Problem::PRICING;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_control_parameter_name
(JNIEnv* env,
 const MIP_Problem::Control_Parameter_Name& cp_name) {
  jclass j_cp_name_class
    = env->FindClass("parma_polyhedra_library/Control_Parameter_Name");
  jfieldID cp_name_pricing_get_id
    = env->GetStaticFieldID(j_cp_name_class,
			    "PRICING",
			    "Lparma_polyhedra_library/Control_Parameter_Name;");
  switch (cp_name) {
  case MIP_Problem::PRICING:
    return env->GetStaticObjectField(j_cp_name_class,
				     cp_name_pricing_get_id);
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

MIP_Problem::Control_Parameter_Value
build_cxx_control_parameter_value(JNIEnv* env, const jobject& j_cp_value) {
  jclass cp_value_class
    = env->FindClass("parma_polyhedra_library/Control_Parameter_Value");
  jmethodID cp_value_ordinal_id = env->GetMethodID(cp_value_class, "ordinal",
                                                   "()I");
  jint cp_value = env->CallIntMethod(j_cp_value, cp_value_ordinal_id);
  switch (cp_value) {
  case 0:
    return MIP_Problem::PRICING_STEEPEST_EDGE_FLOAT;
  case 1:
    return MIP_Problem::PRICING_STEEPEST_EDGE_EXACT;
  case 2:
    return MIP_Problem::PRICING_TEXTBOOK;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_control_parameter_value
(JNIEnv* env, const MIP_Problem::Control_Parameter_Value& cp_value) {
  jclass j_cp_value_class
    = env->FindClass("parma_polyhedra_library/Control_Parameter_Value");
  jfieldID cp_value_float_get_id
    = env->GetStaticFieldID(j_cp_value_class,
			    "PRICING_STEEPEST_EDGE_FLOAT",
			    "Lparma_polyhedra_library/Control_Parameter_Value;");
  jfieldID cp_value_exact_get_id
    = env->GetStaticFieldID(j_cp_value_class,
			    "PRICING_STEEPEST_EDGE_EXACT",
			    "Lparma_polyhedra_library/Control_Parameter_Value;");
  jfieldID cp_value_textbook_get_id
    = env->GetStaticFieldID(j_cp_value_class,
			    "PRICING_TEXTBOOK",
			    "Lparma_polyhedra_library/Control_Parameter_Value;");
  switch (cp_value) {
  case MIP_Problem::PRICING_STEEPEST_EDGE_FLOAT:
    return env->GetStaticObjectField(j_cp_value_class,
				     cp_value_float_get_id);
  case MIP_Problem::PRICING_STEEPEST_EDGE_EXACT:
    return env->GetStaticObjectField(j_cp_value_class,
				     cp_value_exact_get_id);
  case MIP_Problem::PRICING_TEXTBOOK:
    return env->GetStaticObjectField(j_cp_value_class,
				     cp_value_textbook_get_id);
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

Coefficient
build_cxx_coeff(JNIEnv* env, const jobject& j_coeff) {
  jclass j_coeff_class = env->GetObjectClass(j_coeff);
  jfieldID fid = env->GetFieldID(j_coeff_class, "value",
				 "Ljava/math/BigInteger;");
  jobject bi = env->GetObjectField(j_coeff, fid);
  jclass big_integer_class = env->GetObjectClass(bi);
  jmethodID bi_to_string = env->GetMethodID(big_integer_class, "toString",
					    "()Ljava/lang/String;");
  jstring bi_string = (jstring) env->CallObjectMethod(bi, bi_to_string);
  const char *nativeString = env->GetStringUTFChars(bi_string, 0);
  PPL_DIRTY_TEMP_COEFFICIENT(ppl_coeff);
  ppl_coeff = Coefficient(nativeString);
  env->ReleaseStringUTFChars(bi_string, nativeString);
  return ppl_coeff;
}

jobject
build_java_coeff(JNIEnv* env, const Coefficient& ppl_coeff) {
  std::ostringstream s;
  s << ppl_coeff;
  jclass j_coefficient_class =
    env->FindClass("parma_polyhedra_library/Coefficient");
  jmethodID j_coefficient_ctr_id = env->GetMethodID(j_coefficient_class,
						    "<init>",
						    "(Ljava/lang/String;)V");
  std::string str = s.str();
  jstring coeff_string = env->NewStringUTF(str.c_str());
  return env->NewObject(j_coefficient_class, j_coefficient_ctr_id,
			coeff_string);
}

Constraint
build_cxx_constraint(JNIEnv* env, const jobject& j_constraint) {
  jclass constraint_class
    = env->FindClass("parma_polyhedra_library/Constraint");
  jclass rel_sym_class
    = env->FindClass("parma_polyhedra_library/Relation_Symbol");
  jfieldID lhs_field_id
    = env->GetFieldID(constraint_class, "lhs",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jfieldID rhs_field_id
    = env->GetFieldID(constraint_class, "rhs",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jobject lhs_value = env->GetObjectField(j_constraint, lhs_field_id);
  jobject rhs_value = env->GetObjectField(j_constraint, rhs_field_id);
  Linear_Expression first_le = build_cxx_linear_expression(env, lhs_value);
  Linear_Expression second_le = build_cxx_linear_expression(env, rhs_value);
  jfieldID kind_field_id
    = env->GetFieldID(constraint_class, "kind",
                      "Lparma_polyhedra_library/Relation_Symbol;");
  jobject kind = env->GetObjectField(j_constraint, kind_field_id);
  jmethodID rel_sym_ordinal_id = env->GetMethodID(rel_sym_class, "ordinal",
						  "()I");
  jint rel_sym = env->CallIntMethod(kind, rel_sym_ordinal_id);
  switch (rel_sym) {
  case 0:
    return Constraint(first_le < second_le);
  case 1:
    return Constraint(first_le <= second_le);
  case 2:
    return Constraint(first_le == second_le);
  case 3:
    if (rel_sym == 3)
      return Constraint(first_le >= second_le);
  case 4:
    return Constraint(first_le > second_le);
  default:
    ;
  }
  jclass newExcCls = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, "ppl.java: \n runtime error");
  // We should not be here!
  throw std::runtime_error("PPL Java interface internal error");
}

Linear_Expression
build_cxx_linear_expression(JNIEnv* env, const jobject& j_le) {
  jclass le_sum_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Sum");
  jclass le_difference_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Difference");
  jclass le_times_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Times");
  jclass le_unary_minus_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Unary_Minus");
  jclass j_coeff_le_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Coefficient");
  jclass j_variable_le_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Variable");
  jclass j_variable_class = env->FindClass("parma_polyhedra_library/Variable");

  jclass current_class = env->GetObjectClass(j_le);
  // Variable
  if (env->IsAssignableFrom(j_variable_le_class, current_class)) {
    jfieldID arg_field_id
      = env->GetFieldID(j_variable_le_class,
                        "arg",
                        "Lparma_polyhedra_library/Variable;");
    jobject var = env->GetObjectField(j_le, arg_field_id);
    jfieldID varid_field_id = env->GetFieldID(j_variable_class,
					      "varid",
					      "I");
    // Retrieve the value.
    jint varid = env->GetIntField(var, varid_field_id);
    return Linear_Expression(Variable(varid));
  }
  // Coefficient
  if (env->IsAssignableFrom(j_coeff_le_class, current_class)) {
    jfieldID coeff_field_id
      = env->GetFieldID(j_coeff_le_class,
                        "coeff",
                        "Lparma_polyhedra_library/Coefficient;");
    jobject ppl_coeff = env->GetObjectField(j_le, coeff_field_id);

    return Linear_Expression(build_cxx_coeff(env, ppl_coeff));
  }
  // Sum
  if (env->IsAssignableFrom(le_sum_class, current_class)) {
    jfieldID l_field_id
      = env->GetFieldID(current_class, "lhs",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jfieldID r_field_id
      = env->GetFieldID(current_class, "rhs",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jobject l_value = env->GetObjectField(j_le, l_field_id);
    jobject r_value = env->GetObjectField(j_le, r_field_id);
    return (build_cxx_linear_expression(env, l_value)
	    + build_cxx_linear_expression(env, r_value));
  }
  // Difference
  if (env->IsAssignableFrom(current_class, le_difference_class)) {
    jfieldID l_field_id
      = env->GetFieldID(current_class, "lhs",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jfieldID r_field_id
      = env->GetFieldID(current_class, "rhs",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jobject l_value = env->GetObjectField(j_le, l_field_id);
    jobject r_value = env->GetObjectField(j_le, r_field_id);
    return (build_cxx_linear_expression(env, l_value)
	    - build_cxx_linear_expression(env, r_value));
  }
  // Times
  if (env->IsAssignableFrom(le_times_class, current_class)) {
    jfieldID le_field_id
      = env->GetFieldID(current_class, "rhs",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jfieldID le_coeff_field_id
      = env->GetFieldID(current_class, "lhs",
			"Lparma_polyhedra_library/Linear_Expression_Coefficient;");
    jobject le_value = env->GetObjectField(j_le, le_field_id);
    jobject le_coeff_value = env->GetObjectField(j_le, le_coeff_field_id);
    jfieldID coeff_field_id
      = env->GetFieldID(j_coeff_le_class,
                        "coeff",
                        "Lparma_polyhedra_library/Coefficient;");
    jobject ppl_coeff = env->GetObjectField(le_coeff_value, coeff_field_id);
    return (build_cxx_coeff(env, ppl_coeff)
	    * build_cxx_linear_expression(env, le_value));
  }
  // Unary_Minus
  if (env->IsAssignableFrom(current_class, le_unary_minus_class)) {
    jfieldID le_field_id
      = env->GetFieldID(current_class, "arg",
                        "Lparma_polyhedra_library/Linear_Expression;");
    jobject le_value = env->GetObjectField(j_le, le_field_id);
    return (-build_cxx_linear_expression(env, le_value));
  }
  // We should not be here!
  throw std::runtime_error("PPL Java interface internal error");
}

Generator
build_cxx_generator(JNIEnv* env, const jobject& j_generator) {
  jclass generator_class = env->FindClass("parma_polyhedra_library/Generator");
  jclass generator_type_class
    = env->FindClass("parma_polyhedra_library/Generator_Type");

  jfieldID j_le_field
    = env->GetFieldID(generator_class, "le",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jobject j_le = env->GetObjectField(j_generator, j_le_field);
  jfieldID j_coeff_field
    = env->GetFieldID(generator_class, "den",
                      "Lparma_polyhedra_library/Coefficient;");
  jobject j_coeff = env->GetObjectField(j_generator, j_coeff_field);

  jfieldID generator_type_field
    = env->GetFieldID(generator_class, "gt",
                      "Lparma_polyhedra_library/Generator_Type;");
  jobject generator_type = env->GetObjectField(j_generator,
					       generator_type_field);
  jmethodID generator_type_ordinal_id = env->GetMethodID(generator_type_class,
							 "ordinal",
							 "()I");
  jint generator_type_ordinal = env->CallIntMethod(generator_type,
						   generator_type_ordinal_id);
  switch (generator_type_ordinal) {
  case 0:
    return line(build_cxx_linear_expression(env, j_le));
  case 1:
    return ray(build_cxx_linear_expression(env, j_le));
  case 2:
    return point(build_cxx_linear_expression(env, j_le),
		 build_cxx_coeff(env, j_coeff));
  case 3:
    return closure_point(build_cxx_linear_expression(env, j_le),
			 build_cxx_coeff(env, j_coeff));
  default:
    ;
  }
  jclass newExcCls = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, "ppl.java: \n runtime error");
  // We should not be here!
  throw std::runtime_error("PPL Java interface internal error");
}

Grid_Generator
build_cxx_grid_generator(JNIEnv* env, const jobject& j_grid_generator) {
  jclass grid_generator_class
    = env->FindClass("parma_polyhedra_library/Grid_Generator");
  jclass grid_generator_type_class
    = env->FindClass("parma_polyhedra_library/Grid_Generator_Type");

  jfieldID j_le_field
    = env->GetFieldID(grid_generator_class,
                      "le",
                      "Lparma_polyhedra_library/Linear_Expression;");
  jobject j_le = env->GetObjectField(j_grid_generator, j_le_field);
  jfieldID j_coeff_field
    = env->GetFieldID(grid_generator_class,
                      "coeff",
                      "Lparma_polyhedra_library/Coefficient;");
  jobject j_coeff = env->GetObjectField(j_grid_generator, j_coeff_field);

  jfieldID grid_generator_type_field
    = env->GetFieldID(grid_generator_class,
		      "gt",
		      "Lparma_polyhedra_library/Grid_Generator_Type;");
  jobject grid_generator_type = env->GetObjectField(j_grid_generator,
						    grid_generator_type_field);
  jmethodID grid_generator_type_ordinal_id
    = env->GetMethodID(grid_generator_type_class,
		       "ordinal",
		       "()I");
  jint grid_generator_type_ordinal
    = env->CallIntMethod(grid_generator_type,
			 grid_generator_type_ordinal_id);
  switch (grid_generator_type_ordinal) {
  case 0:
    return grid_line(build_cxx_linear_expression(env, j_le));
  case 1:
    return parameter(build_cxx_linear_expression(env, j_le),
		     build_cxx_coeff(env, j_coeff));
  case 2:
    return grid_point(build_cxx_linear_expression(env, j_le),
		      build_cxx_coeff(env, j_coeff));
  default:
    ;
  }
  jclass newExcCls = env->FindClass("java/lang/RuntimeException");
  env->ThrowNew(newExcCls, "ppl.java: \n runtime error");
  // We should not be here!
  throw std::runtime_error("PPL Java interface internal error");
}

void*
get_ptr(JNIEnv* env, const jobject& ppl_object) {
  jclass ppl_object_class = env->GetObjectClass(ppl_object);
  jfieldID pointer_field = env->GetFieldID(ppl_object_class, "ptr", "J");
  jlong pointer_value = env->GetLongField(ppl_object, pointer_field);
  void* ptr = reinterpret_cast<void*>(pointer_value);
  assert(reinterpret_cast<jlong>(ptr) == pointer_value);
  return unmark(ptr);
}

bool
is_java_marked(JNIEnv* env, const jobject& ppl_object) {
  jclass ppl_object_class = env->GetObjectClass(ppl_object);
  jfieldID pointer_field = env->GetFieldID(ppl_object_class, "ptr", "J");
  jlong pointer_value = env->GetLongField(ppl_object, pointer_field);
  const void* ptr = reinterpret_cast<const void*>(pointer_value);
  assert(reinterpret_cast<jlong>(ptr) == pointer_value);
  return marked(ptr);
}

Grid_Generator_System
build_cxx_grid_generator_system(JNIEnv* env, const jobject& j_iterable) {
  jclass j_iterable_class = env->GetObjectClass(j_iterable);
  jclass iterator_java_class = env->FindClass("java/util/Iterator");
  Grid_Generator_System ggs;
  jmethodID iterator_method_id = env->GetMethodID(j_iterable_class,
						  "iterator",
 						  "()Ljava/util/Iterator;");
  jobject j_iterator = env->CallObjectMethod(j_iterable, iterator_method_id);
  jmethodID has_next_method_id = env->GetMethodID(iterator_java_class,
  						  "hasNext",
  						  "()Z");
  jboolean has_next_value = env->CallBooleanMethod(j_iterator,
						   has_next_method_id);
  jmethodID next_method_id = env->GetMethodID(iterator_java_class,
					      "next",
					      "()Ljava/lang/Object;");

  while (has_next_value) {
    jobject j_grid_generator = env->CallObjectMethod(j_iterator,
                                                     next_method_id);
    ggs.insert(build_cxx_grid_generator(env, j_grid_generator));
    has_next_value = env->CallBooleanMethod(j_iterator,
					    has_next_method_id);
  }
  return ggs;
}

Constraint_System
build_cxx_constraint_system(JNIEnv* env, const jobject& j_iterable) {
  jclass j_iterable_class = env->GetObjectClass(j_iterable);
  jclass iterator_java_class = env->FindClass("java/util/Iterator");
  Constraint_System cs;
  jmethodID iterator_method_id = env->GetMethodID(j_iterable_class,
						  "iterator",
 						  "()Ljava/util/Iterator;");
  jobject j_iterator = env->CallObjectMethod(j_iterable, iterator_method_id);
  jmethodID has_next_method_id = env->GetMethodID(iterator_java_class,
  						  "hasNext",
  						  "()Z");
  jboolean has_next_value = env->CallBooleanMethod(j_iterator,
						   has_next_method_id);
  jmethodID next_method_id = env->GetMethodID(iterator_java_class,
					      "next",
					      "()Ljava/lang/Object;");

  while (has_next_value) {
    jobject j_constraint = env->CallObjectMethod(j_iterator,
						 next_method_id);
    cs.insert(build_cxx_constraint(env, j_constraint));
    has_next_value = env->CallBooleanMethod(j_iterator,
					    has_next_method_id);
  }
  return cs;
}

Generator_System
build_cxx_generator_system(JNIEnv* env, const jobject& j_iterable) {
  jclass j_iterable_class = env->GetObjectClass(j_iterable);
  jclass iterator_java_class = env->FindClass("java/util/Iterator");
  Generator_System gs;
  jmethodID iterator_method_id = env->GetMethodID(j_iterable_class,
						  "iterator",
 						  "()Ljava/util/Iterator;");
  jobject j_iterator = env->CallObjectMethod(j_iterable, iterator_method_id);
  jmethodID has_next_method_id = env->GetMethodID(iterator_java_class,
  						  "hasNext",
  						  "()Z");
  jboolean has_next_value = env->CallBooleanMethod(j_iterator,
						   has_next_method_id);
  jmethodID next_method_id = env->GetMethodID(iterator_java_class,
					      "next",
					      "()Ljava/lang/Object;");

  while (has_next_value) {
    jobject j_constraint = env->CallObjectMethod(j_iterator,
						 next_method_id);
    gs.insert(build_cxx_generator(env, j_constraint));
    has_next_value = env->CallBooleanMethod(j_iterator,
					    has_next_method_id);
  }
  return gs;
}

Congruence_System
build_cxx_congruence_system(JNIEnv* env, const jobject& j_iterable) {
  jclass j_iterable_class = env->GetObjectClass(j_iterable);
  jclass iterator_java_class = env->FindClass("java/util/Iterator");
  Congruence_System cgs;
  jmethodID iterator_method_id = env->GetMethodID(j_iterable_class,
						  "iterator",
 						  "()Ljava/util/Iterator;");
  jobject j_iterator = env->CallObjectMethod(j_iterable, iterator_method_id);
  jmethodID has_next_method_id = env->GetMethodID(iterator_java_class,
  						  "hasNext",
  						  "()Z");
  jboolean has_next_value = env->CallBooleanMethod(j_iterator,
						   has_next_method_id);
  jmethodID next_method_id = env->GetMethodID(iterator_java_class,
					      "next",
					      "()Ljava/lang/Object;");

  while (has_next_value) {
    jobject j_congruence = env->CallObjectMethod(j_iterator,
						 next_method_id);
    cgs.insert(build_cxx_congruence(env, j_congruence));
    has_next_value = env->CallBooleanMethod(j_iterator,
					    has_next_method_id);
  }
  return cgs;
}

jobject
build_java_linear_expression_coefficient(JNIEnv* env,
                                         const Coefficient& c) {
  jclass j_le_coeff_class
    = env->FindClass("parma_polyhedra_library/Linear_Expression_Coefficient");
  jobject j_coeff =  build_java_coeff(env, c);
  jmethodID j_le_coeff_ctr_id
    = env->GetMethodID(j_le_coeff_class,
		       "<init>",
		       "(Lparma_polyhedra_library/Coefficient;)V");
  return env->NewObject(j_le_coeff_class, j_le_coeff_ctr_id,
			j_coeff);
}

void
set_generator(JNIEnv* env, jobject& to_be_set, const jobject& gen) {
  jclass j_generator_class
    = env->FindClass("parma_polyhedra_library/Generator");
  jmethodID j_coeff_set_id
    = env->GetMethodID(j_generator_class, "set",
                       "(Lparma_polyhedra_library/Generator;)V");
  env->CallVoidMethod(to_be_set, j_coeff_set_id, gen);
}

void set_grid_generator(JNIEnv* env, jobject& to_be_set,
                        const jobject& gen) {
  jclass j_generator_class
    = env->FindClass("parma_polyhedra_library/Grid_Generator");
  jmethodID j_coeff_set_id
    = env->GetMethodID(j_generator_class, "set",
                       "(Lparma_polyhedra_library/Grid_Generator;)V");
  env->CallVoidMethod(to_be_set, j_coeff_set_id, gen);
}

void
set_coefficient(JNIEnv* env, jobject& to_be_set,
                const jobject& c) {
  jclass j_coeff_class
    = env->FindClass("parma_polyhedra_library/Coefficient");
  jmethodID j_coeff_set_id
    = env->GetMethodID(j_coeff_class, "set",
                       "(Lparma_polyhedra_library/Coefficient;)V");
  env->CallVoidMethod(to_be_set, j_coeff_set_id, c);
}

void
set_by_reference(JNIEnv* env, jobject& by_ref_to_be_set,
                 const jobject& to_insert) {
  jclass by_reference_class
    = env->FindClass("parma_polyhedra_library/By_Reference");
  jfieldID obj_field_id = env->GetFieldID(by_reference_class,
					  "obj",
					  "Ljava/lang/Object;");
  env->SetObjectField(by_ref_to_be_set, obj_field_id, to_insert);
}

jobject
get_by_reference(JNIEnv* env, const jobject& by_reference) {
  jclass by_reference_class
    = env->FindClass("parma_polyhedra_library/By_Reference");
  jfieldID obj_field_id = env->GetFieldID(by_reference_class,
					  "obj",
					  "Ljava/lang/Object;");
  return env->GetObjectField(by_reference, obj_field_id);
}

void
set_pair_element(JNIEnv* env, jobject& pair_to_be_set,
                 int arg,  const jobject& to_insert) {
  jclass pair_class = env->FindClass("parma_polyhedra_library/Pair");
  switch (arg) {
  case 0:
    {
      jfieldID obj_field_id = env->GetFieldID(pair_class,
                                              "first",
                                              "Ljava/lang/Object;");
      env->SetObjectField(pair_to_be_set, obj_field_id, to_insert);
      return;
    }
  case 1:
    {
      jfieldID obj_field_id = env->GetFieldID(pair_class,
                                              "second",
                                              "Ljava/lang/Object;");
      env->SetObjectField(pair_to_be_set, obj_field_id, to_insert);
      return;
    }
  default:
    throw std::runtime_error("PPL Java interface internal error: pair value"
			     " not allowed");
  }
}


jobject
get_pair_element(JNIEnv* env, int arg, const jobject& j_pair) {
  jclass pair_class = env->FindClass("parma_polyhedra_library/Pair");
  switch (arg) {
  case 0:
    {
      jfieldID obj_field_id = env->GetFieldID(pair_class,
                                              "first",
                                              "Ljava/lang/Object;");
      return env->GetObjectField(j_pair, obj_field_id);
    }
  case 1:
    {
      jfieldID obj_field_id = env->GetFieldID(pair_class,
                                              "second",
                                              "Ljava/lang/Object;");
      return env->GetObjectField(j_pair, obj_field_id);
    }
  default:
    throw std::runtime_error("PPL Java interface internal error: pair value"
			     " not allowed");
  }
}

jboolean
is_null(JNIEnv* env, jobject obj) {
  jclass by_reference_class
    = env->FindClass("parma_polyhedra_library/By_Reference");
  jmethodID j_reference_is_null_id
    = env->GetStaticMethodID(by_reference_class,
                             "is_null",
                             "(Ljava/lang/Object;)Z");
  return env->CallStaticBooleanMethod(by_reference_class,
				      j_reference_is_null_id, obj);
}

jobject
build_java_constraint(JNIEnv* env, const Constraint& c) {
  jclass j_constraint_class
    = env->FindClass("parma_polyhedra_library/Constraint");
  jclass j_rel_sym_class
    = env->FindClass("parma_polyhedra_library/Relation_Symbol");
  jmethodID j_constraint_ctr_id
    = env->GetMethodID(j_constraint_class,
		       "<init>",
		       "(Lparma_polyhedra_library/Linear_Expression;"
		       "Lparma_polyhedra_library/Relation_Symbol;"
		       "Lparma_polyhedra_library/Linear_Expression;)V");
  jfieldID rel_sym_eq_get_id
    = env->GetStaticFieldID(j_rel_sym_class,
			    "EQUAL",
			    "Lparma_polyhedra_library/Relation_Symbol;");
  jfieldID rel_sym_gt_get_id
    = env->GetStaticFieldID(j_rel_sym_class,
 			    "GREATER_THAN",
 			    "Lparma_polyhedra_library/Relation_Symbol;");
  jfieldID rel_sym_gtoeq_get_id
    = env->GetStaticFieldID(j_rel_sym_class,
 			    "GREATER_OR_EQUAL",
 			    "Lparma_polyhedra_library/Relation_Symbol;");
  jobject lhs = build_linear_expression(env, c);
  jobject rhs
    = build_java_linear_expression_coefficient(env, -c.inhomogeneous_term());
  jobject relation;
  switch (c.type()) {
  case Constraint::EQUALITY:
    relation = env->GetStaticObjectField(j_rel_sym_class,
                                         rel_sym_eq_get_id);
    break;
  case Constraint::NONSTRICT_INEQUALITY:
    relation = env->GetStaticObjectField(j_rel_sym_class,
                                         rel_sym_gtoeq_get_id);
    break;
  case Constraint::STRICT_INEQUALITY:
    relation = env->GetStaticObjectField(j_rel_sym_class,
					 rel_sym_gt_get_id);
    break;
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
  return env->NewObject(j_constraint_class,j_constraint_ctr_id,
                        lhs, relation, rhs);
}

jobject
build_java_congruence(JNIEnv* env, const Congruence& cg) {
  jclass j_congruence_class
    = env->FindClass("parma_polyhedra_library/Congruence");
  jmethodID j_congruence_ctr_id
    = env->GetMethodID(j_congruence_class,
		       "<init>",
		       "(Lparma_polyhedra_library/Linear_Expression;"
		       "Lparma_polyhedra_library/Linear_Expression;"
		       "Lparma_polyhedra_library/Coefficient;)V");

  jobject j_modulus = build_java_coeff(env, cg.modulus());
  jobject lhs = build_linear_expression(env, cg);
  jobject rhs
    = build_java_linear_expression_coefficient(env, -cg.inhomogeneous_term());
  return env->NewObject(j_congruence_class, j_congruence_ctr_id,
			lhs, rhs,
			j_modulus);
}

jobject
build_java_generator(JNIEnv* env, const Generator& g) {
  jclass j_generator_class
    = env->FindClass("parma_polyhedra_library/Generator");
  jmethodID line_ctr_id
    = env->GetStaticMethodID(j_generator_class,
                             "line",
                             "(Lparma_polyhedra_library/Linear_Expression;)"
                             "Lparma_polyhedra_library/Generator;");
  jmethodID ray_ctr_id
    = env->GetStaticMethodID(j_generator_class,
                             "ray",
                             "(Lparma_polyhedra_library/Linear_Expression;)"
                             "Lparma_polyhedra_library/Generator;");
  jmethodID point_ctr_id
    = env->GetStaticMethodID(j_generator_class,
                             "point",
                             "(Lparma_polyhedra_library/Linear_Expression;"
                             "Lparma_polyhedra_library/Coefficient;)"
                             "Lparma_polyhedra_library/Generator;");
  jmethodID closure_point_ctr_id
    = env->GetStaticMethodID(j_generator_class,
                             "closure_point",
                             "(Lparma_polyhedra_library/Linear_Expression;"
                             "Lparma_polyhedra_library/Coefficient;)"
                             "Lparma_polyhedra_library/Generator;");
  jobject j_g_le = build_linear_expression(env, g);
  jobject jcoeff = build_java_coeff(env, Coefficient(1));
  switch (g.type()) {
  case Generator::LINE:
    return env->CallStaticObjectMethod(j_generator_class,
				       line_ctr_id, j_g_le);
  case Generator::RAY:
    return env->CallStaticObjectMethod(j_generator_class,
				       ray_ctr_id, j_g_le);
  case Generator::POINT:
    {
      const Coefficient& divisor = g.divisor();
      j_g_le = build_linear_expression(env, g);
      jcoeff = build_java_coeff(env, divisor);
      return env->CallStaticObjectMethod(j_generator_class,
					 point_ctr_id, j_g_le, jcoeff);
    }
  case Generator::CLOSURE_POINT:
    {
      const Coefficient& divisor = g.divisor();
      j_g_le = build_linear_expression(env, g);
      jcoeff = build_java_coeff(env, divisor);
      return env->CallStaticObjectMethod(j_generator_class,
					 closure_point_ctr_id, j_g_le, jcoeff);
    }
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_grid_generator(JNIEnv* env, const Grid_Generator& g) {
  jclass j_grid_generator_class
    = env->FindClass("parma_polyhedra_library/Grid_Generator");
  jmethodID line_ctr_id
    = env->GetStaticMethodID(j_grid_generator_class,
                             "grid_line",
                             "(Lparma_polyhedra_library/Linear_Expression;)"
                             "Lparma_polyhedra_library/Grid_Generator;");
  jmethodID parameter_ctr_id
    = env->GetStaticMethodID(j_grid_generator_class,
                             "parameter",
                             "(Lparma_polyhedra_library/Linear_Expression;"
                             "Lparma_polyhedra_library/Coefficient;)"
                             "Lparma_polyhedra_library/Grid_Generator;");
  jmethodID point_ctr_id
    = env->GetStaticMethodID(j_grid_generator_class,
                             "grid_point",
                             "(Lparma_polyhedra_library/Linear_Expression;"
                             "Lparma_polyhedra_library/Coefficient;)"
                             "Lparma_polyhedra_library/Grid_Generator;");
  jobject j_g_le = build_linear_expression(env, g);
  jobject jcoeff = build_java_coeff(env, Coefficient(1));
  switch (g.type()) {
  case Grid_Generator::LINE:
    return env->CallStaticObjectMethod(j_grid_generator_class,
				       line_ctr_id, j_g_le);
  case Grid_Generator::PARAMETER:
    {
      const Coefficient& divisor = g.divisor();
      j_g_le = build_linear_expression(env, g);
      jcoeff = build_java_coeff(env, divisor);
      return env->CallStaticObjectMethod(j_grid_generator_class,
					 parameter_ctr_id, j_g_le, jcoeff);
    }
  case Grid_Generator::POINT:
    {
      const Coefficient& divisor = g.divisor();
      j_g_le = build_linear_expression(env, g);
      jcoeff = build_java_coeff(env, divisor);
      return env->CallStaticObjectMethod(j_grid_generator_class,
					 point_ctr_id, j_g_le, jcoeff);
    }
  default:
    throw std::runtime_error("PPL Java interface internal error");
  }
}

jobject
build_java_constraint_system(JNIEnv* env, const Constraint_System& cs) {
  jclass j_cs_class
    = env->FindClass("parma_polyhedra_library/Constraint_System");
  jmethodID j_cs_ctr_id = env->GetMethodID(j_cs_class, "<init>", "()V");
  jmethodID j_cs_add_id = env->GetMethodID(j_cs_class, "add",
					   "(Ljava/lang/Object;)Z");
  jobject j_cs = env->NewObject(j_cs_class, j_cs_ctr_id);
  for (Constraint_System::const_iterator v_begin = cs.begin(),
 	 v_end = cs.end(); v_begin != v_end; ++v_begin) {
    jobject j_constraint = build_java_constraint(env, *v_begin);
    env->CallBooleanMethod(j_cs, j_cs_add_id, j_constraint);
  }
  return j_cs;
}

jobject
build_java_generator_system(JNIEnv* env, const Generator_System& gs) {
  jclass j_gs_class
    = env->FindClass("parma_polyhedra_library/Generator_System");
  jmethodID j_gs_ctr_id = env->GetMethodID(j_gs_class, "<init>", "()V");
  jmethodID j_gs_add_id = env->GetMethodID(j_gs_class, "add",
					   "(Ljava/lang/Object;)Z");
  jobject j_gs = env->NewObject(j_gs_class, j_gs_ctr_id);
  for (Generator_System::const_iterator v_begin = gs.begin(),
 	 v_end = gs.end(); v_begin != v_end; ++v_begin) {
    jobject j_generator = build_java_generator(env, *v_begin);
    env->CallBooleanMethod(j_gs, j_gs_add_id, j_generator);
  }
  return j_gs;
}

jobject
build_java_grid_generator_system(JNIEnv* env,
				 const Grid_Generator_System& gs) {
  jclass j_gs_class
    = env->FindClass("parma_polyhedra_library/Grid_Generator_System");
  jmethodID j_gs_ctr_id = env->GetMethodID(j_gs_class, "<init>", "()V");
  jmethodID j_gs_add_id = env->GetMethodID(j_gs_class, "add",
					   "(Ljava/lang/Object;)Z");
  jobject j_gs = env->NewObject(j_gs_class, j_gs_ctr_id);
  for (Grid_Generator_System::const_iterator v_begin = gs.begin(),
 	 v_end = gs.end(); v_begin != v_end; ++v_begin) {
    jobject j_generator = build_java_grid_generator(env, *v_begin);
    env->CallBooleanMethod(j_gs, j_gs_add_id, j_generator);
  }
  return j_gs;
}

jobject
build_java_congruence_system(JNIEnv* env, const Congruence_System& cgs) {
  jclass j_cgs_class
    = env->FindClass("parma_polyhedra_library/Congruence_System");
  jmethodID j_cgs_ctr_id = env->GetMethodID(j_cgs_class, "<init>", "()V");
  jmethodID j_cgs_add_id = env->GetMethodID(j_cgs_class, "add",
					    "(Ljava/lang/Object;)Z");
  jobject j_cgs = env->NewObject(j_cgs_class, j_cgs_ctr_id);
  for (Congruence_System::const_iterator v_begin = cgs.begin(),
  	 v_end = cgs.end(); v_begin != v_end; ++v_begin) {
    jobject j_congruence = build_java_congruence(env,*v_begin);
    env->CallBooleanMethod(j_cgs, j_cgs_add_id, j_congruence);
  }
  return j_cgs;
}

} // namespace Java

} // namespace Interfaces

} // namespace Parma_Polyhedra_Library
