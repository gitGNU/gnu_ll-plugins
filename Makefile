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
	libpdeditor.a \
	libshapereditor.a \
	libspringeditor.a \
	libtransitioneditor.a \
	libvgknob.a \
	libvuwidget.a

PROGRAMS = elven

LV2_BUNDLES = \
	arpeggiator.lv2 \
	beep.lv2 \
	control2midi.lv2 \
	euphoria.lv2 \
	euphoria_gtk.lv2 \
	klaviatur.lv2 \
	klaviatur_gtk.lv2 \
	peakmeter.lv2 \
	peakmeter_gtk.lv2 \
	tableosc.lv2 \
	math-constants.lv2 \
	math-functions.lv2 \
	sineshaper.lv2 \
	sineshaper_gtk.lv2 \
	rudolf556.lv2 \
	rudolf556_gtk.lv2


# Archives with useful code bits

libkeyboard_a_SOURCES = keyboard.hpp keyboard.cpp
libkeyboard_a_CFLAGS = `pkg-config --cflags gtkmm-2.4`
libkeyboard_a_SOURCEDIR = libraries/widgets

libenvelopeeditor_a_SOURCES = envelopeeditor.hpp envelopeeditor.cpp
libenvelopeeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libenvelopeeditor_a_SOURCEDIR = libraries/widgets

libpdeditor_a_SOURCES = pdeditor.hpp pdeditor.cpp
libpdeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libpdeditor_a_SOURCEDIR = libraries/widgets

libshapereditor_a_SOURCES = shapereditor.hpp shapereditor.cpp
libshapereditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libshapereditor_a_SOURCEDIR = libraries/widgets

libspringeditor_a_SOURCES = springeditor.hpp springeditor.cpp
libspringeditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libspringeditor_a_SOURCEDIR = libraries/widgets

libtransitioneditor_a_SOURCES = transitioneditor.hpp transitioneditor.cpp
libtransitioneditor_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libtransitioneditor_a_SOURCEDIR = libraries/widgets

libvgknob_a_SOURCES = vgknob.hpp vgknob.cpp
libvgknob_a_CFLAGS = `pkg-config --cflags gtkmm-2.4 cairomm-1.0`
libvgknob_a_SOURCEDIR = libraries/widgets

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
control2midi_lv2_PEGFILES = control2midi.peg
control2midi_lv2_SOURCEDIR = plugins/control2midi
control2midi_so_SOURCES = control2midi.cpp
control2midi_so_CFLAGS = $(PLUGINCFLAGS)
control2midi_so_LDFLAGS = $(PLUGINARCHIVES)

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

# Klaviatur
klaviatur_lv2_MODULES = klaviatur.so
klaviatur_lv2_DATA = manifest.ttl klaviatur.ttl
klaviatur_lv2_PEGFILES = klaviatur.peg
klaviatur_lv2_SOURCEDIR = plugins/klaviatur
klaviatur_so_SOURCES = klaviatur.cpp
klaviatur_so_CFLAGS = $(PLUGINCFLAGS) -Ilibraries/components
klaviatur_so_LDFLAGS = $(PLUGINARCHIVES)

# Klaviatur GUI
klaviatur_gtk_lv2_MODULES = klaviatur_gtk.so
klaviatur_gtk_lv2_MANIFEST = gui_manifest.ttl
klaviatur_gtk_lv2_SOURCEDIR = plugins/klaviatur
klaviatur_gtk_so_SOURCES = klaviatur_gtk.cpp
klaviatur_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui` -Ilibraries/widgets
klaviatur_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui` 
klaviatur_gtk_so_ARCHIVES = libraries/widgets/libkeyboard.a
klaviatur_gtk_lv2_POSTINSTALL = $(RESIDENTGUI) >> klaviatur.lv2/klaviatur.ttl

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

# Peak meter
peakmeter_lv2_MODULES = peakmeter.so
peakmeter_so_SOURCES = peakmeter.cpp
peakmeter_so_CFLAGS = $(PLUGINCFLAGS)
peakmeter_so_LDFLAGS = $(PLUGINARCHIVES)
peakmeter_lv2_DATA = manifest.ttl peakmeter.ttl icon.svg
peakmeter_lv2_SOURCEDIR = plugins/peakmeter

# Peak meter GUI
peakmeter_gtk_lv2_MODULES = peakmeter_gtk.so
peakmeter_gtk_lv2_SOURCEDIR = plugins/peakmeter
peakmeter_gtk_lv2_MANIFEST = gui_manifest.ttl
peakmeter_gtk_so_SOURCES = peakmeter_gtk.cpp
peakmeter_gtk_so_CFLAGS = `pkg-config --cflags gtkmm-2.4 lv2-gui` -Ilibraries/widgets
peakmeter_gtk_so_LDFLAGS = `pkg-config --libs gtkmm-2.4 lv2-gui` 
peakmeter_gtk_so_ARCHIVES = libraries/widgets/libvuwidget.a

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

