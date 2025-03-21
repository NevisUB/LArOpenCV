#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_CoreDict
SHLIB = libLArOpenCV_Core.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell ${LAROPENCV_PYTHON}-config --includes)
INCFLAGS += -I$(shell ${LAROPENCV_PYTHON} -c "from __future__ import print_function; import numpy; print(numpy.get_include())")
INCFLAGS += -I$(LAROPENCV_BASEDIR)/LArOpenCV
INCFLAGS += -I$(OPENCV_INCDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(shell ${LAROPENCV_PYTHON}-config --prefix)/lib/
LDFLAGS += $(shell ${LAROPENCV_PYTHON}-config --ldflags)
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE
