#!/usr/bin/bash

# This section of code determines where the evd is stored.

if [ -z ${LARLITE_BASEDIR+x} ]; then 
  echo "Must set up larlite to use this!";
  return 
fi


export OPENCV_INCDIR=/uboone/app/users/vgenty/opencv/include
export OPENCV_LIBDIR=/uboone/app/users/vgenty/opencv/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENCV_LIBDIR
