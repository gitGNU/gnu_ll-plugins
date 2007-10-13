# This package requires that the lv2-c++-tools package is already installed.
# It can be downloaded using CVS from the module lv2-c++-tools in the same
# repository as the one you downloaded ll-plugins from.


PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = 0.1.360
PKG_DEPS = \
	cairomm-1.0>=1.2.4 \
	gsl>=1.8 \
	gtkmm-2.4>=2.8.8 \
	jack>=0.107.2 \
	lash-1.0>=0.5.1 \
	lv2-plugin>=0.1.353 \
	lv2-gui>=0.1.354 \
	paq>=0.1.338 \
	sndfile>=1.0.16


ARCHIVES = \
	libenvelopeeditor.a \
	libkeyboard.a \
	libpatternwidget.a \
	libpdeditor.a \
	libshapereditor.a \
	libspringeditor.a \
	libtransitioneditor.a \
	libvgknob.a \
	libvuwidget.a

PROGRAMS = elven

LV2_BUNDLES = \
	arpeggiator.lv2 \
	audio_identity.lv2 \
	beep.lv2 \
	control2midi.lv2 \
	gcf.lv2 \
	klaviatur.lv2 \
	midi_identity.lv2 \
	math-constants.lv2 \
	math-functions.lv2 \
	nekobee_blue_gui.lv2 \
	phase-distortion-osc.lv2 \
	sineshaper.lv2 \
	sineshaper_gtk.lv2 \
	vumeter.lv2 \
	vumeter_gtk.lv2 \
	envelope.lv2 \
	envelope_gtk.lv2 \
	euphoria.lv2 \
	euphoria_gtk.lv2 \
	horizon.lv2 \
	horizon_gtk.lv2
#	trilobeat.lv2 \
#	trilobyte.lv2 \


# Archives with useful code bits

libkeyboard_a_SOURCES = keyboard.hpp keyboard.cpp
libkeyboard_a_CFLAGS = `pkg-config --cflags gtkmm-2.4`
libkeyboard_a_SOURCEDIR = libraries/widgets

libvgknob_a_SOURCES = vgknob.hpp vgknob.cpp
libvgknob_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libvgknob_a_SOURCEDIR = libraries/widgets

libenvelopeeditor_a_SOURCES = envelopeeditor.hpp envelopeeditor.cpp
libenvelopeeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libenvelopeeditor_a_SOURCEDIR = libraries/widgets

libshapereditor_a_SOURCES = shapereditor.hpp shapereditor.cpp
libshapereditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libshapereditor_a_SOURCEDIR = libraries/widgets

libspringeditor_a_SOURCES = springeditor.hpp springeditor.cpp
libspringeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libspringeditor_a_SOURCEDIR = libraries/widgets

libpdeditor_a_SOURCES = pdeditor.hpp pdeditor.cpp
libpdeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libpdeditor_a_SOURCEDIR = libraries/widgets

libtransitioneditor_a_SOURCES = transitioneditor.hpp transitioneditor.cpp
libtransitioneditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libtransitioneditor_a_SOURCEDIR = libraries/widgets

libvuwidget_a_SOURCES = vuwidget.hpp vuwidget.cpp
libvuwidget_a_CFLAGS = `pkg-config --cflags gtkmm-2.4`
libvuwidget_a_SOURCEDIR = libraries/widgets

libpatternwidget_a_SOURCES = patternwidget.hpp patternwidget.cpp
libpatternwidget_a_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Ilibraries/components
libpatternwidget_a_SOURCEDIR = libraries/widgets


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

# Beep
beep_lv2_MODULES = beep.so
beep_lv2_DATA = manifest.ttl beep.ttl
beep_lv2_SOURCEDIR = plugins/beep
beep_so_SOURCES = beep.cpp
beep_so_CFLAGS = $(PLUGINCFLAGS)
beep_so_LDFLAGS = $(PLUGINARCHIVES)

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
klaviatur_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
klaviatur_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
klaviatur_gtk_so_ARCHIVES = libraries/widgets/libkeyboard.a

