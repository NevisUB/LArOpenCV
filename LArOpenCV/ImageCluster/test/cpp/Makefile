CC=g++
INCFLAGS =-I.
INCFLAGS+=-I$(LAROPENCV_BASEDIR)
INCFLAGS+=-I$(OPENCV_INCDIR)
INCFLAGS+=-I$(LARLITE_USERDEVDIR)
INCFLAGS+=-I$(GEO2D_BASEDIR)
INCFLAGS+=$(shell larlite-config --includes)
INCFLAGS+=$(shell root-config --cflags)


LDFLAGS =-L.
LDFLAGS+=-L$(OPENCV_LIBDIR)
LDFLAGS+=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
LDFLAGS+=-L$(GEO2D_LIBDIR)
LDFLAGS+=-lGeo2D_Core
LDFLAGS+=$(shell larlite-config --libs)
LDFLAGS+=-lBasicTool_FhiclLite
LDFLAGS+=-lLArOpenCV_ImageClusterBase
LDFLAGS+=-lLArOpenCV_ImageClusterAlgoModule
LDFLAGS+=-lLArOpenCV_ImageClusterAlgoData
LDFLAGS+=-lLArOpenCV_ImageClusterAlgoClass
LDFLAGS+=$(shell root-config --libs)

SOURCES=test.cxx
OBJECTS=$(SOURCES:.cxx=.o)
EXECUTABLE=test

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

clean:
	rm *.o test
