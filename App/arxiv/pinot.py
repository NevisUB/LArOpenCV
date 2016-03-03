#by vic
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc
import matplotlib.cm as cm

import sys

from array import array

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv
from ROOT import larutil
geoutil = larutil.GeometryUtilities.GetME()

from methods import *

my_proc = fmwk.ana_processor()

# Config file
cfg="SBCluster.fcl"

for i in xrange(0,10):
    my_proc.add_input_file( "/Users/vgenty/data/larlite_reco2d_01%d.root" % i )
    my_proc.add_input_file( "/Users/vgenty/data/larlite_mcinfo_01%d.root" % i )    
    

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)

mcunit = fmwk.MCGetter()

my_proc.add_process(myunit)
my_proc.add_process(mcunit)

# In[ ]:

we = int(0)

while ( my_proc.process_event() ) :
    
    manager  = myunit.algo_manager(2) # Y plane?
    clusters = manager.Clusters()

    pca  = manager.GetAlg(1)
    meta = manager.MetaData(1)

    nclusters = pca._cparms_v.size()

    fig, ax = plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):

        ax = plt.gca()
        
        params  = pca._cparms_v[i]
        cluster = clusters[params.in_index_]
        hits    = params.hits_
        line    = params.line_
        charge  = params.charge_
        boxes   = params.boxes_
        combine = params.combined_
        path    = params.path_
        
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

        # colors = []
        # for c in range(charge.size()):
        #     ax.plot(xy2[c][0],xy2[c][1],'o',color=str(1- ( charge[c] / 255.0 )))

        
        xy2 = np.array(xy2)
        # ax  = plt.gca()
        ax.plot(xy1[:,0],xy1[:,1],color='blue')
        ax.plot(xy2[:,0],xy2[:,1],'o',color='black')
        
        #ax.plot([line[0],line[2]],[line[1],line[3]],'-',color='pink',lw=2)
        #ax.plot([meanline[0],meanline[2]],
        #        [meanline[1],meanline[3]],
        #        '--',color='purple',lw=2)
        #print meanline[0]
        #ax.plot(params.startx_,params.starty_,'o')
        #ax.plot(params.meanx_,params.meany_,'o',lw=10,color="blue")

        for r in xrange(boxes.size()): # same size as number of boxes right...
            box   = boxes[r]
            re    = box.box_

            if box.empty_ :
                continue;
            
            ##vic
            cov = np.abs(box.cov_)
            
            xes,yes = get_bounded_line(box.line_,re)
            ax.plot(xes,yes,'-',color='red',lw=2)


        doit = False
        
        for i in xrange(boxes.size()) :

            if boxes[i].empty_ :
                continue;
            
            c = combine[i]
            if c.size() == 0:
                continue;
            
            color=np.random.rand(3,1)
            fi = boxes[i].box_
            ec = 'None'

            
            doit = False

            if i == path.seed_:
                rectangle = plt.Rectangle((fi.x, fi.y), fi.width, fi.height, fc='white',ec='black',alpha=1,lw=3)
                ax.add_patch(rectangle)
                doit = True;

                
            rectangle = plt.Rectangle((fi.x, fi.y), fi.width, fi.height, fc=color,ec=None,alpha=0.2,lw=2)
            ax.add_patch(rectangle)

                
            for cb in xrange(c.size()) :
                con = c[cb]

                re = boxes[con].box_

                if doit:
                    rectangle = plt.Rectangle((re.x, re.y), re.width, re.height, fc='white',ec='black',alpha=1,lw=3)
                    ax.add_patch(rectangle)
            
                rectangle = plt.Rectangle((re.x, re.y), re.width, re.height, fc=color,ec=None,alpha=0.2,lw=2)
                ax.add_patch(rectangle)
        
        # circle = plt.Circle( (path.avg_center_pca.x,path.avg_center_pca.y), radius=3, fc='blue')
        # ax.add_patch(circle)

        mcshowers = mcunit.showers_
        for i in xrange( mcshowers.size() ):
            # print "=="
            # print "Shower: %d" % i
            # print "~~"
            shower = mcshowers[i]
            # print "PDGCode: %d" % shower.PdgCode()
            # print "Trackid: %d" % shower.TrackID()
            # print "Process: %s" % shower.Process()
            # print "Mom PDG: %d" % shower.MotherPdgCode()
            # print "Mom tid: %d" % shower.MotherTrackID()
            # print "Mom proc:%s" % shower.MotherProcess()
            
            angle = geoutil.Get2DangleFrom3D(2,shower.StartDir()) * 180.0/np.pi
            # print "The angle is... %f" %  angle
            
            a = array('d',[0,0,0])

            a[0] = shower.Start().X()
            a[1] = shower.Start().Y()
            a[2] = shower.Start().Z()


            b = array('d',[0,0,0])

            b[0] = shower.End().X()
            b[1] = shower.End().Y()
            b[2] = shower.End().Z()
            
            ptstart = geoutil.Get2DPointProjection(a,2);
            ptend   = geoutil.Get2DPointProjection(b,2);

            startx = ptstart.w - meta.origin().x
            starty = ptstart.t - meta.origin().y
            endx   = ptend.w - meta.origin().x
            endy   = ptend.t - meta.origin().y

            xx = np.array([startx,endx])
            yy = np.array([starty,endy])

            ax.plot(yy,xx,'-o',color='purple',lw=2)# how to subtrack off to get to actual image ?

            
        centerx = path.avg_center_pca.x
        centery = path.avg_center_pca.y
        dirx    = path.combined_e_vec_.x
        diry    = path.combined_e_vec_.y

        step = ( starty - centerx ) / ( dirx )
        
        ax.plot([centerx,centerx + dirx*step],
                [centery,centery + diry*step],'-',color='gray',lw=3)
        

            
    plt.show()
    
