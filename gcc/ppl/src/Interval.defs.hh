/* Declarations for the Interval class and its constituents.
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

#ifndef PPL_Interval_defs_hh
#define PPL_Interval_defs_hh 1

#include "globals.defs.hh"
#include "meta_programming.hh"
#include "assign_or_swap.hh"
#include "Interval.types.hh"
#include "Interval_Info.defs.hh"
#include <iosfwd>

// Temporary!
#include <iostream>

namespace Parma_Polyhedra_Library {

enum I_Result {
  I_EMPTY = 0,
  /*
  I_L_EQ = V_EQ,
  I_L_GT = V_GT,
  I_L_GE = V_GE,
  I_U_EQ = V_EQ << 6,
  I_U_LT = V_LT << 6,
  I_U_LE = V_LE << 6,
  */
  I_MAYBE_EMPTY = 1 << 12,
  I_SINGULARITIES = 1 << 13
};

enum Ternary { T_YES, T_NO, T_MAYBE };

inline I_Result
combine(Result l, Result u) {
  return static_cast<I_Result>(l | (u << 6));
}

inline Result
lower(I_Result r) {
  return static_cast<Result>(r & 63);
}

inline Result
upper(I_Result r) {
  return static_cast<Result>((r >> 6) & 63);
}

template <typename Info>
inline bool
unrepresentability_error(I_Result r, const Info&) {
  return !Info::store_special
    && (is_special(lower(r)) || is_special(upper(r)));
}

using namespace Boundary_NS;
using namespace Interval_NS;

struct Interval_Base {};

template <typename T, typename Enable = void>
struct Is_Singleton : public Is_Native_Or_Checked<T> {};

template <typename T>
struct Is_Interval : public Is_Same_Or_Derived<Interval_Base, T> {};

//! A generic, not necessarily closed, possibly restricted interval.
/*! \ingroup PPL_CXX_interface
  The class template type parameter \p Boundary represents the type
  of the interval boundaries, and can be chosen, among other possibilities,
  within one of the following number families:

  - a bounded precision native integer type (that is,
    from <CODE>signed char</CODE> to <CODE>long long</CODE>
    and from <CODE>int8_t</CODE> to <CODE>int64_t</CODE>);
  - a bounded precision floating point type (<CODE>float</CODE>,
    <CODE>double</CODE> or <CODE>long double</CODE>);
  - an unbounded integer or rational type, as provided by the C++ interface
    of GMP (<CODE>mpz_class</CODE> or <CODE>mpq_class</CODE>).

  The class template type parameter \p Info allows to control a number
  of features of the class, among which:

  - the ability to support open as well as closed boundaries;
  - the ability to represent empty intervals in addition to nonempty ones;
  - the ability to represent intervals of extended number families
    that contain positive and negative infinities;
  - the ability to support (independently from the type of the boundaries)
    plain intervals of real numbers and intervals subject to generic
    <EM>restrictions</EM> (e.g., intervals of integer numbers).
*/
template <typename Boundary, typename Info>
class Interval : public Interval_Base, private Info {
private:
  PPL_COMPILE_TIME_CHECK(!Info::store_special
                         || !std::numeric_limits<Boundary>::has_infinity,
                         "store_special is meaningless"
                         " when boundary type may contains infinity");
  Info& w_info() const {
    return const_cast<Interval&>(*this);
  }
  bool is_empty_nocache() const {
    return lt(UPPER, upper(), info(), LOWER, lower(), info());
  }
  bool is_singleton_nocache() const {
    return eq(LOWER, lower(), info(), UPPER, upper(), info());
  }
  Result lower_normalize() const {
    Result r;
    if (info().get_boundary_property(LOWER, NORMALIZED)
	|| info().get_boundary_property(LOWER, SPECIAL))
      r = V_EQ;
    else {
      Boundary& l = const_cast<Boundary&>(lower());
      if (info().get_boundary_property(LOWER, OPEN)) {
	r = info().restrict(round_dir_check(LOWER, true), l, V_GT);
	if (r != V_GT)
	  w_info().set_boundary_property(LOWER, OPEN, false);
      }
      else {
	r = info().restrict(round_dir_check(LOWER, true), l, V_GE);
	if (r == V_GT)
	  w_info().set_boundary_property(LOWER, OPEN);
      }
      w_info().set_boundary_property(LOWER, NORMALIZED);
    }
    return r;
  }
  Result upper_normalize() const {
    Result r;
    if (info().get_boundary_property(UPPER, NORMALIZED)
	|| info().get_boundary_property(UPPER, SPECIAL))
      r = V_EQ;
    else {
      Boundary& u = const_cast<Boundary&>(upper());
      if (info().get_boundary_property(UPPER, OPEN)) {
	r = info().restrict(round_dir_check(UPPER, true), u, V_LT);
	if (r != V_LT)
	  w_info().set_boundary_property(UPPER, OPEN, false);
      }
      else {
	r = info().restrict(round_dir_check(UPPER, true), u, V_LE);
	if (r == V_LT)
	  w_info().set_boundary_property(UPPER, OPEN);
      }
      w_info().set_boundary_property(UPPER, NORMALIZED);
    }
    return r;
  }


public:
  typedef Boundary boundary_type;
  typedef Info info_type;

