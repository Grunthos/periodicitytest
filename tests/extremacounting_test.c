// Tests the functions from extremcounting.h.

# include "../periodicitytest/basics.h"
# include "../periodicitytest/interval.h"
# include "../periodicitytest/extremacounting.h"
# include "testfunctions.h"
# include <math.h>
# include <assert.h>
# include <stdlib.h>
# include <unistd.h>

ValueType nu;

ValueType noise()
{
	if (rand()%2)
		return (rand()/((ValueType) RAND_MAX) - 0.5) * nu;
	else
		return 0;
}

void test_closest_fractions (ValueType x, IndexType n, IndexType p, IndexType q, IndexType r, IndexType s)
{
	Interval B = closest_fractions(x,n);
	assert (p==B.p);
	assert (q==B.q);
	assert (r==B.r);
	assert (s==B.s);
}

void test_plain_with_five_values (ValueType a, ValueType b, ValueType c, ValueType d, ValueType e, IndexType extrema, ValueType nu)
{
	TimeSeries T1 = timeseries_alloc (5);
	set_ts_value (T1,0,a);
	set_ts_value (T1,1,b);
	set_ts_value (T1,2,c);
	set_ts_value (T1,3,d);
	set_ts_value (T1,4,e);
	assert (extrema == count_extrema_plain (T1, nu));
	
	TimeSeries T2 = timeseries_alloc (5);
	set_ts_value (T2,4,a);
	set_ts_value (T2,3,b);
	set_ts_value (T2,2,c);
	set_ts_value (T2,1,d);
	set_ts_value (T2,0,e);
	assert (extrema == count_extrema_plain (T2, nu));
}

void test_plain_with_single_period (double (*f)(double), IndexType extrema)
{
	const int n = 100;
	TimeSeries T = timeseries_alloc(n);

	for (int i=0; i<n; i++)
		set_ts_value (T,i,f(i*2*M_PI/n)+noise());

	assert ( extrema == count_extrema_plain (T, nu) );
	
	timeseries_free(T);
}


void test_single_independence_from_beginning (double (*f)(double))
{
	int k = 100;
	TimeSeries T = timeseries_alloc(2*k);

	for (int i=0; i<k; i++)
	{
		ValueType const value = f(i*2*M_PI/k);
		set_ts_value (T,i+k,value+noise());
		set_ts_value (T,i,value+noise());
	}

	IndexType extrema = count_extrema_per_single_period (T, (Interval){k-1,0,1,0}, k, nu);
	for (int j=0; j<k; j++)
		assert ( count_extrema_per_single_period (ts_crop (T,j,length(T)-j), (Interval){k-1,0,1,0}, k, nu) == extrema );
	
	timeseries_free(T);
}

void test_single_with_periodic_function (double (*f)(double), IndexType extrema)
{
	const int n = 1131;
	TimeSeries T = timeseries_alloc(n);
	
	for (int i=0; i<n; i++)
		set_ts_value (T,i,(*f)(i/4.5)+noise());

	for (int k=30; k<n; k++)
	{
		Interval B = closest_fractions (9*M_PI, k);
		assert (extrema == count_extrema_per_single_period (T, B, k, nu));
	}
	
	timeseries_free(T);
}

void test_full_with_different_lengths_and_starts (double (*f)(double))
{
	const int n = 1131;
	TimeSeries T = timeseries_alloc(n);
	
	for (int i=0; i<n; i++)
		set_ts_value (T,i,(*f)(i/4.5)+noise());
	
	for (int j=0; j<33; j++)
		for (int N=29; N<n-j; N++)
		{
			TimeSeries cropped_T = ts_crop (T,j,N);
			Interval B = closest_fractions(9*M_PI, N);
			
			IndexType extrema_A = count_extrema_plain (cropped_T, nu);
			IndexType extrema_B = count_extrema_all_periods (cropped_T, B, length(T), nu);
			assert (extrema_A == extrema_B);
		}
	
	timeseries_free(T);
}

void test_full_with_different_period_lengths (double (*f)(double))
{
	const int n = 1000;
	TimeSeries T = timeseries_alloc(n);
	
	for (double period=28.37465; period<n-1; period*=1+rand()*0.2/RAND_MAX)
	{
		// Avoid certain period lengths, which make the test (but not what is tested) faulty due to rounding errors.
		if (fabs(round(period*n*8)-period*n*8)<1e-8) continue;
		
		for (int i=0; i<n; i++)
			set_ts_value (T,i,(*f)(i*2*M_PI/period));
		
		Interval B = closest_fractions(period, n);
		
		IndexType extrema_A = count_extrema_plain (T, nu);
		IndexType extrema_B = count_extrema_all_periods (T, B, length(T), nu);
		assert (extrema_A == extrema_B);
	}
	
	timeseries_free(T);
}

