# include "basics_standalone.h"
# include <stdio.h>
# include <stdlib.h>
# include <assert.h>

static inline void * safe_malloc (size_t const size)
{
	void * pointer = malloc (size);
	if (pointer == NULL)
	{
		fprintf (stderr, "Error: Not enough memory.\n");
		exit(1);
	}
	return pointer;
}

static inline void * safe_realloc (void * pointer, size_t const size)
{
	pointer = realloc (pointer, size);
	if (pointer == NULL)
	{
		fprintf (stderr, "Error: Not enough memory.\n");
		exit(1);
	}
	return pointer;
}

TimeSeries timeseries_alloc (IndexType const n)
{
	ValueType * data = safe_malloc (n*sizeof(ValueType));
	return (TimeSeries) {data, n};
}

void timeseries_free (TimeSeries const T)
{
	free (T.data);
}

TimeSeries timeseries_from_stdin ()
{
	TimeSeries T = timeseries_alloc (100);
	
	IndexType i = 0;
	while (scanf (ValueType_formatting_string, &(T.data[i])) == 1)
	{
		i++;
		if (i==T.length)
		{
			T.length *= 2;
			T.data = safe_realloc (T.data, T.length*sizeof(ValueType));
		}
	}
	T.length = i;
	T.data = safe_realloc (T.data, T.length*sizeof(ValueType));
	
	return T;
}

// Returns a cropped view of a time series.
TimeSeries ts_crop (TimeSeries const T, IndexType const offset, IndexType const size)
{
	assert (offset+size<=length(T));
	return (TimeSeries) {T.data + offset, size};
}