  typedef Interval_NS::Property Property;

  void lower_load() {
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    lower_loaded = 1;
#endif
  }

  void upper_load() {
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    upper_loaded = 1;
#endif
  }

  void complete_init() {
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    assert(lower_loaded);
    assert(upper_loaded);
    completed = 1;
#endif
  }

  void complete_init_internal() {
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    lower_loaded = 1;
    upper_loaded = 1;
    completed = 1;
#endif
  }

  template <typename T>
  typename Enable_If<Is_Singleton<T>::value || Is_Interval<T>::value, Interval&>::type
  operator=(const T& x) {
    assign(x);
    return *this;
  }

  template <typename T>
  typename Enable_If<Is_Singleton<T>::value || Is_Interval<T>::value, Interval&>::type
  operator+=(const T& x) {
    add_assign(*this, x);
    return *this;
  }
  template <typename T>
  typename Enable_If<Is_Singleton<T>::value || Is_Interval<T>::value, Interval&>::type
  operator-=(const T& x) {
    sub_assign(*this, x);
    return *this;
  }
  template <typename T>
  typename Enable_If<Is_Singleton<T>::value || Is_Interval<T>::value, Interval&>::type
  operator*=(const T& x) {
    mul_assign(*this, x);
    return *this;
  }
  template <typename T>
  typename Enable_If<Is_Singleton<T>::value || Is_Interval<T>::value, Interval&>::type
  operator/=(const T& x) {
    div_assign(*this, x);
    return *this;
  }

  //! Swaps \p *this with \p y.
  void swap(Interval& y);

  Info& info() {
    return *this;
  }

  const Info& info() const {
    return *this;
  }

  Boundary& lower() {
    return lower_;
  }

  const Boundary& lower() const {
    return lower_;
  }

  Boundary& upper() {
    return upper_;
  }

  const Boundary& upper() const {
    return upper_;
  }

  Ternary is_empty_cached() const {
    if (info().get_interval_property(CARDINALITY_0))
      return info().get_interval_property(CARDINALITY_IS) ? T_YES : T_NO;
    else if (info().get_interval_property(CARDINALITY_IS))
      return T_NO;
    else
      return T_MAYBE;
  }

  Ternary is_singleton_cached() const {
    if (info().get_interval_property(CARDINALITY_1))
      return info().get_interval_property(CARDINALITY_IS) ? T_YES : T_NO;
    else if (info().get_interval_property(CARDINALITY_IS))
      return T_NO;
    else
      return T_MAYBE;
  }