int main()
{
	initialise();
	
	srand (getpid());
	
	//-------------------
	
	test_closest_fractions (9*M_PI, 1000, 311, 11, 820, 29);
	test_closest_fractions (exp(M_PI), 995, 833, 36, 162, 7);
	test_closest_fractions (exp(M_PI), 996, 995, 43, 162, 7);
	test_closest_fractions (exp(M_PI), 1157, 995, 43, 162, 7);
	test_closest_fractions (exp(M_PI), 1158, 1157, 50, 162, 7);
	test_closest_fractions (2, 10, 9, 5, 2, 1);
	
	//-------------------
	
	test_plain_with_five_values (0, 1, 1, 1, 0, 1, 0.9);
	test_plain_with_five_values (0, 1, 1, 1, 0, 0, 1.0);
	test_plain_with_five_values (0, 2, 1, 2, 0, 1, 1.0);
	test_plain_with_five_values (0, 2, 1, 2, 0, 3, 0.9);
	test_plain_with_five_values (-42, 3, -3, 42, 0, 3, 0.0);
	
	//-------------------
	
	for (int i=-1; i < 5; i++)
	{
		nu = (i==-1) ? 1e-100 : i * 0.02;
		if (i==-1) assert(nu!=0);
		
		test_plain_with_five_values (0+noise(), 0+noise(), 0+noise(), 0+noise(), 0+noise(), 0, nu);
		test_plain_with_five_values (1+noise(), 2+noise(), 3+noise(), 4+noise(), 5+noise(), 0, nu);
		test_plain_with_five_values (1+noise(), 2+noise(), 3+noise(), 4+noise(), 1+noise(), 1, nu);
		test_plain_with_five_values (1+noise(), 2+noise(), 3+noise(), 1+noise(), 1+noise(), 1, nu);
		test_plain_with_five_values (1+noise(), 2+noise(), 3+noise(), 1+noise(), 0+noise(), 1, nu);
		test_plain_with_five_values (1+noise(), 2+noise(), 3+noise(), 1+noise(), 2+noise(), 2, nu);
		test_plain_with_five_values (0+noise(), 0+noise(), 1+noise(), 0+noise(), 0+noise(), 1, nu);
		test_plain_with_five_values (0+noise(), 0+noise(), 1+noise(), 0+noise(), 1+noise(), 2, nu);
		test_plain_with_five_values (0+noise(), 1+noise(), 0+noise(), 1+noise(), 0+noise(), 3, nu);
		test_plain_with_five_values (0+noise(), 1+noise(), 1+noise(), 1+noise(), 0+noise(), 1, nu);
		
		test_plain_with_single_period (&sin, 2);
		test_plain_with_single_period (&cos, 1);
		test_plain_with_single_period (&tan, 4);
		test_plain_with_single_period (&sawtooth, 0);
		test_plain_with_single_period (&mostly_constant_function, 0);
		test_plain_with_single_period (&nasty_function, 4);
		test_plain_with_single_period (&zero, 0);
		test_plain_with_single_period (&rounded_sin, 2);
		test_plain_with_single_period (&sin_x_plus_sin_2x, 4);
		
		//-------------------
		
		test_single_with_periodic_function (&sin, 2);
		test_single_with_periodic_function (&cos, 2);
		test_single_with_periodic_function (&tan, 4);
		test_single_with_periodic_function (&sawtooth, 2);
		test_single_with_periodic_function (&mostly_constant_function, 2);
		test_single_with_periodic_function (&nasty_function, 6);
		test_single_with_periodic_function (&zero, 0);
		test_single_with_periodic_function (&rounded_sin, 2);
		test_single_with_periodic_function (&sin_x_plus_sin_2x, 4);
		
		//-------------------
		
		test_single_independence_from_beginning (&sin);
		test_single_independence_from_beginning (&tan);
		test_single_independence_from_beginning (&nasty_function);
		test_single_independence_from_beginning (&mostly_constant_function);
		test_single_independence_from_beginning (&sawtooth);
		test_single_independence_from_beginning (&random_double);
		test_single_independence_from_beginning (&random_bool);
		test_single_independence_from_beginning (&zero);
		test_single_independence_from_beginning (&rounded_sin);
		test_single_independence_from_beginning (&sin_x_plus_sin_2x);
		
		//-------------------
		
		test_full_with_different_lengths_and_starts (&sin);
		test_full_with_different_lengths_and_starts (&cos);
		test_full_with_different_lengths_and_starts (&tan);
		test_full_with_different_lengths_and_starts (&nasty_function);
		test_full_with_different_lengths_and_starts (&mostly_constant_function);
		test_full_with_different_lengths_and_starts (&sawtooth);
		test_full_with_different_lengths_and_starts (&zero);
		test_full_with_different_lengths_and_starts (&rounded_sin);
		test_full_with_different_lengths_and_starts (&sin_x_plus_sin_2x);
		
		//-------------------
		
		test_full_with_different_period_lengths (&sin);
		test_full_with_different_period_lengths (&cos);
		test_full_with_different_period_lengths (&tan);
		test_full_with_different_period_lengths (&nasty_function);
		test_full_with_different_period_lengths (&mostly_constant_function);
		test_full_with_different_period_lengths (&sawtooth);
		test_full_with_different_period_lengths (&zero);
		test_full_with_different_period_lengths (&rounded_sin);
		test_full_with_different_period_lengths (&sin_x_plus_sin_2x);
	}
	
	finalise();
}
