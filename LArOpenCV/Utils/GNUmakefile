#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_UtilsDict
SHLIB = libLArOpenCV_Utils.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h NDArrayConverter.h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell python-config --includes)
INCFLAGS += -I$(shell python -c "import numpy; print numpy.get_include()")
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(OPENCV_INCDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(shell python-config --prefix)/lib/
LDFLAGS += $(shell python-config --ldflags)
LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_highgui -lopencv_imgcodecs

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE
