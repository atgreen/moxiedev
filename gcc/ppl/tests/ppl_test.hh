/* Header file for test programs.
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

#ifndef PPL_ppl_test_hh
#define PPL_ppl_test_hh 1

#include "ppl_header.hh"
#include "Partial_Function.defs.hh"
#include "Random_Number_Generator.defs.hh"
#include <stdexcept>
#include <sstream>
#include <list>
#include <iterator>
#include <string>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <cstdlib>

#ifndef NOISY
#define NOISY 0
#endif

#ifndef VERY_NOISY
#define VERY_NOISY 0
#endif

#define TRY try

#define CATCH \
catch (const std::overflow_error& e) { \
  std::cerr << "arithmetic overflow (" << e.what() << ")" \
            << std::endl; \
  exit(1); \
} \
catch (const std::exception& e) { \
  std::cerr << "std::exception caught: " \
            << e.what() << " (type == " << typeid(e).name() << ")" \
            << std::endl; \
  exit(1); \
}

#define BEGIN_MAIN				\
int						\
main() try {					\
  set_handlers();				\
  bool succeeded = false;			\
  bool overflow = false;			\
  std::list<std::string> failed_tests;

#define END_MAIN							\
  if (failed_tests.empty())						\
    return 0;								\
  else {								\
    std::cerr << "failed tests: ";					\
    std::copy(failed_tests.begin(), failed_tests.end(),			\
	      std::ostream_iterator<std::string>(std::cerr, " "));	\
    std::cerr << std::endl;						\
    return 1;								\
  }									\
}									\
catch (const std::overflow_error& e) {					\
  std::cerr << "arithmetic overflow (" << e.what() << ")"		\
            << std::endl;						\
  exit(1);								\
}									\
catch (const std::exception& e) {					\
  std::cerr << "std::exception caught: "				\
	    << e.what() << " (type == " << typeid(e).name() << ")"	\
	    << std::endl;						\
  exit(1);								\
}

#define ANNOUNCE_TEST(test)		 \
  nout << "\n=== " #test " ===" << std::endl

#define RUN_TEST(test)							\
  try {									\
    overflow = false;							\
    succeeded = test();							\
  }									\
  catch (const std::overflow_error& e) {				\
    nout << "arithmetic overflow (" << e.what() << ")"			\
	 << std::endl;							\
    overflow = true;							\
    succeeded = false;							\
  }									\
  catch (const std::exception& e) {					\
    nout << "std::exception caught: "					\
	 << e.what() << " (type == " << typeid(e).name() << ")"		\
	 << std::endl;							\
    succeeded = false;							\
  }									\
  catch (...) {								\
    nout << "unknown exception caught"					\
	 << std::endl;							\
    succeeded = false;							\
  }

#define DO_TEST(test)			 \
  ANNOUNCE_TEST(test);			 \
  RUN_TEST(test);			 \
  if (!succeeded)			 \
    failed_tests.push_back(#test);

#define DO_TEST_F(test)			 \
  ANNOUNCE_TEST(test);			 \
  RUN_TEST(test);			 \
  if (succeeded)			 \
    failed_tests.push_back(#test);

#define DO_TEST_OVERFLOW(test)		 \
  ANNOUNCE_TEST(test);			 \
  RUN_TEST(test);			 \
  if (succeeded || !overflow)		 \
    failed_tests.push_back(#test);

#define DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)			\
  ANNOUNCE_TEST(test);							\
  RUN_TEST(test);							\
  if (!succeeded)							\
    if (!overflow || has_exact_coefficient_type(shape(0, EMPTY)))	\
      failed_tests.push_back(#test);

#if PPL_COEFFICIENT_BITS == 0

#define DO_TEST_F64(test) DO_TEST(test)
#define DO_TEST_F64A(test) DO_TEST(test)
#define DO_TEST_F32(test) DO_TEST(test)
#define DO_TEST_F32A(test) DO_TEST(test)
#define DO_TEST_F16(test) DO_TEST(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape) DO_TEST(test)

#elif PPL_COEFFICIENT_BITS == 64

#ifdef NDEBUG

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST(test)
#define DO_TEST_F32(test) DO_TEST(test)
#define DO_TEST_F32A(test) DO_TEST(test)
#define DO_TEST_F16(test) DO_TEST(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#else // PPL_COEFFICIENT_BITS == 64 && !defined(NDEBUG)

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST(test)
#define DO_TEST_F32A(test) DO_TEST(test)
#define DO_TEST_F16(test) DO_TEST(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#endif // !defined(NDEBUG)

#elif PPL_COEFFICIENT_BITS == 32

#ifdef NDEBUG

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST(test)
#define DO_TEST_F16(test) DO_TEST(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#else // PPL_COEFFICIENT_BITS == 32 && !defined(NDEBUG)

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16(test) DO_TEST(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#endif // !defined(NDEBUG)

#elif PPL_COEFFICIENT_BITS == 16

#ifdef NDEBUG

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A(test) DO_TEST(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#else // PPL_COEFFICIENT_BITS == 16 && !defined(NDEBUG)

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F8(test) DO_TEST(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#endif // !defined(NDEBUG)

#elif PPL_COEFFICIENT_BITS == 8

#ifdef NDEBUG

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F8(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F8A(test) DO_TEST(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_MAY_OVERFLOW_IF_INEXACT(test, shape)

#else // PPL_COEFFICIENT_BITS == 8 && !defined(NDEBUG)

#define DO_TEST_F64(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F8(test) DO_TEST_OVERFLOW(test)
#define DO_TEST_F8A(test) DO_TEST_OVERFLOW(test)

#define DO_TEST_F64_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F64A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F32A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F16A_MAY_OVERFLOW_IF_INEXACT(test, shape) \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F8_MAY_OVERFLOW_IF_INEXACT(test, shape)   \
  DO_TEST_OVERFLOW(test)
#define DO_TEST_F8A_MAY_OVERFLOW_IF_INEXACT(test, shape)  \
  DO_TEST_OVERFLOW(test)

#endif // !defined(NDEBUG)

#endif // PPL_COEFFICIENT_BITS == 8


// Turn s into a string: PPL_TEST_STR(x + y) => "x + y".
#define PPL_TEST_STR(s) #s

// Turn the expansion of s into a string: PPL_TEST_XSTR(x) => "s expanded".
#define PPL_TEST_XSTR(s) PPL_TEST_STR(s)


#ifdef DERIVED_TEST
#define C_Polyhedron NNC_Polyhedron
#endif

#ifndef BOX_INSTANCE
#define BOX_INSTANCE rt_r_oc
#endif

#ifndef BD_SHAPE_INSTANCE
#define BD_SHAPE_INSTANCE mpq_class
#endif

#ifndef OCTAGONAL_SHAPE_INSTANCE
#define OCTAGONAL_SHAPE_INSTANCE mpq_class
#endif

// These using directive and declaration are just to avoid the
// corresponding namespace qualifications in all the tests.
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::Test;
using std::endl;

namespace Parma_Polyhedra_Library {

namespace Test {

static bool
check_noisy(const char*
#if PPL_HAVE_DECL_GETENV || NOISY || VERY_NOISY
            environment_variable
#endif
) {
#if PPL_HAVE_DECL_GETENV
  return getenv(environment_variable) != 0;
#else
#if NOISY
  if (strcmp(environment_variable, "PPL_NOISY_TESTS") == 0)
    return true;
#endif
#if VERY_NOISY
  if (strcmp(environment_variable, "PPL_VERY_NOISY_TESTS") == 0)
    return true;
#endif
  return false;
#endif
}

template<typename CharT, typename Traits = std::char_traits<CharT> >
class nullbuf : public std::basic_streambuf<CharT, Traits> {
protected:
  virtual typename Traits::int_type overflow(typename Traits::int_type c) {
    return Traits::not_eof(c);
  }
};

template <class CharT, class Traits = std::char_traits<CharT> >
class noisy_ostream : public std::basic_ostream<CharT, Traits> {
private:
  nullbuf<CharT, Traits> black_hole;

public:
  noisy_ostream(const std::basic_ostream<CharT, Traits>& os,
		const char* environment_variable)
    : std::basic_ostream<CharT, Traits>(check_noisy(environment_variable)
					? os.rdbuf()
					: &black_hole) {
  }
};

static noisy_ostream<char> nout(std::cout, "PPL_NOISY_TESTS");
static noisy_ostream<char> vnout(std::cout, "PPL_VERY_NOISY_TESTS");

void
set_handlers();

//! Utility typedef to allow a macro argument to denote the long double type.
typedef long double long_double;

struct Floating_Real_Open_Interval_Info_Policy {
  const_bool_nodef(store_special, false);
  const_bool_nodef(store_open, true);
  const_bool_nodef(cache_empty, true);
  const_bool_nodef(cache_singleton, true);
  const_bool_nodef(cache_normalized, false);
  const_int_nodef(next_bit, 0);
  const_bool_nodef(may_be_empty, true);
  const_bool_nodef(may_contain_infinity, false);
  const_bool_nodef(check_empty_result, false);
  const_bool_nodef(check_inexact, false);
};

typedef Interval_Restriction_None
<Interval_Info_Bitset<unsigned int, Floating_Real_Open_Interval_Info_Policy> >
Floating_Real_Open_Interval_Info;

typedef Interval<float, Floating_Real_Open_Interval_Info> fl_r_oc;
typedef Interval<double, Floating_Real_Open_Interval_Info> db_r_oc;
typedef Interval<long double, Floating_Real_Open_Interval_Info> ld_r_oc;

struct Rational_Real_Open_Interval_Info_Policy {
  const_bool_nodef(store_special, true);
  const_bool_nodef(store_open, true);
  const_bool_nodef(cache_empty, true);
  const_bool_nodef(cache_singleton, true);
  const_bool_nodef(cache_normalized, false);
  const_int_nodef(next_bit, 0);
  const_bool_nodef(may_be_empty, true);
  const_bool_nodef(may_contain_infinity, false);
  const_bool_nodef(check_empty_result, false);
  const_bool_nodef(check_inexact, false);
};

typedef Interval_Restriction_None
<Interval_Info_Bitset<unsigned int, Rational_Real_Open_Interval_Info_Policy> >
Rational_Real_Open_Interval_Info;

typedef Interval<mpq_class, Rational_Real_Open_Interval_Info> rt_r_oc;

//! The incarnation of Box under test.
typedef Box<BOX_INSTANCE> TBox;

//! The incarnation of BD_Shape under test.
typedef BD_Shape<BD_SHAPE_INSTANCE> TBD_Shape;

//! The incarnation of Octagonal_Shape under test.
typedef Octagonal_Shape<OCTAGONAL_SHAPE_INSTANCE> TOctagonal_Shape;

template <typename Shape>
inline bool
has_exact_coefficient_type(const Shape&) {
  return std::numeric_limits<typename Shape::coefficient_type>::is_exact;
}

template <typename Interval>
inline bool
has_exact_coefficient_type(const Box<Interval>&) {
  return std::numeric_limits<typename Interval::boundary_type>::is_exact;
}

bool
check_distance(const Checked_Number<mpq_class, Extended_Number_Policy>& d,
	       const char* max_d_s, const char* d_name);

template <typename T>
bool
check_result_i(const BD_Shape<T>& computed_result,
	       const BD_Shape<mpq_class>& known_result,
	       const char* max_r_d_s,
	       const char* max_e_d_s,
	       const char* max_l_d_s) {
  BD_Shape<mpq_class> q_computed_result(computed_result);
  // Handle in a more efficient way the case where equality is expected.
  if (max_r_d_s == 0 && max_e_d_s == 0 && max_l_d_s == 0) {
    if (q_computed_result != known_result) {
      using IO_Operators::operator<<;
      nout << "Equality does not hold:"
	   << "\ncomputed result is\n"
	   << q_computed_result
	   << "\nknown result is\n"
	   << known_result
	   << endl;
      return false;
    }
    else
      return true;
  }

  if (!q_computed_result.contains(known_result)) {
    using IO_Operators::operator<<;
    nout << "Containment does not hold:"
	 << "\ncomputed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
    return false;
  }

  Checked_Number<mpq_class, Extended_Number_Policy> r_d;
  rectilinear_distance_assign(r_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> e_d;
  euclidean_distance_assign(e_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> l_d;
  l_infinity_distance_assign(l_d, known_result, q_computed_result, ROUND_UP);
  bool ok_r = check_distance(r_d, max_r_d_s, "rectilinear");
  bool ok_e = check_distance(e_d, max_e_d_s, "euclidean");
  bool ok_l = check_distance(l_d, max_l_d_s, "l_infinity");
  bool ok = ok_r && ok_e && ok_l;
  if (!ok) {
    using IO_Operators::operator<<;
    nout << "Computed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
  }
  return ok;
}

template <typename T>
bool
check_result(const BD_Shape<T>& computed_result,
	     const BD_Shape<mpq_class>& known_result,
	     const char* max_r_d_s,
	     const char* max_e_d_s,
	     const char* max_l_d_s) {
  return std::numeric_limits<T>::is_integer
    ? check_result_i(computed_result, known_result,
		     "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result,
		     max_r_d_s, max_e_d_s, max_l_d_s);
}

template <typename T>
bool
check_result(const BD_Shape<T>& computed_result,
	     const BD_Shape<T>& known_result) {
  if (computed_result == known_result)
    return true;
  else {
    using IO_Operators::operator<<;
    nout << "Equality does not hold:"
         << "\ncomputed result is\n"
         << computed_result
         << "\nknown result is\n"
         << known_result
         << endl;
    return false;
  }
}

template <typename T>
bool
check_result(const BD_Shape<T>& computed_result,
	     const BD_Shape<mpq_class>& known_result) {
  return std::numeric_limits<T>::is_integer
    ? check_result_i(computed_result, known_result, "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result, 0, 0, 0);
}

bool
check_result(const BD_Shape<mpq_class>& computed_result,
	     const BD_Shape<mpq_class>& known_result);

template <typename T>
bool
check_result_i(const Octagonal_Shape<T>& computed_result,
	       const Octagonal_Shape<mpq_class>& known_result,
	       const char* max_r_d_s,
	       const char* max_e_d_s,
	       const char* max_l_d_s) {
  Octagonal_Shape<mpq_class> q_computed_result(computed_result);
  // Handle in a more efficient way the case where equality is expected.
  if (max_r_d_s == 0 && max_e_d_s == 0 && max_l_d_s == 0) {
    if (q_computed_result != known_result) {
      using IO_Operators::operator<<;
      nout << "Equality does not hold:"
	   << "\ncomputed result is\n"
           << q_computed_result
	   << "\nknown result is\n"
           << known_result
	   << endl;
      return false;
    }
    else
      return true;
  }

  if (!q_computed_result.contains(known_result)) {
    using IO_Operators::operator<<;
    nout << "Containment does not hold:"
	 << "\ncomputed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
    return false;
  }

  Checked_Number<mpq_class, Extended_Number_Policy> r_d;
  rectilinear_distance_assign(r_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> e_d;
  euclidean_distance_assign(e_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> l_d;
  l_infinity_distance_assign(l_d, known_result, q_computed_result, ROUND_UP);
  bool ok_r = check_distance(r_d, max_r_d_s, "rectilinear");
  bool ok_e = check_distance(e_d, max_e_d_s, "euclidean");
  bool ok_l = check_distance(l_d, max_l_d_s, "l_infinity");
  bool ok = ok_r && ok_e && ok_l;
  if (!ok) {
    using IO_Operators::operator<<;
    nout << "Computed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
  }
  return ok;
}

template <typename T>
bool
check_result(const Octagonal_Shape<T>& computed_result,
	     const Octagonal_Shape<mpq_class>& known_result,
	     const char* max_r_d_s,
	     const char* max_e_d_s,
	     const char* max_l_d_s) {
  return std::numeric_limits<T>::is_integer
    ? check_result_i(computed_result, known_result,
		     "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result,
		     max_r_d_s, max_e_d_s, max_l_d_s);
}

template <>
inline bool
check_result(const Octagonal_Shape<mpq_class>& computed_result,
	     const Octagonal_Shape<mpq_class>& known_result,
	     const char*,
	     const char*,
	     const char*) {
  return check_result_i(computed_result, known_result,
			0, 0, 0);
}

template <typename T>
bool
check_result(const Octagonal_Shape<T>& computed_result,
	     const Octagonal_Shape<mpq_class>& known_result) {
  return std::numeric_limits<T>::is_integer
    ? check_result_i(computed_result, known_result, "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result, 0, 0, 0);
}


template <typename Interval>
bool
check_result_i(const Box<Interval>& computed_result,
	       const Rational_Box& known_result,
	       const char* max_r_d_s,
	       const char* max_e_d_s,
	       const char* max_l_d_s) {
  Rational_Box q_computed_result(computed_result);
  // Handle in a more efficient way the case where equality is expected.
  if (max_r_d_s == 0 && max_e_d_s == 0 && max_l_d_s == 0) {
    if (q_computed_result != known_result) {
      using IO_Operators::operator<<;
      nout << "Equality does not hold:"
	   << "\ncomputed result is\n"
	   << q_computed_result
	   << "\nknown result is\n"
	   << known_result
	   << endl;
      return false;
    }
    else
      return true;
  }

  if (!q_computed_result.contains(known_result)) {
    using IO_Operators::operator<<;
    nout << "Containment does not hold:"
	 << "\ncomputed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
    nout << "Individual dimensions where containment does not hold"
	 << "\n(Variable: computed-result known-result):\n";
    for (dimension_type i = 0; i < computed_result.space_dimension(); ++i) {
      if (!q_computed_result.get_interval(Variable(i))
          .contains(known_result.get_interval(Variable(i)))) {
        using IO_Operators::operator<<;
	nout << Variable(i) << ": "
	     << q_computed_result.get_interval(Variable(i))
	     << ' '
	     << known_result.get_interval(Variable(i))
	     << endl;
      }
    }
    return false;
  }

  Checked_Number<mpq_class, Extended_Number_Policy> r_d;
  rectilinear_distance_assign(r_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> e_d;
  euclidean_distance_assign(e_d, known_result, q_computed_result, ROUND_UP);
  Checked_Number<mpq_class, Extended_Number_Policy> l_d;
  l_infinity_distance_assign(l_d, known_result, q_computed_result, ROUND_UP);
  bool ok_r = check_distance(r_d, max_r_d_s, "rectilinear");
  bool ok_e = check_distance(e_d, max_e_d_s, "euclidean");
  bool ok_l = check_distance(l_d, max_l_d_s, "l_infinity");
  bool ok = ok_r && ok_e && ok_l;
  if (!ok) {
    using IO_Operators::operator<<;
    nout << "Computed result is\n"
	 << q_computed_result
	 << "\nknown result is\n"
	 << known_result
	 << endl;
  }
  return ok;
}

template <typename Interval>
bool
check_result(const Box<Interval>& computed_result,
	     const Rational_Box& known_result,
	     const char* max_r_d_s,
	     const char* max_e_d_s,
	     const char* max_l_d_s) {
  return std::numeric_limits<typename Interval::boundary_type>::is_integer
    ? check_result_i(computed_result, known_result,
		     "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result,
		     max_r_d_s, max_e_d_s, max_l_d_s);
}

template <typename Interval>
bool
check_result(const Box<Interval>& computed_result,
	     const Box<Interval>& known_result) {
  if (computed_result == known_result)
    return true;
  else {
    using IO_Operators::operator<<;
    nout << "Equality does not hold:"
         << "\ncomputed result is\n"
         << computed_result
         << "\nknown result is\n"
         << known_result
         << endl;
    return false;
  }
}

template <typename Interval>
bool
check_result(const Box<Interval>& computed_result,
	     const Rational_Box& known_result) {
  return std::numeric_limits<typename Interval::boundary_type>::is_integer
    ? check_result_i(computed_result, known_result, "+inf", "+inf", "+inf")
    : check_result_i(computed_result, known_result, 0, 0, 0);
}

bool
check_result(const Rational_Box& computed_result,
	     const Rational_Box& known_result);

bool
check_result(const Generator& computed_result,
             const Generator& known_result,
             const char* max_r_d_s,
             const char* max_e_d_s,
             const char* max_l_d_s);

bool
check_result(const Checked_Number<mpq_class, Extended_Number_Policy>& computed,
             const Checked_Number<mpq_class, Extended_Number_Policy>& known,
             const char* max_r_d_s);

class FCAIBVP;

bool
operator==(const FCAIBVP& x, const FCAIBVP& y);

bool
operator!=(const FCAIBVP& x, const FCAIBVP& y);

std::ostream&
operator<<(std::ostream& s, const FCAIBVP& x);

/*! \brief
  A class for representing Finite Conjunctions of Attribute
  Independent Boolean Variable Properties.
*/
class FCAIBVP {
private:
  typedef size_t dim_t;

