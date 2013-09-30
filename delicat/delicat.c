/* 
   delicat - Allows concatenation of streams so that de-concatenation is possible.

   Copyright (C) 2013 Tassos Bassoukos

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

#include <termios.h>
#include <grp.h>
#include <pwd.h>
*/

#include <stdio.h>
#include <sys/types.h>
#include "system.h"
#include "delicat-opt.h"

char *xmalloc ();
char *xrealloc ();
char *xstrdup ();

int
main (int argc, char **argv)
{

  {
    int arg_ct = optionProcess( &delicatOptions, argc, argv );
    argc -= arg_ct;
    /*
    if ((argc < ARGC_MIN) || (argc > ARGC_MAX)) 
      {
	fprintf( stderr, "%s ERROR:  remaining args (%d) "
		 "out of range\n", delicatOptions.pzProgName,
		 argc );
	
	USAGE( EXIT_FAILURE );
      }
    */
    argv += arg_ct;
  }

  /*
  if (HAVE_OPT(OPT_NAME))
    respond_to_opt_name();
  */

  /* do the work */

  return EXIT_SUCCESS;
}
