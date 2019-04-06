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

# Python version
if [[ -z $LAROPENCV_PYTHON_VERSION ]]; then
    export LAROPENCV_PYTHON_VERSION=`python -V 2>&1 | awk '{ print $2 }' | sed 's/\./\ /g' | awk '{ print $1 }'`
    echo "LAROPENCV_PYTHON_VERSION =  ${LAROPENCV_PYTHON_VERSION}"
    export LAROPENCV_PYTHON="python${LAROPENCV_PYTHON_VERSION}"
else
    echo "LAROPENCV_PYTHON_VERSION =  ${LAROPENCV_PYTHON_VERSION} (ALREADY SPECIFIED)"
fi
echo "LAROPENCV_PYTHON = ${LAROPENCV_PYTHON}"

