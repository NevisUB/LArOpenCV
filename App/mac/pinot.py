
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
        rect    = params.rect_
        llines  = params.llines_
        meanline= params.meanline_
        combine = params.combined_

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

        for r in xrange(rect.size()):
            re    = rect[r]
            lline = llines[r]

            cov = np.abs(params.covar_[r])
            rectangle = plt.Rectangle((re.x, re.y), re.width, re.height, fc=str(cov),alpha=0.2)
            ax.add_patch(rectangle)

            #ax.plot([re.x,re.x + re.width,re.x+re.width,re.x,re.x],
            #        [re.y,re.y,re.y+re.height,re.y+re.height,re.y],color='green')

            slope  = ( lline[3] - lline[1] ) / ( lline[2] - lline[0] )
            yinter = lline[1]
            
            xx = np.arange(lline[0],lline[2],0.1)
            yy = ( xx - re.x )*slope + yinter

            xx = xx.tolist()
            yy = yy.tolist()

            xes = [x for x in xx if yy[ xx.index(x) ] >= re.y and yy[ xx.index(x) ] <= (re.y + re.height) ]
            yes = [y for y in yy if y >= re.y and y <= (re.y + re.height)]

            xes = np.array(xes)
            yes = np.array(yes)
            
            ax.plot(xes,yes,'-',color='red',lw=2)

        for i in xrange(rect.size()) :
            c = combine[i]
            if c.size() == 0:
                continue;
            color=np.random.rand(3,1)
            fi = rect[i]
            ax.plot([fi.x,fi.x + fi.width,fi.x+fi.width,fi.x,fi.x],
                    [fi.y,fi.y,fi.y+fi.height,fi.y+fi.height,fi.y],color=color,lw=2)
            
            for cb in xrange(c.size()) :
                con = c[cb]

                re = rect[con]
                
                ax.plot([re.x,re.x + re.width,re.x+re.width,re.x,re.x],
                        [re.y,re.y,re.y+re.height,re.y+re.height,re.y],color=color,lw=2)
        
        #plt.savefig("oo%d.png" % we, format='png', dpi=1000)
        plt.show()
        #we += 1
        
         

    # In[ ]:



