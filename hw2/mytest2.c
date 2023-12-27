
#include <stdio.h>

void foo( )
{ 
	int x, y;
	int *p, *p2; 
  int **pp, **pp2;

  p = &x;
  p = *pp;
  p = p2;

  *pp = &x;
  *pp = p;
  *pp = *pp2;
}
