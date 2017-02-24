from setuptools import setup, Extension
from io import open
import numpy

requirements = [
	'numpy',
]

setup(
	name = 'periodicitytest',
	description = 'a highly specific test for periodicity of time series based on folding',
	long_description = open('README.md', encoding='utf8').read(),
	author = 'Gerrit Ansmann',
	author_email = 'gansmann@uni-bonn.de',
	url = 'http://github.com/neurophysik/periodicitytest',
	packages = ['periodicitytest'],
	install_requires = requirements,
	classifiers = [
		'Development Status :: 4 - Beta',
		'License :: OSI Approved :: BSD License',
		'Operating System :: POSIX',
		'Operating System :: MacOS :: MacOS X',
		'Operating System :: Microsoft :: Windows',
		'Programming Language :: Python',
		'Programming Language :: C',
		'Topic :: Scientific/Engineering :: Mathematics',
		],
	ext_modules = [Extension(
		"periodicitytest._periodicitytest",
		sources = [
			"periodicitytest/basics_python.c",
			"periodicitytest/extremacounting.c",
			"periodicitytest/interval.c",
			"periodicitytest/search.c",
			"periodicitytest/_periodicitytest.c",
			],
		extra_compile_args = [
			"-D PYTHON",
			"-fPIC",
			"-Wall",
			"--pedantic",
			"-std=c11",
			"-Wno-unknown-pragmas",
			"-Wno-unused-function",
			],
		extra_link_args = ["-lm"],
		include_dirs = [numpy.get_include()]
		)],
	verbose = True
)