  bool is_empty() const {
    switch (is_empty_cached()) {
    case T_NO:
      return false;
    case T_YES:
      return true;
    case T_MAYBE:
      bool r = is_empty_nocache();
      if (r) {
	w_info().set_interval_property(CARDINALITY_IS, r);
	w_info().set_interval_property(CARDINALITY_1, false);
      }
      w_info().set_interval_property(CARDINALITY_0);
      return r;
    }
    assert(false);
    return false;
  }

  bool is_singleton() const {
    switch (is_singleton_cached()) {
    case T_NO:
      return false;
    case T_YES:
      return true;
    case T_MAYBE:
      bool r = is_singleton_nocache();
      if (r) {
	w_info().set_interval_property(CARDINALITY_IS, r);
	w_info().set_interval_property(CARDINALITY_0, false);
      }
      w_info().set_interval_property(CARDINALITY_1);
      return r;
    }
    assert(false);
    return false;
  }

  bool has_restriction() const {
    return info().has_restriction();
  }

  I_Result normalize() const {
    assert(OK());
    if (has_restriction()) {
      Result rl = lower_normalize();
      Result ru = upper_normalize();
      // FIXME: this invalidation is not needed if interval is unchanged
      invalidate_cardinality_cache();
      info().normalize();
      assert(OK());
      return combine(rl, ru);
    }
    else
      return combine(V_EQ, V_EQ);
  }

  bool lower_is_open() const {
    assert(OK());
    return is_open(LOWER, lower(), info());
  }

  bool upper_is_open() const {
    assert(OK());
    return is_open(UPPER, upper(), info());
  }

  Result lower_shrink() {
    assert(OK());
    return shrink(LOWER, lower(), info(), false);
  }

  Result upper_shrink() {
    assert(OK());
    return shrink(UPPER, upper(), info(), false);
  }

  bool lower_is_unbounded() const {
    assert(OK());
    return Boundary_NS::is_unbounded(LOWER, lower(), info());
  }

  bool upper_is_unbounded() const {
    assert(OK());
    return Boundary_NS::is_unbounded(UPPER, upper(), info());
  }

  bool is_unbounded() const {
    assert(OK());
    return lower_is_unbounded() || upper_is_unbounded();
  }

  bool is_universe() const {
    assert(OK());
    return lower_is_unbounded() && upper_is_unbounded()
      && !has_restriction();
  }

  void invalidate_cardinality_cache() const {
    w_info().set_interval_property(CARDINALITY_IS, false);
    w_info().set_interval_property(CARDINALITY_0, false);
    w_info().set_interval_property(CARDINALITY_1, false);
  }

  template <typename T>
  Result lower_set_uninit(const T& x, bool open = false) {
    info().clear_boundary_properties(LOWER);
    Result rl = Boundary_NS::assign(LOWER, lower(), info(), LOWER, x, f_info(x, open));
    lower_load();
    return rl;
  }

  Result lower_set_uninit(const Unbounded&) {
    info().clear_boundary_properties(LOWER);
    Result rl = set_unbounded(LOWER, lower(), info());
    lower_load();
    return rl;
  }

  template <typename T>
  Result lower_set(const T& x, bool open = false) {
    assert(OK());
    info().clear_boundary_properties(LOWER);
    Result rl = Boundary_NS::assign(LOWER, lower(), info(), LOWER, x, f_info(x, open));
    invalidate_cardinality_cache();
    assert(OK());
    return rl;
  }

  Result lower_set(const Unbounded&) {
    assert(OK());
    info().clear_boundary_properties(LOWER);
    Result rl = set_unbounded(LOWER, lower(), info());
    invalidate_cardinality_cache();
    assert(OK());
    return rl;
  }

  template <typename T>
  Result lower_narrow(const T& x, bool open = false) {
    assert(OK());
    if (ge(LOWER, lower(), info(), LOWER, x, f_info(x, open)))
      return V_EQ;
    return lower_set(x, open);
  }

