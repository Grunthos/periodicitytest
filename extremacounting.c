# include "extremacounting.h"
# include <stdbool.h>
# include <assert.h>
# include "interval.h"
# include <stdio.h>

// General remarks:
// The central functions (count_extrema_*) become increasingly complex. I suggest to read them in order.
// "old", "new", "next" and "previous" always refer to the timeseries' evolution as measured or within a reconstructed period.
// Functions with the _sigma suffix take error allowance into account; functions with the _nosigma suffix explicitly don’t.


static inline void update_min_and_max (ValueType const current, ValueType * const min, ValueType * const max)
{
	if (current > *max)
		*max = current;
	else if (current < *min)
		*min = current;
}

char get_last_sign_sigma (TimeSeries const T, ValueType const sigma)
{
	ValueType max = get_ts_value (T, length(T)-1);
	ValueType min = max;
	
	for (IndexType i=length(T)-1; i!=0; i--)
	{
		ValueType const current = get_ts_value (T, i-1);
		update_min_and_max (current, &min, &max);
		if (max-min > sigma)
			return (max==current) ? -1 : 1;
	}
	
	return 0;
}

char get_first_sign_sigma (TimeSeries const T, ValueType const sigma)
{
	ValueType max = get_ts_value (T, 0);
	ValueType min = max;
	
	for (IndexType i=1; i<length(T); i++)
	{
		ValueType const current = get_ts_value (T, i);
		update_min_and_max (current, &min, &max);
		if (max-min > sigma)
			return (max==current) ? 1 : -1;
	}
	
	return 0;
}

// Count the local extrema of a time series as it is.
IndexType count_extrema_plain_sigma (TimeSeries const T, ValueType const sigma)
{
	IndexType number_of_extrema = 0;
	
	bool sign;
	switch (get_last_sign_sigma (T,sigma))
	{
		case  0: return 0;
		case  1: sign = true; break;
		case -1: sign = false;
	}
	
	ValueType extremum = get_ts_value (T, length(T)-1);
	
	for (IndexType i=length(T)-1; i!=0; i--)
	{
		ValueType const current = get_ts_value (T, i-1);
		
		if ((current>extremum) != sign)
			extremum = current;
		else if ((sign && (current-extremum>sigma)) || (!sign && (extremum-current>sigma)))
		{
			sign = !sign;
			extremum = current;
			number_of_extrema++;
		}
	}
	
	return number_of_extrema;
}

IndexType count_extrema_plain_nosigma (TimeSeries const T)
{
	IndexType number_of_extrema = 0;
	
	// Sign of the last observed change of the timeseries value
	char sign = 0; 
	
	// Iterating backwards only to maintain analogy to following functions.
	for (IndexType i=length(T)-1; i!=0; i--)
	{
		char const current_sign = diff_sign (T, i, i-1);
		if (current_sign != sign)
		{
			if (sign == -current_sign)
			{
				number_of_extrema++;
				sign = current_sign;
			}
			else if (sign == 0)
				sign = current_sign;
		}
	}

	return number_of_extrema;
}

IndexType count_extrema_plain (TimeSeries const T, ValueType const sigma)
{
	if (sigma==0)
		return count_extrema_plain_nosigma(T);
	else
		return count_extrema_plain_sigma(T, sigma);
}

// Next and previous index modulo tau for tau in B.
static inline IndexType next_index (IndexType const index, Interval const B)
{
	return (index+B.r) % (B.p+B.r);
}

static inline IndexType previous_index (IndexType const index, Interval const B)
{
	return (index+B.p) % (B.p+B.r);
	//   = (index-B.r) % (B.p+B.r), if C's modulo worked like that
}