# Math constants
math-constants_lv2_MODULES = math-constants.so
math-constants_so_SOURCES = math-constants.cpp
math-constants_so_CFLAGS = `pkg-config --cflags lv2-plugin`
math-constants_so_LDFLAGS = `pkg-config --libs lv2-plugin`
math-constants_lv2_DATA = \
	manifest.ttl math-constants.ttl \
	e.svg pi.svg pi_2.svg pi_4.svg sqrt2.svg \
	1_pi.svg 1_sqrt2.svg 2_pi.svg 2_sqrtpi.svg
math-constants_lv2_SOURCEDIR = plugins/math-constants

# Math functions
math-functions_lv2_MODULES = math-functions.so
math-functions_so_SOURCES = math-functions.cpp
math-functions_so_CFLAGS = `pkg-config --cflags lv2-plugin`
math-functions_so_LDFLAGS = `pkg-config --libs lv2-plugin`
math-functions_lv2_DATA = manifest.ttl math-functions.ttl
math-functions_lv2_SOURCEDIR = plugins/math-functions

# Sineshaper
sineshaper_lv2_MODULES = sineshaper.so
sineshaper_lv2_DATA = manifest.ttl sineshaper.ttl presets.ttl icon.svg
sineshaper_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_lv2_PEGFILES = sineshaper.peg
sineshaper_so_SOURCES = \
	adsr.hpp \
	dcblocker.hpp \
	delay.hpp \
	frequencytable.hpp \
	midiiterator.hpp \
	sineoscillator.hpp \
	sineshaper.hpp sineshaper.cpp \
	sineshaperports.hpp \
	slide.hpp \
	wavewrapper.hpp
sineshaper_so_CFLAGS = `pkg-config --cflags lv2-plugin` -Ilibraries/components
sineshaper_so_LDFLAGS = `pkg-config --libs lv2-plugin`

# Sineshaper GUI
sineshaper_gtk_lv2_MODULES = sineshaper_gtk.so
sineshaper_gtk_lv2_MANIFEST = gui_manifest.ttl
sineshaper_gtk_lv2_DATA = dial.png sineshaper.png icon.svg
sineshaper_gtk_lv2_SOURCEDIR = plugins/sineshaper
sineshaper_gtk_so_SOURCES = \
	sineshaper_gtk.cpp \
	sineshaperwidget.cpp sineshaperwidget.hpp \
	skindial_gtkmm.cpp skindial_gtkmm.hpp
sineshaper_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui`
sineshaperwidget_cpp_CFLAGS = -DVERSION=\"$(PACKAGE_VERSION)\"
sineshaper_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui` 
sineshaper_gtk_so_SOURCEDIR = plugins/sineshaper

# Rudolf 556
rudolf556_lv2_MODULES = rudolf556.so
rudolf556_lv2_DATA = \
	manifest.ttl rudolf556.ttl \
	icon.svg \
	bass_h00.flac bass_h05.flac bass_h10.flac \
	snare_bonk.flac snare_noise.flac \
	hihat_base.flac hihat_hit.flac
rudolf556_lv2_SOURCEDIR = plugins/rudolf556
rudolf556_lv2_PEGFILES = rudolf556.peg
rudolf556_so_SOURCES = rudolf556.cpp
rudolf556_so_CFLAGS = `pkg-config --cflags lv2-plugin sndfile samplerate`
rudolf556_so_LDFLAGS = `pkg-config --libs lv2-plugin sndfile samplerate`

# Rudolf 556 GUI
rudolf556_gtk_lv2_MODULES = rudolf556_gtk.so
rudolf556_gtk_lv2_MANIFEST = gui_manifest.ttl
rudolf556_gtk_lv2_DATA = rudolf556.png
rudolf556_gtk_lv2_SOURCEDIR = plugins/rudolf556
rudolf556_gtk_so_SOURCES = \
	rudolf556_gtk.cpp \
	rudolf556widget.cpp rudolf556widget.hpp
rudolf556_gtk_so_CFLAGS = `pkg-config --cflags lv2-gui`
rudolf556_gtk_so_LDFLAGS = `pkg-config --libs lv2-gui` 



# The shared headers need to go in the distribution too
EXTRA_DIST = \
	libraries/components/adsr.hpp \
	libraries/components/chebyshevshaper.hpp \
	libraries/components/dcblocker.hpp \
	libraries/components/delay.hpp \
	libraries/components/delayline.hpp \
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
