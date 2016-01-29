
# coding: utf-8

# In[1]:



# In[1]:

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
cfg="pcacluster.fcl"

my_proc.add_input_file("/Users/vgenty/Desktop/single_e.root")

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.

while ( my_proc.process_event() ) :

    manager = myunit.algo_manager(0)
    clusters = manager.Clusters()

    pcatrackshower = manager.GetAlg(1)

    center_points = pcatrackshower._cntr_pt_v
    eigen_vecs    = pcatrackshower._eigen_vecs_v
    eigen_vals    = pcatrackshower._eigen_val_v
    
    nclusters = clusters.size()
    
    

    
    
    print "nclusters: ",nclusters

    fig,ax = plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):
        cluster   = clusters[i]
        center_pt = center_points[i]
        eigen_val = eigen_vals[i]
        eigen_vec = eigen_vecs[i]
        
        if cluster.size() < 60:
            continue

        
        xy1 = []

        for p in xrange(cluster.size()):
            pt = cluster[p]
            xy1.append( (pt.x, pt.y) )
            
        xy1.append(xy1[0])

        scale_factor = 0.08;
        
        line1 = [[center_pt.x,center_pt.y],[center_pt.x + 
                                            scale_factor* ( eigen_val[0]*eigen_vec[0].x ),
                                            center_pt.y + 
                                            scale_factor* ( eigen_val[0]*eigen_vec[0].y )]]
        line2 = [[center_pt.x,center_pt.y],[center_pt.x + 
                                            scale_factor* ( eigen_val[1]*eigen_vec[1].x ),
                                            center_pt.y + 
                                            scale_factor* ( eigen_val[1]*eigen_vec[1].y )]]
        lines = []
        lines.append(line1)
        lines.append(line2)
    
        xy1 = np.array(xy1)
        lc = mc.LineCollection(lines, linewidths=5,color='blue',alpha=0.7)
        ax.add_collection(lc)
        ax.plot(xy1[:,0],xy1[:,1],color='red')
        ax.plot(center_pt.x,center_pt.y,'o',color='purple',lw=5)
        ax.autoscale()
        plt.show()

