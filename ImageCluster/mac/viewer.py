import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc
import matplotlib.cm as cm

import sys

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv

from methods import *

my_proc = fmwk.ana_processor()

# Config file
cfg="../../App/mac/SBCluster.fcl"


algid = int(sys.argv[2])

my_proc.add_input_file(sys.argv[1])
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

while( my_proc.process_event() ) :

    manager  = myunit.algo_manager()
    print "NUMBER OF CLUSTERS", manager.NumClusters()

    if manager.NumClusters() == 0:
        print "No clusters found at all...\n";
        continue

    fig,_ = plt.subplots(figsize=(15,10))

    ax0 = plt.subplot(3,1,1)
    ax1 = plt.subplot(3,1,2)
    ax2 = plt.subplot(3,1,3)

    axx = {0 : ax0, 1 : ax1, 2: ax2}
    
    for c in xrange(manager.NumClusters(algid)):

        cluster = manager.Cluster(c,algid)

        if cluster.PlaneID() > 3:
            print "BAD PLANEID"
            print cluster.PlaneID()
            continue

        ax = axx[cluster.PlaneID()]

        contour = cluster._contour

        if contour.size() == 0:
            print "CLUSTER OF ZERO SIZE FOUND!"
            continue

        if contour.size() > 100000:
            print "CLUSTER CONTOUR WAY TOO BIG"
            continue
        
        
        cx,cy = get_xy(contour)

        if  cluster._insideHits.size() > 0:
            px,py = get_xy(cluster._insideHits)
            ax.plot(px,py,'o',color='black')

        cx.append(cx[0])
        cy.append(cy[0])

        ax.plot(cx,cy,'-',lw=3)
        ax.plot(cluster._startPt.x,
                cluster._startPt.y,'o',color='purple')

        ax.plot(cluster._endPt.x,
                cluster._endPt.y,'o',color='green')

    plt.show()

    plt.cla()
    plt.clf()
    print "closing..."
    plt.close("all")