static inline IndexType first_iteration_with_difference_sigma (
		TimeSeries const T,
		Interval const B,
		IndexType * const index,
		ValueType const sigma
	)
{
	*index = next_index (0,B);
	IndexType number_of_iterations = 1;
	ValueType max = get_ts_value (T, 0);
	ValueType min = max;
	for (; *index!=0; *index=next_index(*index,B), number_of_iterations++)
	{
		ValueType const current = get_ts_value (T, *index);
		update_min_and_max (current, &min, &max);
		if (max-min > sigma)
			break;
	}
	
	return number_of_iterations;
}

static inline IndexType first_iteration_with_difference_nosigma (TimeSeries const T, Interval const B, IndexType * const index)
{
	IndexType number_of_iterations = 0;
	*index = 0;
	IndexType prev;
	
	do
	{
		prev = *index;
		*index = next_index (*index,B);
		number_of_iterations++;
	} while (!diff_sign (T,*index,prev) && (*index != 0));
	
	return number_of_iterations;
}

// Count the local extrema of one reconstructed period of a segment of length B.p+B.r of T for some tau in B, taking into account extrema close to muliples of the period length.
// Stops counting and returns number of counted extrema up to that point if the number of extrema exceeds max_number_of_extrema.

IndexType count_extrema_per_single_period (
		TimeSeries const T,
		Interval const B,
		IndexType const max_number_of_extrema,
		ValueType const sigma
	)
{
	assert (B.p+B.r <= length(T));
	
	IndexType index;
	IndexType additional_iterations;
	
	if (sigma==0)
		additional_iterations = first_iteration_with_difference_nosigma (T, B, &index);
	else
		additional_iterations = first_iteration_with_difference_sigma (T, B, &index, sigma);
	
	IndexType const count = B.p + B.r + additional_iterations;
	
	if (index == 0) return 0; // if timeseries is constant up to B.p+B.r
	
	IndexType number_of_extrema = 0;
	
	if (sigma==0)
	{
		char sign = diff_sign (T, index, previous_index(index, B));
		
		for (IndexType i=count; i>0; i--)
		{
			IndexType const next = index;
			index = previous_index (index,B);
			
			if ( diff_sign (T, next, index) == -sign )
			{
				if (++number_of_extrema > max_number_of_extrema)
					return number_of_extrema;
				sign = diff_sign (T, next, index);
			}
		}
	}
	else
	{
		ValueType extremum = get_ts_value (T, index);
		assert (extremum != get_ts_value (T,0));
		bool sign = (extremum > get_ts_value (T,0)); 
		
		// Iterating backwards only to maintain analogy to following functions.
		for (IndexType i=count; i>0; i--)
		{	
			index = previous_index (index,B);
			ValueType const current = get_ts_value (T, index);
			
			if ((current>extremum) != sign)
				extremum = current;
			else if ((sign && (current-extremum>sigma)) || (!sign && (extremum-current>sigma)))
			{
				sign = !sign;
				extremum = current;
				if (++number_of_extrema > max_number_of_extrema)
					return number_of_extrema;
			}
		}
	}
	
	assert (index == 0);
	
	assert (number_of_extrema % 2 == 0);
	return number_of_extrema;
}

static inline IndexType next_index_II (IndexType index, Interval const B, TimeSeries const T)
{
	do
		index = (index+B.r) % (B.p+B.r);
	while (index>length(T)-1);
	return index;
}

static inline IndexType previous_index_II (IndexType index, Interval const B, TimeSeries const T)
{
	do
		index = (index+B.p) % (B.p+B.r);
	while (index>length(T)-1);
	return index;
}

static inline bool compare_mod_tau (IndexType const i, IndexType const j, Interval const B)
{
	return (i*(B.s+B.q) % (B.r+B.p) <= j*(B.s+B.q) % (B.r+B.p));
}

// Finds the index of the last extremum and the last sign of differences. Returns whether the time series is not constant.

