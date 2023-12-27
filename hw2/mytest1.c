#include <stdio.h>

void foo( )
{ 
	int x, y;
	int *p, *p2; 
  int **pp, **pp2;
  x = y;
  x = *p;
  x = **pp;
  *p = *p2;
  *p = x;
  *p = **pp;
  **pp = **pp2;
  **pp = *p;
  **pp = x;
  // ((a+d) + x ) + y
  // tmp = a+d
  // tmp1 = tmp + x
  // tmp2 = tmp1 + y
  //
// 	g = a + d + x +y; 
// 	*p = i + 2; 
// 	h = a + d + x +y; 
// 	f = y + 1; 
}
