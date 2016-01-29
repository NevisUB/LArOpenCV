
# coding: utf-8

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
cfg="cdcluster.fcl"

my_proc.add_input_file("/Users/vgenty/Desktop/single_e.root")

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.
my_proc.process_event()
manager = myunit.algo_manager(0)
clusters = manager.Clusters()

cdtrack_shower = manager.GetAlg(1)

defects = cdtrack_shower._defects_v
hullpts = cdtrack_shower._hullpts_v

nclusters = clusters.size()

fig,ax = plt.subplots(figsize=(10,6))

print "nclusters: ",nclusters

for i in xrange(nclusters):
    
    cluster = clusters[i]
    defect  = defects [i]
    hullpt  = hullpts [i]

    if cluster.size() < 60:
        continue
        
    xy1 = []
            
    for p in xrange(cluster.size()):
        pt = cluster[p]
        xy1.append( (pt.x, pt.y) )

    xy1.append(xy1[0])
    
    xy2 = []
            
    for p in xrange(hullpt.size()):
        pt = cluster[hullpt[p]]
        xy2.append( (pt.x, pt.y) )

    xy2.append(xy2[0])

    
    lines = []
    pxy = []


    for dd in xrange(defect.size()) :
        deff = defect[dd]

        s = deff[0]
        e = deff[1]
        f = deff[2]
        d = deff[3]

        start = xy1[s]
        end =   xy1[e]
        far =   xy1[f]
        
        lines.append([start, far])
        lines.append([far, end])
        pxy.append(xy1[f])

        
    xy1 = np.array(xy1)
    xy2 = np.array(xy2)
    pxy = np.array(pxy)
    
    lc = mc.LineCollection(lines, linewidths=10,color='green',alpha=0.7)
    ax.plot(xy1[:,0],xy1[:,1],'-',color='red')
    ax.plot(xy2[:,0],xy2[:,1],'-',color='blue')
    ax.add_collection(lc)
    ax.plot(pxy[:,0],pxy[:,1],'o',color='purple')
    ax.autoscale()

    plt.show()
