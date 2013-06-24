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

#include "options.h"

/* binsearch():
   Binary search through a sorted list of keywords.
*/
static int binsearch(char *word, int len, option_list *opts, int n) {
  int cond, low, mid, high;

  low = 0;
  high = n - 1;
  while (low <= high) {
    mid = (low + high) / 2;
    if ((cond = strncasecmp(word, opts[mid].name,len)) < 0)
        high = mid - 1;
    else if (cond > 0)
        low = mid + 1;
    else
        return mid;
  }
  return BINSEARCH_NOT_FOUND;
}

/* 
   parse_options()
   
   This parser handles the syntax "option = value [, option = value ] ...".
   The option parser does not allocate or copy any strings.  If an option is
   found in the string *s, then opts[idx].value will be set to a pointer 
   (within s) to the given value, and opts[idx].value_len will indicate its 
   length, where idx is the index in opts[] of the named option.
   opts[] is searched as a btree, so it MUST be sorted (ascending & 
   case-insensitive) on the option name.
*/
#define skip_whitespace(p) while(*p && isspace(*p)) p++ 

opt_parser_return parse_options(int n_opts, option_list *opts,
                                unsigned int valid_opts, unsigned int *set_opts,
                                char *s) {

  char *opt, *val, *r;
  int i, j;
  int optidx;

  opt = s;
  *set_opts = 0;
  do {
    i = j = 0 ;
    skip_whitespace(opt);

    /* the first word is the option name, extending from "p" for length "i" */
    for( val = opt; *val && isalpha(*val) ; val++, i++);	

    /* The option is the string of length "i" beginning at "opt" */
    if(i == 0) return PARS_SYNTAX_ERROR;
    optidx = binsearch(opt, i, opts, n_opts);
    if(optidx == BINSEARCH_NOT_FOUND) return PARS_ILLEGAL_OPTION;
    if(valid_opts & ( 1 << optidx))
      *set_opts |= (1 << optidx); 
    else
      return PARS_ILLEGAL_OPTION;
        
    /* Next, there must be an equals sign */
    skip_whitespace(val);
    if(*val == '=') val++;
    else return PARS_SYNTAX_ERROR;
    skip_whitespace(val);
 
    /* Everything until the next comma or whitespace is the value */ 
    for( r = val ;
         *r && *r != ',' && (! isspace(*r)) ; 
         r++, j++);

    /* The value is the string of length "j" beginning at "val" */
    opts[optidx].value = val;
    opts[optidx].value_len =
      ( j > opts[optidx].max_value_len ? opts[optidx].max_value_len : j );
    
    skip_whitespace(r);
    opt = r+1;

  } while(*r == ',');

  if (*r == '\0') return PARS_OK;
  return PARS_SYNTAX_ERROR;
}
