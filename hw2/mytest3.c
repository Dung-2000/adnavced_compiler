#include <stdio.h>

void foo( )
{ 
	// int x, y;
	int *p, *p2; 
  int **pp, **pp2;

  pp = pp2;
  pp = &p;

  p = p2;
}
