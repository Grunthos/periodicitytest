#! /usr/bin/env python
from __future__ import print_function
import sys
import subprocess

if len(sys.argv)==1:
	do_python = True
	do_c = True
	do_test = True
else:
	do_python = ("Python" in sys.argv)
	do_c = ("C" in sys.argv)
	do_test = not ("notest" in sys.argv)

if do_python:
	try:
		import numpy as np
	except ImportError:
		warn("Numpy not found!")
		do_python = False

CC = ["clang"]

general_flags = [
	"-Wall",
	"-Wextra",
	"-lm",
	"--pedantic",
	"-O3",
	]

if "gcc" in CC:
	general_flags += [
		"-std=c11",
		#"-march=native",
		#"-mtune=native"
		]
elif "clang" in CC:
	general_flags += ["-Wno-unknown-pragmas"]

if do_c:
	standalone_flags = [
		"-DSTANDALONE"
		]

if do_python:
	python_flags = [
		"-D PYTHON",
		"-fPIC",
		"-I" + np.get_include(),
		"-Wno-unused-function"
		]
	python_flags += subprocess.check_output(["python-config", "--includes"]).decode().split()
	python_flags += subprocess.check_output(["python-config", "--libs"]).decode().split()

def run_command (components):
	command = " ".join(components)
	print(command)
	if subprocess.check_call(components):
		exit(1)

def announce(text):
	print ((4+len(text)) *"=")
	print ("  " + text + "  ")
	print ((4+len(text)) *"=")

def test (commands):
	for command in commands:
		print(command)
		if subprocess.check_call(command, shell=True):
			exit(1)
		else:
			print("Success.")

def O (Cfile, suffix=""):
	return [Cfile, "-o", Cfile.replace(".c","") + suffix]

def build (test=True, python=True):
	if python:
		F = general_flags + python_flags
		L = ["basics_python.c"]
		suffix = "_python"
	else:
		F = general_flags + standalone_flags
		L = ["basics_standalone.c"]
		suffix = "_standalone"
	
	L += ["interval.c", "extremacounting.c", "search.c"]
	
	if test:
		F += ["-g"]
		L += ["testfunctions.c"]
		run_command(CC + L + O("extremacounting_test.c", suffix) + F)
		run_command(CC + L + O("search_test.c", suffix) + F)
	else:
		F += ["-DNDEBUG"]
		if python:
			run_command(CC + L + ["periodicitytest.c","-o","periodicitytest.so"] + F + ["-shared"])
		else:
			run_command(CC + L + O("standalone.c") + F)

if do_c:
	if do_test:
		announce ("Building standalone tests.")
		build (True, False)

		announce("Testing standalone.")
		test(["./extremacounting_test_standalone","./search_test_standalone"])
	
	announce("Building standalone.")
	build (False, False)

if do_python:
	if do_test:
		announce ("Building Python tests.")
		build (True, True)
		
		announce("Testing libraries for Python.")
		test(["./extremacounting_test_python", "./search_test_python"])
	
	announce("Building Python module.")
	build (False, True)
	
	if do_test:
		announce("Testing Python module.")
		import periodicitytest_test
		print("Success.")
