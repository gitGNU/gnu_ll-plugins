PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = 0.1.131
PKG_DEPS = jack>=0.102.6 lash-1.0>=0.5.1 liblo>=0.22 gtkmm-2.4>=2.10.1 libglademm-2.4>=2.6.2 gsl>=1.8

ARCHIVES = liblv2_plugin.a libpaq.a liblv2_oscui.a

liblv2_plugin_a_SOURCES = \
	lv2plugin.hpp lv2plugin.cpp \
	lv2instrument.hpp lv2instrument.cpp
liblv2_plugin_a_CFLAGS = -Iextensions/instrument -I.
liblv2_plugin_a_SOURCEDIR = libraries/lv2plugin

libpaq_a_SOURCES = \
	turtleparser.hpp turtleparser.cpp \
	unicode.hpp \
	namespaces.hpp \
	rdf.hpp rdf.cpp \
	query.hpp query.cpp
libpaq_a_SOURCEDIR = libraries/paq

liblv2_oscui_a_SOURCES = lv2uiclient.hpp lv2uiclient.cpp
liblv2_oscui_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 liblo`
liblv2_oscui_a_SOURCEDIR = libraries/lv2oscui


PROGRAMS = lv2peg lv2host paqtest

lv2peg_SOURCES = lv2peg.cpp
lv2peg_CFLAGS = -Ilibraries/paq -DVERSION=\"$(PACKAGE_VERSION)\"
lv2peg_LDFLAGS = libraries/paq/libpaq.a
lv2peg_SOURCEDIR = programs/lv2peg

lv2host_SOURCES = \
	lv2host.hpp lv2host.cpp \
	osccontroller.hpp osccontroller.cpp \
	eventqueue.hpp eventqueue.cpp \
	main.cpp
lv2host_CFLAGS = `pkg-config --cflags jack liblo lash-1.0` -Iextensions/miditype -Iextensions/instrument -Ilibraries/paq -Ilibraries/components -I.
lv2host_LDFLAGS = `pkg-config --libs jack liblo lash-1.0` libraries/paq/libpaq.a -lpthread
lv2host_SOURCEDIR = programs/lv2host

paqtest_SOURCES = main.cpp
paqtest_LDFLAGS = libraries/paq/libpaq.a
paqtest_SOURCEDIR = libraries/paq

LV2_PLUGINS = control2midi.lv2 midi_identity.lv2 arpeggiator.lv2 math-constants.lv2 math-functions.lv2 phase-distortion-osc.lv2 euphoria.lv2 sineshaper.lv2 klaviatur.lv2 audio_identity.lv2 azr3.lv2

PLUGINFLAGS = -Wl,-whole-archive libraries/lv2plugin/liblv2_plugin.a -Wl,-no-whole-archive
INSTRUMENTFLAGS = -Wl,-whole-archive libraries/lv2plugin/liblv2_plugin.a -Wl,-no-whole-archive

# Control2MIDI
control2midi_lv2_SOURCES = control2midi.cpp
control2midi_lv2_DATA = manifest.ttl control2midi.ttl
control2midi_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -I.
control2midi_lv2_LDFLAGS = $(PLUGINFLAGS)
control2midi_lv2_SOURCEDIR = plugins/control2midi

# Phase distortion oscillator
phase-distortion-osc_lv2_SOURCES = phase-distortion-osc.cpp
phase-distortion-osc_lv2_DATA = manifest.ttl phase-distortion-osc.ttl
phase-distortion-osc_lv2_CFLAGS = -Ilibraries/lv2plugin -Ilibraries/components -I.
phase-distortion-osc_lv2_LDFLAGS = $(PLUGINFLAGS)
phase-distortion-osc_lv2_SOURCEDIR = plugins/phase-distortion-osc

# Klaviatur
klaviatur_lv2_SOURCES = klaviatur.cpp
klaviatur_lv2_DATA = manifest.ttl klaviatur.ttl
klaviatur_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -Ilibraries/components -I.
klaviatur_lv2_LDFLAGS = $(PLUGINFLAGS)
klaviatur_lv2_PEGFILES = klaviatur.peg
klaviatur_lv2_SOURCEDIR = plugins/klaviatur
klaviatur_lv2_PROGRAMS = klaviatur_gtk
klaviatur_gtk_SOURCES = klaviatur_gtk.cpp keyboard.cpp keyboard.hpp
klaviatur_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4 liblo` -Ilibraries/lv2oscui -I.
klaviatur_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo` libraries/lv2oscui/liblv2_oscui.a
klaviatur_gtk_SOURCEDIR = plugins/klaviatur

# Euphoria
euphoria_lv2_SOURCES = euphoria.cpp shaper.hpp shaper.cpp envelope.hpp
euphoria_lv2_DATA = manifest.ttl euphoria.ttl presets.ttl
euphoria_lv2_CFLAGS = -Ilibraries/lv2plugin -Ilibraries/components -Iextensions/instrument -Iextensions/miditype -I. `pkg-config --cflags gsl`
euphoria_lv2_LDFLAGS = $(INSTRUMENTFLAGS) `pkg-config --libs gsl`
euphoria_lv2_PEGFILES = euphoria.peg
euphoria_lv2_PROGRAMS = euphoria_gtk
euphoria_lv2_SOURCEDIR = plugins/euphoria
euphoria_gtk_SOURCES = \
	euphoria_gtk.cpp \
	envelopeeditor.cpp envelopeeditor.hpp \
	euphoriawidget.cpp euphoriawidget.hpp \
	pdeditor.cpp pdeditor.hpp \
	shapereditor.cpp shapereditor.hpp \
	vgknob.cpp vgknob.hpp
euphoria_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4 liblo` -Ilibraries/lv2oscui -I.
euphoria_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo` libraries/lv2oscui/liblv2_oscui.a
euphoria_gtk_SOURCEDIR = plugins/euphoria

# Sineshaper
sineshaper_lv2_SOURCES = \
	sineshaper.hpp sineshaper.cpp \
	sineshaperports.hpp \
	midiiterator.hpp
sineshaper_lv2_DATA = manifest.ttl sineshaper.ttl sineshaper.glade dial.png sineshaper.png presets.ttl
sineshaper_lv2_CFLAGS = -Ilibraries/lv2plugin -Ilibraries/components -Iextensions/miditype -Iextensions/instrument -I.
sineshaper_lv2_LDFLAGS = $(INSTRUMENTFLAGS)
sineshaper_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_lv2_PROGRAMS = sineshaper_gtk
sineshaper_gtk_SOURCES = sineshapergui.cpp sineshapergui.hpp main.cpp skindial_gtkmm.hpp skindial_gtkmm.cpp
sineshaper_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4 libglademm-2.4 liblo` -Ilibraries/lv2oscui -Ilibraries/components -Iextensions/instrument -I.
sineshaper_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo libglademm-2.4` libraries/lv2oscui/liblv2_oscui.a
sineshaper_gtk_SOURCEDIR = plugins/sineshaper

# MIDIIdentity
midi_identity_lv2_SOURCES = midi_identity.cpp
midi_identity_lv2_DATA = manifest.ttl midi_identity.ttl
midi_identity_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -Ilibraries/components -I.
midi_identity_lv2_LDFLAGS = $(PLUGINFLAGS)
midi_identity_lv2_SOURCEDIR = plugins/midi_identity

# AudioIdentity
audio_identity_lv2_SOURCES = audio_identity.cpp
audio_identity_lv2_DATA = manifest.ttl audio_identity.ttl
audio_identity_lv2_CFLAGS = -Ilibraries/lv2plugin -I.
audio_identity_lv2_LDFLAGS = $(PLUGINFLAGS)
audio_identity_lv2_SOURCEDIR = plugins/audio_identity

# Arpeggiator
arpeggiator_lv2_SOURCES = arpeggiator.cpp
arpeggiator_lv2_DATA = manifest.ttl arpeggiator.ttl
arpeggiator_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -I.
arpeggiator_lv2_LDFLAGS = $(PLUGINFLAGS)
arpeggiator_lv2_SOURCEDIR = plugins/arpeggiator

# Math constants
math-constants_lv2_SOURCES = math-constants.cpp
math-constants_lv2_DATA = manifest.ttl math-constants.ttl
math-constants_lv2_CFLAGS = -Ilibraries/lv2plugin -I.
math-constants_lv2_LDFLAGS = $(PLUGINFLAGS)
math-constants_lv2_SOURCEDIR = plugins/math-constants

# Math functions
math-functions_lv2_SOURCES = math-functions.cpp
math-functions_lv2_DATA = manifest.ttl math-functions.ttl
math-functions_lv2_CFLAGS = -Ilibraries/lv2plugin -I.
math-functions_lv2_LDFLAGS = $(PLUGINFLAGS)
math-functions_lv2_SOURCEDIR = plugins/math-functions

azr3_lv2_SOURCES = \
	azr3.cpp azr3.hpp \
	Globals.h \
	fx.h fx.cpp \
	voice_classes.h voice_classes.cpp \
	programlist.hpp \
	cknob.xpm minioffon.xpm onoffgreen.xpm panelfx.xpm vonoff.xpm voice.xpm num_yellow.xpm dbblack.xpm dbbrown.xpm dbwhite.xpm
azr3_lv2_DATA = manifest.ttl azr3.ttl presets.ttl
azr3_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/miditype -Iextensions/instrument -Ilibraries/components -I.
azr3_lv2_LDFLAGS = $(INSTRUMENTFLAGS)
azr3_lv2_SOURCEDIR = plugins/azr3
azr3_lv2_PROGRAMS = azr3_gtk
azr3_gtk_SOURCES = azr3_gtk.cpp knob.hpp knob.cpp switch.hpp switch.cpp drawbar.hpp drawbar.cpp textbox.hpp textbox.cpp
azr3_gtk_CFLAGS = `pkg-config --cflags gtkmm-2.4 liblo` -Ilibraries/lv2oscui
azr3_gtk_LDFLAGS = `pkg-config --libs gtkmm-2.4 gthread-2.0 liblo` libraries/lv2oscui/liblv2_oscui.a
azr3_gtk_SOURCEDIR = plugins/azr3


# The shared headers need to go in the distribution too
EXTRA_DIST = \
  lv2.h \
	libraries/components/adsr.hpp \
	libraries/components/chebyshevshaper.hpp \
	libraries/components/dcblocker.hpp \
	libraries/components/delay.hpp \
	libraries/components/envelopegenerator.hpp \
	libraries/components/frequencytable.hpp \
	libraries/components/ladspawrapper.hpp \
	libraries/components/lv2-midifunctions.h \
	libraries/components/monophonicmidinote.hpp \
	libraries/components/mooglpf.hpp \
	libraries/components/pdosc.hpp \
	libraries/components/polyphonicmidinote.hpp \
	libraries/components/programmanager.hpp \
	libraries/components/randomsineoscillator.hpp \
	libraries/components/ringbuffer.hpp \
	libraries/components/sineoscillator.hpp \
	libraries/components/slide.hpp \
	libraries/components/voicehandler.hpp \
	libraries/components/wavewrapper.hpp \
	\
	extensions/miditype/lv2-miditype.h \
	extensions/midimap/lv2-midimap.rdfs \
	extensions/instrument/lv2-instrument.h \
	extensions/transporttype/lv2-transport.h



# Do the magic
include Makefile.template
