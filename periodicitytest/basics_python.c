# include "basics_python.h"
# include <stdio.h>
# include <stdlib.h>
# include <assert.h>

TimeSeries timeseries_alloc (IndexType const n)
{
	npy_intp dims[1] = {n};
	
	# pragma GCC diagnostic push
	# pragma GCC diagnostic ignored "-pedantic"
	TimeSeries T = (TimeSeries) PyArray_SimpleNew (1, dims, TYPE_INDEX);
	# pragma GCC diagnostic pop
	
	if (T == NULL)
	{
		PyErr_SetString (PyExc_ValueError, "Error: Could not allocate array.");
		exit(1);
	}
	
	return T;
}

void timeseries_free (TimeSeries const T)
{
	PyArray_free (T);
}

// Returns a cropped view of a time series.
TimeSeries ts_crop (TimeSeries const T, IndexType const offset, IndexType const size)
{
	assert (offset+size<=length(T));
	
	PyObject * slice = PySlice_New (
	                                PyInt_FromLong(offset),
	                                PyInt_FromLong(size+offset),
	                                PyInt_FromLong(1)
	                               );
	
	return (TimeSeries) PyObject_GetItem ((PyObject *) T, slice);
}

void initialise()
{
	Py_Initialize();
	import_array();
}

void finalise()
{
	Py_Finalize();
}
