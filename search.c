# include "search.h"
# include "extremacounting.h"
# include <stdlib.h>
# include <assert.h>
# include <stdbool.h>
# include <stdio.h>

TimeSeries T;
ValueType sigma;
IndexType max_tau;
IndexType extrema;

static inline bool passes_thourough_check(Interval const B)
{
	IndexType max = 1 + extrema / ((L(length(T))*L(B.s)) / B.r);
	IndexType result = count_extrema_all_periods (T, B, max, sigma);
	return ( extrema == result);
}

static inline bool passes_quick_check(Interval const B)
{
	assert (B.s != 0);
 	IndexType max = 1 + extrema / ((L(length(T))*L(B.s)) / B.r);	
	assert (B.p+B.r<=length(T));
	
	return (count_extrema_per_single_period (T, B, max, sigma) <= max);
	
	// For randomising the checked segment (no empirical runtime improvement):
	// IndexType offset = rand() % (length(T)-B.p-B.r);
	// return (count_extrema_per_single_period (ts_crop(T,offset,B.p+B.r), B, max, sigma) <= max);
}


// Returns the first maximal subinterval of B such that T complies with all period lengths in that subinterval.
// If left_only, only returns such intervals starting at the left margin of B.
// Returns not_periodic, if no interval meets the respective criteria.

static Interval test_interval (Interval const B, bool const left_only)
{
	assert( B.p*B.s+1 == B.r*B.q );
	
	if (B.p+B.r < length(T))
	{
		if (passes_quick_check (B))
		{
			Interval result_left = test_interval (left_partition(B), left_only);
			
			if (is_periodic (result_left))
			{
				if ( share_right_border (result_left, left_partition(B)) )
				{
					Interval result_right = test_interval (right_partition(B), true);
					return merge_if_possible (result_left, result_right);
				}
				else
					return result_left;
			}
			else
				if (!left_only)
				{
					Interval result_right = test_interval (right_partition(B), false);
					return result_right;
				}
		}
	}
	else
	{
		if (passes_thourough_check (B))
			return B;
	}
	
	return not_periodic;
}

static Interval test_for_period_2 ()
{
	ValueType maximum = get_ts_value (T, 0);
	ValueType minimum = get_ts_value (T, 0);
	for (IndexType i=2; i<length(T); i+=2)
	{
		ValueType new_value = get_ts_value (T,i);
		if (new_value > maximum)
			maximum = new_value;
		else if (new_value < minimum)
			minimum = new_value;
		else
			continue;
		
		if (maximum-minimum > sigma)
			return not_periodic;
	}
	
	maximum = get_ts_value (T, 1);
	minimum = get_ts_value (T, 1);
	for (IndexType i=3; i<length(T); i+=2)
	{
		ValueType new_value = get_ts_value (T,i);
		if (new_value > maximum)
			maximum = new_value;
		else if (new_value < minimum)
			minimum = new_value;
		else
			continue;
		
		if (maximum-minimum > sigma)
			return not_periodic;
	}
	
	return (Interval) {2, 1, 2, 1};
}

static Interval test_for_constancy ()
{
	ValueType maximum = get_ts_value (T, 0);
	ValueType minimum = get_ts_value (T, 0);
	for (IndexType i=1; i<length(T); i++)
	{
		ValueType new_value = get_ts_value (T,i);
		if (new_value > maximum)
			maximum = new_value;
		else if (new_value < minimum)
			minimum = new_value;
		else
			continue;
		
		if (maximum-minimum > sigma)
			return not_periodic;
	}
	
	return (Interval) {2, 1, max_tau, 1};
}

// Returns the first maximal subinterval of (2,max_tau] such that T complies with all period lengths in that subinterval.
// Returns not_periodic, if no interval meets the respective criteria.

Interval test_from_2_to_max_tau ()
{
	// If it weren't for stack overflows, this could simply read something along the lines of
	// return test_interval (T, (Interval){2,1,1,0}, extrema, false);
	// The only thing missing for this to work is something to take care of max_tau.
	
	Interval result = not_periodic;
	
	for (Interval B=(Interval){2,1,3,1}; B.r<=max_tau; B.p++,B.r++)
	{
		if (!is_periodic (result))
			result = test_interval (B, false);
		else
		{
			Interval next_result = test_interval (B, true);
			result = merge_if_possible (result, next_result);
		}
		
		if (is_periodic (result))
		{
			if ( share_right_border (result, B) )
				continue;
			else
				return result;
		}
	}
	
	return result;
}


// Returns the first maximal interval of [2,max_tau] such that T complies with all period lengths in that subinterval.
// Returns not_periodic, if no interval meets the respective criteria.
// No sanity checks on max_tau are performed.

Interval periodicity_test (
		TimeSeries const T_passed,
		IndexType const max_tau_passed,
		ValueType const sigma_passed
	)
{
	T = T_passed;
	sigma = sigma_passed;
	max_tau = max_tau_passed;
	
	extrema = count_extrema_plain();
	
	if (extrema == 0)
		return test_for_constancy();
	
	if ( (extrema == length(T)-2) && is_periodic (test_for_period_2()) )
		return (Interval) {2, 1, 2, 1};
	
	return test_from_2_to_max_tau();
}