# Trilobyte
trilobyte_lv2_MODULES = trilobyte.so
trilobyte_so_SOURCES = trilobyte.cpp
trilobyte_so_CFLAGS = $(ADVANCEDCFLAGS) -Ilibraries/components
trilobyte_so_LDFLAGS = $(ADVANCEDARCHIVES)
trilobyte_gtk_so_SOURCES = trilobyte_gtk.cpp
trilobyte_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets -Ilibraries/components
trilobyte_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui`
trilobyte_gtk_so_ARCHIVES = libraries/widgets/libpatternwidget.a
trilobyte_gtk_so_SOURCEDIR = plugins/trilobyte
trilobyte_lv2_DATA = manifest.ttl trilobyte.ttl patternbg.png
trilobyte_lv2_PEGFILES = trilobyte.peg
trilobyte_lv2_SOURCEDIR = plugins/trilobyte

# Trilobeat
trilobeat_lv2_MODULES = trilobeat.so
trilobeat_so_SOURCES = trilobeat.cpp
trilobeat_so_CFLAGS = $(ADVANCEDCFLAGS) -Ilibraries/components
trilobeat_so_LDFLAGS = $(ADVANCEDARCHIVES)
trilobeat_lv2_DATA = manifest.ttl trilobeat.ttl patternbg.png
trilobeat_lv2_PEGFILES = trilobeat.peg
trilobeat_lv2_SOURCEDIR = plugins/trilobeat
#trilobeat_lv2_MODULES = trilobeat_gtk.so
#trilobeat_gtk_so_SOURCES = trilobeat_gtk.cpp
#trilobeat_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4` -Iextensions/gui -Ilibraries/lv2gtk2gui -Ilibraries/widgets -Ilibraries/components -I.
#trilobeat_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4`
#trilobeat_gtk_so_ARCHIVES = libraries/lv2gtk2gui/liblv2_gtk2gui.a libraries/widgets/libpatternwidget.a
#trilobeat_gtk_so_SOURCEDIR = plugins/trilobeat

# Euphoria
euphoria_lv2_MODULES = euphoria.so
euphoria_lv2_DATA = manifest.ttl euphoria.ttl presets.ttl
euphoria_lv2_PEGFILES = euphoria.peg
euphoria_lv2_SOURCEDIR = plugins/euphoria
euphoria_so_SOURCES = \
	euphoria.cpp \
	shaper.hpp shaper.cpp \
	pdoscillator.hpp \
	wsvoice.cpp wsvoice.hpp \
	pdvoice.hpp pdvoice.cpp \
	chorus.hpp chorus.cpp \
	echo.hpp echo.cpp \
	reverb.hpp
euphoria_so_CFLAGS = $(ADVANCEDCFLAGS) -Ilibraries/components `pkg-config --cflags gsl`
euphoria_so_LDFLAGS = `pkg-config --libs gsl lv2-plugin`

# Euphoria GUI
euphoria_gtk_lv2_MANIFEST = gui_manifest.ttl
euphoria_gtk_lv2_MODULES = euphoria_gtk.so
euphoria_gtk_lv2_SOURCEDIR = plugins/euphoria
euphoria_gtk_so_SOURCES = \
	euphoria_gtk.cpp \
	euphoriawidget.cpp euphoriawidget.hpp
euphoria_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0 lv2-gui` -Ilibraries/widgets
euphoria_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 cairomm-1.0 lv2-gui` 
euphoria_gtk_so_ARCHIVES = \
	libraries/widgets/libvgknob.a \
	libraries/widgets/libenvelopeeditor.a \
	libraries/widgets/libshapereditor.a \
	libraries/widgets/libspringeditor.a \
	libraries/widgets/libpdeditor.a \
	libraries/widgets/libtransitioneditor.a \

