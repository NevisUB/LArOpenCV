# specific names for this package
DICT  = LArOpenCV_ImageClusterBase
SHLIB = libLArOpenCV_ImageClusterBase.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell basictool-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += -I$(LARLITE_USERDEVDIR)
INCFLAGS += -I$(LARLITE_COREDIR)
INCFLAGS += $(shell ${LAROPENCV_PYTHON}-config --includes)
INCFLAGS += -I$(shell ${LAROPENCV_PYTHON} -c "from __future__ import print_function; import numpy; print(numpy.get_include())")
INCFLAGS += -I$(OPENCV_INCDIR)
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(GEO2D_BASEDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -L$(shell ${LAROPENCV_PYTHON}-config --prefix)/lib/
LDFLAGS += $(shell ${LAROPENCV_PYTHON}-config --ldflags)
LDFLAGS += $(shell basictool-config --libs)
LDFLAGS += $(shell recotool-config --libs)
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core 
LDFLAGS += -L$(GEO2D_LIBDIR) -lGeo2D_Core

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

