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
cfg="satellites_config.fcl"

for i in xrange(1,10):
    my_proc.add_input_file( "/Volumes/Slow/uboone_data/pi0/larlite_reco2d_00%d.root" % i )
    #my_proc.add_input_file("/Volumes/Slow/uboone_data/01292015/e/larlite_reco2d_000%d.root" % i)


my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.

while ( my_proc.process_event() ) :
    print "Next event...\n";

    manager  = myunit.algo_manager(0)
    clusters = manager.Clusters()

    satellitemerge = manager.GetAlg(2)
    sic = satellitemerge._secret_initial_sats
    in_clusters = sic.size()
    out_clusters= clusters.size()

    print "In : %d   Out: %d " % (in_clusters,out_clusters)

    fig, ax = plt.subplots(figsize=(10,6))

    xy = []
    for n in xrange(clusters.size()):
        cluster = clusters[n]
        i = []
        j = []
        for p in xrange(cluster.size()):
            pt = cluster[p]
            i.append(pt.x)
            j.append(pt.y)

            
        xy.append((i,j))

    for p in xy:
        ax.plot(p[0],p[1],'-',color='blue',lw=2)

    yx = []
    for n in xrange(sic.size()):
        cluster = sic[n]
        i = []
        j = []
        for p in xrange(cluster.size()):
            pt = cluster[p]
            i.append(pt.x)
            j.append(pt.y)

            
        yx.append((i,j))

    for p in yx:
        ax.plot(p[0],p[1],'-',color='red',lw=2)

        
    plt.show()
        
