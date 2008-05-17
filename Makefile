PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = $(shell git describe --match 'Version_*' | sed 's/Version_//' | sed 's/-/ /g' | awk '{ print $$1 " " $$2}' | sed -r 's/\.([0-9]+) / \1 /' | awk '{ print $$1 "." $$2+$$3 }')$(shell if test $$(git ls-files --modified | wc -l) -gt 0 ; then echo .EDITED; fi)
PKG_DEPS = \
	cairomm-1.0>=1.2.4 \
	gtkmm-2.4>=2.8.8 \
	jack>=0.109.0 \
	lash-1.0>=0.5.1 \
	lv2-plugin>=1.0.0 \
	lv2-gui>=1.0.0 \
	paq>=1.0.0 \
	sndfile>=1.0.16 \
	samplerate>=0.1.2


DOCS = AUTHORS COPYING


ARCHIVES = \
	libenvelopeeditor.a \
	libkeyboard.a \
	libvuwidget.a

PROGRAMS = elven

LV2_BUNDLES = \
	arpeggiator.lv2 \
	beep.lv2 \
	control2midi.lv2 \
	klaviatur.lv2 \
	nekobee_blue_gui.lv2 \
	phase-distortion-osc.lv2 \
	vumeter.lv2 \
	vumeter_gtk.lv2 \
	tableosc.lv2


# Archives with useful code bits

libkeyboard_a_SOURCES = keyboard.hpp keyboard.cpp
libkeyboard_a_CFLAGS = `pkg-config --cflags gtkmm-2.4`
libkeyboard_a_SOURCEDIR = libraries/widgets

libenvelopeeditor_a_SOURCES = envelopeeditor.hpp envelopeeditor.cpp
libenvelopeeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libenvelopeeditor_a_SOURCEDIR = libraries/widgets

libvuwidget_a_SOURCES = vuwidget.hpp vuwidget.cpp
libvuwidget_a_CFLAGS = `pkg-config --cflags gtkmm-2.4`
libvuwidget_a_SOURCEDIR = libraries/widgets


# Executable programs

elven_SOURCES = \
	debug.hpp \
	lv2guihost.hpp lv2guihost.cpp \
	lv2host.hpp lv2host.cpp \
	main.cpp \
	midiutils.hpp
elven_CFLAGS = `pkg-config --cflags jack gtkmm-2.4 lash-1.0 sigc++-2.0 lv2-plugin lv2-gui paq` -Ilibraries/components -DVERSION=\"$(PACKAGE_VERSION)\"
elven_LDFLAGS = `pkg-config --libs jack gtkmm-2.4 lash-1.0 sigc++-2.0 paq` -lpthread
elven_SOURCEDIR = programs/elven


# The plugins

PLUGINARCHIVES = `pkg-config --libs lv2-plugin`
PLUGINCFLAGS = `pkg-config --cflags lv2-plugin`
ADVANCEDARCHIVES = $(PLUGINARCHIVES)
ADVANCEDCFLAGS = $(PLUGINCFLAGS)
RESIDENTGUI = lv2soname '<http://ll-plugins.nongnu.org/lv2/dev/klaviatur/0.0.0/gui>' '<http://lv2plug.in/ns/extensions/ui\#requiredFeature>' -lgtkmm-2.4

# Beep
beep_lv2_MODULES = beep.so beep_gtk.so
beep_lv2_DATA = manifest.ttl beep.ttl
beep_lv2_SOURCEDIR = plugins/beep
beep_so_SOURCES = beep.cpp
beep_so_CFLAGS = $(PLUGINCFLAGS)
beep_so_LDFLAGS = $(PLUGINARCHIVES)
beep_gtk_so_SOURCES = beep_gtk.cpp
beep_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui`
beep_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui`

# Control2MIDI
control2midi_lv2_MODULES = control2midi.so
control2midi_lv2_DATA = manifest.ttl control2midi.ttl
control2midi_lv2_SOURCEDIR = plugins/control2midi
control2midi_so_SOURCES = control2midi.cpp
control2midi_so_CFLAGS = $(PLUGINCFLAGS)
control2midi_so_LDFLAGS = $(PLUGINARCHIVES)

# Phase distortion oscillator
phase-distortion-osc_lv2_MODULES = phase-distortion-osc.so
phase-distortion-osc_lv2_DATA = manifest.ttl phase-distortion-osc.ttl
phase-distortion-osc_lv2_SOURCEDIR = plugins/phase-distortion-osc
phase-distortion-osc_so_SOURCES = phase-distortion-osc.cpp
phase-distortion-osc_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
phase-distortion-osc_so_LDFLAGS = $(PLUGINARCHIVES)

# Klaviatur
klaviatur_lv2_MODULES = klaviatur.so klaviatur_gtk.so
klaviatur_lv2_DATA = manifest.ttl klaviatur.ttl
klaviatur_lv2_PEGFILES = klaviatur.peg
klaviatur_lv2_SOURCEDIR = plugins/klaviatur
klaviatur_so_SOURCES = klaviatur.cpp
klaviatur_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
klaviatur_so_LDFLAGS = $(PLUGINARCHIVES)
klaviatur_gtk_so_SOURCES = klaviatur_gtk.cpp
klaviatur_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui` -Ilibraries/widgets
klaviatur_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui` 
klaviatur_gtk_so_ARCHIVES = libraries/widgets/libkeyboard.a
klaviatur_lv2_POSTINSTALL = $(RESIDENTGUI) >> klaviatur.lv2/klaviatur.ttl

