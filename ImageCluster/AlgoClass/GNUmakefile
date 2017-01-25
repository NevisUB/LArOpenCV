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
INCFLAGS+= $(shell basictool-config --includes)
INCFLAGS += -I$(OPENCV_INCDIR)
INCFLAGS += -I$(LAROPENCV_BASEDIR)
INCFLAGS += -I$(LAROPENCV_IMAGECLUSTERDIR)
INCFLAGS += -I$(GEO2D_BASEDIR)


# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += $(shell basictool-config --libs)
LDFLAGS += -L$(LARLITE_LIBDIR) -lLArOpenCV_Core -lLArOpenCV_ImageClusterAlgoData -lLArOpenCV_Utils -lLArOpenCV_ImageClusterBase -lRecoTool_ClusterRecoUtil -lRecoTool_ClusterRecoUtilAlg
LDFLAGS += -L$(GEO2D_LIBDIR) -lGeo2D_Core
LDFLAGS += -L$(OPENCV_LIBDIR) -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