# Horizon
horizon_lv2_MODULES = horizon.so
horizon_lv2_DATA = manifest.ttl horizon.ttl
horizon_lv2_PEGFILES = horizon.peg
horizon_lv2_SOURCEDIR = plugins/horizon
horizon_so_SOURCES = \
	action.hpp action.cpp \
	actiontrigger.hpp actiontrigger.cpp \
	chunk.hpp chunk.cpp \
	effect.hpp effect.cpp \
	effectstack.hpp effectstack.cpp \
	horizon.cpp \
	mixer.hpp mixer.cpp \
	normaliseeffect.hpp normaliseeffect.cpp \
	reverseeffect.hpp reverseeffect.cpp \
	sample.hpp sample.cpp \
	samplebuffer.hpp samplebuffer.cpp \
	segmentation.hpp segmentation.cpp \
	voice.hpp voice.cpp
horizon_so_CFLAGS = `pkg-config --cflags sndfile lv2-plugin` -Ilibraries/components
horizon_so_LDFLAGS = `pkg-config --libs sndfile lv2-plugin`

# Horizon GUI
horizon_gtk_lv2_MODULES = horizon_gtk.so
horizon_gtk_lv2_MANIFEST = gui_manifest.ttl
horizon_gtk_lv2_SOURCEDIR = plugins/horizon
horizon_gtk_so_SOURCES = \
	actiontriggermodel.cpp actiontriggermodel.hpp \
	chunkeditor.cpp chunkeditor.hpp \
	controlsourcegui.cpp controlsourcegui.hpp \
	effectmodel.cpp effectmodel.hpp \
	effectstackmodel.cpp effectstackmodel.hpp \
	effectstackview.cpp effectstackview.hpp \
	horizonkeyboard.cpp horizonkeyboard.hpp \
	labelslider.cpp labelslider.hpp \
	lfo_gui.cpp lfo_gui.hpp \
	mod_gui.cpp mod_gui.hpp \
	optionslider.cpp optionslider.hpp \
	sampleeditor.cpp sampleeditor.hpp \
	samplemodel.cpp samplemodel.hpp \
	sampleview.cpp sampleview.hpp \
	triggereditor.cpp triggereditor.hpp \
	horizon_gtk.cpp
horizon_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0 lv2-gui` -Ilibraries/widgets
horizon_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 cairomm-1.0 lv2-gui` 
horizon_gtk_so_ARCHIVES = libraries/widgets/libkeyboard.a

# Sineshaper
sineshaper_lv2_MODULES = sineshaper.so
sineshaper_lv2_DATA = manifest.ttl sineshaper.ttl presets.ttl icon.svg
sineshaper_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_lv2_PEGFILES = sineshaper.peg
sineshaper_so_SOURCES = \
	sineshaper.hpp sineshaper.cpp \
	sineshaperports.hpp \
	midiiterator.hpp
sineshaper_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
sineshaper_so_LDFLAGS = $(PLUGINARCHIVES)

# Sineshaper GUI
sineshaper_gtk_lv2_MODULES = sineshaper_gtk.so
sineshaper_gtk_lv2_MANIFEST = gui_manifest.ttl
sineshaper_gtk_lv2_DATA = sineshaper.glade dial.png sineshaper.png icon.svg
sineshaper_gtk_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_gtk_so_SOURCES = \
	sineshaper_gtk.cpp \
	sineshaperwidget.cpp sineshaperwidget.hpp \
	skindial_gtkmm.cpp skindial_gtkmm.hpp
sineshaper_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
sineshaperwidget_cpp_CFLAGS = -DVERSION=\"$(PACKAGE_VERSION)\"
sineshaper_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
sineshaper_gtk_so_SOURCEDIR = plugins/sineshaper

