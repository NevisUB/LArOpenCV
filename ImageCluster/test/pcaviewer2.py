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
cfg="pcacluster.fcl"

for i in xrange(1,10):
    my_proc.add_input_file("/Volumes/Slow/uboone_data/01292015/e/larlite_reco2d_000%d.root" % i)


my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

# Let's run it.

while ( my_proc.process_event() ) :

    print "Next event...\n";
    
    manager  = myunit.algo_manager(0)
    clusters = manager.Clusters()

    pcatrackshower = manager.GetAlg(1)

    center_points = pcatrackshower._cntr_pt_v
    eigen_vecs    = pcatrackshower._eigen_vecs_v
    eigen_vals    = pcatrackshower._eigen_val_v
    lines         = pcatrackshower._line_v;
    dlines        = pcatrackshower._dlines_v;
    ddds          = pcatrackshower._ddd_v;
    trunk_idx     = pcatrackshower._trunk_index_v;
    
    nclusters = clusters.size()
    
    print dlines.size();
    
    print "nclusters: ",nclusters

    fig= plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):

        
        cluster   = clusters[i]
        center_pt = center_points[i]
        eigen_val = eigen_vals[i]
        eigen_vec = eigen_vecs[i]
        line      = lines[i]
        dline     = dlines[i]
        ddd       = ddds[i]
        tidx      = trunk_idx[i]
        
        if ddd.size() < 20:
            continue

        ax = plt.subplot(2,1,1)
        xy1 = []

        for p in xrange(cluster.size()):
            pt = cluster[p]
            xy1.append( (pt.x, pt.y) )

        xy1.append(xy1[0])
        xy1 = np.array(xy1)


        dd = []
        for d in xrange(dline.size()):
            df = dline[d];
            dd.append( [ [ df[0],df[1] ],[ df[2],df[3] ] ] )
        

        lc = mc.LineCollection(dd, linewidths=1,color='blue',alpha=0.7)
        
        subimg = pcatrackshower.GetSubImage(i)
        
        plt.imshow(subimg,cmap = cm.Greys,interpolation='none')
        ax.plot(xy1[:,0],xy1[:,1],color='red')
        ax.plot(center_pt.x,center_pt.y,'o',color='purple',lw=5)
        ax.plot(line[0],line[1],'o',color='green',lw=10)
        ax.plot(line[2],line[3],'o',color='green',lw=10)
        ax.plot([line[0],line[2]],[line[1],line[3]],'k-',lw=2);
        ax.add_collection(lc)
        ax.set_xlim(line[0]-5,line[2]+5)
        
        print ddd.size()
        print tidx.first
        print tidx.second
        
        ax.plot([ddd[tidx.first].first ,ddd[tidx.second].first],
                [(line[3]-line[1])/(line[2]-line[0])*ddd[tidx.first ].first  + line[1],
                 (line[3]-line[1])/(line[2]-line[0])*ddd[tidx.second].first  + line[1]],
                 '-',color='orange',lw=30,alpha=0.5)

        
        
        ax = plt.subplot(2,1,2)

        x = []
        y = []
        for i in xrange(ddd.size()):
            d = ddd[i];
            x.append(d.first); y.append(d.second)

        ax.plot(x,y,'-')

        plt.show()
        

