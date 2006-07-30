PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = 0.1.68
PKG_DEPS = jack>=0.102.6 liblo>=0.22 gtkmm-2.4>=2.8.8 libglademm-2.4>=2.6.2

ARCHIVES = liblv2_plugin.a libpaq.a liblv2_oscui.a

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

liblv2_oscui_a_SOURCES = lv2uiclient.hpp lv2uiclient.cpp lv2_shm.h lv2_shm.c
liblv2_oscui_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 liblo`
liblv2_oscui_a_SOURCEDIR = libraries/lv2oscui


PROGRAMS = lv2peg lv2host

lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -Ilibraries/paq
lv2peg_LDFLAGS = libraries/paq/libpaq.a
lv2peg_SOURCEDIR = programs/lv2peg

lv2host_SOURCES = \
	lv2host.hpp lv2host.cpp \
	osccontroller.hpp osccontroller.cpp \
	eventqueue.hpp eventqueue.cpp \
	main.cpp
lv2host_CFLAGS = `pkg-config --cflags jack liblo` -Iextensions/miditype -Iextensions/instrument -Ilibraries/paq -Ilibraries/components
lv2host_LDFLAGS = `pkg-config --libs jack liblo` libraries/paq/libpaq.a -lpthread
lv2host_SOURCEDIR = programs/lv2host

LV2_PLUGINS = sineshaper.lv2 control2midi.lv2 midi_identity.lv2 arpeggiator.lv2 math-constants.lv2 math-functions.lv2 azr3.lv2

# Control2MIDI
control2midi_lv2_SOURCES = control2midi.cpp
control2midi_lv2_DATA = manifest.ttl control2midi.ttl
control2midi_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
control2midi_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
control2midi_lv2_SOURCEDIR = plugins/control2midi

# Sineshaper
sineshaper_lv2_SOURCES = \
	sineshaper.hpp sineshaper.cpp \
	sineshaperports.hpp \
	midiiterator.hpp
sineshaper_lv2_DATA = manifest.ttl sineshaper.ttl sineshaper.glade dial.png sineshaper.png presets
sineshaper_lv2_CFLAGS = -Ilibraries/lv2plugin -Ilibraries/components -Iextensions/miditype -Iextensions/instrument
sineshaper_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
sineshaper_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_lv2_PROGRAMS = sineshaper_gtk
sineshaper_gtk_SOURCES = sineshapergui.cpp sineshapergui.hpp main.cpp skindial_gtkmm.hpp skindial_gtkmm.cpp
sineshaper_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4 libglademm-2.4` -Ilibraries/lv2oscui -Ilibraries/components -Iextensions/instrument
sineshaper_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo libglademm-2.4` libraries/lv2oscui/liblv2_oscui.a
sineshaper_gtk_SOURCEDIR = plugins/sineshaper

# MIDIIdentity
midi_identity_lv2_SOURCES = midi_identity.cpp
midi_identity_lv2_DATA = manifest.ttl midi_identity.ttl
midi_identity_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
midi_identity_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
midi_identity_lv2_SOURCEDIR = plugins/midi_identity

# Arpeggiator
arpeggiator_lv2_SOURCES = arpeggiator.cpp
arpeggiator_lv2_DATA = manifest.ttl arpeggiator.ttl
arpeggiator_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype
arpeggiator_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
arpeggiator_lv2_SOURCEDIR = plugins/arpeggiator

# Math constants
math-constants_lv2_SOURCES = math-constants.cpp
math-constants_lv2_DATA = manifest.ttl math-constants.ttl
math-constants_lv2_CFLAGS = -Ilibraries/lv2plugin
math-constants_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
math-constants_lv2_SOURCEDIR = plugins/math-constants

# Math functions
math-functions_lv2_SOURCES = math-functions.cpp
math-functions_lv2_DATA = manifest.ttl math-functions.ttl
math-functions_lv2_CFLAGS = -Ilibraries/lv2plugin
math-functions_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
math-functions_lv2_SOURCEDIR = plugins/math-functions

azr3_lv2_SOURCES = \
	azr3.cpp azr3.hpp \
	Globals.h \
	fx.h fx.cpp \
	voice_classes.h voice_classes.cpp \
	cknob.xpm minioffon.xpm onoffgreen.xpm panelfx.xpm vonoff.xpm
azr3_lv2_DATA = manifest.ttl azr3.ttl
azr3_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -Iextensions/instrument -Ilibraries/components
azr3_lv2_LDFLAGS = libraries/lv2plugin/liblv2_plugin.a
azr3_lv2_SOURCEDIR = plugins/azr3
azr3_lv2_PROGRAMS = azr3_gtk
azr3_gtk_SOURCES = azr3_gtk.cpp knob.hpp knob.cpp switch.hpp switch.cpp drawbar.hpp drawbar.cpp
azr3_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Ilibraries/lv2oscui
azr3_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo` libraries/lv2oscui/liblv2_oscui.a
azr3_gtk_SOURCEDIR = plugins/azr3


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
	libraries/components/programmanager.hpp \
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
