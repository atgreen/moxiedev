/* Top level for The Incredible Moxielyzer (C) 2009 Anthony Green

This file is part of The Incredible Moxielyzer

The Incredible Moxielyzer is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3, or (at
your option) any later version.

The Incredible Moxielyzer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Incredible Moxielyzer; see the file COPYING3.  If not
see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bfd.h>

#include "moxielyzer.h"

struct plugin *plugins[100];
unsigned plugin_count = 0;

void 
register_plugin (struct plugin *p)
{
  plugins[plugin_count++] = p;
}

struct plugin *
find_plugin (char *name)
{
  int i;
  for (i = 0; plugins[i]; i++)
    {
      if (plugins[i] && strcmp (name, plugins[i]->name) == 0)
	return plugins[i];
    }
  return 0;
}

static void 
usage_list_plugins (void)
{
  int i;
  printf ("Usage: moxielyzer PLUGIN FILENAME\n");
  printf ("List of PLUGIN values: %s\n", plugins[0]->name);
  for (i = 1; plugins[i]; i++)
    printf ("                       %s\n", plugins[i]->name);
  printf ("Run \"moxielyzer PLUGIN\" alone for a plugin description.\n");
  exit (1);
}

static void 
usage_desc_plugin (char *name)
{
  struct plugin *p = find_plugin (name);
  printf ("Usage: moxielyzer %s FILENAME\n", name);
  puts (p->desc);
  exit (1);
}

static void
analyze (bfd *abfd, asection *section, void *vanalyzer)
{
  bfd_size_type datasize = 0;
  bfd_byte *data = NULL;
  bfd_vma *addr;

  if (section->flags & SEC_CODE)
    {
      datasize = bfd_get_section_size (section);
      if (datasize == 0)
	return;

      data = malloc (datasize);
      
      bfd_get_section_contents (abfd, section, data, 0, datasize);
      
      moxielyze (datasize, data, addr, (moxie_analyzer *)vanalyzer);

      free (data);
    }
}

static int
compare_pnames (const void * a, const void * b)
{
  return strcmp (((struct plugin *) b)->name,
		 ((struct plugin *) a)->name);
}

int 
main (int argc, char *argv[])
{
  bfd *abfd;
  char *pluginame = argv[1];
  char *filename = argv[2];
  struct plugin *p = plugins[0];

  /* Sort the plugins by name.  */
  qsort(plugins, plugin_count, sizeof(struct plugin *), compare_pnames);

  if (argc == 1)
    usage_list_plugins();
  else if (argc == 2)
    usage_desc_plugin(pluginame);
  else if (argc > 3)
    usage_list_plugins();

  p = find_plugin (pluginame);
  if (p == 0)
    {
      fprintf (stderr, "moxielyzer: missing plugin \"%s\"\n", pluginame);
      exit (1);
    }

  abfd = bfd_openr (filename, "elf32-moxie");

  if (bfd_check_format (abfd, bfd_object))
    {
      moxie_analyzer analyzer;
      memset (&analyzer, 0, sizeof(moxie_analyzer));

      analyzer.plugin = p;
      if (p->init)
	(p->init)(&analyzer);

      bfd_map_over_sections (abfd, analyze, &analyzer);

      if (p->fini)
	(p->fini)(&analyzer);
    }
  else
    {
      fprintf (stderr, "moxielyzer: wrong file type.\n");
      exit (1);
    }

  bfd_close (abfd);
}
