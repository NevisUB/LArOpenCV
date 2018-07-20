#!/usr/bin/bash

# This section of code determines where the evd is stored.

if [ -z ${LARLITE_BASEDIR+x} ]; then 
  echo "Must set up larlite to use this!";
  return 
fi
if [[ -z $OPENCV_INCDIR ]]; then
    export OPENCV_INCDIR=/usr/local/include
fi
if [[ -z $OPENCV_LIBDIR ]]; then
    export OPENCV_LIBDIR=/usr/local/lib
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENCV_LIBDIR
fi

me="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo $me

export LAROPENCV_BASEDIR=$me
export LAROPENCV_IMAGECLUSTERDIR=$me/ImageCluster
export PYTHONPATH=$PYTHONPATH:$LAROPENCV_BASEDIR/python
export PATH=${LAROPENCV_BASEDIR}/bin:${PATH}
