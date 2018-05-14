/* Return specific DWARF attribute of a DIE, integrating indirections.
   Copyright (C) 2005, 2018 Red Hat, Inc.
   This file is part of elfutils.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <dwarf.h>
#include "libdwP.h"

Dwarf_Attribute *
dwarf_attr_integrate (Dwarf_Die *die, unsigned int search_name,
		      Dwarf_Attribute *result)
{
  Dwarf_Die die_mem;

  do
    {
      Dwarf_Attribute *attr = INTUSE(dwarf_attr) (die, search_name, result);
      if (attr != NULL)
	return attr;

      attr = INTUSE(dwarf_attr) (die, DW_AT_abstract_origin, result);
      if (attr == NULL)
	attr = INTUSE(dwarf_attr) (die, DW_AT_specification, result);
      if (attr == NULL)
	break;

      die = INTUSE(dwarf_formref_die) (attr, &die_mem);
    }
  while (die != NULL);

  /* Not NULL if it didn't have abstract_origin and specification
     attributes.  If it is a split CU then see if the skeleton
     has it.  */
  if (die != NULL && is_cudie (die)
      && die->cu->unit_type == DW_UT_split_compile)
    {
      Dwarf_CU *skel_cu = __libdw_find_split_unit (die->cu);
      if (skel_cu != NULL)
	{
	  Dwarf_Die skel_die = CUDIE (skel_cu);
	  return INTUSE(dwarf_attr) (&skel_die, search_name, result);
	}
    }
  return NULL;
}
INTDEF (dwarf_attr_integrate)