  template <typename T>
  Result lower_widen(const T& x, bool open = false) {
    assert(OK());
    if (le(LOWER, lower(), info(), LOWER, x, f_info(x, open)))
      return V_EQ;
    return lower_set(x, open);
  }

  Result lower_widen(const Unbounded&) {
    assert(OK());
    if (lower_is_unbounded())
      return V_EQ;
    info().clear_boundary_properties(LOWER);
    Result rl = set_unbounded(LOWER, lower(), info());
    invalidate_cardinality_cache();
    assert(OK());
    return V_EQ;
  }

  template <typename T>
  Result upper_set_uninit(const T& x, bool open = false) {
    info().clear_boundary_properties(UPPER);
    Result rl = Boundary_NS::assign(UPPER, upper(), info(), UPPER, x, f_info(x, open));
    upper_load();
    return rl;
  }
  Result upper_set_uninit(const Unbounded&) {
    info().clear_boundary_properties(UPPER);
    Result rl = set_unbounded(UPPER, upper(), info());
    upper_load();
    return rl;
  }

  template <typename T>
  Result upper_set(const T& x, bool open = false) {
    assert(OK());
    info().clear_boundary_properties(UPPER);
    Result rl = Boundary_NS::assign(UPPER, upper(), info(), UPPER, x, f_info(x, open));
    invalidate_cardinality_cache();
    assert(OK());
    return rl;
  }

  Result upper_set(const Unbounded&) {
    assert(OK());
    info().clear_boundary_properties(UPPER);
    Result rl = set_unbounded(UPPER, upper(), info());
    invalidate_cardinality_cache();
    assert(OK());
    return rl;
  }

  template <typename T>
  Result upper_narrow(const T& x, bool open = false) {
    assert(OK());
    if (le(UPPER, upper(), info(), UPPER, x, f_info(x, open)))
      return V_EQ;
    return upper_set(x, open);
  }

  template <typename T>
  Result upper_widen(const T& x, bool open = false) {
    assert(OK());
    if (ge(UPPER, upper(), info(), UPPER, x, f_info(x, open)))
      return V_EQ;
    return upper_set(x, open);
  }

  Result upper_widen(const Unbounded&) {
    assert(OK());
    if (upper_is_unbounded())
      return V_EQ;
    info().clear_boundary_properties(UPPER);
    Result rl = set_unbounded(UPPER, upper(), info());
    invalidate_cardinality_cache();
    assert(OK());
    return rl;
  }

  I_Result assign(Degenerate_Element e) {
    I_Result r;
    Result rl, ru;
    info().clear();
    switch (e) {
    case EMPTY:
      info().set_interval_property(CARDINALITY_IS);
      info().set_interval_property(CARDINALITY_0);
      lower_set_uninit(1);
      upper_set_uninit(0);
      r = I_EMPTY;
      break;
    case UNIVERSE:
      info().set_interval_property(CARDINALITY_0, true);
      info().set_interval_property(CARDINALITY_1, true);
      rl = lower_set_uninit(UNBOUNDED);
      ru = upper_set_uninit(UNBOUNDED);
      r = combine(rl, ru);
      break;
    default:
      assert(0);
      r = I_EMPTY;
      break;
    }
    complete_init();
    assert(OK());
    return r;
  }

  template <typename From>
  typename Enable_If<Is_Special<From>::value, I_Result>::type
  assign(const From&) {
    info().clear();
    info().set_interval_property(CARDINALITY_0, true);
    info().set_interval_property(CARDINALITY_1, true);
    Result rl, ru;
    switch (From::code) {
    case VC_MINUS_INFINITY:
      rl = Boundary_NS::set_minus_infinity(LOWER, lower(), info());
      ru = Boundary_NS::set_minus_infinity(UPPER, upper(), info());
      break;
    case VC_PLUS_INFINITY:
      rl = Boundary_NS::set_plus_infinity(LOWER, lower(), info());
      ru = Boundary_NS::set_plus_infinity(UPPER, upper(), info());
      break;
    default:
      assert(0);
      rl = VC_NAN;
      ru = VC_NAN;
    }
    complete_init_internal();
    assert(OK());
    return combine(rl, ru);
  }