static inline bool prescan_nosigma (
		TimeSeries const T,
		Interval const B,
		bool * const undetectable_final_extremum,
		IndexType * const index_of_last_extremum
	)
{
	*undetectable_final_extremum = false;
	
	IndexType index = length(T)-1;
	char sign = 0;
	bool passed_length_minus_2 = false;
	
	do
	{
		IndexType const next = index;
		index = previous_index_II (index, B, T);
		char const current_sign = diff_sign (T, index, next);
		
		if (current_sign != sign)
		{
			if (sign == -current_sign)
			{
				*index_of_last_extremum = index;
				if (!passed_length_minus_2)
				{
					assert (diff_sign (T, next, length(T)-1) != 0);
					
					*undetectable_final_extremum = diff_signs_differ (T, length(T)-1, next, length(T)-2);
				}
				return true;
			}
			else if (sign == 0)
				sign = current_sign;
		}
		
		if (index == length(T)-2)
			passed_length_minus_2 = true;
		
	} while (index != length(T)-1);
	
	return false;
}

static inline bool prescan_sigma (
		TimeSeries const T,
		Interval const B,
		char * const sign,
		IndexType * const index,
		ValueType const sigma
	)
{
	*index = length(T)-1;
	ValueType max = get_ts_value (T, *index);
	ValueType min = max;
	*sign = 0;
	
	do
	{
		*index = previous_index_II (*index, B, T);
		ValueType const current = get_ts_value (T, *index);
		update_min_and_max (current, &min, &max);
		
		if (*sign == 0)
		{
			if (max-min > sigma)
				*sign = (max == current) ? -1 : 1;
		}
		else if (*sign == -1)
		{
			if (max-current>sigma)
				return true;
		}
		else if (*sign == 1)
		{
			if (current-min>sigma)
				return true;
		}
	}
	while (*index != length(T)-1);
	
	return false;
}

static inline IndexType first_iteration_with_difference_nosigma_II (TimeSeries const T, Interval const B, IndexType * const index)
{
	*index = 0;
	IndexType number_of_iterations  = 0;
	IndexType prev;
	
	do
	{
		prev = *index;
		*index = next_index_II (*index, B, T);
		number_of_iterations++;
	} while (!diff_sign(T,*index,prev));
	
	return number_of_iterations;
}

static inline IndexType first_iteration_with_difference_sigma_II (
		TimeSeries const T,
		Interval const B,
		IndexType * const index,
		char * const first_reconstructed_sign,
		ValueType const sigma
	)
{
	IndexType number_of_iterations = 1;
	ValueType max = get_ts_value (T, 0);
	ValueType min = max;
	for (*index=next_index_II(0,B,T); *index!=0; *index=next_index_II(*index,B,T), number_of_iterations++)
	{
		ValueType const current = get_ts_value (T, *index);
		update_min_and_max (current, &min, &max);
		if (max-min > sigma)
		{
			*first_reconstructed_sign = (current==max)?1:-1;
			return number_of_iterations;
		}
	}
	
	*first_reconstructed_sign = 0;
	return number_of_iterations;
}

// Returns the exact number of extrema a time series would have if it complies with some period length within B.
// Stops counting and returns length(T) if the number of extrema exceeds max_number_of_extrema.

