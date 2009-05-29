/* Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is free software; as a special exception the author gives
unlimited permission to copy and/or distribute it, with or without
modifications, as long as this notice is preserved.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. */

#ifndef PPL_globals_types_hh
#define PPL_globals_types_hh 1

#include <cstddef>

namespace Parma_Polyhedra_Library {

//! An unsigned integral type for representing space dimensions.
/*! \ingroup PPL_CXX_interface */
typedef size_t dimension_type;

//! An unsigned integral type for representing memory size in bytes.
/*! \ingroup PPL_CXX_interface */
typedef size_t memory_size_type;

//! Kinds of degenerate abstract elements.
/*! \ingroup PPL_CXX_interface */
enum Degenerate_Element {
  //! The universe element, i.e., the whole vector space.
  UNIVERSE,
  //! The empty element, i.e., the empty set.
  EMPTY
};

//! Relation symbols.
/*! \ingroup PPL_CXX_interface */
enum Relation_Symbol {
  //! Less than.
  LESS_THAN,
  //! Less than or equal to.
  LESS_OR_EQUAL,
  //! Equal to.
  EQUAL,
  //! Greater than or equal to.
  GREATER_OR_EQUAL,
  //! Greater than.
  GREATER_THAN,
  //! Not equal to.
  NOT_EQUAL
};

//! Complexity pseudo-classes.
/*! \ingroup PPL_CXX_interface */
enum Complexity_Class {
  //! Worst-case polynomial complexity.
  POLYNOMIAL_COMPLEXITY,
  //! Worst-case exponential complexity but typically polynomial behavior.
  SIMPLEX_COMPLEXITY,
  //! Any complexity.
  ANY_COMPLEXITY
};

//! Possible optimization modes.
/*! \ingroup PPL_CXX_interface */
enum Optimization_Mode {
  //! Minimization is requested.
  MINIMIZATION,
  //! Maximization is requested.
  MAXIMIZATION
};

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_globals_types_hh)
