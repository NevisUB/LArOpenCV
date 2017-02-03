#dirtyness by vic
CXX=g++
CXXFLAGS += -O3 #-g

#############INCLUDES
##ROOT
BASIC_ROOTINC=$(shell root-config --cflags)
CXXINCS +=$(BASIC_ROOTINC)
##larlite
LARLITE_INCDIR=$(shell larlite-config --includes)
CXXINCS += $(LARLITE_INCDIR)
##geo2d
CXXINCS += -I$(GEO2D_BASEDIR)
##laropencv
CXXINCS += -I$(LAROPENCV_BASEDIR)
##opencv
CXXINCS +=-I$(OPENCV_INCDIR)

#############LIBRARIES
##ROOT
BASIC_ROOTLIBS=$(shell root-config --libs)
CXXLIBS += $(BASIC_ROOTLIBS)
##larlite
##laropencv
CXXLIBS += -L$(LARLITE_LIBDIR)
CXXLIBS += -lLArOpenCV_ImageClusterAlgoFunction
##geo2d
CXXLIBS += -L$(GEO2D_LIBDIR)
CXXLIBS += -lGeo2D_Core
##opencv
CXXLIBS +=-L$(OPENCV_LIBDIR)
OPENCV_LIBS=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
CXXLIBS += $(OPENCV_LIBS)

#############OUTPUT
SWFILE=QPoint.cxx
OWFILE=q_point.o
BWFILE=q_point

all:
	$(CXX) $(CXXFLAGS) $(CXXINCS) -c $(SWFILE) -o $(OWFILE)
	$(CXX) $(CXXLIBS) -o $(BWFILE) $(OWFILE)
clean:
	rm -rf $(OWFILE) $(BWFILE)
	rm -rf *.png
