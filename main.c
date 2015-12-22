#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asm.h"
#define AARCH64_TIMING
#include "timing.c"


#define FIELD(name) void name(void *dest, void *src);
#include "test.def"
#undef FIELD

struct test
{
  const char *name;
  void (*func)(void *dest, void *src);
}tests[] = 
{
#define FIELD(name) { #name, name },
#include "test.def"
#undef FIELD
{ NULL, NULL }
};


int main(void)
{
   void *pagesrc = NULL;
   void *pagedst = NULL;
   int i;
   posix_memalign (&pagesrc, PAGE_SIZE, PAGE_SIZE);
   posix_memalign (&pagedst, PAGE_SIZE, PAGE_SIZE);

   for(i = 0; tests[i].name != NULL; i++)
     {
	memset (pagesrc, i, PAGE_SIZE);
	start_time();
	tests[i].func(pagedst, pagesrc);
	stop_time();
	printf ("%s: ", tests[i].name);
	print_time();
	printf ("\n");
     }
  return 0;
}
