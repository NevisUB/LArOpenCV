import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc
import matplotlib.cm as cm

import sys

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv

my_proc = fmwk.ana_processor()

# Config file
cfg="../../App/mac/SBCluster.fcl"

my_proc.add_input_file(sys.argv[1])

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

while( my_proc.process_event() ) :

    manager  = myunit.algo_manager()
    fig,ax = plt.subplots(figsize=(15,10))
    print "NUMBER OF CLUSTERS", manager.NumClusters()

    if manager.NumClusters() == 0:
        continue
    
    for c in xrange(manager.NumClusters()):
        xx = []; yy = [];

        cluster = manager.Cluster(c,2)
        
        if cluster.PlaneID() == 2:
            continue
        
        if  manager.Cluster(c,2)._contour.size() > 10000:
            continue
    
        for i in xrange(manager.Cluster(c,2)._contour.size()):
            px = []; py = [];
            contour = manager.Cluster(c,2)._contour[i]

            for p in xrange(cluster._insideHits.size()):
                hit = cluster._insideHits[p]
                px.append(hit.x)
                py.append(hit.y)
            
            xx.append(contour.x)
            yy.append(contour.y)
            
        plt.plot(px,py,'o',color='black')
        
        plt.plot(cluster._centerPt.x,
                 cluster._centerPt.y,'o',color='green')
        
        xx.append(xx[0])
        yy.append(yy[0])

        plt.plot(xx,yy,'-',lw=3)
    
    plt.show()
    

