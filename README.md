The following describes how to use this source depending on how you want to use it:

A. As a Python module
---------------------

Build the module by running

```sh
python build.py Python
```

or 

```sh
python build.py Python notest
```

if you want to skip the tests to speed things up.

This generates a Python module file called `periodicitytest.so`, which contains one function called periodicitytest. It can be loaded, e.g., as follows:

```python
from periodicitytest import periodicitytest
```

For further documentation, see `periodicitytest`'s docstring.

B. As a standalone program
--------------------------

Build the program by running

```sh
python build.py C
```

or 

```sh
python build.py C notest
```

if you want to skip the tests to speed things up.

This generates an executable called `standalone`. It takes the maximum period length max_tau and the noise allowance sigma as an argument. The time series is read from STDIN.

Take a look at `standalone.c` if you want to modify input and output.

C. As a C library
-----------------

The central function is contained and documented in `search.h`. For input and output it requires simple datatypes defined in `basics_standalone.h` and `interval.h`. It needs to compiled with `-DSTANDALONE`. Several costly assertions can be avoided by compiling with `-DNDEBUG`.
