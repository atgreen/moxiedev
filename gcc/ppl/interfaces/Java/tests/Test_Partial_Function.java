/* Test_Partial_Function Java class of the Parma Polyhedra Library Java
   interface.
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

import java.util.TreeMap;
import parma_polyhedra_library.*;


// This class implements the Partial_Function interface defined
// in the parma_polyhedra_library package.
public class Test_Partial_Function implements Partial_Function {

    private TreeMap<Long, Long> map;
    private long max;

    public Test_Partial_Function() {
	map = new TreeMap<Long, Long>();
	max  = 0;
    }

    public boolean maps(Long i, By_Reference<Long> j) {
	if (map.containsKey(i)) {
	    j.set(map.get(i));
	    return true;
	}
        return false;
    }

    public long max_in_codomain() {
	return max;
    }

    public boolean has_empty_codomain() {
	return map.isEmpty();
    }

    void insert(long i, long j) {
	map.put(i, j);
	if (j > max)
	    max = j;
    }
}
