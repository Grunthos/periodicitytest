#! /usr/bin/env python

# Tests the Python module.
from __future__ import print_function, division
from math import *
import numpy as np
from periodicitytest import periodicitytest
import random

def pertest_noise_wrapper(T, max_tau):
	noisy_T = T + np.random.uniform(0.0, nu, len(T))
	return periodicitytest(noisy_T, max_tau, nu)

# Test whether a period length makes the test (but not what is tested) faulty due to rounding errors.
is_problematic = lambda period,n: not (1e-8 < period*n*8 % 1 < 1-1e-8)

def closest_fractions (x, n):
	p,q,r,s = 0,1,1,0

	while p+r < n:
		if p+r < x*(q+s):
			p += r
			q += s
		else:
			r += p
			s += q
	
	return p,q,r,s

def contains_Interval (A, B):
	return (A[0]*B[1] <= B[0]*A[1]) and (B[2]*A[3] <= A[2]*B[3])

def nasty_function(t):
	"""
	A 2-pi-periodic function that looks like this:
	    _         
	   2 |  /|          /\    /|
	    _| / |  __   __/  \__/ |  __
	   1 |   | /  | /          | /
	    _|   |/   |/           |/
	   0 ---------------------------
	         |        /\       |
	         0        pi      2pi
	"""

	x = 4/pi * (t%(2*pi))

	if   x<1: return x
	elif x<2: return 1
	elif x<3: return x-2
	elif x<4: return 1
	elif x<5: return x-3
	elif x<6: return -x+7
	elif x<7: return 1
	else    : return x-6

def mostly_constant_function (t):
	x = 4/pi * (t%(2*pi))
	#return 1 if x<0.283 else 0
	return 1 if x<0.283 else 0


def test_half_timeseries_length (f):
	max_n = 2000
	
	for n in np.random.randint(100, max_n, 10):
		for period in np.random.uniform(n//2-1, n//2, 5):
			if is_problematic(period, n):
				continue
			
			T = np.array(list(map(lambda i: f(i*2*pi/period), range(max_n))))
			
			result = pertest_noise_wrapper(T[:n], n-1)
			assert result[0]
			
			control = closest_fractions(period, n)
			assert contains_Interval(result[1],control)

def test_varying_periods (f):
	n = 1000
	
	for period in np.random.uniform(28.3,n-1,20):
		if is_problematic(period, n):
			continue
		
		T = np.array(list(map(lambda i: f(i*2*pi/period), range(n))))
		
		result = pertest_noise_wrapper(T[:n], n-1)
		assert result[0]
		
		control = closest_fractions(period, n)
		assert contains_Interval(result[1],control)

def test_random_timeseries ():
	n = 1000
	T = np.random.rand(n)
	assert not pertest_noise_wrapper(T, n//2)[0]

def test_constant_timeseries ():
	n = 1000
	T = np.ones(n)*np.random.rand()
	
	for max_tau in np.random.randint(5,n,30):
		result = pertest_noise_wrapper(T, max_tau)
		assert result[0]
		control = (2,1,max_tau,1)
		assert result[1]==control

def test_period_k_timeseries (k):
	max_n = 2000
	some_numbers = np.arange(0,100,max(0.01, 2*nu))
	
	for n in np.random.randint(100,max_n,20):
		np.random.shuffle(some_numbers)
		T = np.array(list(map(lambda i: some_numbers[i%k], range(n))))
		result = pertest_noise_wrapper(T, n-1)
		
		assert result[0]
		
		def test(i):
			if k%i==0:
				return contains_Interval(result[1], (k//i,1,k/i,1))
			else:
				return contains_Interval(result[1], closest_fractions(k/i, n))
		
		assert any(test(i) for i in range(1,k))

def test_varying_amplitude (f):
	eps = 0.001
	n = 10000
	T = np.array( list(map(lambda i: (1+eps*i)*f(i), range(n))) )
	
	assert not pertest_noise_wrapper(T,n//2)[0]

def test_varying_frequency (f):
	eps = 0.00001
	n = 10000
	T = np.array( list(map(lambda i: f((1+eps*i)*i), range(n))) )
	
	assert not pertest_noise_wrapper(T,n//2)[0]

for nu in list(np.arange(0,0.1,0.02))+[1e-100]:
	test_half_timeseries_length(sin)
	test_half_timeseries_length(cos)
	test_half_timeseries_length(nasty_function)
	test_half_timeseries_length(mostly_constant_function)
	test_half_timeseries_length(lambda x: x%(2*pi))
	test_half_timeseries_length(lambda x: round(100*sin(x)))
	test_half_timeseries_length(lambda x: sin(x)+sin(2*x))
	
	test_varying_periods(sin)
	test_varying_periods(cos)
	test_varying_periods(lambda x: tan(x/2))
	test_varying_periods(nasty_function)
	test_varying_periods(mostly_constant_function)
	test_varying_periods(lambda x: x%(2*pi))
	test_varying_periods(lambda x: round(100*sin(x)))
	test_varying_periods(lambda x: sin(x)+sin(2*x))
	
	test_random_timeseries()
	
	test_constant_timeseries()
	
	for k in range(2,20):
		test_period_k_timeseries(20)
	
	test_varying_amplitude(sin)
	test_varying_amplitude(cos)
	test_varying_amplitude(nasty_function)
	test_varying_amplitude(lambda x: x%(2*pi))
	test_varying_amplitude(lambda x: round(100*sin(x)))
	test_varying_amplitude(lambda x: sin(x)+sin(2*x))
	
	test_varying_frequency(sin)
	test_varying_frequency(cos)
	test_varying_frequency(nasty_function)
	test_varying_frequency(lambda x: x%(2*pi))
	test_varying_frequency(lambda x: round(100*sin(x)))
	test_varying_frequency(lambda x: sin(x)+sin(2*x))
