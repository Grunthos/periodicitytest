// The standalone program.

# include "basics_standalone.h"
# include "search.h"
# include "interval.h"
# include <stdio.h>
# include <stdlib.h>

int main (int argc, char **argv)
{
	TimeSeries T = timeseries_from_stdin();
	
	ValueType sigma = 0.0;
	
	IndexType max_tau = (length(T)-1)/2;
	if (argc>1)
		max_tau = atoi(argv[1]);
	
	if (max_tau > length(T)-1)
	{
		max_tau = length(T)-1;
		fprintf(stderr, "Warning: Lowering maximum period length to length of time series.\n");
	}
	else if (max_tau <= 2)
	{
		max_tau = 3;
		fprintf(stderr, "Warning: Raising maximum period length to 3.\n");
	}
	
	if (argc>2)
		sigma = atof(argv[2]);
	
	if (sigma<0)
	{
		sigma = 0.0;
		fprintf(stderr, "Warning: Raising error allowance to 0.\n");
	}
	
	
	Interval result = periodicity_test (T, max_tau, sigma);
	
	if (is_periodic(result))
	{
		printf(
			"Data complies with any period between %.16f and %.16f.",
			result.p/((double) result.q),
			result.r/((double) result.s)
		);
	}
	else
		printf("Data does not comply with no period length in the specified interval.");
	
	timeseries_free (T);
}
