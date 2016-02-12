
# coding: utf-8

# In[1]:

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
cfg="SBCluster.fcl"

for i in xrange(0,10):
    my_proc.add_input_file( "/Volumes/Slow/uboone_data/pi0/larlite_reco2d_01%d.root" % i )
    

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)


# In[ ]:

we = int(0)

while ( my_proc.process_event() ) :
    
    manager  = myunit.algo_manager(2) # Y plane?
    clusters = manager.Clusters()

    pca = manager.GetAlg(1)

    nclusters = pca._cparms_v.size()

    fig, ax = plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):

        params  = pca._cparms_v[i]
        cluster = clusters[params.in_index_]
        hits    = params.hits_
        line    = params.line_
        charge  = params.charge_
        boxes   = params.boxes_
        combine = params.combined_

        if hits.size() < 10 :
            continue
        
        xy1 = []

        for p in xrange(cluster.size()):
            pt = cluster[p]
            xy1.append( (pt.x, pt.y) )

        xy1.append(xy1[0])
        xy1 = np.array(xy1)
        xy2 = []
        for p in xrange(hits.size()):
            hit = hits[p]
            xy2.append( (hit.first,hit.second) )

        colors = []
        for c in range(charge.size()):
            colors.append(charge[c])
        
        xy2 = np.array(xy2)
        ax  = plt.gca()
        ax.plot(xy1[:,0],xy1[:,1],color='blue')
        ax.plot(xy2[:,0],xy2[:,1],'o',color='black')

        #ax.plot([line[0],line[2]],[line[1],line[3]],'-',color='pink',lw=2)
        #ax.plot([meanline[0],meanline[2]],
        #        [meanline[1],meanline[3]],
        #        '--',color='purple',lw=2)
        #print meanline[0]
        #ax.plot(params.startx_,params.starty_,'o')
        #ax.plot(params.meanx_,params.meany_,'o',lw=10,color="blue")

        print "I have " + str( boxes.size() ) + " boxes..."
        for r in xrange(boxes.size()): # same size as number of boxes right...
            box   = boxes[r]
            re    = box.box_

            if box.empty_ :
                continue;
            
            # print "Its subdivided?: " + str(box.subdivided_)
            # print "Its empty?:      " + str(box.empty_)
            # print "subboxes.size()  " + str(box.subboxes_.size())
            
            # if box.subdivided_ == True:
                
            #     print "Its subdivided..."
                
            #     subboxes   = box.subboxes_;
            #     n_subboxes = subboxes.size();

            #     for i in xrange(n_subboxes) :
            #         subbox = subboxes[i]

            #         if subbox.empty_ == True:
            #             print "It's empty..."
            #             continue

            #         subline = subbox.line_
            #         subre   = subbox.box_

            #         rectangle = plt.Rectangle((subre.x, subre.y), subre.width, subre.height, color="green",alpha=0.1)
            #         ax.add_patch(rectangle)
                    
            #         xes, yes = get_bounded_line(subline,subre)
            #         ax.plot(xes,yes,'-',color='blue',lw=2)
                    
            #         ax.plot(subbox.e_center_.x+re.x,subbox.e_center_.y+re.y,'o',color='pink')

            #         # ctors = subbox.pts_; npts = ctors.size();
            #         # ww = []; tt = []
            #         # for i in xrange(npts):
            #         #     pt = ctors[i]
            #         #     ww.append(pt.x+re.x); tt.append(pt.y+re.y)

            #         # ax.plot(ww,tt,'o',color='pink',alpha=1,markersize=7)
            # else :

            ##vic
            cov = np.abs(box.cov_)
            
            rectangle = plt.Rectangle((re.x, re.y), re.width, re.height, fc=str(cov),alpha=0.2)
            ax.add_patch(rectangle)
                
            xes,yes = get_bounded_line(box.line_,re)
            ax.plot(xes,yes,'-',color='red',lw=2)

        for i in xrange(boxes.size()) :

            if boxes[i].empty_ :
                continue;
            
            c = combine[i]
            if c.size() == 0:
                continue;
            
            color=np.random.rand(3,1)
            fi = boxes[i].box_

            ax.plot([fi.x,fi.x + fi.width,fi.x+fi.width,fi.x,fi.x],
                    [fi.y,fi.y,fi.y+fi.height,fi.y+fi.height,fi.y],color=color,lw=2)
            
            for cb in xrange(c.size()) :
                con = c[cb]

                re = boxes[con].box_
                
                ax.plot([re.x,re.x + re.width,re.x+re.width,re.x,re.x],
                        [re.y,re.y,re.y+re.height,re.y+re.height,re.y],color=color,lw=2)
        
        #plt.savefig("oo%d.png" % we, format='png', dpi=1000)
        plt.show()
        #we += 1
        
         

    # In[ ]:



