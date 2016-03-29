import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc
import matplotlib.cm as cm

import cv2

import sys

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv

my_proc = fmwk.ana_processor()

# Config file
cfg="pcacluster.fcl"

for i in xrange(1,10):
    my_proc.add_input_file( "/Volumes/Slow/uboone_data/pi0/larlite_reco2d_00%d.root" % i )
    #my_proc.add_input_file("/Volumes/Slow/uboone_data/01292015/e/larlite_reco2d_000%d.root" % i)


my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.
u = 0
my_proc.process_event()
my_proc.process_event()

print "Next event...\n";

manager  = myunit.algo_manager(0)
clusters = manager.Clusters()
nclusters = clusters.size()
pcatrackshower = manager.GetAlg(1)
u += 1
subimg = pcatrackshower.GetSubImage(4)

kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
r = cv2.dilate(subimg,kernel,iterations = 1)
r = cv2.blur(r,(2,2))
ret,r = cv2.threshold(r,0,255,cv2.THRESH_BINARY)

plt.imshow(r,cmap = cm.Greys,interpolation='none')

plt.show()
