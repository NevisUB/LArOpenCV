import matplotlib
import matplotlib.pyplot as plt

import sys

from larlite import larlite as fmwk

from viewer_methods import draw_cluster
import algo_viewer as av

my_proc = fmwk.ana_processor()

# Config file
cfg="../../../App/mac/SBCluster.fcl"

algid  = int(sys.argv[2])

my_proc.add_input_file(sys.argv[1])
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

k = 0 
while( my_proc.process_event() ) :
    k += 1
    manager  = myunit.algo_manager()
    print "NUMBER OF CLUSTERS", manager.NumClusters()
    print "You have chosen algorithm:"
    print manager.GetClusterAlg(algid)
    print ""
    
    if manager.NumClusters() == 0:
        print "No clusters found at all...\n";
        continue

    fig,_ = plt.subplots(figsize=(15,10))

    ax0 = plt.subplot(3,1,1)
    ax1 = plt.subplot(3,1,2)
    ax2 = plt.subplot(3,1,3)

    axx = {0 : ax0, 1 : ax1, 2: ax2}

    
    algo_drawer = av.AlgoViewer( manager.GetClusterAlg(algid),
                                 plt )
    for c in xrange(manager.NumClusters(algid)):
        
        cluster = manager.Cluster(c,algid)
        
        try:
            if cluster.PlaneID() >= 3:
                print "BAD PLANEID"
                continue
        except ReferenceError:
            "__reference_error__"
            continue

        ax = axx[ cluster.PlaneID() ]
        
        draw_cluster(ax,cluster)
        algo_drawer.draw(ax,cluster,c)
        
    plt.show()

    #All possible ways to close viewer
    plt.cla()
    plt.clf()
    plt.close("all")
