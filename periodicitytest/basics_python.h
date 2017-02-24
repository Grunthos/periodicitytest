// Datatypes, structs and functions for the Python interface.

# include <stdbool.h>

# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-pedantic"
# define NPY_NO_DEPRECATED_API NPY_1_8_API_VERSION
# include <Python.h>
# include <numpy/arrayobject.h>
# pragma GCC diagnostic pop

# ifndef BASICS
# define BASICS

// Data type used for indizes of the time series.
typedef unsigned int IndexType;

// Cast to longer data type than index type (needed sometimes to avoid integer overflows)
static inline unsigned long L(IndexType i)
{
	return ((unsigned long) i);
}

// Data type for values of the time series (if sigma>0) and for the test scripts.
typedef double ValueType;
# define TYPE_INDEX NPY_DOUBLE

// Initialise and finalise the Python environment.
#if PY_MAJOR_VERSION >= 3
void * initialise();
#else
void initialise();
#endif
void finalise();

typedef PyArrayObject * TimeSeries;

static inline IndexType length (TimeSeries const T)
{
	return PyArray_DIM (T, 0);
}

static inline char diff_sign (TimeSeries const T, IndexType const i, IndexType const j)
{
	return PyArray_DESCR(T)->f->compare (PyArray_GETPTR1 (T, i), PyArray_GETPTR1 (T, j), T);
}

static inline bool diff_signs_differ (TimeSeries const T, IndexType const i, IndexType const j, IndexType const k)
{
	return (
			   PyArray_DESCR(T)->f->compare (PyArray_GETPTR1(T,i), PyArray_GETPTR1(T,j), T)
			!= PyArray_DESCR(T)->f->compare (PyArray_GETPTR1(T,i), PyArray_GETPTR1(T,k), T)
			);
}

static inline ValueType get_ts_value(TimeSeries const T, IndexType const i)
{
	return * (ValueType*) PyArray_GETPTR1(T, i);
}

// Only required for testing purposes:
TimeSeries timeseries_alloc ();
void timeseries_free ();
TimeSeries ts_crop();
static inline void set_ts_value (TimeSeries const T, IndexType const index, ValueType const value)
{
	* (ValueType *) PyArray_GETPTR1 (T, index) = value;
}

# endif