# Nekobee Blue GUI
nekobee_blue_gui_lv2_MODULES = nekobee_blue_gui.so
nekobee_blue_gui_lv2_DATA = manifest.ttl drawing.png
nekobee_blue_gui_lv2_SOURCEDIR = plugins/nekobee_blue_gui
nekobee_blue_gui_so_SOURCES = \
	nekobee_blue_gui.cpp \
	nekobeewidget.cpp nekobeewidget.hpp
nekobee_blue_gui_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui`
nekobee_blue_gui_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 

# MIDIIdentity
midi_identity_lv2_MODULES = midi_identity.so
midi_identity_so_SOURCES = midi_identity.cpp
midi_identity_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
midi_identity_so_LDFLAGS = $(PLUGINARCHIVES)
midi_identity_lv2_DATA = manifest.ttl midi_identity.ttl
midi_identity_lv2_SOURCEDIR = plugins/midi_identity

# Householder FDN
#hhfdn_lv2_SOURCES = hhfdn.cpp
#hhfdn_lv2_DATA = manifest.ttl hhfdn.ttl
#hhfdn_lv2_CFLAGS = -Ilibraries/lv2plugin -Iextensions/MidiPort -Ilibraries/components -I.
#hhfdn_lv2_LDFLAGS = $(PLUGINARCHIVES)
#hhfdn_lv2_SOURCEDIR = plugins/householderfdn

# AudioIdentity
audio_identity_lv2_MODULES = audio_identity.so
audio_identity_so_SOURCES = audio_identity.cpp
audio_identity_so_CFLAGS = $(PLUGINCFLAGS)
audio_identity_so_LDFLAGS = $(PLUGINARCHIVES)
audio_identity_lv2_DATA = manifest.ttl audio_identity.ttl
audio_identity_lv2_SOURCEDIR = plugins/audio_identity

# GCF
gcf_lv2_MODULES = gcf.so gcf_gtk.so
gcf_lv2_DATA = manifest.ttl gcf.ttl
gcf_lv2_SOURCEDIR = plugins/gcf
gcf_so_SOURCES = gcf.cpp
gcf_so_CFLAGS = $(PLUGINCFLAGS)
gcf_so_LDFLAGS = $(PLUGINARCHIVES)
gcf_gtk_so_SOURCES = gcf_gtk.cpp tunerwidget.cpp tunerwidget.hpp
gcf_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
gcf_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 

# Arpeggiator
arpeggiator_lv2_MODULES = arpeggiator.so
arpeggiator_so_SOURCES = arpeggiator.cpp
arpeggiator_so_CFLAGS = $(PLUGINCFLAGS)
arpeggiator_so_LDFLAGS = $(PLUGINARCHIVES)
arpeggiator_lv2_DATA = manifest.ttl arpeggiator.ttl
arpeggiator_lv2_SOURCEDIR = plugins/arpeggiator

# Math constants
math-constants_lv2_MODULES = math-constants.so
math-constants_so_SOURCES = math-constants.cpp
math-constants_so_CFLAGS = $(PLUGINCFLAGS)
math-constants_so_LDFLAGS = $(PLUGINARCHIVES)
math-constants_lv2_DATA = manifest.ttl math-constants.ttl
math-constants_lv2_SOURCEDIR = plugins/math-constants

# Math functions
math-functions_lv2_MODULES = math-functions.so
math-functions_so_SOURCES = math-functions.cpp
math-functions_so_CFLAGS = $(PLUGINCFLAGS)
math-functions_so_LDFLAGS = $(PLUGINARCHIVES)
math-functions_lv2_DATA = manifest.ttl math-functions.ttl
math-functions_lv2_SOURCEDIR = plugins/math-functions


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


# The shared headers need to go in the distribution too
EXTRA_DIST = COPYING \
	\
	lv2.h \
	\
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
	libraries/components/lv2-midifunctions.h \
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
	libraries/components/wavewrapper.hpp \
	\
	extensions/midimap/lv2-midimap.rdfs \
	extensions/transporttype/lv2-transport.h


# Do the magic
include Makefile.template
