#by vic
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
#import matplotlib.collections as mc
#import matplotlib.cm as cm

import sys

from array import array

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv
#from ROOT import larutil
#geoutil = larutil.GeometryUtilities.GetME()
#from methods import *

my_proc = fmwk.ana_processor()

# Config file
cfg="SBCluster.fcl"

my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/reco_larlite/eminus/reco2d_0.root")
my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/mcinfo_larlite/eminus/mcinfo_0.root")
    
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)

mcunit = fmwk.MCGetter()

my_proc.add_process(myunit)
#my_proc.add_process(mcunit)

while ( my_proc.process_event() ) :
    
    manager  = myunit.algo_manager(2) # Y plane?
    clusters = manager.Clusters()

    ts  = manager.GetAlg(1)
    meta = manager.MetaData(1)

    nclusters = ts._cparms_v.size()

    fig, ax = plt.subplots(figsize=(10,6))

    for i in xrange(nclusters):
        #print "\nStarting new cluster now...", nclusters

        ax = plt.gca()
        
        params  = ts._cparms_v[i]
        cluster = clusters[params.in_index_]
        hits    = params.hits_
        nhits    = params.nhits_
        start_end = params.startend_
	area    = params.area_
	perimeter = params.perimeter_
	rectangle = params.rectangle_
        
        if area < 1850: 
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
        xy2 = np.array(xy2)

	xy3 = []
	for p in rectangle:
           xy3.append( (p.x,p.y) )
	   print "indices : ", p.x, ",", p.y
	xy3.append((rectangle[0].x,rectangle[0].y))
        xy3 = np.array(xy3)

        ax.plot(xy1[:,0],xy1[:,1],color='blue')
        ax.plot(xy2[:,0],xy2[:,1],'.',color='black')
        ax.plot(xy3[:,0],xy3[:,1],color='purple')
	ax.plot(start_end[0].first,start_end[0].second,'o',color='green' )
	ax.plot(start_end[1].first,start_end[1].second,'o',color='red' )

	print "Area: ", area 

        #plt.show()
	#sys.stdin.readline()


    plt.show()



     #   mcshowers = mcunit.showers_
     #   for i in xrange( mcshowers.size() ):
     #       # print "=="
     #       # print "Shower: %d" % i
     #       # print "~~"
     #       shower = mcshowers[i]
     #       # print "PDGCode: %d" % shower.PdgCode()
     #       # print "Trackid: %d" % shower.TrackID()
     #       # print "Process: %s" % shower.Process()
     #       # print "Mom PDG: %d" % shower.MotherPdgCode()
     #       # print "Mom tid: %d" % shower.MotherTrackID()
     #       # print "Mom proc:%s" % shower.MotherProcess()
     #       
     #       angle = geoutil.Get2DangleFrom3D(2,shower.StartDir()) * 180.0/np.pi
     #       # print "The angle is... %f" %  angle
     #       
     #       a = array('d',[0,0,0])

     #       a[0] = shower.Start().X()
     #       a[1] = shower.Start().Y()
     #       a[2] = shower.Start().Z()

     #       b = array('d',[0,0,0])

     #       b[0] = shower.End().X()
     #       b[1] = shower.End().Y()
     #       b[2] = shower.End().Z()
     #       
     #       ptstart = geoutil.Get2DPointProjection(a,2);
     #       ptend   = geoutil.Get2DPointProjection(b,2);

     #       startx = ptstart.w - meta.origin().x
     #       starty = ptstart.t - meta.origin().y
     #       endx   = ptend.w - meta.origin().x
     #       endy   = ptend.t - meta.origin().y

     #       xx = np.array([startx,endx])
     #       yy = np.array([starty,endy])

            #ax.plot(yy,xx,'-o',color='purple',lw=2)# how to subtrack off to get to actual image ?

            
       # step = ( starty - centerx ) / ( dirx )
        
       # ax.plot([centerx,centerx + dirx*step],
       #         [centery,centery + diry*step],'-',color='gray',lw=3)
        

    
