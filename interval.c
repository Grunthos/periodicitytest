# include "interval.h"

Interval closest_fractions (double x, IndexType n)
{
	Interval B = (Interval) {0, 1, 1, 0};
	
	while (B.p+B.r < n)
	{
		if (B.p+B.r < x*(B.q+B.s))
		{
			B.p = B.p+B.r;
			B.q = B.q+B.s;
		}
		else
		{
			B.r = B.p+B.r;
			B.s = B.q+B.s;
		}
	}
	
	return B;
}

bool compare_Interval (Interval A, Interval B)
{
	return (A.p==B.p) && (A.q==B.q) && (A.r==B.r) && (A.s==B.s);
}

bool contains_Interval (Interval A, Interval B)
{
	return is_periodic(A) && (A.p*B.q <= B.p*A.q) && (B.r*A.s <= A.r*B.s);
}

Interval merge_if_possible (Interval const A, Interval const B)
{
	if ( is_periodic(B) && (A.r == B.p) && (A.s == B.q) )
		return (Interval) {A.p, A.q, B.r, B.s};
	else
		return A;
}