IndexType count_extrema_all_periods_nosigma (
		TimeSeries const T,
		Interval const B,
		IndexType const max_number_of_extrema
	)
{
	IndexType number_of_extrema = 0;
	bool undetectable_final_extremum;
	bool undetectable_initial_extremum = false;
	IndexType index_of_last_extremum;
	IndexType extrema_till_last_extremum;
	
	if (!prescan_nosigma (T, B, &undetectable_final_extremum, &index_of_last_extremum))
		return 0;
	
	IndexType index;
	IndexType additional_iterations = first_iteration_with_difference_nosigma_II (T, B, &index);
	IndexType const count = length(T) + additional_iterations;

	char sign = diff_sign (T, index, previous_index_II (index, B, T));
	bool passed_1 = false;
	
	for (IndexType i=count; i>0; i--)
	{
		IndexType const next = index;
		index = previous_index_II (index, B, T);
		
		if ( (index==index_of_last_extremum) && (i<=length(T)) ) 
			extrema_till_last_extremum = number_of_extrema;
		
		if ( diff_sign (T, index, next) == sign )
		{
			if (++number_of_extrema > max_number_of_extrema)
				return length(T);
			sign = diff_sign (T, next, index);
			if ( passed_1 && diff_signs_differ (T, 0, next, 1) )
				undetectable_initial_extremum = true;
		}
		
		if ( (index==1) && (i<=length(T)) )
			passed_1 = true;
	}
	
	assert (index == 0);
	assert (number_of_extrema%2 == 0);
	
	IndexType full_periods = ((length(T)-1)*B.s) / B.r;
	
	// Count one period less if the prescan passed 0.
	if (compare_mod_tau (length(T)-1, index_of_last_extremum, B))
		full_periods--;
	
	# pragma GCC diagnostic push
	# pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	return 
		  number_of_extrema * full_periods
		+ (number_of_extrema - extrema_till_last_extremum)
		- undetectable_initial_extremum
		- undetectable_final_extremum;
	# pragma GCC diagnostic pop
}

IndexType count_extrema_all_periods_sigma (
		TimeSeries const T,
		Interval const B,
		IndexType const max_number_of_extrema,
		ValueType const sigma
	)
{
	char last_reconstructed_sign;
	char first_reconstructed_sign;
	IndexType index_of_last_extremum;
	IndexType extrema_till_last_extremum;
	
	if (!prescan_sigma (T, B, &last_reconstructed_sign, &index_of_last_extremum, sigma))
		return 0;
	
	IndexType index;
	IndexType additional_iterations = first_iteration_with_difference_sigma_II (T, B, &index, &first_reconstructed_sign, sigma);
	IndexType const count = length(T) + additional_iterations;
	
	assert (last_reconstructed_sign!=0);
	assert (last_reconstructed_sign!=0);
	bool undetectable_final_extremum = (last_reconstructed_sign != get_last_sign_sigma(T,sigma));
	bool undetectable_initial_extremum = (first_reconstructed_sign != get_first_sign_sigma(T,sigma));

	ValueType extremum = get_ts_value (T, index);
	assert (extremum != get_ts_value (T,0));
	bool sign = (extremum > get_ts_value (T,0)); 
	
	IndexType number_of_extrema = 0;
	
	for (IndexType i=count; i>0; i--)
	{	
		index = previous_index_II (index,B,T);
		ValueType const current = get_ts_value (T, index);
		
		if ( (index==index_of_last_extremum) && (i<=length(T)) ) 
			extrema_till_last_extremum = number_of_extrema;
		
		if ((current>extremum) != sign)
			extremum = current;
		else if ((sign && (current-extremum>sigma)) || (!sign && (extremum-current>sigma)))
		{
			sign = !sign;
			extremum = current;
			if (++number_of_extrema > max_number_of_extrema)
				return length(T);
		}
	}
	
	assert (index == 0);
	assert (number_of_extrema%2 == 0);
	
	IndexType full_periods = ((length(T)-1)*B.s) / B.r;
	
	// Count one period less if the prescan passed 0.
	if (compare_mod_tau (length(T)-1, index_of_last_extremum, B))
		full_periods--;
	
	return 
		  number_of_extrema * full_periods
		+ (number_of_extrema - extrema_till_last_extremum)
		- undetectable_initial_extremum
		- undetectable_final_extremum;
}

IndexType count_extrema_all_periods (
		TimeSeries const T,
		Interval const B,
		IndexType const max_number_of_extrema,
		ValueType const sigma
	)
{
	if (sigma==0)
		return count_extrema_all_periods_nosigma (T, B, max_number_of_extrema);
	else
		return count_extrema_all_periods_sigma (T, B, max_number_of_extrema, sigma);
}