  typedef std::set<dim_t> Set;

  Set set;

public:
  FCAIBVP();

  explicit FCAIBVP(const Variable& x);

  explicit FCAIBVP(const Variables_Set& y);

  FCAIBVP(const FCAIBVP& y, unsigned offset);

  memory_size_type total_memory_in_bytes() const;

  memory_size_type external_memory_in_bytes() const;

  bool is_top() const;

  bool is_bottom() const;

  bool definitely_entails(const FCAIBVP& y) const;

  void upper_bound_assign(const FCAIBVP& y);

  void difference_assign(const FCAIBVP& y);

  void meet_assign(const FCAIBVP& y);

  void weakening_assign(const FCAIBVP& y);

  static bool has_nontrivial_weakening();

  bool OK() const;

  friend std::ostream&
  Parma_Polyhedra_Library::Test::operator<<(std::ostream& s,
                                            const FCAIBVP& x);
};

inline
FCAIBVP::FCAIBVP()
  : set() {
}

inline
FCAIBVP::FCAIBVP(const Variable& x)
  : set() {
  set.insert(x.id());
}

inline
FCAIBVP::FCAIBVP(const Variables_Set& y)
  : set() {
  for (Variables_Set::const_iterator i = y.begin(),
	 y_end = y.end(); i != y_end; ++i)
    set.insert(*i);
}

