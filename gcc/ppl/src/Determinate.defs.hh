/* Determinate class declaration.
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

#ifndef PPL_Determinate_defs_hh
#define PPL_Determinate_defs_hh

#include "Determinate.types.hh"
#include "Constraint_System.types.hh"
#include "Congruence_System.types.hh"
#include "Variable.defs.hh"
#include "globals.types.hh"
#include <iosfwd>
#include <cassert>

namespace Parma_Polyhedra_Library {

/*! \brief
  Returns <CODE>true</CODE> if and only if
  \p x and \p y are the same domain element.

  \relates Determinate
*/
template <typename PS>
bool operator==(const Determinate<PS>& x, const Determinate<PS>& y);

/*! \brief
  Returns <CODE>true</CODE> if and only if
  \p x and \p y are different domain elements.

  \relates Determinate
*/
template <typename PS>
bool operator!=(const Determinate<PS>& x, const Determinate<PS>& y);

namespace IO_Operators {

//! Output operator.
/*! \relates Parma_Polyhedra_Library::Determinate */
template <typename PS>
std::ostream&
operator<<(std::ostream&, const Determinate<PS>&);

} // namespace IO_Operators

} // namespace Parma_Polyhedra_Library

//! Wraps a PPL class into a determinate constraint system interface.
/*! \ingroup PPL_CXX_interface */
template <typename PS>
class Parma_Polyhedra_Library::Determinate {
public:
  //! \name Constructors and Destructor
  //@{

  /*! \brief
    Injection operator: builds the determinate constraint system element
    corresponding to the base-level element \p p.
  */
  Determinate(const PS& p);

  /*! \brief
    Injection operator: builds the determinate constraint system element
    corresponding to the base-level element represented by \p cs.
  */
  Determinate(const Constraint_System& cs);

  //! \brief
  //! Injection operator: builds the determinate constraint system element
  //! corresponding to the base-level element represented by \p cgs.
  Determinate(const Congruence_System& cgs);

  //! Copy constructor.
  Determinate(const Determinate& y);

  //! Destructor.
  ~Determinate();

  //@} // Constructors and Destructor

  //! \name Member Functions that Do Not Modify the Domain Element
  //@{

  //! Returns a const reference to the embedded element.
  const PS& element() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is the top of the
    determinate constraint system (i.e., the whole vector space).
  */
  bool is_top() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is the bottom
    of the determinate constraint system.
  */
  bool is_bottom() const;

  //! Returns <CODE>true</CODE> if and only if \p *this entails \p y.
  bool definitely_entails(const Determinate& y) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this and \p y
    are equivalent.
  */
  bool is_definitely_equivalent_to(const Determinate& y) const;

  /*! \brief
    Returns a lower bound to the total size in bytes of the memory
    occupied by \p *this.
  */
  memory_size_type total_memory_in_bytes() const;

  /*! \brief
    Returns a lower bound to the size in bytes of the memory
    managed by \p *this.
  */
  memory_size_type external_memory_in_bytes() const;

  /*!
    Returns <CODE>true</CODE> if and only if this domain
    has a nontrivial weakening operator.
  */
  static bool has_nontrivial_weakening();

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  //@} // Member Functions that Do Not Modify the Domain Element


  //! \name Member Functions that May Modify the Domain Element
  //@{

  //! Assigns to \p *this the upper bound of \p *this and \p y.
  void upper_bound_assign(const Determinate& y);

  //! Assigns to \p *this the meet of \p *this and \p y.
  void meet_assign(const Determinate& y);

  //! Assigns to \p *this the result of weakening \p *this with \p y.
  void weakening_assign(const Determinate& y);

  /*! \brief
    Assigns to \p *this the \ref Concatenating_Polyhedra "concatenation"
    of \p *this and \p y, taken in this order.
  */
  void concatenate_assign(const Determinate& y);

  //! Returns a reference to the embedded element.
  PS& element();

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  /*! \brief
    On return from this method, the representation of \p *this
    is not shared by different Determinate objects.
  */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
  void mutate();

  //! Assignment operator.
  Determinate& operator=(const Determinate& y);

  //! Swaps \p *this with \p y.
  void swap(Determinate& y);

