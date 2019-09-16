#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_ImageClusterAlgoClass
SHLIB = libLArOpenCV_ImageClusterAlgoClass.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell basictool-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell ${LAROPENCV_PYTHON}-config --includes)
INCFLAGS += -I$(shell ${LAROPENCV_PYTHON} -c "from __future__ import print_function; import numpy; print(numpy.get_include())")
INCFLAGS += -I$(LARLITE_USERDEVDIR)
INCFLAGS += -I$(LARLITE_COREDIR)
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
LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core -lLArOpenCV_ImageClusterAlgoFunction -lLArOpenCV_ImageClusterAlgoData -lLArOpenCV_ImageClusterBase
LDFLAGS += -L$(GEO2D_LIBDIR) -lGeo2D_Core -lGeo2D_PyUtil
#LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

