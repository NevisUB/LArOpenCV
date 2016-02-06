#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = Kazu_LArCV_YoloAPIDict
SHLIB = libKazu_LArCV_YoloAPI.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I/Applications/BasicSoftwares/YOLO/darknet/src
INCFLAGS += -I/usr/local/include
INCFLAGS += -I/usr/local/cuda/include
# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += -lm -pthread -lstdc++ -L /usr/local/lib -lopencv_calib3d -lopencv_core -lopencv_cudaarithm -lopencv_cudabgsegm -lopencv_cudafeatures2d -lopencv_cudafilters -lopencv_cudaimgproc -lopencv_cudalegacy -lopencv_cudaobjdetect -lopencv_cudaoptflow -lopencv_cudastereo -lopencv_cudawarping -lopencv_cudev -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -lopencv_viz -L/usr/local/cuda/lib -lcuda -lcudart -lcublas -lcurand -L/Applications/BasicSoftwares/YOLO/darknet/lib -lyolo 

# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE
