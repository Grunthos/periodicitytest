// Basic datatypes, structs and data-assessing functions.

# include <stdbool.h>
# include <assert.h>

# ifndef BASICS
# define BASICS


// Data type used for indizes of the time series. Must be capable of representing roughly twice the length of the time series.
typedef unsigned int IndexType;

// Cast to longer data type than index type (needed sometimes to avoid integer overflows)
static inline unsigned long L(IndexType i)
{
	return ((unsigned long) i);
}

// Data type for values of the time series. Can be any numerical type that supports comparison, even integer.
typedef double ValueType;

# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-variable"
static char ValueType_formatting_string[] = "%lf";
# pragma GCC diagnostic pop


typedef struct TimeSeries
{
	ValueType * data;
	IndexType length;
} TimeSeries;

static inline IndexType length (TimeSeries const T)
{
	return T.length;
}

static inline ValueType get_ts_value(TimeSeries const T, IndexType const i)
{
	assert(i<T.length);
	return T.data[i];
}

static inline void set_ts_value (TimeSeries const T, IndexType const index, ValueType const value)
{
	T.data[index] = value;
}

TimeSeries timeseries_alloc (IndexType const n);
TimeSeries timeseries_from_stdin ();
void timeseries_free (TimeSeries const T);

// If you just want to apply the test and did redefine any types, you can stop here.
// =================================================================================

TimeSeries ts_crop();

static inline char diff_sign (TimeSeries const T, IndexType const i, IndexType const j)
{
	ValueType const a = get_ts_value(T,i);
	ValueType const b = get_ts_value(T,j);
	return (a>b)-(a<b);
}

static inline bool diff_signs_differ (TimeSeries const T, IndexType const i, IndexType const j, IndexType const k)
{
	return diff_sign(T,i,j) != diff_sign(T,i,k);
}

// These functions do nothing and are only required due to analogy reasons to the Python counterpart.
static inline void initialise(){}
static inline void finalise(){}

# endif