  //@} // Member Functions that May Modify the Domain Element

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  //! A function adapter for the Determinate class.
  /*! \ingroup PPL_CXX_interface
    It lifts a Binary_Operator_Assign function object, taking arguments
    of type PS, producing the corresponding function object taking
    arguments of type Determinate<PS>.

    The template parameter Binary_Operator_Assign is supposed to
    implement an <EM>apply and assign</EM> function, i.e., a function
    having signature <CODE>void foo(PS& x, const PS& y)</CODE> that
    applies an operator to \c x and \c y and assigns the result to \c x.
    For instance, such a function object is obtained by
    <CODE>std::mem_fun_ref(&C_Polyhedron::intersection_assign)</CODE>.
  */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
  template <typename Binary_Operator_Assign>
  class Binary_Operator_Assign_Lifter {
  public:
    //! Explicit unary constructor.
    explicit
    Binary_Operator_Assign_Lifter(Binary_Operator_Assign op_assign);

    //! Function-application operator.
    void operator()(Determinate& x, const Determinate& y) const;

  private:
    //! The function object to be lifted.
    Binary_Operator_Assign op_assign_;
  };

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  /*! \brief
    Helper function returning a Binary_Operator_Assign_Lifter object,
    also allowing for the deduction of template arguments.
  */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
  template <typename Binary_Operator_Assign>
  static Binary_Operator_Assign_Lifter<Binary_Operator_Assign>
  lift_op_assign(Binary_Operator_Assign op_assign);

private:
  //! The possibly shared representation of a Determinate object.
  /*! \ingroup PPL_CXX_interface
    By adopting the <EM>copy-on-write</EM> technique, a single
    representation of the base-level object may be shared by more than
    one object of the class Determinate.
  */
  class Rep {
  private:
    /*! \brief
      Count the number of references:
      -   0: leaked, \p ph is non-const;
      -   1: one reference, \p ph is non-const;
      - > 1: more than one reference, \p ph is const.
    */
    mutable unsigned long references;

    //! Private and unimplemented: assignment not allowed.
    Rep& operator=(const Rep& y);

    //! Private and unimplemented: copies not allowed.
    Rep(const Rep& y);

    //! Private and unimplemented: default construction not allowed.
    Rep();

  public:
    //! A possibly shared base-level domain element.
    PS ph;

    /*! \brief
      Builds a new representation by creating a domain element
      of the specified kind, in the specified vector space.
    */
    Rep(dimension_type num_dimensions, Degenerate_Element kind);

    //! Builds a new representation by copying base-level element \p p.
    Rep(const PS& p);

    //! Builds a new representation by copying the constraints in \p cs.
    Rep(const Constraint_System& cs);

    //! Builds a new representation by copying the constraints in \p cgs.
    Rep(const Congruence_System& cgs);

    //! Destructor.
    ~Rep();

    //! Registers a new reference.
    void new_reference() const;

    /*! \brief
      Unregisters one reference; returns <CODE>true</CODE> if and only if
      the representation has become unreferenced.
    */
    bool del_reference() const;

    //! True if and only if this representation is currently shared.
    bool is_shared() const;

    /*! \brief
      Returns a lower bound to the total size in bytes of the memory
      occupied by \p *this.
    */
    memory_size_type total_memory_in_bytes() const;

    /*! \brief
      Returns a lower bound to the size in bytes of the memory
      managed by \p *this.
    */
    memory_size_type external_memory_in_bytes() const;
  };

  /*! \brief
    A pointer to the possibly shared representation of
    the base-level domain element.
  */
  Rep* prep;

  friend bool
  operator==<PS>(const Determinate<PS>& x, const Determinate<PS>& y);
  friend bool
  operator!=<PS>(const Determinate<PS>& x, const Determinate<PS>& y);
};


namespace std {

//! Specializes <CODE>std::swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Determinate */
template <typename PS>
void swap(Parma_Polyhedra_Library::Determinate<PS>& x,
	  Parma_Polyhedra_Library::Determinate<PS>& y);

} // namespace std

#include "Determinate.inlines.hh"

#endif // !defined(PPL_Determinate_defs_hh)
