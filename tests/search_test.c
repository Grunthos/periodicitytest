// Test the periodicity_test function from search.h

# include "../periodicitytest/search.h"
# include "../periodicitytest/basics.h"
# include "../periodicitytest/interval.h"
# include "testfunctions.h"
# include <math.h>
# include <assert.h>
# include <unistd.h>
# include <stdlib.h>

ValueType nu;

ValueType noise()
{
	if (rand()%2)
		return (rand()/((ValueType) RAND_MAX) - 0.5) * nu;
	else
		return 0;
}

// Test whether a period length makes the test (but not what is tested) faulty due to rounding errors.
static inline bool is_problematic(double period, IndexType n)
{
	return (fabs(round(period*n*8)-period*n*8) < 1e-8);
}

void test_half_timeseries_length (double (*f)(double))
{
	const int max_n = 2000;
	TimeSeries T = timeseries_alloc (max_n);
	
	for (int n=100; n<max_n; n*=1+rand()*0.5/RAND_MAX)
		for (double period=n/2-1.234; period<=n/2; period+=rand()*0.1/RAND_MAX)
		{
			if (is_problematic(period,n)) continue;
			
			for (int i=0; i<n; i++)
				set_ts_value (T,i,(*f)(i*2*M_PI/period)+noise());
			
			Interval result = periodicity_test (ts_crop(T,0,n), n-1, nu);
			Interval control = closest_fractions (period, n);
			
			assert(contains_Interval (result, control));
		}
	
	timeseries_free(T);
}

void test_varying_periods (double (*f)(double))
{
	const int n = 1000;
	TimeSeries T = timeseries_alloc (n);
	
	for (double period=28.37465; period<n-1; period*=1+rand()*0.2/RAND_MAX)
	{
		// Avoid certain period lengths, which make the test (but not what is tested) faulty due to rounding errors.
		if (is_problematic(period,n)) continue;
		
		for (int i=0; i<n; i++)
			set_ts_value (T,i,(*f)(i*2*M_PI/period)+noise());
		
		Interval result = periodicity_test (T, n-1, nu);
		Interval control = closest_fractions (period, n);
		
		assert(contains_Interval (result, control));
	}
	
	timeseries_free(T);
}


void test_random_timeseries ()
{
	const int n = 1000;
	TimeSeries T = timeseries_alloc (n);
	
	for (int j=0; j<100; j++)
	{
		for (int i=0; i<n; i++)
			set_ts_value (T,i,rand()%2);
		
		assert (!is_periodic (periodicity_test (T, n/2, nu)));
	}
	
	timeseries_free(T);
}

void test_constant_timeseries ()
{
	const int n = 1000;
	TimeSeries T = timeseries_alloc (n);
	ValueType value = rand();
	for (int i=0; i<n; i++)
		set_ts_value (T,i,value+noise());
	
	for (int max_tau=5; max_tau<n; max_tau*=2+rand()%2)
	{
		Interval result = periodicity_test (T, max_tau, nu);
		Interval control = (Interval) {2,1,max_tau,1};
		
		assert(compare_Interval (result, control));
	}
	
	timeseries_free(T);
}

void test_period_k_timeseries (const int k)
{
	const int max_n = 2000;
	TimeSeries T = timeseries_alloc (max_n);
	ValueType * values = malloc(k*sizeof(ValueType));
	
	for (int n=100; n<max_n; n*=1+rand()*0.5/RAND_MAX)
	{
		for (int i=0; i<k; i++)
		{
			bool duplicate;
			do
			{
				values[i] = rand()%1000;
				duplicate = false;
				for (int j=0; j<i; j++)
					if (values[i]==values[j])
						duplicate = true;
			}
			while (duplicate);
		}	
			
		for (int i=0; i<n; i++)
			set_ts_value (T, i, values[i%k]+noise());
		
		Interval result = periodicity_test (ts_crop(T,0,n), n-1, nu);
		
		bool matches = false;
		
		for (int i=1; i<k; i++)
		{
			if (k%i == 0)
			{
				if (contains_Interval (result, (Interval){k/i,1,k/i,1}))
				{
					matches = true;
					break;
				}
			}
			else
			{
				if (contains_Interval (result, closest_fractions (k/((double) i), n)))
				{
					matches = true;
					break;
				}
			}
		}
		
		assert (matches);
	}
	
	free (values);
	timeseries_free (T);
}

void test_varying_amplitude (double (*f)(double))
{
	const ValueType nu = 0.001;
	const IndexType n = 10000;
	TimeSeries T = timeseries_alloc (n);
	
	for (IndexType i=0; i<n; i++)
		set_ts_value (T,i,(1+nu*i)*f(i)+noise());
	
	assert( !is_periodic ( periodicity_test (T, n/2, nu) ) );
	
	timeseries_free(T);
}

void test_varying_frequency (double (*f)(double))
{
	const ValueType nu = 0.00001;
	const IndexType n = 10000;
	TimeSeries T = timeseries_alloc (n);
	
	for (IndexType i=0; i<n; i++)
		set_ts_value (T,i,f((1+nu*i)*i)+noise());
	
	assert( !is_periodic ( periodicity_test (T, n/2, nu) ) );
	
	timeseries_free(T);
}

int main()
{
	initialise ();
	
	srand (getpid ());
// 	srand (42);
	
	nu = 0;
	test_random_timeseries ();
	
	for (int i=-1; i < 5; i++)
	{
		nu = (i==-1) ? 1e-100 : i * 0.02;
		if (i==-1) assert(nu!=0);
		
		test_varying_periods (&sin);
		test_varying_periods (&cos);
		test_varying_periods (&nasty_function);
		test_varying_periods (&mostly_constant_function);
		test_varying_periods (&sawtooth);
		test_varying_periods (&rounded_sin);
		test_varying_periods (&sin_x_plus_sin_2x);
		
		test_half_timeseries_length (&sin);
		test_half_timeseries_length (&cos);
		test_half_timeseries_length (&nasty_function);
		test_half_timeseries_length (&mostly_constant_function);
		test_half_timeseries_length (&sawtooth);
		test_half_timeseries_length (&rounded_sin);
		test_half_timeseries_length (&sin_x_plus_sin_2x);
		
		test_constant_timeseries ();
		
		for (int k=2; k<20; k++)
			test_period_k_timeseries (k);
		
		test_varying_amplitude (&sin);
		test_varying_amplitude (&cos);
		test_varying_amplitude (&nasty_function);
		test_varying_amplitude (&sawtooth);
		test_varying_amplitude (&rounded_sin);
		test_varying_amplitude (&sin_x_plus_sin_2x);
		
		test_varying_frequency (&sin);
		test_varying_frequency (&cos);
		test_varying_frequency (&nasty_function);
		test_varying_frequency (&mostly_constant_function);
		test_varying_frequency (&sawtooth);
		test_varying_frequency (&rounded_sin);
		test_varying_frequency (&sin_x_plus_sin_2x);
		
	}
	
	finalise ();
}
