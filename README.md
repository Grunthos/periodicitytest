Quick and dirty introduction to usage
=====================================

For an extensive description and explanation, see [arXiv:1506.01287](http://arxiv.org/abs/1506.01287).

Very briefly, this test decides whether a given time series can be interpolated by a periodic function with the same number of local extrema as the time series, where only local extrema with a prominence higher than a given error allowance σ are taken into account. The method also returns the period length. It is not intended for experimental but for simulated data.

The method has two parameters that can be chosen straightforwardly:

* The **maximum allowed period length τ<sub>max</sub>**: Choose this parameter reasonably low to limit the runtime and to avoid period lengths that are close to the length of the time series. Unless you know what you are doing, τ<sub>max</sub> should at least be half the length of the time series.

* The **error allowance σ:** Choose this parameter to correspond to the maximum expected absolute numerical error of your simulation, e.g., the maximum absolute integration error of an adaptive integrator.

Instructions for building
=========================

The following describes how to use this source depending on how you want to use it:

A. As a Python module
---------------------

Build and install the module by running (for example)

```sh
python3 setup.py install --user
```

This generates a Python module called `periodicitytest`, which contains one function called `periodicitytest`. It can be loaded, e.g., as follows:

```python
from periodicitytest import periodicitytest
```

For further documentation, see `periodicitytest`'s docstring.

B. As a standalone program
--------------------------

Build the program by running

```sh
python3 setup_C.py
```

or 

```sh
python3 setup_C.py test
```

if you want to run the tests.

This generates an executable called `periodicitytest` in the folder `bin`. It takes the maximum period length τ<sub>max</sub> and the error allowance σ as an argument. The time series is read from STDIN.

Take a look at `standalone.c` if you want to modify input and output.

C. As a C library
-----------------

The central function is contained and documented in `search.h`. For input and output it requires simple datatypes defined in `basics_standalone.h` and `interval.h`. It needs to compiled with `-DSTANDALONE`. Several costly assertions can be avoided by compiling with `-DNDEBUG`.
