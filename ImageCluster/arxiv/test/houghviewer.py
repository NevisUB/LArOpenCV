import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc


import sys

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv

my_proc = fmwk.ana_processor()

# Config file
cfg="houghcluster.fcl"

my_proc.add_input_file("/Users/vgenty/Desktop/single_mu.root")

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.

while ( my_proc.process_event() ) :

    manager = myunit.algo_manager(0)
    clusters = manager.Clusters()

    houghptrackshower = manager.GetAlg(1)

    houghlines = houghptrackshower._houghs_v;

    print houghptrackshower
    
    nclusters  = clusters.size()
    
    print "nclusters: ",nclusters

    fig,ax = plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):
        cluster   = clusters[i]
        houghs    = houghlines[i]
        
        if cluster.size() < 60:
            continue
        
        xy1 = []

        for p in xrange(cluster.size()):
            pt = cluster[p]
            xy1.append( (pt.x, pt.y) )
            
        xy1.append(xy1[0])
        xy1 = np.array(xy1)

        lines = []

        for v in xrange(houghs.size()) :
            hough = houghs[v]
            lines.append([ [ hough[0],hough[1] ], [ hough[2], hough[3] ] ] )
        
        print lines
        lc = mc.LineCollection(lines, linewidths=5,color='blue',alpha=0.7)
        plt.plot(xy1[:,0],xy1[:,1],color='red')
        ax.add_collection(lc)
        ax.autoscale()
        plt.show()

