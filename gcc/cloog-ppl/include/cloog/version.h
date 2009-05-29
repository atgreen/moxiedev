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


#ifndef CLOOG_VERSION_H
#define CLOOG_VERSION_H
#if defined(__cplusplus)
extern "C" 
  {
#endif 

#define CLOOG_BITS "64"

/* The major number of the CLooG version.  */
#define CLOOG_VERSION_MAJOR 0

/* The minor number of the CLooG version.  */
#define CLOOG_VERSION_MINOR 15

/* The revision number of the CLooG version.  */
#define CLOOG_VERSION_REVISION 0

/* The beta number of the CLooG version.  This is zero for official
   releases and nonzero for development snapshots.  */
#define CLOOG_VERSION_BETA 0

/* A string containing the CLooG version.

  Let M and m denote the numbers associated to CLooG_VERSION_MAJOR and
  CLooG_VERSION_MINOR, respectively.  The format of CLooG_VERSION is M
  "." m if both CLooG_VERSION_REVISION (r) and CLooG_VERSION_BETA
  (b) are zero, M "." m "pre" b if CLooG_VERSION_REVISION is zero and
  CLooG_VERSION_BETA is not zero, M "." m "." r if
  CLooG_VERSION_REVISION is not zero and CLooG_VERSION_BETA is zero, M
  "." m "." r "pre" b if neither CLooG_VERSION_REVISION nor
  CLooG_VERSION_BETA are zero.  */
#define CLOOG_VERSION "0.15"

extern int ppl_version_major (void);
extern int ppl_version_minor (void);
extern int ppl_version_revision (void);
extern int ppl_version_beta (void);
extern const char *cloog_version(void);

#if defined(__cplusplus)
  }
#endif 
#endif /* define _H */