inline
FCAIBVP::FCAIBVP(const FCAIBVP& y, unsigned offset)
  : set() {
  for (Set::const_iterator i = y.set.begin(),
	 y_set_end = y.set.end(); i != y_set_end; ++i)
    set.insert(*i + offset);
}

inline memory_size_type
FCAIBVP::total_memory_in_bytes() const {
  return 1;
}

inline bool
FCAIBVP::is_top() const {
  return set.empty();
}

inline bool
FCAIBVP::is_bottom() const {
  return false;
}

inline bool
FCAIBVP::definitely_entails(const FCAIBVP& y) const{
  const FCAIBVP& x = *this;
  return std::includes(x.set.begin(), x.set.end(),
		       y.set.begin(), y.set.end());
}

inline void
FCAIBVP::upper_bound_assign(const FCAIBVP& y) {
  FCAIBVP& x = *this;
  FCAIBVP z;
  std::set_intersection(x.set.begin(), x.set.end(),
			y.set.begin(), y.set.end(),
			std::inserter(z.set, z.set.begin()));
  std::swap(x, z);
}

inline void
FCAIBVP::difference_assign(const FCAIBVP& y) {
  FCAIBVP& x = *this;
  FCAIBVP z;
  std::set_difference(x.set.begin(), x.set.end(),
		      y.set.begin(), y.set.end(),
		      std::inserter(z.set, z.set.begin()));
  std::swap(x, z);
}

