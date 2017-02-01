#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = LArOpenCV_AppDict
SHLIB = libLArOpenCV_App.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell basictool-config --includes)
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += -I$(LARLITE_USERDEVDIR)
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(OPENCV_INCDIR)
INCFLAGS += -I$(GEO2D_BASEDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -L$(shell python-config --prefix)/lib/
LDFLAGS += $(shell basictool-config --libs)
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += $(shell python-config --ldflags)
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
LDFLAGS += -L$(LARLITE_LIBDIR)

LDFLAGS += -lLArOpenCV_ImageClusterBase
LDFLAGS += -lLArOpenCV_ImageClusterCluster
# LDFLAGS += -lLArOpenCV_ImageClusterDirection
LDFLAGS += -lLArOpenCV_ImageClusterUtil
# LDFLAGS += -lLArOpenCV_ImageClusterMatch
# LDFLAGS += -lLArOpenCV_ImageClusterMerge
# LDFLAGS += -lLArOpenCV_ImageClusterFilter
#LDFLAGS += -lLArOpenCV_ImageClusterReCluster
# LDFLAGS += -lLArOpenCV_ImageClusterStartPoint
# LDFLAGS += -lLArOpenCV_ImageClusterDebug

LDFLAGS += -lLArOpenCV_Utils -lLArOpenCV_Core -lRecoTool_ClusterRecoUtil
LDFLAGS += -L$(GEO2D_LIBDIR) -lGeo2D_Core

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

