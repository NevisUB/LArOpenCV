#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_ImageClusterMatch
SHLIB = libLArOpenCV_ImageClusterMatch.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell basictool-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell python-config --includes)
INCFLAGS += -I$(shell python -c "import numpy; print numpy.get_include()")
INCFLAGS += -I$(OPENCV_INCDIR)
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(LAROPENCV_IMAGECLUSTERDIR)/Base
INCFLAGS += -I$(LAROPENCV_IMAGECLUSTERDIR)/Util

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -L$(shell python-config --prefix)/lib/
LDFLAGS += $(shell python-config --ldflags)
LDFLAGS += $(shell basictool-config --libs)
LDFLAGS += $(shell recotool-config --libs)
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core -lLArOpenCV_Utils -lLArOpenCV_ImageClusterBase -lLArOpenCV_ImageClusterUtil -lRecoTool_ClusterRecoUtil -lRecoTool_ClusterRecoUtilAlg

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