inline void
FCAIBVP::meet_assign(const FCAIBVP& y) {
  set.insert(y.set.begin(), y.set.end());
}

inline void
FCAIBVP::weakening_assign(const FCAIBVP& y) {
  difference_assign(y);
}

inline bool
FCAIBVP::has_nontrivial_weakening() {
  return true;
}

inline bool
FCAIBVP::OK() const {
  return true;
}

inline bool
operator==(const FCAIBVP& x, const FCAIBVP& y) {
  return x.definitely_entails(y) && y.definitely_entails(x);
}

inline bool
operator!=(const FCAIBVP& x, const FCAIBVP& y) {
  return !(x == y);
}

void
print_constraint(const Constraint& c,
		 const std::string& intro = "",
		 std::ostream& s = nout);

void
print_constraints(const Constraint_System& cs,
		  const std::string& intro = "",
		  std::ostream& s = nout);

void
print_constraints(const Polyhedron& ph,
		  const std::string& intro = "",
		  std::ostream& s = nout);

template <typename Interval>
void
print_constraints(const Box<Interval>& box,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << box << std::endl;
}

template <typename T>
void
print_constraints(const BD_Shape<T>& bd,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << bd << std::endl;
}

template <typename T>
void
print_constraints(const Octagonal_Shape<T>& oc,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << oc << std::endl;
}

