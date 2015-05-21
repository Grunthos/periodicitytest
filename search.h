// The main interface if you just want to apply the test from C.

# include "basics.h"
# include "interval.h"

# ifndef PERIODICITYTEST
# define PERIODICITYTEST

Interval periodicity_test (TimeSeries const T, IndexType const max_tau, ValueType const sigma);
// Returns the first maximal interval (p/q,r/s) such that T complies with any period length from this interval and p/q<max_tau. T may or may not comply with a period length of r/s.
// Returns not_periodic (see interval.h), if no such interval exists.
// Performs no sanity checks on the input.

# endif