  I_Result set_infinities() {
    info().clear();
    info().set_interval_property(CARDINALITY_0, true);
    info().set_interval_property(CARDINALITY_1, true);
    // FIXME: what about restrictions?
    Result rl = Boundary_NS::set_minus_infinity(LOWER, lower(), info());
    Result ru = Boundary_NS::set_plus_infinity(UPPER, upper(), info());
    complete_init_internal();
    assert(OK());
    return combine(rl, ru);
  }

  bool is_topologically_closed() const {
    assert(OK());
    return !Info::store_open
      || is_empty()
      || ((lower_is_unbounded() || !lower_is_open())
          && (upper_is_unbounded() || !upper_is_open()));
  }

  //! Assigns to \p *this its topological closure.
  void topological_closure_assign() {
    if (!Info::store_open || is_empty())
      return;

    if (!lower_is_unbounded())
      info().set_boundary_property(LOWER, OPEN, false);

    if (!upper_is_unbounded())
      info().set_boundary_property(UPPER, OPEN, false);
  }

  bool is_infinity() const {
    assert(OK());
    if (is_reverse_infinity(LOWER, lower(), info()))
      return 1;
    else if (is_reverse_infinity(UPPER, upper(), info()))
      return -1;
    else
      return 0;
  }

  bool contains_integer_point() const {
    assert(OK());
    if (is_empty())
      return false;
    if (is_unbounded())
      return true;
    Boundary l;
    if (lower_is_open()) {
      add_assign_r(l, lower(), Boundary(1), ROUND_DOWN);
      floor_assign_r(l, l, ROUND_DOWN);
    }
    else
      ceil_assign_r(l, lower(), ROUND_DOWN);
    Boundary u;
    if (upper_is_open()) {
      sub_assign_r(u, upper(), Boundary(1), ROUND_UP);
      ceil_assign_r(u, u, ROUND_UP);
    }
    else
      floor_assign_r(u, upper(), ROUND_UP);
    return u >= l;
  }

  //! Returns the total size in bytes of the memory occupied by \p *this.
  memory_size_type total_memory_in_bytes() const;

  //! Returns the size in bytes of the memory managed by \p *this.
  memory_size_type external_memory_in_bytes() const;

  void ascii_dump(std::ostream& s) const;
  bool ascii_load(std::istream& s);

