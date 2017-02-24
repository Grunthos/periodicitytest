// Just a file for loading the right header and defining pi.

# ifndef M_PI
# define M_PI 3.14159265358979323846264338327950288419716939937510
# endif

# ifdef STANDALONE
	# include "basics_standalone.h"
# endif

# ifdef PYTHON
	# include "basics_python.h"
# endif
