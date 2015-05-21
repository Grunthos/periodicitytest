from math import *
import numpy as np

def f(t):
	x = 4/pi * (t%(2*pi))

	if (x<1):
		return x
	elif (x<2):
		return 1
	elif (x<3):
		return x-2
	elif (x<4):
		return 1
	elif (x<5):
		return x-3
	elif (x<6):
		return -x+7
	elif (x<7):
		return 1
	else:
		return x-6

for t in np.arange(0, 10, 0.01):
	print "%f\t%f" % (t,f(t))