template <typename PH>
void
print_constraints(const Pointset_Powerset<PH>& pps,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << pps << std::endl;
}

template <typename PH>
void
print_congruences(const Pointset_Powerset<PH>& pps,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << pps << std::endl;
}

/*
template <typename PH>
void
print_constraints(const Pointset_Ask_Tell<PH>& pat,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << std::endl;
  using IO_Operators::operator<<;
  s << pat << std::endl;
}
*/

void
print_congruence(const Congruence& c,
		 const std::string& intro = "",
		 std::ostream& s = nout);

void
print_congruences(const Congruence_System& cgs,
		  const std::string& intro = "",
		  std::ostream& s = nout);

void
print_congruences(const Grid& gr,
		  const std::string& intro = "",
		  std::ostream& s = nout);

void
print_generator(const Generator& g,
		const std::string& intro = "",
		std::ostream& s = nout);

void
print_generator(const Grid_Generator& g,
		const std::string& intro = "",
		std::ostream& s = nout);

void
print_generators(const Generator_System& gs,
		 const std::string& intro = "",
		 std::ostream& s = nout);

void
print_generators(const Grid_Generator_System& gs,
		 const std::string& intro = "",
		 std::ostream& s = nout);

void
print_generators(const Polyhedron& ph,
		 const std::string& intro = "",
		 std::ostream& s = nout);

void
print_generators(const Grid& gr,
		 const std::string& intro = "",
		 std::ostream& s = nout);

template <typename D1, typename D2, typename R>
void
print_constraints(const Partially_Reduced_Product<D1, D2, R>& pd,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  print_constraints(pd.constraints(), intro, s);
}

template <typename D1, typename D2, typename R>
void
print_congruences(const Partially_Reduced_Product<D1, D2, R>& pd,
		  const std::string& intro = "",
		  std::ostream& s = nout) {
  print_congruences(pd.congruences(), intro, s);
}

void
print_function(const Parma_Polyhedra_Library::Test::Partial_Function& function,
	       const std::string& intro = "",
	       std::ostream& s = nout);

} // namespace Test

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_ppl_test_hh)
