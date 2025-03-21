#by vic
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
#import matplotlib.collections as mc
#import matplotlib.cm as cm

import sys
import math

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

#my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/reco_larlite/eminus/reco2d_0.root")
#my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/mcinfo_larlite/eminus/mcinfo_0.root")
my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc7/mcinfo_larlite/pi0/larlite_mcinfo_000.root")
my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc7/reco_larlite/pi0/larlite_reco2d_000.root") 
#my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/reco_larlite/gamma/1694754_0/larlite_reco2d.root")
#my_proc.add_input_file("/Users/ah673/WorkArea/data/BNBFiles/mcc6/mcinfo_larlite/gamma/mcinfo_0.root")
    
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)

mcunit = fmwk.MCGetter()

my_proc.add_process(myunit)
my_proc.add_process(mcunit)

while ( my_proc.process_event() ) :
    
    manager  = myunit.algo_manager(2) # Y plane?
    clusters = manager.Clusters()

    ts  = manager.GetAlg(1)
    meta = manager.MetaData(1)

    nclusters = ts._cparms_v.size()

    fig, ax = plt.subplots(figsize=(10,6))
    end = 0

    for i in xrange(nclusters):
        #print "\nStarting new cluster now...", nclusters

        #ax = plt.gca()
        
        params  = ts._cparms_v[i]
        cluster = clusters[params.in_index_]
        hits    = params.hits_
        nhits    = params.nhits_
        start_end = params.startend_
	area    = params.area_
	perimeter = params.perimeter_
	rectangle = params.rectangle_
	angle = params.angle_
        
        if area < 900: 
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
	   #print "indices : ", p.x, ",", p.y
	xy3.append((rectangle[0].x,rectangle[0].y))
        xy3 = np.array(xy3)

        ax.plot(xy1[:,0],xy1[:,1],color='blue')
        ax.plot(xy2[:,0],xy2[:,1],'.',color='black')
        ax.plot(xy3[:,0],xy3[:,1],color='purple')
	ax.plot(start_end[0].first,start_end[0].second ,'o',color='green' )
	ax.plot(start_end[1].first ,start_end[1].second ,'o',color='red' )
	#ax.plot(start_end[0].first + meta.origin().y,start_end[0].second + meta.origin().x,'o',color='green' )
	#ax.plot(start_end[1].first + meta.origin().y,start_end[1].second + meta.origin().x,'o',color='red' )

	end = start_end

	print "Area: ", area , " , ", angle

	#sys.stdin.readline()
#    plt.show()

    mcshowers = mcunit.showers_
    mctruths  = mcunit.truths_
    mcs_v = []
    for i in xrange( mcshowers.size() ):
       # print "=="
       # print "Shower: %d" % i
       # print "~~"
       shower = mcshowers[i]
       mcs_v.append(shower)
       # print "PDGCode: %d" % shower.PdgCode()
       # print "Trackid: %d" % shower.TrackID()
       # print "Process: %s" % shower.Process()
       # print "Mom PDG: %d" % shower.MotherPdgCode()
       # print "Mom tid: %d" % shower.MotherTrackID()
       # print "Mom proc:%s" % shower.MotherProcess()
       
       #angle2 = geoutil.Get2DangleFrom3D(2,shower.StartDir()) * 180.0/np.pi
       # print "The angle is... %f" %  angle2

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
       endx   = ptend.w   - meta.origin().x 

       starty = ptstart.t - meta.origin().y 
       endy   = ptend.t   - meta.origin().y 

       xx = np.array([startx,endx])
       yy = np.array([starty,endy])

       ax.plot(yy,xx,'-o',color='purple',lw=2)# how to subtrack off to get to actual image ?

       print "gamma energy : ", shower.DetProfile().E()
       
    if len(mcs_v) == 2:
      s0 = mcs_v[0]
      s1 = mcs_v[1]
      angle = math.acos(s0.StartDir().Dot( s1.StartDir())/s1.StartDir().Mag()/s0.StartDir().Mag())
      angle_2 = 2*math.asin(134.98 / 2 / math.sqrt(s0.DetProfile().E() * s1.DetProfile().E() ) ) 
    
      mass = 2 * math.sin(angle/2) * math.sqrt(s0.DetProfile().E() * s1.DetProfile().E() )
      mass_2 = 2 * math.sin(angle_2/2) * math.sqrt(s0.DetProfile().E() * s1.DetProfile().E() )

      pi0_e = s0.DetProfile().E() + s1.DetProfile().E()

      print "pi0_e: ", pi0_e, "\nAngle: (", angle*180/math.pi, " vs ", angle_2*180/math.pi, ")\nMass : (", mass, " vs ", mass_2, ")"

    #for m in xrange( mctruths.size() ):
    #  truth = mctruths[m]
    #  ps = truth.GetParticles()
    #  for p in ps:
    #    if p.PdgCode() == 111:
    #      print "pi0 energy : ", p.Trajectory().at(0).E()*1000, ", mass: ", p.Mass()
    

    plt.show()
    
