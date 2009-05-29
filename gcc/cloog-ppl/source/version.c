/******************************************************************************
 *               CLooG : the Chunky Loop Generator (experimental)             *
 ******************************************************************************
 *                                                                            *
 * Copyright (C) 2001-2008 Cedric Bastoul                                     *
 *                                                                            *
 * This is free software; you can redistribute it and/or modify it under the  *
 * terms of the GNU General Public License as published by the Free Software  *
 * Foundation; either version 2 of the License, or (at your option) any later *
 * version.                                                                   *
 *                                                                            *
 * This software is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with software; if not, write to the Free Software Foundation, Inc.,        *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                     *
 *                                                                            *
 * CLooG, the Chunky Loop Generator                                           *
 * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                         *
 *                                                                            *
 ******************************************************************************/

#include "../include/cloog/version.h"

int cloog_version_major (void)
{
  return CLOOG_VERSION_MAJOR;
}

int cloog_version_minor (void)
{
  return CLOOG_VERSION_MINOR;
}

int cloog_version_revision (void)
{
  return CLOOG_VERSION_REVISION;
}

int cloog_version_beta (void)
{
  return CLOOG_VERSION_BETA;
}

const char *cloog_version(void)
{
  return "CLooG "CLOOG_VERSION" "CLOOG_BITS" bits";
}

