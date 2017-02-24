// Defines a central data structure for the output and internal purposes.

# include "basics.h"
# include <stdbool.h>

# ifndef INTERVAL
# define INTERVAL

// Struct for the interval (p/q, r/s].
typedef struct Interval
{
	IndexType p;
	IndexType q;
	IndexType r;
	IndexType s;
} Interval;

// Define special value to be returned when the periodicity test fails. This can be anything that cannot be returned otherwise. is_periodic() has to be defined accordingly.
# define not_periodic ((Interval) {0,0,0,0})

static inline bool is_periodic (Interval const B)
{
	return ((B.p!=0) && (B.q!=0) && (B.r!=0) && (B.s!=0));
}

// If you just want to apply the test, you can stop here.
// ======================================================

// Returns the fractions with a numerator smaller than n that are closest to x.
Interval closest_fractions (double x, IndexType n);

bool compare_Interval ();
bool contains_Interval ();

static inline Interval left_partition (Interval const B)
{
	return (Interval) {B.p, B.q, B.p+B.r, B.q+B.s};
}

static inline Interval right_partition (Interval const B)
{
	return (Interval) {B.p+B.r, B.q+B.s, B.r, B.s};
}

static inline bool share_right_border (Interval const A, Interval const B)
{
	return ( (A.r==B.r) && (A.s==B.s) );
}

// Return the union A and B if it is an interval, otherwise return A.
Interval merge_if_possible (Interval const A, Interval const B);

# endif