  bool OK() const {
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    if (!completed) {
	std::cerr << "The interval initialization has not been completed."
                  << std::endl;
	return false;
    }
#endif
#if 0
    if (!Info::may_be_empty && is_empty()) {
#ifndef NDEBUG
	std::cerr << "The interval is unexpectedly empty.\n";
#endif
	return false;
    }
#endif

    if (is_open(LOWER, lower(), info())) {
      if (is_plus_infinity(LOWER, lower(), info())) {
#ifndef NDEBUG
	std::cerr << "The lower boundary is +inf open.\n";
#endif
      }
    }
    else if (!Info::may_contain_infinity
	     && (is_minus_infinity(LOWER, lower(), info())
		 || is_plus_infinity(LOWER, lower(), info()))) {
#ifndef NDEBUG
      std::cerr << "The lower boundary is unexpectedly infinity.\n";
#endif
      return false;
    }
    if (!info().get_boundary_property(LOWER, SPECIAL)) {
      if (is_not_a_number(lower())) {
#ifndef NDEBUG
	std::cerr << "The lower boundary is not a number.\n";
#endif
	return false;
      }
#if 0
      if (info().get_boundary_property(LOWER, NORMALIZED)
	  && !info().is_restricted(lower())) {
#ifndef NDEBUG
	std::cerr << "The lower boundary is marked to be normalized, "
                  << "but it is not.\n";
#endif
	return false;
      }
#endif
    }

    if (is_open(UPPER, upper(), info())) {
      if (is_minus_infinity(UPPER, upper(), info())) {
#ifndef NDEBUG
	std::cerr << "The upper boundary is -inf open.\n";
#endif
      }
    }
    else if (!Info::may_contain_infinity
	     && (is_minus_infinity(UPPER, upper(), info())
		 || is_plus_infinity(UPPER, upper(), info()))) {
#ifndef NDEBUG
      std::cerr << "The upper boundary is unexpectedly infinity."
                << std::endl;
#endif
      return false;
    }
    if (!info().get_boundary_property(UPPER, SPECIAL)) {
      if (is_not_a_number(upper())) {
#ifndef NDEBUG
	std::cerr << "The upper boundary is not a number.\n";
#endif
	return false;
      }
#if 0
      if (info().get_boundary_property(UPPER, NORMALIZED)
	  && !info().is_restricted(upper())) {
#ifndef NDEBUG
	std::cerr << "The upper boundary is marked to be normalized, "
                  << "but it is not.\n";
#endif
	return false;
      }
#endif
    }

    Ternary t;

    t = is_empty_cached();
    if (t == T_YES) {
      if (!is_empty_nocache()) {
#ifndef NDEBUG
	std::cerr << "The interval is marked to be empty, "
                  << "but actually it is not empty.\n";
#endif
	return false;
      }
    }
    else if (t == T_NO) {
      if (is_empty_nocache()) {
#ifndef NDEBUG
	std::cerr << "The interval is marked to be not empty, "
                  << "but actually it is empty.\n";
#endif
	return false;
      }
    }

    t = is_singleton_cached();
    if (t == T_YES) {
      if (!is_singleton_nocache()) {
#ifndef NDEBUG
	std::cerr << "The interval is marked to be singleton, "
                  << "but actually it is not singleton.\n";
#endif
	return false;
      }
    }
    else if (t == T_NO) {
      if (is_singleton_nocache()) {
#ifndef NDEBUG
	std::cerr << "The interval is marked to be not singleton, "
                  << "but actually it is singleton.\n";
#endif
	return false;
      }
    }

    if (info().get_interval_property(CARDINALITY_IS) &&
	info().get_interval_property(CARDINALITY_0)
        == info().get_interval_property(CARDINALITY_1)) {
#ifndef NDEBUG
      std::cerr << "The interval is marked to know its cardinality, "
                << "but this is unspecified or ambiguous.\n";
#endif
      return false;
    }

    // Everything OK.
    return true;
  }

  Interval()
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    : lower_loaded(0), upper_loaded(0), completed(0)
#endif
  {
  }

  template <typename T>
  explicit Interval(const T& x)
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
    : lower_loaded(0), upper_loaded(0), completed(0)
#endif
  {
    assign(x);
  }

  template <typename T>
  typename Enable_If<Is_Singleton<T>::value
                     || Is_Interval<T>::value, bool>::type
  contains(const T& y) const;

  template <typename T>
  typename Enable_If<Is_Singleton<T>::value
                     || Is_Interval<T>::value, bool>::type
  strictly_contains(const T& y) const;

  template <typename T>
  typename Enable_If<Is_Singleton<T>::value
                     || Is_Interval<T>::value, bool>::type
  is_disjoint_from(const T& y) const;

  template <typename From1, typename From2>
  I_Result assign(const From1& l, const From2& u);

  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  assign(const From& x);

