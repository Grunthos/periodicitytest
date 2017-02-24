#! /usr/bin/env python
from __future__ import print_function
import sys
import os
import subprocess

do_test = "test" in sys.argv

CC = [os.environ['CC']]

flags = [
	"-Wall",
	"-Wextra",
	"-lm",
	"--pedantic",
	"-O3",
	"-DSTANDALONE"
	]

if "gcc" in CC[0]:
	flags += [
		"-std=c11",
		#"-march=native",
		#"-mtune=native"
		]
elif "clang" in CC[0]:
	flags += ["-Wno-unknown-pragmas"]

if not os.path.exists("bin"):
	os.mkdir("bin")

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

def O(folder, Cfile, output=None):
	if output is None:
		return [
			os.path.join(folder,Cfile),
			"-o", os.path.join("bin", Cfile.replace(".c",""))
			]
	else:
		return [
			os.path.join(folder,Cfile),
			"-o", os.path.join("bin", output)
			]

def build (tests=False):
	F = flags
	L = [
		os.path.join("periodicitytest", "basics_standalone.c"),
		os.path.join("periodicitytest", "interval.c"),
		os.path.join("periodicitytest", "extremacounting.c"),
		os.path.join("periodicitytest", "search.c"),
		]
	
	if tests:
		F += ["-g"]
		L += [os.path.join("tests","testfunctions.c")]
		run_command(CC + L + O("tests", "extremacounting_test.c") + F)
		run_command(CC + L + O("tests", "search_test.c") + F)
	else:
		F += ["-DNDEBUG"]
		run_command(CC + L + O("periodicitytest", "standalone.c", "periodicitytest") + F)

if do_test:
	announce("Building tests.")
	build(tests=True)

	announce("Running tests.")
	test([
		os.path.join("bin", "extremacounting_test"),
		os.path.join("bin", "search_test")
		])

announce("Building executable.")
build()