# Nekobee Blue GUI
nekobee_blue_gui_lv2_MODULES = nekobee_blue_gui.so
nekobee_blue_gui_lv2_DATA = manifest.ttl drawing.png
nekobee_blue_gui_lv2_SOURCEDIR = plugins/nekobee_blue_gui
nekobee_blue_gui_so_SOURCES = \
	nekobee_blue_gui.cpp \
	nekobeewidget.cpp nekobeewidget.hpp
nekobee_blue_gui_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui`
nekobee_blue_gui_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
nekobee_blue_gui_lv2_POSTINSTALL = $(RESIDENTGUI) >> nekobee_blue_gui.lv2/manifest.ttl

# Householder FDN
#hhfdn_lv2_SOURCES = hhfdn.cpp
#hhfdn_lv2_DATA = manifest.ttl hhfdn.ttl
#hhfdn_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/MidiPort -Ilibraries/components -I.
#hhfdn_lv2_LDFLAGS = $(PLUGINARCHIVES)
#hhfdn_lv2_SOURCEDIR = plugins/householderfdn

# Arpeggiator
arpeggiator_lv2_MODULES = arpeggiator.so
arpeggiator_so_SOURCES = arpeggiator.cpp
arpeggiator_so_CFLAGS = $(PLUGINCFLAGS)
arpeggiator_so_LDFLAGS = $(PLUGINARCHIVES)
arpeggiator_lv2_DATA = manifest.ttl arpeggiator.ttl
arpeggiator_lv2_SOURCEDIR = plugins/arpeggiator

# VU meter
vumeter_lv2_MODULES = vumeter.so
vumeter_so_SOURCES = vumeter.cpp
vumeter_so_CFLAGS = $(PLUGINCFLAGS)
vumeter_so_LDFLAGS = $(PLUGINARCHIVES)
vumeter_lv2_DATA = manifest.ttl vumeter.ttl icon.svg
vumeter_lv2_SOURCEDIR = plugins/vumeter

# VU meter GUI
vumeter_gtk_lv2_MODULES = vumeter_gtk.so
vumeter_gtk_lv2_SOURCEDIR = plugins/vumeter
vumeter_gtk_lv2_MANIFEST = gui_manifest.ttl
vumeter_gtk_so_SOURCES = vumeter_gtk.cpp
vumeter_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
vumeter_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
vumeter_gtk_so_ARCHIVES = libraries/widgets/libvuwidget.a

# Envelope
envelope_lv2_MODULES = envelope.so
envelope_so_SOURCES = envelope.cpp
envelope_so_CFLAGS = $(ADVANCEDCFLAGS) -Ilibraries/components
envelope_so_LDFLAGS = $(ADVANCEDARCHIVES)
envelope_lv2_DATA = manifest.ttl envelope.ttl icon.svg
envelope_lv2_SOURCEDIR = plugins/envelope

# VU meter GUI
envelope_gtk_lv2_MODULES = envelope_gtk.so
envelope_gtk_lv2_SOURCEDIR = plugins/envelope
envelope_gtk_lv2_MANIFEST = gui_manifest.ttl
envelope_gtk_so_SOURCES = envelope_gtk.cpp
envelope_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
envelope_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
envelope_gtk_so_ARCHIVES = libraries/widgets/libenvelopeeditor.a

# Table oscillator
tableosc_lv2_MODULES = tableosc.so
tableosc_lv2_DATA = manifest.ttl tableosc.ttl presets.ttl money.wav
tableosc_lv2_SOURCEDIR = plugins/tableosc
tableosc_so_SOURCES = tableosc.cpp
tableosc_so_CFLAGS = $(PLUGINCFLAGS) `pkg-config --cflags sndfile samplerate`
tableosc_so_LDFLAGS = $(PLUGINARCHIVES) `pkg-config --libs sndfile samplerate`


# The shared headers need to go in the distribution too
EXTRA_DIST = \
	libraries/components/adsr.hpp \
	libraries/components/chebyshevshaper.hpp \
	libraries/components/dcblocker.hpp \
	libraries/components/delay.hpp \
	libraries/components/delayline.hpp \
	libraries/components/distortion.hpp \
	libraries/components/envelopegenerator.hpp \
	libraries/components/filter.hpp \
	libraries/components/frequencytable.hpp \
	libraries/components/householderfdn.hpp \
	libraries/components/ladspawrapper.hpp \
	libraries/components/markov.hpp \
	libraries/components/monophonicmidinote.hpp \
	libraries/components/monostep.hpp \
	libraries/components/mooglpf.hpp \
	libraries/components/pdosc.hpp \
	libraries/components/polyphonicmidinote.hpp \
	libraries/components/programmanager.hpp \
	libraries/components/randomsineoscillator.hpp \
	libraries/components/ringbuffer.hpp \
	libraries/components/sineoscillator.hpp \
	libraries/components/slide.hpp \
	libraries/components/voicehandler.hpp \
	libraries/components/wavewrapper.hpp


# Do the magic
include Makefile.template
