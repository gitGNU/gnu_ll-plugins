PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = 0.1.7
PKG_DEPS = jack>=0.102.6 liblo>=0.22


ARCHIVES = liblv2_plugin.a libpaq.a

liblv2_plugin_a_SOURCES = \
	lv2plugin.hpp lv2plugin.cpp \
	lv2instrument.hpp lv2instrument.cpp
liblv2_plugin_a_CFLAGS = -Iextensions/instrument
liblv2_plugin_a_SOURCEDIR = libraries/lv2plugin

libpaq_a_SOURCES = \
	turtleparser.hpp turtleparser.cpp \
	unicode.hpp \
	namespaces.hpp \
	rdf.hpp rdf.cpp \
	query.hpp query.cpp
libpaq_a_SOURCEDIR = libraries/paq


PROGRAMS = lv2peg lv2host

lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -Ilibraries/paq
lv2peg_LDFLAGS = libpaq.a
lv2peg_SOURCEDIR = programs/lv2peg

lv2host_SOURCES = lv2host.hpp lv2host.cpp main.cpp
lv2host_CFLAGS = `pkg-config --cflags jack liblo` -Iextensions/miditype -Iextensions/instrument -Ilibraries/paq
lv2host_LDFLAGS = `pkg-config --libs jack liblo` libpaq.a
lv2host_SOURCEDIR = programs/lv2host

LV2_PLUGINS = sineshaper.lv2 control2midi.lv2 midi_identity.lv2 arpeggiator.lv2

# Control2MIDI
control2midi_lv2_SOURCES = control2midi.cpp
control2midi_lv2_TURTLE = manifest.ttl control2midi.ttl
control2midi_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
control2midi_lv2_LDFLAGS = liblv2_plugin.a
control2midi_lv2_SOURCEDIR = plugins/control2midi

# Sineshaper
sineshaper_lv2_SOURCES = \
	sineshaper.hpp sineshaper.cpp \
	sineshaperports.hpp \
	midiiterator.hpp
sineshaper_lv2_TURTLE = manifest.ttl sineshaper.ttl
sineshaper_lv2_CFLAGS = -Ilibraries/lv2plugin -Ilibraries/components -Iextensions/miditype -Iextensions/instrument
sineshaper_lv2_LDFLAGS = liblv2_plugin.a
sineshaper_lv2_SOURCEDIR = plugins/sineshaper

# MIDIIdentity
midi_identity_lv2_SOURCES = midi_identity.cpp
midi_identity_lv2_TURTLE = manifest.ttl midi_identity.ttl
midi_identity_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
midi_identity_lv2_LDFLAGS = liblv2_plugin.a
midi_identity_lv2_SOURCEDIR = plugins/midi_identity

# Arpeggiator
arpeggiator_lv2_SOURCES = arpeggiator.cpp
arpeggiator_lv2_TURTLE = manifest.ttl arpeggiator.ttl
arpeggiator_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
arpeggiator_lv2_LDFLAGS = liblv2_plugin.a
arpeggiator_lv2_SOURCEDIR = plugins/arpeggiator


# The shared headers need to go in the distribution too
EXTRA_DIST = \
	libraries/components/adsr.hpp \
	libraries/components/chebyshevshaper.hpp \
	libraries/components/dcblocker.hpp \
	libraries/components/delay.hpp \
	libraries/components/envelopegenerator.hpp \
	libraries/components/frequencytable.hpp \
	libraries/components/ladspawrapper.hpp \
	libraries/components/monophonicmidinote.hpp \
	libraries/components/mooglpf.hpp \
	libraries/components/polyphonicmidinote.hpp \
	libraries/components/randomsineoscillator.hpp \
	libraries/components/ringbuffer.hpp \
	libraries/components/sineoscillator.hpp \
	libraries/components/slide.hpp \
	libraries/components/wavewrapper.hpp \
	\
	extensions/miditype/lv2-miditype.h \
	extensions/midimap/lv2-midimap.rdfs \
	extensions/instrument/lv2-instrument.h \
	extensions/transporttype/lv2-transport.h



# Do the magic
include Makefile.template
