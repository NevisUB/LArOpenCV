#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_ImageClusterAlgoData
SHLIB = libLArOpenCV_ImageClusterAlgoData.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(GEO2D_BASEDIR)
INCFLAGS += -I$(OPENCV_INCDIR)
# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core -lLArOpenCV_ImageClusterBase -lLArOpenCV_ImageClusterAlgoFunction
LDFLAGS += -L$(GEO2D_LIBDIR) -lGeo2D_Core
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_imgproc

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

