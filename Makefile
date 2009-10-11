PACKAGE_NAME = elven
PACKAGE_VERSION = $(shell ./VERSION)
PKG_DEPS = \
	redland>=1.0.9

ARCHIVES = libpaq.a

# The static version of PAQ
libpaq_a_SOURCES = \
	namespaces.hpp \
	query.hpp query.cpp \
	rdf.hpp rdf.cpp \
	turtleparser.hpp turtleparser.cpp \
	unicode.hpp
libpaq_a_SOURCEDIR = paq

# extra files
DOCS = COPYING AUTHORS README ChangeLog


# Do the magic
include Makefile.template

