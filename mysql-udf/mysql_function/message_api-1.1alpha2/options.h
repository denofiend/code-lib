/* Copyright (C) 2006 MySQL AB

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/

#if defined UNIT_TEST
#include "harness.h"
#else

#include <my_global.h> 
#include <my_sys.h>

#endif

#include <ctype.h>
#include <string.h>

struct option_list {
  const char *    name;
  int             max_value_len;
  char *          value;
  unsigned int    value_len;
};
typedef struct option_list      option_list;  

enum opt_parser_return {
  PARS_OK = 0,
  PARS_ILLEGAL_OPTION,
  PARS_SYNTAX_ERROR,
  
};
typedef enum opt_parser_return  opt_parser_return;

enum {
  BINSEARCH_NOT_FOUND = -1
};

/* Prototypes */
opt_parser_return parse_options(int, option_list *, 
                                unsigned int, unsigned int *, char *);


