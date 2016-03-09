#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_PlayDict
SHLIB = libLArOpenCV_Play.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell python-config --includes)
INCFLAGS += -I$(shell python -c "import numpy; print numpy.get_include()")
INCFLAGS += -I/usr/local/include/
INCFLAGS += -I$(LARLITE_USERDEVDIR)/OpenCV

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -L$(shell python-config --prefix)/lib/
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += $(shell python-config --ldflags)
LDFLAGS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_features2d -lopencv_imgproc
LDFLAGS += -L$(LARLITE_LIBDIR) -lOpenCV_Utils

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

