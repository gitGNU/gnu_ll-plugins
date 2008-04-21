# This package requires that the lv2-c++-tools package is already installed.
# It can be downloaded using CVS from the module lv2-c++-tools in the same
# repository as the one you downloaded ll-plugins from.


PACKAGE_NAME = ll-plugins
PACKAGE_VERSION = 0.1.426
PKG_DEPS = \
	cairomm-1.0>=1.2.4 \
	gsl>=1.8 \
	gtkmm-2.4>=2.8.8 \
	jack>=0.109.0 \
	lash-1.0>=0.5.1 \
	lv2-plugin>=1.0.0 \
	lv2-gui>=1.0.0 \
	paq>=1.0.0 \
	sndfile>=1.0.16 \
	samplerate>=0.1.2


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

PROGRAMS = elven rxc

LV2_BUNDLES = \
	arpeggiator.lv2 \
	beep.lv2 \
	control2midi.lv2 \
	gcf.lv2 \
	klaviatur.lv2 \
	midiproc.lv2 \
	nekobee_blue_gui.lv2 \
	phase-distortion-osc.lv2 \
	vumeter.lv2 \
	vumeter_gtk.lv2 \
	euphoria.lv2 \
	euphoria_gtk.lv2 \
	tableosc.lv2
#	envelope.lv2 \
#	envelope_gtk.lv2 \
#	horizon.lv2 \
#	horizon_gtk.lv2 \
#	trilobeat.lv2 \
#	trilobeat_gtk.lv2 
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

# XXX rxc
rxc_SOURCES = \
	regexcompiler.hpp regexcompiler.cpp \
	main.cpp
rxc_SOURCEDIR = plugins/midiproc


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

# MIDI processor
midiproc_lv2_MODULES = midiproc.so #midiproc_gtk.so
midiproc_lv2_DATA = manifest.ttl midiproc.ttl
midiproc_lv2_PEGFILES = midiproc.peg
midiproc_lv2_SOURCEDIR = plugins/midiproc
midiproc_so_SOURCES = midiproc.cpp
midiproc_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
midiproc_so_LDFLAGS = $(PLUGINARCHIVES)
midiproc_gtk_so_SOURCES = midiproc_gtk.cpp
midiproc_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui` -Ilibraries/widgets
midiproc_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui` 
midiproc_gtk_so_ARCHIVES = libraries/widgets/libkeyboard.a
midiproc_lv2_POSTINSTALL = $(RESIDENTGUI) >> midiproc.lv2/midiproc.ttl

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
trilobyte_lv2_DATA = manifest.ttl trilobyte.ttl
trilobyte_lv2_PEGFILES = trilobyte.peg
trilobyte_lv2_SOURCEDIR = plugins/trilobyte
trilobyte_lv2_POSTINSTALL = $(RESIDENTGUI) >> trilobyte.lv2/trilobyte.ttl

# Trilobeat
trilobeat_lv2_MODULES = trilobeat.so
trilobeat_so_SOURCES = trilobeat.cpp
trilobeat_so_CFLAGS = $(ADVANCEDCFLAGS) -Ilibraries/components
trilobeat_so_LDFLAGS = $(ADVANCEDARCHIVES)
trilobeat_lv2_DATA = manifest.ttl trilobeat.ttl
trilobeat_lv2_PEGFILES = trilobeat.peg
trilobeat_lv2_SOURCEDIR = plugins/trilobeat

# Trilobeat GUI
trilobeat_gtk_lv2_MODULES = trilobeat_gtk.so
trilobeat_gtk_lv2_MANIFEST = gui_manifest.ttl
trilobeat_gtk_so_SOURCES = \
	trilobeat_gtk.cpp \
	gridwidget.cpp gridwidget.hpp \
	keynamewidget.cpp keynamewidget.hpp
trilobeat_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui`
trilobeat_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
trilobeat_gtk_lv2_DATA = gridbg.png keynamebg.png
trilobeat_gtk_lv2_SOURCEDIR = plugins/trilobeat
trilobeat_gtk_lv2_POSTINSTALL = $(RESIDENTGUI) >> trilobeat_gtk.lv2/manifest.ttl

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
#euphoria_gtk_lv2_POSTINSTALL = $(RESIDENTGUI) >> euphoria_gtk.lv2/manifest.ttl

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
horizon_gtk_lv2_POSTINSTALL = $(RESIDENTGUI) >> horizon_gtk.lv2/manifest.ttl

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
EXTRA_DIST = COPYING \
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
