
ifndef LAROPENCV_IMAGECLUSTERDIR
ERROR_MESSAGE := $(error LAROPENCV_IMAGECLUSTERDIR is not set... run configure.sh!)
endif

#####################################################################################
#
# Define directories to be compile upon a global "make"...
#
SUBDIRS := Base AlgoData Util Cluster Direction Filter StartPoint Merge Match ReCluster Debug #Vertex

#####################################################################################
#
# COMPILATION...
#
#.phony: all configure default-config clean
.phony: all clean

all:
	@for i in $(SUBDIRS); do ( echo "" && echo "Compiling $$i..." && cd $$i && $(MAKE) ) || exit $$?; done
#####################################################################################
#
# CLEANs...
#
clean:
	@for i in $(SUBDIRS); do ( echo "" && echo "Cleaning $$i..." && cd $$i && $(MAKE) clean ) || exit $$?; done
