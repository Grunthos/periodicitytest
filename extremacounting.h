// Functions for counting local extrema of regular and folded time series in various ways.

# include "basics.h"

# ifndef EXTREMACOUNTING
# define EXTREMACOUNTING

IndexType count_extrema_plain ();
IndexType count_extrema_per_single_period ();
IndexType count_extrema_all_periods ();

# endif
