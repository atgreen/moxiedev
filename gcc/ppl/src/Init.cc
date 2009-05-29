/* Init class implementation (non-inline functions and static variables).
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

#include <ppl-config.h>

#include "Init.defs.hh"
#include "Variable.defs.hh"
#include "fpu.defs.hh"
#include "Rounding_Dir.defs.hh"
#include "checked.defs.hh"
#include "Coefficient.defs.hh"
#include "Linear_Expression.defs.hh"
#include "Constraint.defs.hh"
#include "Generator.defs.hh"
#include "Congruence.defs.hh"
#include "Grid_Generator.defs.hh"
#include "Constraint_System.defs.hh"
#include "Generator_System.defs.hh"
#include "Congruence_System.defs.hh"
#include "Grid_Generator_System.defs.hh"
#include "Polyhedron.defs.hh"

namespace PPL = Parma_Polyhedra_Library;

unsigned int PPL::Init::count = 0;

PPL::fpu_rounding_direction_type PPL::Init::old_rounding_direction;

extern "C" void
ppl_set_GMP_memory_allocation_functions(void)
#if PPL_CXX_SUPPORTS_ATTRIBUTE_WEAK
  __attribute__((weak));

void
ppl_set_GMP_memory_allocation_functions(void) {
}
#else
  ;
#endif

PPL::Init::Init() {
  // Only when the first Init object is constructed...
  if (count++ == 0) {
    // ... the GMP memory allocation functions are set, ...
    ppl_set_GMP_memory_allocation_functions();
    // ... the default output function for Variable objects is set, ...
    Variable::set_output_function(Variable::default_output_function);
    // ... the Coefficient constants are initialized, ...
    Coefficient_constants_initialize();
    // ... the Linear_Expression class is initialized, ...
    Linear_Expression::initialize();
    // ... the Constraint, Generator, Congruence, Grid_Generator,
    // Constraint_System, Generator_System, Congruence_System,
    // Grid_Generator_System and Polyhedron classes are initialized, ...
    Constraint::initialize();
    Generator::initialize();
    Congruence::initialize();
    Grid_Generator::initialize();
    Constraint_System::initialize();
    Generator_System::initialize();
    Congruence_System::initialize();
    Grid_Generator_System::initialize();
    Polyhedron::initialize();
#if PPL_CAN_CONTROL_FPU
    // ... and the FPU rounding direction is set.
    fpu_initialize_control_functions();
    old_rounding_direction = fpu_get_rounding_direction();
    fpu_set_rounding_direction(round_fpu_dir(ROUND_DIRECT));
#endif
    // The default is choosen to have a precision greater than most
    // precise IEC559 floating point (112 bits of mantissa).
    set_rational_sqrt_precision_parameter(128);
  }
}

PPL::Init::~Init() {
  // Only when the last Init object is destroyed...
  if (--count == 0) {
#if PPL_CAN_CONTROL_FPU
    // ... the FPU rounding direction is restored, ...
    fpu_set_rounding_direction(old_rounding_direction);
#endif
    // ... the Polyhedron, Grid_Generator_System, Congruence_System,
    // Generator_System, Constraint_System, Grid_Generator,
    // Congruence, Generator and Constraint classes are finalized
    // IN THAT ORDER, ...
    Polyhedron::finalize();
    Grid_Generator_System::finalize();
    Congruence_System::finalize();
    Generator_System::finalize();
    Constraint_System::finalize();
    Grid_Generator::finalize();
    Congruence::finalize();
    Generator::finalize();
    Constraint::finalize();
    // ... the Linear_Expression class is finalized, ...
    Linear_Expression::finalize();
    // ... and the Coefficient constants are finalized.
    Coefficient_constants_finalize();
  }
}
