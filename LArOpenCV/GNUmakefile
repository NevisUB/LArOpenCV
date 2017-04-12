#####################################################################################
#
# A top Makefile for building my project.
# One needs to define $LARLITE_USERDEVDIR environment variable and set it to where this
# makefile exists. 
# One can type "make" and this builds packages that are added in $SUBDIR defined below.
# 
# The original is taken from Glenn A. Smith's example for Double Chooz experiment.
#
#####################################################################################
#
# IMPOSE CONDITION BETWEEN LARLITE_USERDEVDIR & PWD =>
#   do not compile if PWD !=$LARLITE_USERDEVDIR is set elsewhere
#
ifndef LAROPENCV_BASEDIR
ERROR_MESSAGE := $(error LAROPENCV_BASEDIR is not defined!)
endif
#
#####################################################################################
#
# Define directories to be compile upon a global "make"...
#
SUBDIRS := Core ImageCluster App #ADD_NEW_SUBDIR ... do not remove this comment from this line

#####################################################################################
#
# COMPILATION...
#
#.phony: all configure default-config clean
.phony: all clean

all:
	@for i in $(SUBDIRS); do ( echo "" && echo "Compiling $$i..." && cd $(LAROPENCV_BASEDIR)/LArOpenCV/$$i && $(MAKE) ) || exit $$?; done
#####################################################################################
#
# CLEANs...
#
clean:
	@for i in $(SUBDIRS); do ( echo "" && echo "Cleaning $$i..." && cd $(LAROPENCV_BASEDIR)/LArOpenCV/$$i && $(MAKE) clean && rm -f $(LARLITE_LIBDIR)/$$i.* ) || exit $$?; done

#####################################################################################
#EOF
