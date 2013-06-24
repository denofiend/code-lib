#include <ctype.h>

typedef int CHARSET_INFO;

#define my_isspace(a,b) isblank(b)
#define my_isvar(a,b)   isalnum(b)

#warning Using the Test Harness

