# include "testfunctions.h"
# include <math.h>
# include <stdlib.h>

# define M_PI 3.14159265358979323846

/* A 2-pi-periodic function that looks like this:
   _         
  2 |  /|          /\    /|
   _| / |  __   __/  \__/ |  __
  1 |   | /  | /          | /
   _|   |/   |/           |/
  0 ---------------------------
        |        /\       |
        0        pi      2pi              */

double nasty_function (double x)
{
	x = 4/M_PI * fmod (x, 2*M_PI);
	
	if (x<1)
		return x;
	else if (x<2)
		return 1;
	else if (x<3)
		return x-2;
	else if (x<4)
		return 1;
	else if (x<5)
		return x-3;
	else if (x<6)
		return -x+7;
	else if (x<7)
		return 1;
	else
		return x-6;
}

double mostly_constant_function (double x)
{
	x = 4/M_PI * fmod (x, 2*M_PI);
	
	if (x<0.283)
		return 1;
	else
		return 0;
}

double sawtooth (double x)
{
	return fmod (x, 2*M_PI);
}

double rounded_sin (double x)
{
	const double faktor = 100;
	return round(faktor*sin(x))*(1/faktor);
}

double sin_x_plus_sin_2x (double x)
{
	return sin(x)+sin(2*x);
}

# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"

double random_double (double x)
{
	return (double) rand();
}

double random_bool (double x)
{
	return (double) (rand()%2);
}

double zero (double x)
{
	return 0;
}

# pragma GCC diagnostic pop
