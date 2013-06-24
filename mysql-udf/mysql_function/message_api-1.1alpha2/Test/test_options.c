#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sp.h"
#include "api_options.h"

CHARSET_INFO my_charset_latin1 ;

void iterate(option_list *opts, int sz) {
  int i;
  char *p;
  
  p = malloc(512);

  for( i = 0 ; i < sz ; i++) {
    if(opts[i].value) {
      strcpy(p,"");
      strncat(p, opts[i].value, opts[i].value_len);
      printf("       %-10s -> %-15s (%d)\n",opts[i].name, p, opts[i].value_len);
    }
  }   
}


int main() {
  opt_parser_return r;
  unsigned int valid_opts, set_opts;
  option_list *Options;

  Options = malloc(sizeof(all_api_options));
  memcpy(Options, all_api_options, sizeof(all_api_options));

  char *test[] = {
    "name = fred" ,                          /* these are good */
    "name = joe, handle=white",
    "name=joe,handle=blue",
    "NAME = joseph",
    "name = !!(*$&^&%*",
    "group = orders , name=bob,handle=12",
    "group = 123456789abcdefghijklmnopqrst",  /* too long */
    "=",                                    /* these are bad */
    " , ",
    "= ,",
    "rollercoaster = thunder road",
    ",,==,",
    "name ",
    "  name",
    " = kpe",
    "GROUP = AAA, name",
    "group = mimoo =",
    "@(#*%&^A", 
    NULL
  };
  int i, j;

  valid_opts = (OPF_name | OPF_group | OPF_handle) ;
  for(i = 0 ; test[i] ; i++) {
    r = parse_options(N_API_OPTIONS, Options, valid_opts, &set_opts, test[i]) ;
    printf("%-40s -- %sparsed \n", test[i], (r ? "not " : ""));
    if(r == PARS_OK)
      iterate(Options, N_API_OPTIONS);
    for(j = 0 ; j < N_API_OPTIONS ; j++) {
      Options[j].value = 0;
      Options[j].value_len = 0;
    }
  }
}
