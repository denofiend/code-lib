/*************************************************************************
  > File Name: foo.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2012年12月29日 星期六 15时08分16秒
 ************************************************************************/

/* if your system is linux*/
#define _GNU_SOURCE 

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (*add_pt)(int a, int b);
static add_pt orig_add;

int add(int a, int b){
	return orig_add(a, b);
}


/* call before main()*/
__attribute__((constructor)) static void __init_scheduler() {

#define INIT_SYSCALL(name) orig_##name = (name##_pt)dlsym(RTLD_NEXT, #name)
//#define INIT_SYSCALL(name) orig_##name = (name##_pt)dlsym(RTLD_DEFAULT, #name)

	INIT_SYSCALL(add);

#undef INIT_SYSCALL
}

int main(int argc, char **argv) {
	printf("%d\n", orig_add(1, 2));
	return 0;
}

