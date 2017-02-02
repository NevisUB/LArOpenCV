#dirtyness by vic
CXX=g++
CXXFLAGS += -std=c++11 -O3 #-g

#############INCLUDES
##opencv
CXXINCS +=-I$(OPENCV_INCDIR)

#############LIBRARIES
##opencv
CXXLIBS +=-L$(OPENCV_LIBDIR)
OPENCV_LIBS=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
CXXLIBS += $(OPENCV_LIBS)

#############OUTPUT
SWFILE=ConstMat.cxx
OWFILE=const_mat.o
BWFILE=const_mat

all:
	$(CXX) $(CXXFLAGS) $(CXXINCS) -c $(SWFILE) -o $(OWFILE)
	$(CXX) $(CXXLIBS) -o $(BWFILE) $(OWFILE)
clean:
	rm -rf $(OWFILE) $(BWFILE)
	rm -rf *.png
