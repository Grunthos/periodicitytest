// The Python interface / module

# include "basics_python.h"
# include "search.h"
# include "interval.h"


const char periodicitytest_docstring[] = "\n"
	"periodicitytest(T[, max_tau, sigma])\n"
	"\n"
	"Tests a time series for periodicity, more precisely the existence of a periodic function that interpolates the time series and each of whose local extrema is captured by the time series. \n"
	"\n"
	"Parameters\n"
	"----------\n"
	"T: If sigma is zero: one-dimensional Numpy array of any type that supports comparison. Otherwise: Type has to be double.\n"
	"The time series.\n\n"
	"max_tau: positive integer (no warning, if negative)\n"
	"Maximum investigated period length. "
	"Defaults to half the time series’ length. "
	"If max_tau < 3 or max_tau > len(T)−1, max_tau is adjusted and a warning is issued.\n\n"
	"sigma: non-negative float\n"
	"Error allowance. A local extremum must differ by more than sigma from the preceding and succeeding local extremum (otherwise it is not regarded as a local extremum."
	"Defaults to 0, in which case the method is more efficient.\n"
	"\n"
	"Returns\n"
	"-------\n"
	"is_periodic: boolean\n"
	"True, if and only if T complies with any period length between 2 and max_tau.\n\n"
	"(p,q,r,s): tuple of four integers or None\n"
	"• If is_periodic: (p/q,r/s) is the first maximal interval such that T complies with any period length from this interval."
	"T may or may not comply with a period length of r/s.\n"
	"• If not is_periodic and max_tau==len(T)-1: p=len(T)−1; q=1; r=1; s=0.\n"
	"• Otherwise: None.\n\n"
	"(x,y): tuple of two floats or None\n"
	"• If is_periodic or max_tau==len(T)-1: x=p/q, y=r/s.\n"
	"• Otherwise: None.\n"
	;


# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"

static PyObject * py_periodicitytest(PyObject *self, PyObject *args)
{
	TimeSeries T;
	IndexType max_tau = 0;
	ValueType sigma = 0;
	
	if (!PyArg_ParseTuple(args, "O!|Id", &PyArray_Type, &T, &max_tau, &sigma))
		return NULL;
	
	if (PyArray_NDIM(T) != 1)
	{
		PyErr_SetString(PyExc_ValueError,"Array must be one-dimensional.");
		return NULL;
	}
	else if ((PyArray_TYPE(T) != TYPE_INDEX) && (sigma != 0))
	{
		PyErr_SetString(PyExc_TypeError,"Unless sigma is 0, array needs to be of type double.");
		return NULL;
	}
	else if (length(T) < 3)
	{
		PyErr_SetString(PyExc_ValueError,"Array length smaller than 3.");
		return NULL;
	}
	
	if (max_tau == 0)
		max_tau = (length(T)-1)/2;
	else if (max_tau > length(T)-1)
	{
		max_tau = length(T)-1;
		PyErr_WarnEx (NULL, "Warning: Lowering maximum period length to length of time series.\n", 1);
	}
	else if (max_tau <= 2)
	{
		max_tau = 3;
		PyErr_WarnEx (NULL, "Warning: Raising maximum period length to 3.\n", 1);
	}
	
	if (sigma < 0)
	{
		PyErr_SetString(PyExc_ValueError,"Negative sigma.");
		return NULL;
	}
	
	
	
	Interval result = periodicity_test (T, max_tau, sigma);
	
	if (is_periodic(result))
	{
		PyObject * result_interval = PyTuple_Pack (4,
												PyLong_FromLong(result.p),
												PyLong_FromLong(result.q),
												PyLong_FromLong(result.r),
												PyLong_FromLong(result.s));
		PyObject * result_interval_2 = PyTuple_Pack (2,
												PyFloat_FromDouble(result.p/((double) result.q)),
												PyFloat_FromDouble(result.r/((double) result.s)));
		return PyTuple_Pack (3, Py_True, result_interval, result_interval_2);
	}
	else if (max_tau == length(T)-1)
	{
		PyObject * result_interval = PyTuple_Pack (4,
												PyLong_FromLong(length(T)-1),
												PyLong_FromLong(1),
												PyLong_FromLong(1),
												PyLong_FromLong(0));
		PyObject * result_interval_2 = PyTuple_Pack (2,
												PyFloat_FromDouble((double) length(T)-1),
												PyFloat_FromDouble(INFINITY));
		return PyTuple_Pack (3, Py_False, result_interval, result_interval_2);
	}
	else
		return PyTuple_Pack (3, Py_False, Py_None, Py_None);

}

# pragma GCC diagnostic pop

static PyMethodDef periodicitytest_methods[] = {
	{"periodicitytest", py_periodicitytest, METH_VARARGS, periodicitytest_docstring},
	{NULL, NULL, 0, NULL}
};


#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef =
{
        PyModuleDef_HEAD_INIT,
        "_periodicitytest",
        NULL,
        -1,
        periodicitytest_methods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC PyInit__periodicitytest(void)
{
	PyObject * module = PyModule_Create(&moduledef);
	import_array();
	return module;
}

#else

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC init_periodicitytest()
{
	Py_InitModule("_periodicitytest", periodicitytest_methods);
	import_array();
}

#endif