  template <typename Type>
  typename Enable_If<Is_Singleton<Type>::value
                     || Is_Interval<Type>::value, bool>::type
  can_be_exactly_joined_to(const Type& x) const;

  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  join_assign(const From& x);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value
                       || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value
                          || Is_Interval<From2>::value)), I_Result>::type
  join_assign(const From1& x, const From2& y);

  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  intersect_assign(const From& x);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value
                       || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value
                          || Is_Interval<From2>::value)), I_Result>::type
  intersect_assign(const From1& x, const From2& y);

  /*! \brief
    Assigns to \p *this the smallest interval containing the set-theoretic
    difference of \p *this and \p x.
  */
  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  difference_assign(const From& x);

  /*! \brief
    Assigns to \p *this the smallest interval containing the set-theoretic
    difference of \p x and \p y.
  */
  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value
                       || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value
                          || Is_Interval<From2>::value)), I_Result>::type
  difference_assign(const From1& x, const From2& y);

  /*! \brief
    Assigns to \p *this the largest interval contained in the set-theoretic
    difference of \p *this and \p x.
  */
  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  lower_approximation_difference_assign(const From& x);

  /*! \brief
    Assigns to \p *this a \ref Meet_Preserving_Simplification
    "meet-preserving simplification" of \p *this with respect to \p y.

    \return
    \c false if and only if the meet of \p *this and \p y is empty.
  */
  template <typename From>
  typename Enable_If<Is_Interval<From>::value, bool>::type
  simplify_using_context_assign(const From& y);

  /*! \brief
    Assigns to \p *this an interval having empty intersection with \p y.
    The assigned interval should be as large as possible.

    \note
    Depending on interval restrictions, there could be many
    maximal intervals all inconsistent with respect to \p y.
  */
  template <typename From>
  typename Enable_If<Is_Interval<From>::value, void>::type
  empty_intersection_assign(const From& y);

  /*! \brief
    Refines \p to according to the existential relation \p rel with \p x.

    The \p to interval is restricted to become, upon successful exit,
    the smallest interval of its type that contains the set
    \f[
      \{\,
        a \in \mathtt{to}
      \mid
        \exists b \in \mathtt{x} \st a \mathrel{\mathtt{rel}} b
      \,\}.
    \f]
    \return
    ???
  */
  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  refine_existential(Relation_Symbol rel, const From& x);

  /*! \brief
    Refines \p to so that it satisfies the universal relation \p rel with \p x.

    The \p to interval is restricted to become, upon successful exit,
    the smallest interval of its type that contains the set
    \f[
      \{\,
        a \in \mathtt{to}
      \mid
        \forall b \in \mathtt{x} \itc a \mathrel{\mathtt{rel}} b
      \,\}.
    \f]
    \return
    ???
  */
  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  refine_universal(Relation_Symbol rel, const From& x);

  template <typename From>
  typename Enable_If<Is_Singleton<From>::value
                     || Is_Interval<From>::value, I_Result>::type
  neg_assign(const From& x);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value || Is_Interval<From2>::value)), I_Result>::type
  add_assign(const From1& x, const From2& y);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value || Is_Interval<From2>::value)), I_Result>::type
  sub_assign(const From1& x, const From2& y);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value || Is_Interval<From2>::value)), I_Result>::type
  mul_assign(const From1& x, const From2& y);

  template <typename From1, typename From2>
  typename Enable_If<((Is_Singleton<From1>::value || Is_Interval<From1>::value)
		      && (Is_Singleton<From2>::value || Is_Interval<From2>::value)), I_Result>::type
  div_assign(const From1& x, const From2& y);

  template <typename From, typename Iterator>
  typename Enable_If<Is_Interval<From>::value, void>::type
  CC76_widening_assign(const From& y, Iterator first, Iterator last);

private:
  Boundary lower_;
  Boundary upper_;
#ifdef PPL_ABI_BREAKING_EXTRA_DEBUG
  unsigned int lower_loaded:1;
  unsigned int upper_loaded:1;
  unsigned int completed:1;
#endif
};

} // namespace Parma_Polyhedra_Library

#include "Interval.inlines.hh"
#include "Interval.templates.hh"

#endif // !defined(PPL_Interval_defs_hh)
