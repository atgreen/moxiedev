/* Pending_List class declaration.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Watchdog Library (PWL).

The PWL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PWL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PWL_Pending_List_defs_hh
#define PWL_Pending_List_defs_hh 1

#include "Pending_List.types.hh"
#include "Pending_Element.defs.hh"
#include "EList.defs.hh"
#include "Time.types.hh"
#include "Handler.types.hh"

//! An ordered list for recording pending watchdog events.
class Parma_Watchdog_Library::Pending_List {
public:
  //! A non-const iterator to traverse the list.
  typedef EList<Pending_Element>::Iterator Iterator;

  //! Constructs an empty list.
  Pending_List();

  //! Destructor.
  ~Pending_List();

  //! Inserts a new Pending_Element object with the given attributes.
  Iterator insert(const Time& deadline,
		  const Handler& handler,
		  bool& expired_flag);

  /*! \brief
    Removes the element pointed to by \p position, returning
    an iterator pointing to the next element, if any, or end(), otherwise.
  */
  Iterator erase(Iterator position);

  //! Returns <CODE>true</CODE> if and only if the list is empty.
  bool empty() const;

  //! Returns an iterator pointing to the beginning of the list.
  Iterator begin();

  //! Returns an iterator pointing one past the last element in the list.
  Iterator end();

  //! Checks if all the invariants are satisfied.
  bool OK() const;

private:
  EList<Pending_Element> active_list;
  EList<Pending_Element> free_list;
};

#include "Pending_List.inlines.hh"

#endif // !defined(PWL_Pending_List_defs_hh)
